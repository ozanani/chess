#include "Game.h"

/*
Sets the game board and the pieces to initial game state.
Assumes gameBoard is not null.
*/
static void setupInitialGameBoard(ChessBoard gameBoard) {
	// set empty cells
	for (int i = 2; i < BOARD_ROWS_NUMBER - 2; i++) {
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			gameBoard[i][j] = BOARD_EMPTY_CELL;
		}
	}

	strncpy(gameBoard[0], BOARD_WHITE_FIRST_ROW_INITAL_PIECES, BOARD_COLUMNS_NUMBER);
	strncpy(gameBoard[1], BOARD_WHITE_SECOND_ROW_INITIAL_PIECES, BOARD_COLUMNS_NUMBER);
	strncpy(gameBoard[BOARD_ROWS_NUMBER - 1], BOARD_BLACK_FIRST_ROW_INITAL_PIECES, BOARD_COLUMNS_NUMBER);
	strncpy(gameBoard[BOARD_ROWS_NUMBER - 2], BOARD_BLACK_SECOND_ROW_INITIAL_PIECES, BOARD_COLUMNS_NUMBER);
}

Game * gameCreate(int historySize) {
	Game *game;

	if (historySize <= 0) return NULL;

	game = malloc(sizeof(Game));
	if (game == NULL) return NULL;

	game->currentPlayer = White;
	game->isBlackKingChecked = false;
	game->isWhiteKingChecked = false;

	// set game board
	setupInitialGameBoard(game->gameBoard);
	
	game->history = arrayListCreate(historySize);

	if (game->history == NULL) {
		free(game);
		return NULL;
	}

	return game;
}

void gameDestroy(Game * game) {
	if (game == NULL) return;

	arrayListDestroy(game->history);
	free(game);
}

bool gameIsSquareValid(BoardSquare s)
{
	return s.row >= 0 && s.col >= 0 && s.row < BOARD_ROWS_NUMBER && s.col < BOARD_COLUMNS_NUMBER;
}

/*
Assumes s is in range.
*/
static bool gameIsSquareEmpty(ChessBoard gameBoard, BoardSquare s) {
	return gameBoard[s.row][s.col] == BOARD_EMPTY_CELL;
}

void gamePrintGameBoardToFileHandler(FILE * fh, Game * game) {
	for (int i = BOARD_ROWS_NUMBER - 1; i >= 0; i--) {
		fprintf(fh, "%d| ", i + 1);
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			fprintf(fh, "%c ", game->gameBoard[i][j]);
		}
		fprintf(fh, "|\n");
	}

	fprintf(fh, "  -----------------\n");
	fprintf(fh, "   A B C D E F G H\n");
}

GAME_MESSAGE gamePrintBoard(Game * game) {
	if (game == NULL) return GAME_INVALID_ARGUMENT;

	gamePrintGameBoardToFileHandler(stdout, game);

	return GAME_SUCCESS;
}

/*
Gets a game board and a PAWN square coordinates, and sets the movesBoard matrix 
with all the possible moves for the pawn.
Assumes all arguments are valid, and movesBoard contains only false.
*/
static void getWhitePawnLegalMoves(ChessBoard gameBoard,
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER], BoardSquare s) {
	
	int row, col;

	// one step forward
	row = s.row + 1, col = s.col;
	if (row < BOARD_ROWS_NUMBER && gameBoard[row][col] == BOARD_EMPTY_CELL) {
		movesBoard[row][col] = true;

		// if the pawn is in the initial place - two steps forward
		row = s.row + 2;
		if (s.row == 1 && gameBoard[row][col] == BOARD_EMPTY_CELL) {
			movesBoard[row][col] = true;
		}
	}

	// capture - diagonal right
	row = s.row + 1, col = s.col + 1;
	if (row < BOARD_ROWS_NUMBER && col < BOARD_COLUMNS_NUMBER &&
		isupper(gameBoard[row][col])) {
		movesBoard[row][col] = true;
	}

	// capture - diagonal left
	row = s.row + 1, col = s.col - 1;
	if (row < BOARD_ROWS_NUMBER && col >= 0 && isupper(gameBoard[row][col])) {
		movesBoard[row][col] = true;
	}
}

/*
Gets a game board and a BISHOP square coordinates, and sets the movesBoard matrix
with all the possible moves for the pawn.
Assumes all arguments are valid, and movesBoard contains only false.
*/
static void getWhiteBishopLegalMoves(ChessBoard gameBoard,
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER], BoardSquare s) {

	int row, col;

	// right, forward diagonal
	for (row = s.row + 1, col = s.col + 1; row < BOARD_ROWS_NUMBER && col < BOARD_COLUMNS_NUMBER; row++, col++) {
		if (gameBoard[row][col] == BOARD_EMPTY_CELL) movesBoard[row][col] = true;

		else {
			if (isupper(gameBoard[row][col])) movesBoard[row][col] = true;

			break;
		}
	}

	// left, forward diagonal
	for (row = s.row + 1, col = s.col - 1; row < BOARD_ROWS_NUMBER && col >= 0; row++, col--) {
		if (gameBoard[row][col] == BOARD_EMPTY_CELL) movesBoard[row][col] = true;

		else {
			if (isupper(gameBoard[row][col])) movesBoard[row][col] = true;

			break;
		}
	}

	// right, backward diagonal
	for (row = s.row - 1, col = s.col + 1; row >= 0 && col < BOARD_COLUMNS_NUMBER; row--, col++) {
		if (gameBoard[row][col] == BOARD_EMPTY_CELL) movesBoard[row][col] = true;

		else {
			if (isupper(gameBoard[row][col])) movesBoard[row][col] = true;

			break;
		}
	}

	// left, backward diagonal
	for (row = s.row - 1, col = s.col - 1; row >= 0 && col >= 0; row--, col--) {
		if (gameBoard[row][col] == BOARD_EMPTY_CELL) movesBoard[row][col] = true;

		else {
			if (isupper(gameBoard[row][col])) movesBoard[row][col] = true;

			break;
		}
	}
}

/*
Gets a game board and a ROOK square coordinates, and sets the movesBoard matrix
with all the possible moves for the pawn.
Assumes all arguments are valid, and movesBoard contains only false.
*/
static void getWhiteRookLegalMoves(ChessBoard gameBoard,
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER], BoardSquare s) {

	int row, col;

	// forward
	for (row = s.row + 1, col = s.col; row < BOARD_ROWS_NUMBER; row++) {
		if (gameBoard[row][col] == BOARD_EMPTY_CELL) movesBoard[row][col] = true;

		else {
			if (isupper(gameBoard[row][col])) movesBoard[row][col] = true;

			break;
		}
	}

	// backward
	for (row = s.row - 1, col = s.col; row >= 0; row--) {
		if (gameBoard[row][col] == BOARD_EMPTY_CELL) movesBoard[row][col] = true;

		else {
			if (isupper(gameBoard[row][col])) movesBoard[row][col] = true;

			break;
		}
	}

	// right
	for (row = s.row, col = s.col + 1; col < BOARD_COLUMNS_NUMBER; col++) {
		if (gameBoard[row][col] == BOARD_EMPTY_CELL) movesBoard[row][col] = true;

		else {
			if (isupper(gameBoard[row][col])) movesBoard[row][col] = true;

			break;
		}
	}

	// left
	for (row = s.row, col = s.col - 1; col >= 0; col--) {
		if (gameBoard[row][col] == BOARD_EMPTY_CELL) movesBoard[row][col] = true;

		else {
			if (isupper(gameBoard[row][col])) movesBoard[row][col] = true;

			break;
		}
	}
}

/*
Gets a game board and a KNIGHT square coordinates, and sets the movesBoard matrix
with all the possible moves for the pawn.
Assumes all arguments are valid, and movesBoard contains only false.
*/
static void getWhiteKnightLegalMoves(ChessBoard gameBoard,
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER], BoardSquare s) {

	int row, col;

	BoardSquare potentialMoves[8] = {
		{ s.row + 1, s.col - 2 }, // 2 left, 1 up
		{ s.row + 2, s.col - 1 }, // 1 left, 2 up
		{ s.row - 1, s.col - 2 }, // 2 left, 1 down
		{ s.row - 2, s.col - 1 }, // 1 left, 2 down
		{ s.row + 1, s.col + 2 }, // 2 right, 1 up
		{ s.row + 2, s.col + 1 }, // 1 right, 2 up
		{ s.row - 1, s.col + 2 }, // 2 right, 1 down
		{ s.row - 2, s.col + 1 }, // 1 right, 2 down
	};

	for (int i = 0; i < 8; i++) {
		row = potentialMoves[i].row, col = potentialMoves[i].col;

		if (row >= 0 && row < BOARD_ROWS_NUMBER && 
			col >= 0 && col < BOARD_COLUMNS_NUMBER && 
			!islower(gameBoard[row][col])) {

			movesBoard[row][col] = true;
		}
	}
}

/*
Gets a game board and a QUEEN square coordinates, and sets the movesBoard matrix
with all the possible moves for the pawn.
Assumes all arguments are valid, and movesBoard contains only false.
*/
static void getWhiteQueenLegalMoves(ChessBoard gameBoard,
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER], BoardSquare s) {

	getWhiteBishopLegalMoves(gameBoard, movesBoard, s);
	getWhiteRookLegalMoves(gameBoard, movesBoard, s);
}

/*
Gets a game board and a KING square coordinates, and sets the movesBoard matrix
with all the possible moves for the pawn.
Assumes all arguments are valid, and movesBoard contains only false.
*/
static void getWhiteKingLegalMoves(ChessBoard gameBoard,
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER], BoardSquare s) {

	int row, col;

	BoardSquare potentialMoves[8] = {
		{ s.row + 1, s.col - 1 },
		{ s.row + 1, s.col }, 
		{ s.row + 1, s.col + 1 },
		{ s.row - 1, s.col - 1 }, 
		{ s.row - 1, s.col },
		{ s.row - 1, s.col + 1 },
		{ s.row, s.col - 1 }, 
		{ s.row, s.col + 1 },
	};

	for (int i = 0; i < 8; i++) {
		row = potentialMoves[i].row, col = potentialMoves[i].col;

		if (row >= 0 && row < BOARD_ROWS_NUMBER &&
			col >= 0 && col < BOARD_COLUMNS_NUMBER &&
			!islower(gameBoard[row][col])) {

			movesBoard[row][col] = true;
		}
	}
}

/*
Updates movesBoard with the legal moves of the piece in square s. Assumes all arguments are valid,
movesBoard is initialized to false and the piece in s is a white piece.
*/
static void getWhitePieceLegalMoves(ChessBoard gameBoard,
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER], BoardSquare s) {

	switch (gameBoard[s.row][s.col]) {
	case PIECE_PAWN:
		getWhitePawnLegalMoves(gameBoard, movesBoard, s);
		break;
	case PIECE_BISHOP:
		getWhiteBishopLegalMoves(gameBoard, movesBoard, s);
		break;
	case PIECE_ROOK:
		getWhiteRookLegalMoves(gameBoard, movesBoard, s);
		break;
	case PIECE_KNIGHT:
		getWhiteKnightLegalMoves(gameBoard, movesBoard, s);
		break;
	case PIECE_QUEEN:
		getWhiteQueenLegalMoves(gameBoard, movesBoard, s);
		break;
	case PIECE_KING:
		getWhiteKingLegalMoves(gameBoard, movesBoard, s);
		break;
	}
}

/*
If c is non-letter, returns c.
*/
static char changeCaseOfChar(char c) {
	if (isupper(c)) return tolower(c);
	if (islower(c)) return toupper(c);

	return c;
}

/*
Does as the name implies. 180 degrees rotation.
The purpose of this function is to enable the use of WHITE-piece functions on BLACK pieces.
*/
static void rotateBoardAndSwitchColors(ChessBoard gameBoard) {
	char temp;

	for (int i = 0; i < BOARD_ROWS_NUMBER / 2; i++) { // divided by 2 to prevent double swapping
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			temp = changeCaseOfChar(gameBoard[i][j]);
			gameBoard[i][j] = changeCaseOfChar(gameBoard[BOARD_ROWS_NUMBER - 1 - i][BOARD_COLUMNS_NUMBER - 1 - j]);
			gameBoard[BOARD_ROWS_NUMBER - 1 - i][BOARD_COLUMNS_NUMBER - 1 - j] = temp;
		}
	}
}

/*
180 degrees rotation for the moves board.
*/
static void rotateMovesBoard(bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER]) {
	bool temp;

	for (int i = 0; i < BOARD_ROWS_NUMBER / 2; i++) { // divided by 2 to prevent double swapping
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			temp = movesBoard[i][j];
			movesBoard[i][j] = movesBoard[BOARD_ROWS_NUMBER - 1 - i][BOARD_COLUMNS_NUMBER - 1 - j];
			movesBoard[BOARD_ROWS_NUMBER - 1 - i][BOARD_COLUMNS_NUMBER - 1 - j] = temp;
		}
	}
}

/*
Returns { -1, -1 } if the black king is not on the game board.
*/
static BoardSquare getBlackKingSquare(ChessBoard gameBoard) {
	BoardSquare s = { -1, -1 };

	for (int i = BOARD_ROWS_NUMBER - 1; i >= 0; i--) { // starts from the end - optimization
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			if (gameBoard[i][j] == toupper(PIECE_KING)) {
				s.row = i;
				s.col = j;
				
				return s;
			}
		}
	}

	return s;
}

/*
Checks if the BLACK king is in CHECK state. 
*/
static bool checkBlackKingCheckState(ChessBoard gameBoard) {
	BoardSquare kingSquare = getBlackKingSquare(gameBoard);
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER] = { {false} };

	for (int i = 0; i < BOARD_ROWS_NUMBER; i++) {
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			if (islower(gameBoard[i][j])) {
				gameGetLegalMoves(gameBoard, movesBoard, (BoardSquare) { i, j });
				if (movesBoard[kingSquare.row][kingSquare.col]) return true;
			}
		}
	}
	
	return false;
}

bool gameBoardKingIsChecked(ChessBoard gameBoard, ChessPlayer player) {
	bool retValue;

	if (player == Black) return checkBlackKingCheckState(gameBoard);

	rotateBoardAndSwitchColors(gameBoard);
	retValue = checkBlackKingCheckState(gameBoard);
	rotateBoardAndSwitchColors(gameBoard);

	return retValue;
}

void gameGetLegalMoves(ChessBoard gameBoard, bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER], BoardSquare s) {

	bool isWhitePiece = islower(gameBoard[s.row][s.col]);
	
	if (!isWhitePiece) {
		rotateBoardAndSwitchColors(gameBoard);
		s.row = BOARD_ROWS_NUMBER - 1 - s.row;
		s.col = BOARD_COLUMNS_NUMBER - 1 - s.col;
	}

	getWhitePieceLegalMoves(gameBoard, movesBoard, s);

	if (!isWhitePiece) {
		rotateBoardAndSwitchColors(gameBoard);
		rotateMovesBoard(movesBoard);
	}
}

void gameChangePlayer(Game * game) {
	if (game->currentPlayer == White) game->currentPlayer = Black;
	else game->currentPlayer = White;
}

void gameForceSetMove(Game * game, BoardSquare from, BoardSquare to) {	
	HistoryElement histElement = { from, to, game->gameBoard[to.row][to.col],
		game->isWhiteKingChecked, game->isBlackKingChecked };

	// add to history
	arrayListAddLast(game->history, histElement);

	// move!
	game->gameBoard[to.row][to.col] = game->gameBoard[from.row][from.col];
	game->gameBoard[from.row][from.col] = BOARD_EMPTY_CELL;

	// change player
	gameChangePlayer(game);

	// update check status
	game->isBlackKingChecked = gameBoardKingIsChecked(game->gameBoard, Black);
	game->isWhiteKingChecked = gameBoardKingIsChecked(game->gameBoard, White);
}

/*
This is a PRIVATE method that is used to undo a move without any checks.
THIS FUNCTION SHOULD BE WRAPPED BEFORE USING IN GAME.
*/
static void gameForceUndoPrevMove(Game * game) {
	HistoryElement histElement = arrayListGetLast(game->history);
	arrayListRemoveLast(game->history);

	// move piece back
	game->gameBoard[histElement.oldSquare.row][histElement.oldSquare.col] = game->gameBoard[histElement.newSquare.row][histElement.newSquare.col];

	// restore element
	game->gameBoard[histElement.newSquare.row][histElement.newSquare.col] = histElement.prevElementOnNewCell;

	// change player
	gameChangePlayer(game);

	// restore check status
	game->isBlackKingChecked = histElement.isBlackKingChecked;
	game->isWhiteKingChecked = histElement.isWhiteKingChecked;
}

bool gameIsCurrentPlayerChecked(Game * game) {
	if (gameGetCurrentPlayer(game) == White) {
		return game->isWhiteKingChecked;
	}

	return game->isBlackKingChecked;
}

/*
Checks if a black piece is threatened by a white piece.
*/
static bool gameIsBlackPieceThreatened(ChessBoard gameBoard, BoardSquare s) {
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER] = { {false} };

	for (int i = 0; i < BOARD_ROWS_NUMBER; i++) {
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			if (islower(gameBoard[i][j])) {
				gameGetLegalMoves(gameBoard, movesBoard, (BoardSquare) { i, j });
				if (movesBoard[s.row][s.col]) return true;
			}
		}
	}

	return false;
}

/*
Checks if the piece in square s is threatened.
*/
static bool gameIsPieceThreatened(ChessBoard gameBoard, BoardSquare s) {
	bool retValue;
	
	// check if the piece is black
	if (isupper(gameBoard[s.row][s.col])) return gameIsBlackPieceThreatened(gameBoard, s);

	// else - rotate
	rotateBoardAndSwitchColors(gameBoard);
	retValue = gameIsBlackPieceThreatened(gameBoard, 
		(BoardSquare) { BOARD_ROWS_NUMBER - 1 - s.row, BOARD_COLUMNS_NUMBER - 1 - s.col });
	rotateBoardAndSwitchColors(gameBoard);

	return retValue;
}

/*
This function determines whether the move creates a king threat (i.e the king of the player that made
the move is threatened) or piece threat (i.e the piece can be captured after this move).

It sets the given boolean pointers.
These 2 checks are not seperate for optimization.

@param game - the game
@param from - the piece square
@param to - the destination square
@param movesCreatesKingThreat - pointer to the king threat boolean indicator 
@param moveCreatesPieceThreat - pointer to the piece threat boolean indicator 

*/
static void gameDoesMoveCreateKingThreatOrPieceThreat(Game * game, BoardSquare from, BoardSquare to, 
	bool * moveCreatesKingThreat, bool * moveCreatesPieceThreat) {
	bool isWhitePiece = islower(game->gameBoard[from.row][from.col]);

	gameForceSetMove(game, from, to);

	*moveCreatesKingThreat = isWhitePiece ? game->isWhiteKingChecked : game->isBlackKingChecked;
	*moveCreatesPieceThreat = gameIsPieceThreatened(game->gameBoard, to);

	gameForceUndoPrevMove(game);
}


void gameGetMovesByTypes(Game * game, bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER],
	MovesBoardWithTypes movesBoardWithTypes, BoardSquare s) {
	bool moveCaptures, moveCreatesPieceThreat = false, moveCreatesKingThreat = false;

	for (int i = 0; i < BOARD_ROWS_NUMBER; i++) {
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			if (!movesBoard[i][j]) movesBoardWithTypes[i][j] = BoardSquareInvalidMove;
			
			// a valid move
			else {
				// boolean indicators
				moveCaptures = game->gameBoard[i][j] != BOARD_EMPTY_CELL;
				gameDoesMoveCreateKingThreatOrPieceThreat(game, s, (BoardSquare) { i, j }, 
					&moveCreatesKingThreat, &moveCreatesPieceThreat);

				// if the king is threatened, the move is illegal
				if (moveCreatesKingThreat) {
					movesBoardWithTypes[i][j] = BoardSquareKingThreatMove;
					continue;
				}

				if (moveCaptures && moveCreatesPieceThreat) movesBoardWithTypes[i][j] = BoardSquareCaptureAndThreatMove;
				else if (moveCaptures) movesBoardWithTypes[i][j] = BoardSquareCaptureMove;
				else if (moveCreatesPieceThreat) movesBoardWithTypes[i][j] = BoardSquareThreatMove;
				else movesBoardWithTypes[i][j] = BoardSquareValidMove;
			}
		}
	}
}

ChessPlayer gameGetCurrentPlayer(Game * game) {
	if (game == NULL) return White;

	return game->currentPlayer;
}

ChessPlayer gameGetOtherPlayer(Game * game) {
	if (game == NULL) return White;

	if (game->currentPlayer == White) return Black;
	return White;
}

/*
Checks if the given piece is a piece of the current player. 
*/
bool gameIsPieceOfCurrentPlayer(Game * game, BoardSquare s) {
	if (gameGetCurrentPlayer(game) == White) return islower(game->gameBoard[s.row][s.col]);

	return isupper(game->gameBoard[s.row][s.col]);
}

/*
Like get moves by types, but doesn't require a movesBoard as an argument.
*/
static void gameGetMovesByTypesWrapper(Game * game, MovesBoardWithTypes movesBoardWithTypes, BoardSquare s) {
	bool movesBoard[BOARD_ROWS_NUMBER][BOARD_COLUMNS_NUMBER] = { {false} };

	gameGetLegalMoves(game->gameBoard, movesBoard, s);
	gameGetMovesByTypes(game, movesBoard, movesBoardWithTypes, s);
}

GAME_MESSAGE gameSetMove(Game * game, BoardSquare from, BoardSquare to)
{
	MovesBoardWithTypes movesBoardWithTypes = { {BoardSquareInvalidMove} };
	BoardSquareMoveType moveType;

	if (game == NULL) return GAME_INVALID_ARGUMENT;
	if (!gameIsSquareValid(from) || !gameIsSquareValid(to)) return GAME_INVALID_SQUARE;
	if (!gameIsPieceOfCurrentPlayer(game, from)) return GAME_INVALID_PIECE;

	gameGetMovesByTypesWrapper(game, movesBoardWithTypes, from);

	moveType = movesBoardWithTypes[to.row][to.col];

	switch (moveType)
	{
	case BoardSquareValidMove:
	case BoardSquareThreatMove:
		gameForceSetMove(game, from, to);
		return GAME_MOVE_SUCCESS;
	case BoardSquareCaptureMove:
	case BoardSquareCaptureAndThreatMove:
		gameForceSetMove(game, from, to);
		return GAME_MOVE_SUCCESS_CAPTURE;
	case BoardSquareInvalidMove:
		return GAME_MOVE_INVALID;
	case BoardSquareKingThreatMove:
		return GAME_MOVE_FAILED_KING_THREAT;
	default:
		break;
	}

	return GAME_MOVE_INVALID; // just for compilation
}

GAME_MESSAGE gameUndoPrevMove(Game * game) {
	if (game == NULL) return GAME_INVALID_ARGUMENT;

	if (arrayListIsEmpty(game->history)) return GAME_UNDO_NO_HISTORY;

	gameForceUndoPrevMove(game);
	return GAME_UNDO_SUCCESS;
}

/*
Switch the player colors. Rotates the board and changes the current player.
*/
static void gameSwitchPlayersColors(Game * game) {
	rotateBoardAndSwitchColors(game->gameBoard);
	gameChangePlayer(game);
}

/*
Returns true iff the piece on square s has a valid move (a move that creates a threat 
on the current player is not valid).
*/
static bool gamePieceHasNextMove(Game * game, BoardSquare s) {
	MovesBoardWithTypes movesBoardWithTypes = { {BoardSquareInvalidMove} };

	gameGetMovesByTypesWrapper(game, movesBoardWithTypes, s);

	for (int i = 0; i < BOARD_ROWS_NUMBER; i++) {
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			if (gameIsValidMove(movesBoardWithTypes[i][j])) return true;
		}
	}

	return false;
}

/*
True iff the current player has next move to do.
*/
static bool gameCurrentPlayerHasValidMoves(Game * game) {
	// work only on white players.
	bool switchPlayers = (gameGetCurrentPlayer(game) == Black);
	bool hasNextMove = false;
	
	if (switchPlayers) gameSwitchPlayersColors(game);

	for (int i = 0; i < BOARD_ROWS_NUMBER && !hasNextMove; i++) {
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			if (islower(game->gameBoard[i][j])) {
				if (gamePieceHasNextMove(game, (BoardSquare) { i, j })) {
					hasNextMove = true;
					break;
				}
			}
		}
	}

	if (switchPlayers) gameSwitchPlayersColors(game);

	return hasNextMove;
}

GAME_CHECK_WINNER_MESSAGE gameCheckWinner(Game * game) {
	if (gameCurrentPlayerHasValidMoves(game)) return GAME_CHECK_WINNER_CONTINUE;
	
	if (gameIsCurrentPlayerChecked(game)) return GAME_CHECK_WINNER_CURRENT_PLAYER_LOSE;

	return GAME_CHECK_WINNER_DRAW;
}

GAME_MESSAGE gameGetMovesWrapper(Game * game, BoardSquare s, MovesBoardWithTypes movesBoardWithTypes) {
	if (!gameIsSquareValid(s)) return GAME_INVALID_SQUARE;
	if (gameIsSquareEmpty(game->gameBoard, s)) return GAME_INVALID_PIECE;

	gameGetMovesByTypesWrapper(game, movesBoardWithTypes, s);
	return GAME_SUCCESS;
}

char * gameGetCurrentPlayerColorText(Game * game) {
	if (gameGetCurrentPlayer(game) == White) return "white";

	return "black";
}

bool gameIsValidMove(BoardSquareMoveType moveType) {
	return !(moveType == BoardSquareInvalidMove || moveType == BoardSquareKingThreatMove);
}