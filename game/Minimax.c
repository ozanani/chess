#include "Minimax.h"
#include <string.h>

/*
Gets the abuslote score of a piece.
Assume piece is valid (i.e. one of the valid pieces).
*/
static int minimaxGetAbsuloteScoreOfPiece(char piece) {
	switch (tolower(piece)) {
	case PIECE_PAWN:
		return 1;
	case PIECE_KNIGHT:
		return 3;
	case PIECE_BISHOP:
		return 3;
	case PIECE_ROOK:
		return 5;
	case PIECE_QUEEN:
		return 9;
	case PIECE_KING:
		return 100;
	}

	return 0;
}

static int minimaxScoringFunction(ChessBoard gameBoard, ChessPlayer positivePlayer) {
	char currentPiece;
	int whiteScore = 0, blackScore = 0;

	for (int i = 0; i < BOARD_ROWS_NUMBER; i++) {
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			currentPiece = gameBoard[i][j];

			if (isupper(currentPiece)) blackScore += minimaxGetAbsuloteScoreOfPiece(currentPiece);
			else if (islower(currentPiece)) whiteScore += minimaxGetAbsuloteScoreOfPiece(currentPiece);
		}
	}

	if (positivePlayer == White) return whiteScore - blackScore;
	return blackScore - whiteScore;
}

/*
Implementation of the alphabeta pruning minimax algorithm.
See more at https://en.wikipedia.org/wiki/Alpha%E2%80%93beta_pruning
*/
static MoveAndValue minimaxAlphabetaPruning(Game * game, int depth, int alpha, int beta, bool maximizingPlayer) {
	GAME_CHECK_WINNER_MESSAGE checkWinnerMsg = gameCheckWinner(game);
	MovesBoardWithTypes movesBoardWithTypes = { {BoardSquareInvalidMove} };
	BoardSquare currentSquare, destSquare;
	MoveAndValue currentMV = { .value = -1 }, nextMV;

	// check if game has ended. In this case, the move itself doesn't matter
	// - as the move will always be updated in the parent "virtual node"
	if (checkWinnerMsg == GAME_CHECK_WINNER_CURRENT_PLAYER_LOSE) {
		if (maximizingPlayer) currentMV.value = -1000;
		else currentMV.value = 1000;

		return currentMV;
	}
	if (checkWinnerMsg == GAME_CHECK_WINNER_DRAW) {
		currentMV.value = 0;
		return currentMV;
	}

	if (depth == 0) {
		currentMV.value = minimaxScoringFunction(game->gameBoard, maximizingPlayer ? gameGetCurrentPlayer(game) : gameGetOtherPlayer(game));
		return currentMV;
	}

	if (maximizingPlayer) {
		currentMV.value = INT_MIN;

		// go over the pieces
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			for (int i = 0; i < BOARD_ROWS_NUMBER; i++) {
				currentSquare = (BoardSquare) { i, j };

				// if current player piece - check the moves
				if (gameIsPieceOfCurrentPlayer(game, currentSquare)) {
					gameGetMovesWrapper(game, currentSquare, movesBoardWithTypes);

					for (int l = 0; l < BOARD_COLUMNS_NUMBER; l++) {
						for (int k = 0; k < BOARD_ROWS_NUMBER; k++) {

							// check if move is valid
							if (gameIsValidMove(movesBoardWithTypes[k][l])) {
								destSquare = (BoardSquare) { k, l };

								// move, check value and undo move

								// we use gameForceSetMove for optimization: 
								// all the checks that are made in gameSetMove are performed here
								gameForceSetMove(game, currentSquare, destSquare);

								// take the maximum
								nextMV = minimaxAlphabetaPruning(game, depth - 1, alpha, beta, false);
								if (nextMV.value > currentMV.value) {
									currentMV.value = nextMV.value;
									currentMV.move.oldSquare = currentSquare;
									currentMV.move.newSquare = destSquare;
								}

								gameUndoPrevMove(game);

								if (currentMV.value > alpha) alpha = currentMV.value;

								if (beta <= alpha) return currentMV;
							}
						}
					}
				}
			}
		}

		return currentMV;
	}

	else {
		currentMV.value = INT_MAX;

		// go over the pieces
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			for (int i = 0; i < BOARD_ROWS_NUMBER; i++) {
				currentSquare = (BoardSquare) { i, j };

				// if current player piece - check the moves
				if (gameIsPieceOfCurrentPlayer(game, currentSquare)) {
					gameGetMovesWrapper(game, currentSquare, movesBoardWithTypes);

					for (int l = 0; l < BOARD_COLUMNS_NUMBER; l++) {
						for (int k = 0; k < BOARD_ROWS_NUMBER; k++) {

							// check if move is valid
							if (gameIsValidMove(movesBoardWithTypes[k][l])) {
								destSquare = (BoardSquare) { k, l };

								// we use gameForceSetMove for optimization: 
								// all the checks that are made in gameSetMove are performed here
								gameForceSetMove(game, currentSquare, destSquare);

								// take the minimum
								nextMV = minimaxAlphabetaPruning(game, depth - 1, alpha, beta, true);
								if (nextMV.value < currentMV.value) {
									currentMV.value = nextMV.value;
									currentMV.move.oldSquare = currentSquare;
									currentMV.move.newSquare = destSquare;
								}

								gameUndoPrevMove(game);

								if (currentMV.value < beta) beta = currentMV.value;

								if (beta <= alpha) return currentMV;
							}
						}
					}
				}
			}
		}

		return currentMV;
	}
}

Move minimaxSuggestMove(Game * game, int level) {
	return minimaxAlphabetaPruning(game, level, INT_MIN, INT_MAX, true).move;
}