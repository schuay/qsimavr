#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>

#include "uart_streams.h"
#include <assert.h>

#include "timer.h"
#include "lcd.h"
#include "pong.h"
#include "glcd.h"

/**
 * @file main.c
 *
 * The glue between drivers and game logic.
 * Handles high level tasks such as managing wiimote connections,
 * the game state machine, running the main loop including background tasks, etc.
 */

/** The maximum count of connected wiimotes. */
#define WIIMOTE_COUNT (2)

/** Flags which are used to control task execution during the main loop. */
enum task_flags {
    RunLogic = 1 << 0,
    ADCWaiting = 1 << 1,
    MP3DataRequested = 1 << 2,
};

/**
 * The game's state machine.
 *
 * The startup state is GamePaused. In this state, nothing is drawn to the GLCD,
 * game logic is paused, and connections are attempted.
 * If both wiimotes are connected,
 * the state is changed to GameRunning.
 *
 * In GameRunning, game logic is run, and the board is drawn to the LCD.
 * If a wiimote is disconnected, go back to GamePaused.
 * If a point is scored, enter PointScored.
 *
 * In PointScored, an MP3 sound is sent to the MP3 module. No game logic
 * is run, nothing is drawn to the GLCD.
 * Once the MP3 is finished playing, either go back to GameRunning
 * if both wiimotes are still connected,
 * or to GamePaused if a wiimote has disconnected.
 */
enum state {
    GamePaused,
    GameRunning,
    PointScored,
};

/**
 * This struct stores basically everything that is needed
 * for high level game execution.
 */
static volatile struct {
    uint8_t flags; /**< Controls which tasks are executed during the main
                        loop. */
    uint8_t ticks; /**< Keeps track of how many ticks have passed. Some tasks
                        are only executed once every n ticks. */
    uint16_t adc_result; /**< The result of the latest ADC conversion. */
    uint8_t volume; /**< The current volume. */
    uint16_t buttons[WIIMOTE_COUNT]; /**< The status of wiimote buttons. */
    enum state st; /**< Current state of our state machine. */
} glb;

/**
 * Our basic 'scheduler'. The main loop is executed (at least)
 * once per tick. The game logic is executed exactly once per tick.
 */
static void tick(void) {
    glb.flags |= RunLogic;
    glb.ticks++;
}

/**
 * Draws connected players and current scores to the LCD.
 */
static void draw_lcd(void) {
    lcd_clear();
    lcd_putstr_P(PSTR("P1"), 0, 0);
    lcd_putstr_P(PSTR("P2"), 0, 14);
    lcd_putstr_P(PSTR("Connecting"), 0, 3);

    uint8_t p1, p2;
    pong_scores(&p1, &p2);
    lcd_putchar(p1 + '0', 1, 0);
    lcd_putchar(p2 + '0', 1, 15);
}

/**
 * Initializes all required subsystems.
 */
static void init(void) {
    sleep_enable();
    uart_streams_init();
    glcdInit();
    pong_init();

    struct timer_conf conf = { Timer1, false, 5, tick };
    timer_set(&conf);

    memset((void *)&glb, 0, sizeof(glb));
    glb.st = GameRunning;

    sei();

    lcd_init();
}

/**
 * Handles game movement, user input and game logic.
 */
static void task_logic(void) {
    if (glb.ticks % 20 == 0) {
        if (pong_ball_step()) {
            /* A point has been scored.
             * Display score, reset board, and enter PointScored state. */
            draw_lcd();
            if (pong_game_over()) {
                pong_init();
            } else {
                pong_reset();
            }
            glb.st = PointScored;
            return;
        }
    }
    if (glb.ticks % 3 == 0) {
        for (uint8_t i = 0; i < WIIMOTE_COUNT; i++) {
            /* Paddle movement removed. */
        }
        pong_draw();
    }
}

/**
 * Container of all tasks. Executed once per main loop iteration.
 */
static void run_tasks(void) {
    cli();
    enum state st = glb.st;
    sei();

    switch (st) {
    case GameRunning:
        cli();
        if (glb.flags & RunLogic) {
            glb.flags &= ~RunLogic;
            sei();
            task_logic();
        }
        break;
    case PointScored:
        break;
    case GamePaused:
        break;
    default:
        assert(0);
    }

    sei();
}

int main(void) {
    init();

    debug(PSTR("AVR Pong starting up...\n"));
    lcd_putstr_P(PSTR("AVR Pong"), 0, 0);
    lcd_putstr_P(PSTR("Starting up..."), 1, 0);

    for (;;) {
        run_tasks();
        sleep_cpu();
    }

    return 0;
}
