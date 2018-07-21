#include "GuiGameBoard.h"
#include <stdio.h>
#include <stdlib.h>

/*
Returns true iff the game has ended, and sets the appropriate property.
Displays a msg regarding the end of the game.
*/
static bool guiGameBoardHasGameEnded(GuiGameBoard * gameBoard) {
	switch (gameCheckWinner(gameBoard->gh->game)) {
	case GAME_CHECK_WINNER_CONTINUE:
		gameBoard->gameHasEnded = false; // required for case of undo when the game is over
		return false;
	case GAME_CHECK_WINNER_DRAW:
		// we use user event in order for the msg to be shown after the updated window is presented
		guiPushUserEvent(GUI_USEREVENT_MSGBOX, "Game Over", "The game ends in a draw.");
		gameBoard->gameHasEnded = true;
		return true;
	case GAME_CHECK_WINNER_CURRENT_PLAYER_LOSE:
		// the winner is NOT the current player
		if (gameGetCurrentPlayer(gameBoard->gh->game) == White) {
			guiPushUserEvent(GUI_USEREVENT_MSGBOX, "Game Over", "Checkmate! black player wins the game.");
		}
		else {
			guiPushUserEvent(GUI_USEREVENT_MSGBOX, "Game Over", "Checkmate! white player wins the game.");
		}
		gameBoard->gameHasEnded = true;
		return true;
	}

	return false; // for compilation
}

/*
This function has to be called whenever the game STATE has changed, i.e
the chess board.
*/
static void gameBoardChanged(GuiGameBoard * gameBoard) {
	// disable current get moves and chosen square
	gameBoard->squareChosen.row = -1;
	gameBoard->squareGetMoves.row = -1;

	gameBoard->gh->gameIsSaved = false;

	if (guiGameBoardHasGameEnded(gameBoard)) return;

	// no winner - inidicate if a player is checked
	if (gameIsCurrentPlayerChecked(gameBoard->gh->game)) {
		if (gameGetCurrentPlayer(gameBoard->gh->game) == White) {
			guiPushUserEvent(GUI_USEREVENT_MSGBOX, "Check", "White king is threatened.");
		}
		else {
			guiPushUserEvent(GUI_USEREVENT_MSGBOX, "Check", "Black king is threatened.");
		}
	}

	// check if it's computer move
	if (!gameHandlerIsUserTurn(gameBoard->gh)) guiPushUserEvent(GUI_USEREVENT_COMPUTER_TURN, NULL, NULL);
}

GuiWidget* createGameBoard(SDL_Renderer* renderer, SDL_Rect location,
	void(*action)(void), GameHandler * gh)
{
	// allocate data
	GuiWidget* res = (GuiWidget*)malloc(sizeof(GuiWidget));
	if (res == NULL || gh == NULL)
		return NULL;

	GuiGameBoard* data = (GuiGameBoard*)malloc(sizeof(GuiGameBoard));
	if (data == NULL) {
		free(res);
		return NULL;
	}

	// create pieces
	data->whitePawnTexture = guiTransparentTextureFromBMP(renderer, "./img/piece_mw.bmp", 159, 176, 143);
	data->whiteKnightTexture = guiTransparentTextureFromBMP(renderer, "./img/piece_nw.bmp", 159, 176, 143);
	data->whiteRookTexture = guiTransparentTextureFromBMP(renderer, "./img/piece_rw.bmp", 159, 176, 143);
	data->whiteBishopTexture = guiTransparentTextureFromBMP(renderer, "./img/piece_bw.bmp", 159, 176, 143);
	data->whiteQueenTexture = guiTransparentTextureFromBMP(renderer, "./img/piece_qw.bmp", 159, 176, 143);
	data->whiteKingTexture = guiTransparentTextureFromBMP(renderer, "./img/piece_kw.bmp", 159, 176, 143);
	data->blackPawnTexture = guiTransparentTextureFromBMP(renderer, "./img/piece_mb.bmp", 159, 176, 143);
	data->blackKnightTexture = guiTransparentTextureFromBMP(renderer, "./img/piece_nb.bmp", 159, 176, 143);
	data->blackRookTexture = guiTransparentTextureFromBMP(renderer, "./img/piece_rb.bmp", 159, 176, 143);
	data->blackBishopTexture = guiTransparentTextureFromBMP(renderer, "./img/piece_bb.bmp", 159, 176, 143);
	data->blackQueenTexture = guiTransparentTextureFromBMP(renderer, "./img/piece_qb.bmp", 159, 176, 143);
	data->blackKingTexture = guiTransparentTextureFromBMP(renderer, "./img/piece_kb.bmp", 159, 176, 143);

	// check creation
	if (!data->whitePawnTexture || !data->whiteKnightTexture || !data->whiteRookTexture ||
		!data->whiteBishopTexture || !data->whiteQueenTexture || !data->whiteKingTexture ||
		!data->blackPawnTexture || !data->blackKnightTexture || !data->blackRookTexture ||
		!data->blackBishopTexture || !data->blackQueenTexture || !data->blackKingTexture) {

		SDL_DestroyTexture(data->whitePawnTexture);
		SDL_DestroyTexture(data->whiteKnightTexture);
		SDL_DestroyTexture(data->whiteRookTexture);
		SDL_DestroyTexture(data->whiteBishopTexture);
		SDL_DestroyTexture(data->whiteQueenTexture);
		SDL_DestroyTexture(data->whiteKingTexture);
		SDL_DestroyTexture(data->blackPawnTexture);
		SDL_DestroyTexture(data->blackKnightTexture);
		SDL_DestroyTexture(data->blackRookTexture);
		SDL_DestroyTexture(data->blackBishopTexture);
		SDL_DestroyTexture(data->blackQueenTexture);
		SDL_DestroyTexture(data->blackKingTexture);

		free(res);
		free(data);
		return NULL;
	}

	// send computer turn event in case of computer turn
	if (!gameHandlerIsUserTurn(gh)) guiPushUserEvent(GUI_USEREVENT_COMPUTER_TURN, NULL, NULL);

	// store GameBoard & widget details
	data->location = location;
	data->action = action;
	data->gh = gh;
	data->squareChosen = (BoardSquare) { .row = -1,.col = -1 };
	data->squareGetMoves = (BoardSquare) { .row = -1, .col = -1 };
	
	// sets data->gameHasEnded and prints a msg if it did (like when someone loads a ended game)
	guiGameBoardHasGameEnded(data);

	res->destroy = destroyGameBoard;
	res->draw = drawGameBoard;
	res->handleEvent = handleGameBoardEvent;
	res->data = data;
	return res;
}

void destroyGameBoard(GuiWidget* src)
{
	GuiGameBoard* gameBoard = (GuiGameBoard*)src->data;
	free(gameBoard);
	free(src);
}

/*
Returns the rect on board which corresponds to the BoardSquare s.
*/
static SDL_Rect guiGameBoardGetRectForBoardSquare(BoardSquare s) {
	return (SDL_Rect) {
		.x = GUI_GAME_BOARD_START_X + s.col * GUI_SQUARE_SIZE,
			.y = GUI_GAME_BOARD_START_Y + (BOARD_ROWS_NUMBER - 1 - s.row) * GUI_SQUARE_SIZE,
			.h = GUI_SQUARE_SIZE,
			.w = GUI_SQUARE_SIZE
	};
}

/*
Right click -> get moves!
*/
static void handleRightClickOnSquare(GuiGameBoard * gameBoard, BoardSquare s) {
	// if enabled with the same square, switch it off
	if (gameBoard->squareGetMoves.row == s.row && gameBoard->squareGetMoves.col == s.col) {
		gameBoard->squareGetMoves.row = -1;
		return;
	}

	switch (gameGetMovesWrapper(gameBoard->gh->game, s, gameBoard->movesBoardWithTypes)) {
	case GAME_SUCCESS:
		gameBoard->squareGetMoves = s;
		break;
	case GAME_INVALID_PIECE:
	default:
		gameBoard->squareGetMoves.row = -1;
		break;
	}
}

/*
Handles a left click on the specified square.
*/
static void handleLeftClickOnSquare(GuiGameBoard * gameBoard, BoardSquare s) {
	GameHandler * gh = gameBoard->gh;

	// if it's the same square, disable
	if (gameBoard->squareChosen.row == s.row && gameBoard->squareChosen.col == s.col) {
		gameBoard->squareChosen.row = -1;
	}
	else {
		// current user piece, mark it
		if (gameIsPieceOfCurrentPlayer(gh->game, s)) {
			gameBoard->squareChosen = s;
		}

		else if (gameBoard->squareChosen.row != -1) {
			switch (gameSetMove(gh->game, gameBoard->squareChosen, s)) {

			case GAME_INVALID_SQUARE:
				break;
			case GAME_INVALID_PIECE:
				break;
			case GAME_MOVE_INVALID:
				break;
			case GAME_MOVE_FAILED_KING_THREAT:
			case GAME_MOVE_FAILED_THREAT_CAPTURE:
				if (gameIsCurrentPlayerChecked(gh->game)) {
					guiShowMessageBox("Illegal Move", "King is still threatened.");
				}
				else {
					guiShowMessageBox("Illegal Move", "King will be threatened.");
				}
				break;
			case GAME_MOVE_SUCCESS_CAPTURE:
			case GAME_MOVE_SUCCESS:
				gameBoardChanged(gameBoard);
				break;
			default:
				break;
			}

			gameHandlerGameElementAddedToHistory(gh);
		}
	}
}

static void handleRestartEvent(GuiGameBoard * gameBoard) {
	if (!gameHandlerRestartGame(gameBoard->gh)) {
		guiShowMessageBox("ERROR", "failed to create a new game.");
		printf("ERROR: failed to create a new game.\n");
	}

	gameBoardChanged(gameBoard);
}

/* 
we use this mechanism when the game board wants to notify upper elements
that's it's a computer turn and it needs re-rendering
*/
static void handleComputerTurnEvent(GuiGameBoard * gameBoard) {
	if (!gameHandlerIsUserTurn(gameBoard->gh)) {
		gameHandlerComputerTurn(gameBoard->gh);
		gameBoardChanged(gameBoard);
	}
}

static void handleUndoEvent(GuiGameBoard * gameBoard) {
	// check if no history
	if (arrayListIsEmpty(gameBoard->gh->game->history)) return;

	// try twice. if there is only one element, the second call does nothing
	gameUndoPrevMove(gameBoard->gh->game);
	gameUndoPrevMove(gameBoard->gh->game);

	gameBoardChanged(gameBoard);
}

void handleGameBoardEvent(GuiWidget* src, SDL_Event* e) {
	GuiGameBoard* gameBoard = (GuiGameBoard*)src->data;

	// check if the game has ended - handle only 2 events in that case: restart, undo
	if (gameBoard->gameHasEnded &&
		(e->type != SDL_USEREVENT
			|| (e->user.code != GUI_USEREVENT_RESTART && e->user.code != GUI_USEREVENT_UNDO))) {
		return;
	}

	if (e->type == SDL_USEREVENT) {
		switch (e->user.code) {
		case GUI_USEREVENT_RESTART:
			handleRestartEvent(gameBoard);
			return;

		case GUI_USEREVENT_COMPUTER_TURN:
			handleComputerTurnEvent(gameBoard);
			return;

		case GUI_USEREVENT_UNDO:
			handleUndoEvent(gameBoard);
			return;

		default:
			return;
		}
	}

	else if (e->type == SDL_MOUSEBUTTONUP) {

		if (gameHandlerIsUserTurn(gameBoard->gh)) {
			SDL_Point clickPoint = { .x = e->button.x,.y = e->button.y };

			// check if the click is in the whole board
			if (SDL_PointInRect(&clickPoint, &gameBoard->location)) {

				// find the square
				for (int i = 0; i < BOARD_ROWS_NUMBER; i++) {
					for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
						BoardSquare s = { .row = i,.col = j };
						SDL_Rect boardSquareRect = guiGameBoardGetRectForBoardSquare(s);
						if (SDL_PointInRect(&clickPoint, &boardSquareRect)) {
							if (e->button.button == SDL_BUTTON_LEFT) handleLeftClickOnSquare(gameBoard, s);
							else if (e->button.button == SDL_BUTTON_RIGHT) handleRightClickOnSquare(gameBoard, s);

							return;
						}
					}
				}
			}
		}
	}
}

/*
Draws the background of the game board, i.e the squares
*/
static void drawGameBoardSquares(GuiGameBoard * gameBoard, SDL_Renderer * render) {
	// draw game board - first a big rectangle, and then the small recs in it
	SDL_SetRenderDrawColor(render, 238, 238, 210, 0);
	SDL_RenderFillRect(render, &(gameBoard->location));
	SDL_SetRenderDrawColor(render, 118, 150, 86, 0);

	for (int i = 0; i < BOARD_ROWS_NUMBER; i++) {
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			if ((i + j) % 2 == 1) {
				SDL_Rect boardSquareRect = guiGameBoardGetRectForBoardSquare((BoardSquare) { .row = i, .col = j });
				SDL_RenderFillRect(render, &boardSquareRect);
			}
		}
	}
}

static void drawGetMovesSquares(GuiGameBoard * gameBoard, SDL_Renderer * render) {
	if (gameBoard->squareGetMoves.row == -1) return;

	for (int i = 0; i < BOARD_ROWS_NUMBER; i++) {
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {

			// change the square only if it's valid move
			if (!gameIsValidMove(gameBoard->movesBoardWithTypes[i][j])) continue;

			switch (gameBoard->movesBoardWithTypes[i][j]) {
			case BoardSquareValidMove:
				SDL_SetRenderDrawColor(render, 246, 246, 131, 0);
				break;
			case BoardSquareCaptureMove:
				SDL_SetRenderDrawColor(render, 178, 255, 89, 0);
				break;
			case BoardSquareThreatMove:
				SDL_SetRenderDrawColor(render, 244, 63, 48, 0);
				break;
			case BoardSquareCaptureAndThreatMove:
				SDL_SetRenderDrawColor(render, 244, 63, 48, 0);
				break;
			case BoardSquareKingThreatMove:
			case BoardSquareInvalidMove:
				break;
			}

			// create a rect that has a border
			SDL_Rect boardSquareRect = guiGameBoardGetRectForBoardSquare((BoardSquare) { .row = i, .col = j });
			boardSquareRect.x += 2;
			boardSquareRect.y += 2;
			boardSquareRect.h -= 4;
			boardSquareRect.w -= 4;
			SDL_RenderFillRect(render, &boardSquareRect);

			// for a square that is Capture AND Threat, fill half ot if of each (this is the second half)
			if (gameBoard->movesBoardWithTypes[i][j] == BoardSquareCaptureAndThreatMove) {
				SDL_SetRenderDrawColor(render, 178, 255, 89, 0);
				boardSquareRect.w /= 2;
				SDL_RenderFillRect(render, &boardSquareRect);
			}
		}
	}
}

static void drawChosenSquare(GuiGameBoard * gameBoard, SDL_Renderer * render) {
	if (gameBoard->squareChosen.row != -1) {
		SDL_SetRenderDrawColor(render, 186, 202, 69, 0);
		SDL_Rect boardSquareRect = guiGameBoardGetRectForBoardSquare(gameBoard->squareChosen);
		SDL_RenderFillRect(render, &boardSquareRect);
	}
}

static void drawPiecesOnBoard(GuiGameBoard * gameBoard, SDL_Renderer * render) {
	SDL_Texture * pieceTexture;

	for (int i = 0; i < BOARD_ROWS_NUMBER; i++) {
		for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
			char piece = gameBoard->gh->game->gameBoard[i][j];

			switch (tolower(piece)) {
			case PIECE_PAWN:
				if (isupper(piece)) pieceTexture = gameBoard->blackPawnTexture;
				else pieceTexture = gameBoard->whitePawnTexture;
				break;
			case PIECE_KNIGHT:
				if (isupper(piece)) pieceTexture = gameBoard->blackKnightTexture;
				else pieceTexture = gameBoard->whiteKnightTexture;
				break;
			case PIECE_BISHOP:
				if (isupper(piece)) pieceTexture = gameBoard->blackBishopTexture;
				else pieceTexture = gameBoard->whiteBishopTexture;
				break;
			case PIECE_ROOK:
				if (isupper(piece)) pieceTexture = gameBoard->blackRookTexture;
				else pieceTexture = gameBoard->whiteRookTexture;
				break;
			case PIECE_QUEEN:
				if (isupper(piece)) pieceTexture = gameBoard->blackQueenTexture;
				else pieceTexture = gameBoard->whiteQueenTexture;
				break;
			case PIECE_KING:
				if (isupper(piece)) pieceTexture = gameBoard->blackKingTexture;
				else pieceTexture = gameBoard->whiteKingTexture;
				break;
			case BOARD_EMPTY_CELL:
			default:
				pieceTexture = NULL;
				break;
			}

			// copy the piece only if it's not null
			if (pieceTexture) {
				SDL_Rect boardSquareRect = guiGameBoardGetRectForBoardSquare((BoardSquare) { .row = i, .col = j });
				SDL_RenderCopy(render, pieceTexture, NULL, &boardSquareRect);
			}
		}
	}
}

void drawGameBoard(GuiWidget* src, SDL_Renderer* render) {
	GuiGameBoard* gameBoard = (GuiGameBoard*)src->data;

	drawGameBoardSquares(gameBoard, render);
	drawGetMovesSquares(gameBoard, render);
	drawChosenSquare(gameBoard, render);
	drawPiecesOnBoard(gameBoard, render);
}
