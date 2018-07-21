#ifndef GRAPHIC_GAME_H
#define GRAPHIC_GAME_H

#include <stdlib.h>
#include "GuiHelpers.h"
#include "GuiWindow.h"
#include "GuiWelcomeWindow.h"
#include "GuiGameModeWindow.h"
#include "GuiDifficultyWindow.h"
#include "GuiUserColorWindow.h"
#include "GuiGameWindow.h"
#include "GuiSaveLoadWindow.h"
#include "GameHandler.h"

/*
This module is responsible of handling the graphical game. The graphical game requires SDL v2 to be installed
on the target computer.
*/

#define MSGBOX_NO_BUTTON_ID 0
#define MSGBOX_YES_BUTTON_ID 1
#define MSGBOX_CANCEL_BUTTON_ID 2

typedef enum window_e {
	GUI_WINDOW_WELCOME,
	GUI_WINDOW_GAME_MODE,
	GUI_WINDOW_DIFFICULTY,
	GUI_WINDOW_USER_COLOR,
	GUI_WINDOW_LOAD,
	GUI_WINDOW_SAVE,
	GUI_WINDOW_GAME,
	GUI_WINDOW_GAME_FROM_LOAD,
} GuiWindowEnum;

/*
Container for the graphical game. 
*/
typedef struct graphical_game_t {
	SDL_Window * window;
	SDL_Renderer * rend;
	GuiWindow * curWindow;
	bool errorOrStop;
	GuiWindowEnum lastWindow;
	GhSettings settings;
	GameHandler * loadedGameHandler;

	GuiWindow * gameWindowBeforeSaveLoad; // we save the game window when switching to save/load window
	bool inGameWindow; // indicates whether we are currently in the game window
} GraphicalGame;

/*
Creates a graphical game container.
@return the gg, or NULL if the call failed
*/
GraphicalGame * graphicalGameCreate();

/*
Destroys the graphical game.
@param gg the graphical game to destroy
*/
void graphicalGameDestroy(GraphicalGame * gg);

/*
Runs the graphic game.  
@return
0 on success and 1 on error.
*/
int graphicalGameRun();

/*
Sets the loaded game handler to the given gh. Used when loading a game.
@param gg the graphical game
@param gh the game handler
*/
void graphicalGameSetLoadedGameHandler(GraphicalGame * gg, GameHandler * gh);
#endif