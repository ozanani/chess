#ifndef GAME_H_
#define GAME_H_

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "ArrayList.h"
#include "ChessGlobalDefinitions.h"

/*
Game Summary:
This module is an encapsulation of the game. It contains the gameboard, history,
current player, and boolean indicators regarding whether the king of each player is threatened.

A game should be handled with the GameHandler module, which wraps it with the settings and handles 
the game flow. 
*/

#define BOARD_ROWS_NUMBER 8
#define BOARD_COLUMNS_NUMBER 8
#define BOARD_EMPTY_CELL '_'

// Pieces defintions - lowercase
#define PIECE_PAWN 'm'
#define PIECE_BISHOP 'b'
#define PIECE_ROOK 'r'
#define PIECE_KNIGHT 'n'
#define PIECE_QUEEN 'q'
#define PIECE_KING 'k'

#define BOARD_WHITE_FIRST_ROW_INITAL_PIECES "rnbqkbnr"
#define BOARD_WHITE_SECOND_ROW_INITIAL_PIECES "mmmmmmmm"
#define BOARD_BLACK_FIRST_ROW_INITAL_PIECES "RNBQKBNR"
#define BOARD_BLACK_SECOND_ROW_INITIAL_PIECES "MMMMMMMM"

/*
An enum for the current player.
*/
typedef enum chess_player_e {
	White,
	Black
} ChessPlayer;

typedef char ChessBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER];

typedef enum square_type_e {
	BoardSquareInvalidMove,
	BoardSquareValidMove,
	BoardSquareCaptureMove,
	BoardSquareThreatMove,
	BoardSquareCaptureAndThreatMove,
	BoardSquareKingThreatMove
} BoardSquareMoveType;

typedef BoardSquareMoveType MovesBoardWithTypes[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER];

typedef struct game_t {
	ChessBoard gameBoard;
	ChessPlayer currentPlayer;
	bool isBlackKingChecked;
	bool isWhiteKingChecked;
	ArrayList * history;
} Game;

/**
* Type used for returning error codes from game functions.
*/
typedef enum game_message_t {
	GAME_MOVE_INVALID,
	GAME_MOVE_SUCCESS,
	GAME_MOVE_FAILED_KING_THREAT,
	GAME_MOVE_SUCCESS_CAPTURE,
	GAME_MOVE_FAILED_THREAT_CAPTURE,
	GAME_INVALID_SQUARE,
	GAME_INVALID_PIECE,
	GAME_INVALID_ARGUMENT,
	GAME_UNDO_NO_HISTORY,
	GAME_UNDO_SUCCESS,
	GAME_SUCCESS
} GAME_MESSAGE;

/*
Check winner function return messages.
*/
typedef enum game_check_winner_message_t {
	GAME_CHECK_WINNER_CURRENT_PLAYER_LOSE,
	GAME_CHECK_WINNER_DRAW,
	GAME_CHECK_WINNER_CONTINUE
} GAME_CHECK_WINNER_MESSAGE;

/*
Creates a game instance, with the initial Checkmate game state.
@param historySize - the size of game history to be saved
@return the game instance, or NULL if malloc failed
*/
Game * gameCreate(int historySize);
	
/*
Destroys the game instance and frees all memory.
@param game - the game instance
*/
void gameDestroy(Game * game);

/*
Returns true iff the square s is on the board.
@param s the square
@return true iff the square s is on the board.
*/
bool gameIsSquareValid(BoardSquare s);

/*
Moves the piece from square FROM to sqaure TO. Checks if the move is valid and also checks the parameters.
@param game - the game instance
@param from - the start square
@param to - the to square
@return - one of the following, according to the move type
GAME_MOVE_SUCCESS
GAME_MOVE_SUCCESS_CAPTURE
GAME_MOVE_INVALID
GAME_MOVE_FAILED_KING_THREAT
GAME_MOVE_FAILED_THREAT_CAPTURE
*/
GAME_MESSAGE gameSetMove(Game * game, BoardSquare from, BoardSquare to);

/*
Sets a move with without performing any checks and even if the move causes a threat.
Saves the move to history.
THIS FUNCTION SHOULD BE WRAPPED BEFORE USING IN GAME, AND IS PUBLIC ONLY FOR 
OPTIMIZATION OF THE MINIMAX ALGORITHM.
@param game the game
@param from the source square
@param to the destination square
*/
void gameForceSetMove(Game * game, BoardSquare from, BoardSquare to);

/*
Undo the previous move.
@return - one of the following messages (self explanatory)
GAME_UNDO_NO_HISTORY
GAME_UNDO_SUCCESS
GAME_INVALID_ARGUMENT
*/
GAME_MESSAGE gameUndoPrevMove(Game * game);

/*
Prints the game board. 

@param game - the game to print
@return
GAME_SUCCESS on success
GAME_INVALID_ARGUMENT if game is null
*/
GAME_MESSAGE gamePrintBoard(Game * game);

/*
A wrapper that calls gameGetLegalMoves and then gameGetMovesByTypes. 
Checks parameters.
@param game the game
@param s the square to check
@param movesBoardWithTypes the board to fill with the correct move types
@return one of the following messages:
GAME_INVALID_SQUARE
GAME_INVALID_PIECE
GAME_SUCCESS
*/
GAME_MESSAGE gameGetMovesWrapper(Game * game, BoardSquare s, MovesBoardWithTypes movesBoardWithTypes);

/*
Updates movesBoard with the legal moves of the piece in square s. Assumes all arguments are valid
and movesBoard is initialized to false. Doesn't check threats.
@param gameBoard the game board
@param movesBoard a boolean board represents legal moves
@param s the square of the piece
*/
void gameGetLegalMoves(ChessBoard gameBoard,
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER], BoardSquare s);

/*
Gets a moves board with all the legal moves of the piece in square s, and sets the moves types
in the movesBoardWithTypes board.
@param game the game
@param movesBoard a boolean board represents legal moves
@param movesBoardWithTypes the board to fill with the correct move types
@param s the square of the piece
*/
void gameGetMovesByTypes(Game * game, bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER],
	MovesBoardWithTypes movesBoardWithTypes, BoardSquare s);

/*
Determines whether the king of the given player is in check state.
Checks the game board and not the game states.
@param gameBoard - the game board
@param player - the player that the king belongs to
*/
bool gameBoardKingIsChecked(ChessBoard gameBoard, ChessPlayer player);

/*
Gets the current player.
If game is null, return value is undefined.
@param game the game
@return the current player
*/
ChessPlayer gameGetCurrentPlayer(Game * game);

/*
Gets the other player - the player that isn't playing right now.
If game is null, return value is undefined.
@param game the game
@return the other player
*/
ChessPlayer gameGetOtherPlayer(Game * game);

/*
Checks if the current player is checked.
@param game the game
@return true iff the current player is checked
*/
bool gameIsCurrentPlayerChecked(Game * game);

/*
Check if there is a winner or it's a draw (i.e, checks if the current player is losing or 
has no more moves but is not threatened).
@param game the game instance
@return one of the following messages:
GAME_CHECK_WINNER_CONTINUE
GAME_CHECK_WINNER_CURRENT_PLAYER_LOSE
GAME_CHECK_WINNER_DRAW
*/
GAME_CHECK_WINNER_MESSAGE gameCheckWinner(Game * game);

/*
Checks if the given piece is a piece of the current player.
@param game the game
@param s the square of the piece
@returns true iff the given piece is a piece of the current player
*/
bool gameIsPieceOfCurrentPlayer(Game * game, BoardSquare s);

/*
Gets the TEXT of the current player (i.e white or black).
@param game the game
@return
"white" if the current player is the white player
"black" if the current player is the black player
*/
char * gameGetCurrentPlayerColorText(Game * game);

/*
Changes the current player.
@param game the game
*/
void gameChangePlayer(Game * game);

/*
Return true iff the move is valid (i.e, doesn't creates a king's threat
or invalid according to the rules.
@param moveType the move type
@return true iff the move is valid
*/
bool gameIsValidMove(BoardSquareMoveType moveType);

/*
Prints the game board to the given file.
@param fh the file handler (like stdout)
@param game the game
*/
void gamePrintGameBoardToFileHandler(FILE * fh, Game * game);

#endif