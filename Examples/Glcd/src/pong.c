#include <avr/pgmspace.h>

#include "uart_streams.h"
#include <assert.h>
#include <stdio.h>

#include "glcd.h"
#include "glcd_hal.h"
#include "pong.h"
#include "common.h"

#define WIDTH (128)
#define HEIGHT (64)
#define FRAME_HEIGHT (1)
#define PADDLE_HEIGHT (10)
#define MAX_POINTS (5)

struct pong_state_t {
    /* Paddle positions. */
    uint8_t lpady, rpady;

    /* Paddle positions at last rendering pass. */
    uint8_t _lpady, _rpady;

    /* Ball position. */
    uint8_t x, y;

    /* Ball position at last rendering pass. */
    uint8_t _x, _y;

    /* Ball velocity. */
    int8_t dx, dy;

    /* Scores. */
    uint8_t p1, p2;

    /* true if the screen should be cleared before rendering. */
    bool clrscr;
};

static struct pong_state_t state;

void pong_print(void) {
    debug(PSTR("x: %d y: %d\n"), state.x, state.y);
}

void pong_move(uint8_t player, enum direction dir __attribute ((unused))) {
    assert(player < 2);
    uint8_t *pady = NULL;

    switch (player) {
    case 0:
        pady = &state.lpady;
        break;
    case 1:
        pady = &state.rpady;
        break;
    default:
        assert(0);
    }

    switch (dir) {
    case Up:
        if (*pady == 0) {
            break;
        }
        (*pady)--;
        break;
    case Down:
        if (*pady + PADDLE_HEIGHT == HEIGHT - 1) {
            break;
        }
        (*pady)++;
        break;
    default:
        assert(0);
    }
}

void pong_draw(void) {
    /* Run all tasks? */
    bool all = false;

    /* Clear screen if requested. */
    if (state.clrscr) {
        glcdFillScreen(0x00);
        state.clrscr = false;
        all = true;
    }

    /* Draw paddles if positions have changed. */
    if (all || state._lpady != state.lpady) {
        xy_point _padlt = { 0, state._lpady },
                 _padlb = { 0, state._lpady + PADDLE_HEIGHT };

        /* Clear previous paddles. */
        glcdDrawLine(_padlt, _padlb, glcdClearPixel);

        xy_point padlt = { 0, state.lpady },
                 padlb = { 0, state.lpady + PADDLE_HEIGHT };

        /* Draw paddles. */
        glcdDrawLine(padlt, padlb, glcdSetPixel);

        state._lpady = state.lpady;
    }

    if (all || state._rpady != state.rpady) {
        xy_point _padrt = { WIDTH - 1, state._rpady },
                 _padrb = { WIDTH - 1, state._rpady + PADDLE_HEIGHT };

        /* Clear previous paddles. */
        glcdDrawLine(_padrt, _padrb, glcdClearPixel);

        xy_point padrt = { WIDTH - 1, state.rpady },
                 padrb = { WIDTH - 1, state.rpady + PADDLE_HEIGHT };

        /* Draw paddles. */
        glcdDrawLine(padrt, padrb, glcdSetPixel);

        state._rpady = state.rpady;
    }

    /* Draw ball if position has changed. */
    if (all || state._x != state.x || state._y != state.y) {
        /* Clear previous ball, but make sure not to mess with rendered
         * paddles. */
        if (state._x > 0 && state._x < WIDTH - 1) {
            glcdClearPixel(state._x, state._y);
            glcdClearPixel(state._x, state._y + 1);
        }
        if (state._x + 1 > 0 && state._x + 1 < WIDTH - 1) {
            glcdClearPixel(state._x + 1, state._y);
            glcdClearPixel(state._x + 1, state._y + 1);
        }

        /* Draw ball. */
        glcdSetPixel(state.x, state.y);
        glcdSetPixel(state.x + 1, state.y);
        glcdSetPixel(state.x, state.y + 1);
        glcdSetPixel(state.x + 1, state.y + 1);

        state._x = state.x;
        state._y = state.y;
    }
}

void pong_reset(void) {
    state.lpady = state.rpady = (HEIGHT - 2 * FRAME_HEIGHT) / 2;

    state.x = 1;
    state.y = state.lpady + PADDLE_HEIGHT / 2;
    state.dx = 5;
    state.dy = 0;

    state.clrscr = true;
}

void pong_init(void) {
    pong_reset();

    state.p1 = state.p2 = 0;
}

void pong_scores(uint8_t *p1, uint8_t *p2) {
    *p1 = state.p1;
    *p2 = state.p2;
}

bool pong_game_over(void) {
    return state.p1 == MAX_POINTS || state.p2 == MAX_POINTS;
}

/**
 * Returns true if the paddle was hit.
 * @param pady The position of the paddle.
 * @param y The current position of the ball.
 * @param nexty The next position of the ball.
 */
static bool _hit_paddle(uint8_t pady, uint8_t y, uint8_t nexty) {
    uint8_t miny = (y < nexty) ? y : nexty;
    uint8_t maxy = (y < nexty) ? nexty : y;

    return (miny <= pady && maxy >= pady)
        || (miny <= pady + PADDLE_HEIGHT && maxy >= pady + PADDLE_HEIGHT)
        || (miny >= pady && maxy <= pady + PADDLE_HEIGHT);
}

/**
 * Alters ball velocity depending on where the paddle was struck.
 */
static void _alter_velocity(uint8_t pady) {
    state.dy += (state.y - pady - PADDLE_HEIGHT / 2) / 2;
}

#define YLIMIT (10)
bool pong_ball_step(void) {
    assert(state.x < WIDTH);
    assert(state.y < HEIGHT);

    int16_t nextx = state.x + state.dx;
    int16_t nexty = state.y + state.dy;

    if (nexty <= 0 + FRAME_HEIGHT) {
        state.dy = - state.dy;
        nexty = FRAME_HEIGHT - nexty;
    } else if (nexty >= HEIGHT - FRAME_HEIGHT - 1) {
        state.dy = - state.dy;
        nexty = 2 * (HEIGHT - FRAME_HEIGHT - 1) - nexty;
    }

    /* At left or right edge of board. */
    if (nextx <= 0) {
        state.dx = - state.dx;
        nextx = - nextx;
        if (!_hit_paddle(state.lpady, state.y, nexty)) {
            /* P2 scored. */
            state.p2++;
            return true;
        }
        /* Hit paddle, alter velocity. */
        _alter_velocity(state.lpady);
    } else if (nextx >= WIDTH - 1) {
        state.dx = - state.dx;
        nextx = 2 * (WIDTH - 1) - nextx;
        if (!_hit_paddle(state.rpady, state.y, nexty)) {
            /* P1 scored. */
            state.p1++;
            return true;
        }
        /* Hit paddle, alter velocity. */
        _alter_velocity(state.rpady);
    }

    if (state.dy < -YLIMIT) {
        state.dy = -YLIMIT;
    } else if (state.dy > YLIMIT) {
        state.dy = YLIMIT;
    }

    state.x = nextx;
    state.y = nexty;

    return false;
}
