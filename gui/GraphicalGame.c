#include "GraphicalGame.h"
#include <SDL.h>
#include <SDL_video.h>

GraphicalGame * graphicalGameCreate() {
	GraphicalGame * gg;

	gg = malloc(sizeof(GraphicalGame));
	if (gg == NULL) { 
		printf("ERROR: malloc has failed.\n");
		return NULL; 
	}

	// create the window
	gg->window = SDL_CreateWindow("Chess",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		GUI_WINDOW_WIDTH,
		GUI_WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN
	);

	// check window creation
	if (gg->window == NULL) {
		printf("ERROR: window creation failed: %s\n", SDL_GetError());
		graphicalGameDestroy(gg);
		return NULL;
	}

	// create renderer and check creation
	gg->rend = SDL_CreateRenderer(gg->window, -1, SDL_RENDERER_SOFTWARE);

	if (gg->rend == NULL) {
		printf("ERROR: renderer creation failed: %s\n", SDL_GetError());
		graphicalGameDestroy(gg);
		return NULL;
	}	

	gg->errorOrStop = false;
	gg->curWindow = NULL;
	gg->settings = gameHandlerGetDefaultSettings();
	gg->gameWindowBeforeSaveLoad = NULL;
	gg->inGameWindow = false;

	return gg;
}

void graphicalGameDestroy(GraphicalGame * gg) {
	if (gg == NULL) return;

	if (gg->rend != NULL) SDL_DestroyRenderer(gg->rend);
	if (gg->window != NULL) SDL_DestroyWindow(gg->window);

	if (gg->curWindow != NULL) gg->curWindow->destroy(gg->curWindow);

	// we need the next line in case user click X on save window
	if (gg->gameWindowBeforeSaveLoad != NULL) gg->gameWindowBeforeSaveLoad->destroy(gg->gameWindowBeforeSaveLoad);
	
	free(gg);
}

/*
Switches the current window and sets the last window propery accordingly.
On error, sets gg->errorOrStop.
*/
static void guiSwitchWindow(GraphicalGame * gg, GuiWindowEnum nextWindow) {
	if (gg == NULL) return;

	// destroy current window
	destroyWindow(gg->curWindow);
	gg->inGameWindow = false;

	// set the new window
	switch (nextWindow)
	{
	case GUI_WINDOW_WELCOME:
		gg->curWindow = guiWelcomeWindowCreate(gg->window, gg->rend);
		break;
	case GUI_WINDOW_GAME_MODE:
		gg->curWindow = guiGameModeWindowCreate(gg->window, gg->rend);
		gg->lastWindow = GUI_WINDOW_WELCOME;
		break;
	case GUI_WINDOW_DIFFICULTY:
		gg->curWindow = guiDifficultyWindowCreate(gg->window, gg->rend);
		gg->lastWindow = GUI_WINDOW_GAME_MODE;
		break;
	case GUI_WINDOW_USER_COLOR:
		gg->curWindow = guiUserColorWindowCreate(gg->window, gg->rend);
		gg->lastWindow = GUI_WINDOW_DIFFICULTY;
		break;
	case GUI_WINDOW_LOAD:
		gg->curWindow = guiSaveLoadWindowCreate(gg->window, gg->rend, false, NULL);

		// check if we came from the game window
		if (gg->gameWindowBeforeSaveLoad) gg->lastWindow = GUI_WINDOW_GAME;
		else gg->lastWindow = GUI_WINDOW_WELCOME; 
		break;
	case GUI_WINDOW_SAVE:
		gg->curWindow = guiSaveLoadWindowCreate(gg->window,
			gg->rend,
			true,
			((GameWindow *)(gg->gameWindowBeforeSaveLoad->data))->gh);
		gg->lastWindow = GUI_WINDOW_GAME;
		break;
	case GUI_WINDOW_GAME:
		gg->inGameWindow = true;

		// check if we came from the save menu
		if (gg->gameWindowBeforeSaveLoad) {
			gg->curWindow = gg->gameWindowBeforeSaveLoad;
			gg->gameWindowBeforeSaveLoad = NULL;

			// the following is handling for save on quit
			GameWindow * gameWindow = (GameWindow *)(gg->curWindow->data);

			if (gameWindow->gh->gameIsSaved) {
				if (gameWindow->exitAfterSave) {
					gameWindow->exitAfterSave = false;
					gg->errorOrStop = true;
				}
				else if (gameWindow->menuAfterSave) {
					gameWindow->menuAfterSave = false;
					guiSwitchWindow(gg, GUI_WINDOW_WELCOME);
				}
			}
		}
		else {
			gg->curWindow = guiGameWindowCreate(gg->window, gg->rend, gameHandlerNewGame(gg->settings));
		}

		gg->lastWindow = GUI_WINDOW_WELCOME;
		break;
	case GUI_WINDOW_GAME_FROM_LOAD:
		gg->inGameWindow = true;

		// destroy last window if we came from game window
		if (gg->gameWindowBeforeSaveLoad) {
			destroyWindow(gg->gameWindowBeforeSaveLoad);
			gg->gameWindowBeforeSaveLoad = NULL;
		}

		gg->curWindow = guiGameWindowCreate(gg->window, gg->rend, gg->loadedGameHandler);
		gg->lastWindow = GUI_WINDOW_WELCOME;
	default:
		break;
	}
	
	// check for error
	if (gg->curWindow == NULL) {
		gg->errorOrStop = true;
		return;
	}

	// draw
	gg->curWindow->draw(gg->curWindow);
}

/*
This is a wrapper for switching to save or load from the game window, because in this case
we shouldn't dispose the current window.
*/
static void guiSwitchToSaveLoadFromGameWindow(GraphicalGame * gg, GuiWindowEnum nextWindow) {
	if (!gg || ((nextWindow != GUI_WINDOW_LOAD) && (nextWindow != GUI_WINDOW_SAVE))) return;

	// save the game window
	gg->gameWindowBeforeSaveLoad = gg->curWindow;
	gg->curWindow = NULL;

	guiSwitchWindow(gg, nextWindow);
}

static void guiHandleUserEventGameModeChosen(GraphicalGame * gg, GhGameMode gameMode) {
	gg->settings.gameMode = gameMode;

	// multi player - start game
	if (gameMode == GameModeMultiPlayer) {
		guiSwitchWindow(gg, GUI_WINDOW_GAME);
	}

	// single player - continue to next window
	else {
		guiSwitchWindow(gg, GUI_WINDOW_DIFFICULTY);
	}
}

static void guiHandleUserEventDifficultyChosen(GraphicalGame * gg, GhGameDifficultyLevel difficulty) {
	gg->settings.difficultyLevel = difficulty;
	guiSwitchWindow(gg, GUI_WINDOW_USER_COLOR);
}

static void guiHandleUserEventColorChosen(GraphicalGame * gg, GhUserColor color) {
	gg->settings.userColor = color;
	guiSwitchWindow(gg, GUI_WINDOW_GAME);
}

static void guiHandleUserEventSaveSlotChosen(GraphicalGame * gg, bool success) {
	if (!success) {
		guiShowMessageBox("Oops", "An error occured while saving the game.");
	}

	else {
		guiSwitchWindow(gg, GUI_WINDOW_GAME);
	}
}

static void guiHandleUserEventLoadSlotChosen(GraphicalGame * gg, GameHandler * gh) {
	if (gh == NULL) {
		guiShowMessageBox("Oops", "An error occured while loading the game.");
	}

	else {
		graphicalGameSetLoadedGameHandler(gg, gh);
		guiSwitchWindow(gg, GUI_WINDOW_GAME_FROM_LOAD);
	}
}

/*
userWantsToQuit - if true, self explanatory. if false, user wants to get to the main menu
Returns true if the caller has to take control (cancel or no)
*/
static bool guiPromtForUnsavedGame(GraphicalGame * gg, bool userWantsToQuit) {
	GameWindow * gameWindow = (GameWindow *)(gg->curWindow->data);
	int buttonid;

	// check if the game is already saved
	if (gameWindow->gh->gameIsSaved) return true;

	// prompt for save
	SDL_MessageBoxButtonData buttons[] = {
		{ 0, MSGBOX_NO_BUTTON_ID, "no" },
	{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, MSGBOX_YES_BUTTON_ID, "yes" },
	{ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, MSGBOX_CANCEL_BUTTON_ID, "cancel" },
	};

	SDL_MessageBoxColorScheme colorScheme = { {
		{ 255, 0, 0 },
	{ 0, 255, 0 },
	{ 255, 255, 0 },
	{ 0, 0, 255 },
	{ 255, 0, 255 }
		} };

	SDL_MessageBoxData messageboxdata = {
		SDL_MESSAGEBOX_INFORMATION,
		NULL,
		"Warning",
		"Game is not saved. Do you want to save before proceeding?",
		SDL_arraysize(buttons),
		buttons,
		&colorScheme
	};

	if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
		printf("ERROR: failed to show a message box: %s\n", SDL_GetError());
		return false;
	}

	switch (buttonid) {
	case MSGBOX_NO_BUTTON_ID:
		return true;
	case MSGBOX_YES_BUTTON_ID:
		if (userWantsToQuit) gameWindow->exitAfterSave = true;
		else gameWindow->menuAfterSave = true;
		
		guiSwitchToSaveLoadFromGameWindow(gg, GUI_WINDOW_SAVE);
		return false;
	case MSGBOX_CANCEL_BUTTON_ID:
		return false;
	default:
		break;
	}

	return false;
}

int graphicalGameRun() {
	SDL_Event e;
	GraphicalGame * gg;

	// initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("ERROR: SDL initialization failed: %s\n", SDL_GetError());
		return 1;
	}
	atexit(SDL_Quit);
	
	// create game
	gg = graphicalGameCreate();
	if (gg == NULL) return 1;

	// switch to welcome window
	guiSwitchWindow(gg, GUI_WINDOW_WELCOME);

	if (gg->errorOrStop) {
		graphicalGameDestroy(gg);
		return 1;
	}

	while (!(gg->errorOrStop)) {
		SDL_WaitEvent(&e);

		// quit
		if (e.type == SDL_QUIT) {
			// if it was fired from the game window, handle seperatly
			if (gg->inGameWindow) guiPushUserEvent(GUI_USEREVENT_QUIT_FROM_GAME_WINDOW, NULL, NULL);
			else break;
		}

		// user event
		else if (e.type == SDL_USEREVENT) {
			switch (e.user.code) {
			case GUI_USEREVENT_NEW_GAME:
				guiSwitchWindow(gg, GUI_WINDOW_GAME_MODE);
				break;
			case GUI_USEREVENT_LOAD_FROM_WELCOME_WINDOW:
				guiSwitchWindow(gg, GUI_WINDOW_LOAD);
				break;
			case GUI_USEREVENT_LOAD_FROM_GAME_WINDOW:
				guiSwitchToSaveLoadFromGameWindow(gg, GUI_WINDOW_LOAD);
				break;
			case GUI_USEREVENT_SAVE_FROM_GAME_WINDOW:
				guiSwitchToSaveLoadFromGameWindow(gg, GUI_WINDOW_SAVE);
				break;
			case GUI_USEREVENT_BACK:
				guiSwitchWindow(gg, gg->lastWindow);
				break;
			case GUI_USEREVENT_GAME_MODE_CHOSEN:
				guiHandleUserEventGameModeChosen(gg, (GhGameMode)e.user.data1);
				break;
			case GUI_USEREVENT_DIFFICULTY_CHOSEN:
				guiHandleUserEventDifficultyChosen(gg, (GhGameDifficultyLevel)e.user.data1);
				break;
			case GUI_USEREVENT_COLOR_CHOSEN:
				guiHandleUserEventColorChosen(gg, (GhUserColor)e.user.data1);
				break;
			case GUI_USEREVENT_MSGBOX:
				guiShowMessageBox((const char *)e.user.data1, (const char *)e.user.data2);
				break;
			case GUI_USEREVENT_LOAD_SLOT_CHOSEN:
				guiHandleUserEventLoadSlotChosen(gg, (GameHandler *)e.user.data1);
				break;
			case GUI_USEREVENT_SAVE_SLOT_CHOSEN:
				guiHandleUserEventSaveSlotChosen(gg, (bool)e.user.data1);
				break;
			case GUI_USEREVENT_QUIT_FROM_GAME_WINDOW:
				if (guiPromtForUnsavedGame(gg, true)) gg->errorOrStop = true;
				break;
			case GUI_USEREVENT_MENU_FROM_GAME_WINDOW:
				if (guiPromtForUnsavedGame(gg, false)) guiSwitchWindow(gg, GUI_WINDOW_WELCOME);
				break;
			// the following are cases where the current window (i.e gameBoard) has to take control
			case GUI_USEREVENT_COMPUTER_TURN:
			case GUI_USEREVENT_RESTART:
			case GUI_USEREVENT_UNDO:
			case GUI_USEREVENT_SAVE:
			case GUI_USEREVENT_SLOTS_PAGE_CHANGED:
				gg->curWindow->handleEvent(gg->curWindow, &e);
				break;
			case GUI_USEREVENT_WELCOME_WINDOW:
				guiSwitchWindow(gg, GUI_WINDOW_WELCOME);
				break;
			}
		}

		else gg->curWindow->handleEvent(gg->curWindow, &e);

		// draw after user action or computer move
		if (e.type == SDL_MOUSEBUTTONUP || 
			(e.type == SDL_USEREVENT && (
				e.user.code == GUI_USEREVENT_COMPUTER_TURN || 
				e.user.code == GUI_USEREVENT_RESTART || 
				e.user.code == GUI_USEREVENT_UNDO ||
				e.user.code == GUI_USEREVENT_SAVE ||
				e.user.code == GUI_USEREVENT_SLOTS_PAGE_CHANGED))) {
			gg->curWindow->draw(gg->curWindow);
		}
	}	

	// SDL_Quit fires automatically atexit. see start of function (taken from SDL docs)
	graphicalGameDestroy(gg);
	return 0;
}

void graphicalGameSetLoadedGameHandler(GraphicalGame * gg, GameHandler * gh) {
	if (!gg) return;

	gg->loadedGameHandler = gh;

	if (gh) gg->settings = gh->settings;
}