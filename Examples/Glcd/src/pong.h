#ifndef PONG_H
#define PONG_H

#include <stdbool.h>

/**
 * @file pong.h
 *
 * Keeps track of and handles game state.
 */

/**
 * Paddle movement directions.
 */
enum direction {
    Up,
    Down,
};

void pong_print(void);

/**
 * Initializes the game state for a new game.
 */
void pong_init(void);

/**
 * Resets the board to initial state for next point.
 * Resets paddle, ball positions, but does not reset scores.
 */
void pong_reset(void);

/**
 * Performs ball movement.
 * @return Returns true if a player has scored, false otherwise.
 */
bool pong_ball_step(void);

/**
 * Writes current scores into p1 and p2.
 */
void pong_scores(uint8_t *p1, uint8_t *p2);

/**
 * Returns true if either player has reached enough points to win the game.
 */
bool pong_game_over(void);

/**
 * Moves the paddle belonging to player in direction dir.
 */
void pong_move(uint8_t player, enum direction dir);

/**
 * Draws the current state to the GLCD.
 * Uses difference drawing to save processing time.
 */
void pong_draw(void);

#endif /* PONG_H */
