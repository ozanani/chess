#ifndef MINIMAX_H
#define MINIMAX_H

#include <limits.h>
#include "Game.h"

/*
This module handle a move suggestion, using the minimax algorithm.
*/

typedef struct move_t {
	BoardSquare oldSquare;
	BoardSquare newSquare;
} Move;

typedef struct move_and_value_t {
	Move move;
	int value;
} MoveAndValue;

/*
Suggest a move to the current player.
@param level the game level (minimax depth)
@param game the game
@return 
the suggest move
*/
Move minimaxSuggestMove(Game * game, int level);

#endif