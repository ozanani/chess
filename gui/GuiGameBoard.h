#ifndef GUI_GAME_BOARD_H_
#define GUI_GAME_BOARD_H_

#include "GuiWidget.h"
#include "GameHandler.h"
#include "GuiHelpers.h"
#include <SDL.h>
#include <SDL_video.h>

/*
The gui game board widget. It contains all the data required to present the current 
game board to the user, and handles user interactions with the board.
*/

#define GUI_SQUARE_SIZE 90
#define GUI_GAME_BOARD_START_X 20
#define GUI_GAME_BOARD_START_Y 20

typedef struct game_board_t {
	SDL_Renderer* render;
	SDL_Rect location;
	void(*action)(void);

	// pieces
	SDL_Texture * whitePawnTexture;
	SDL_Texture * whiteKnightTexture;
	SDL_Texture * whiteRookTexture;
	SDL_Texture * whiteBishopTexture;
	SDL_Texture * whiteQueenTexture;
	SDL_Texture * whiteKingTexture;
	SDL_Texture * blackPawnTexture;
	SDL_Texture * blackKnightTexture;
	SDL_Texture * blackRookTexture;
	SDL_Texture * blackBishopTexture;
	SDL_Texture * blackQueenTexture;
	SDL_Texture * blackKingTexture;

	// game properties
	GameHandler * gh;
	MovesBoardWithTypes movesBoardWithTypes;
	BoardSquare squareChosen;
	BoardSquare squareGetMoves;
	bool gameHasEnded;

} GuiGameBoard;

GuiWidget* createGameBoard(
	SDL_Renderer* renderer,
	SDL_Rect location,
	void(*action)(void),
	GameHandler * gh);
void destroyGameBoard(GuiWidget* src);
void handleGameBoardEvent(GuiWidget* src, SDL_Event* event);
void drawGameBoard(GuiWidget*, SDL_Renderer*);

#endif