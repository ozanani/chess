#include "GuiGameWindow.h"

static void guiGameWindowButtonRestartAction() {
	guiPushUserEvent(GUI_USEREVENT_RESTART, NULL, NULL);
}

static void guiGameWindowButtonSaveAction() {
	guiPushUserEvent(GUI_USEREVENT_SAVE, NULL, NULL);
}

static void guiGameWindowButtonSavedVAction() {
	guiShowMessageBox("Oops", "Game is already saved.");
}

static void guiGameWindowButtonLoadAction() {
	guiPushUserEvent(GUI_USEREVENT_LOAD_FROM_GAME_WINDOW, NULL, NULL);
}

static void guiGameWindowButtonUndoAction() {
	guiPushUserEvent(GUI_USEREVENT_UNDO, NULL, NULL);
}

static void guiGameWindowButtonUndoDisabledAction() {
	guiShowMessageBox("Oops", "Empty history, no move to undo.");
}

static void guiGameWindowButtonMenuAction() {
	guiPushUserEvent(GUI_USEREVENT_MENU_FROM_GAME_WINDOW, NULL, NULL);
}

static void guiGameWindowButtonQuitAction() {
	guiPushUserEvent(GUI_USEREVENT_QUIT_FROM_GAME_WINDOW, NULL, NULL);
}

/*
True iff buttons created successfully.
*/
static bool guiGameWindowCreateButtons(GameWindow * gameWindow) {
	SDL_Rect restartRect = { .x = 779 ,.y = 134,.h = GUI_BUTTON_SMALL_HEIGHT,.w = GUI_BUTTON_SMALL_WIDTH };
	SDL_Rect saveRect = { .x = 779,.y = 224,.h = GUI_BUTTON_SMALL_HEIGHT,.w = GUI_BUTTON_SMALL_WIDTH };
	SDL_Rect loadRect = { .x = 779,.y = 315,.h = GUI_BUTTON_SMALL_HEIGHT,.w = GUI_BUTTON_SMALL_WIDTH };
	SDL_Rect undoRect = { .x = 779,.y = 405,.h = GUI_BUTTON_SMALL_HEIGHT,.w = GUI_BUTTON_SMALL_WIDTH };
	SDL_Rect menuRect = { .x = 779,.y = 585,.h = GUI_BUTTON_SMALL_HEIGHT,.w = GUI_BUTTON_SMALL_WIDTH };
	SDL_Rect quitRect = { .x = 779,.y = 675,.h = GUI_BUTTON_SMALL_HEIGHT,.w = GUI_BUTTON_SMALL_WIDTH };
	SDL_Rect boardRect = { .x = GUI_GAME_BOARD_START_Y,
		.y = GUI_GAME_BOARD_START_Y,
		.h = GUI_SQUARE_SIZE * BOARD_ROWS_NUMBER,
		.w = GUI_SQUARE_SIZE * BOARD_COLUMNS_NUMBER };

	// create
	gameWindow->widgets[0] = createGameBoard(gameWindow->rend, boardRect, NULL, gameWindow->gh);
	gameWindow->widgets[1] = createButton(gameWindow->rend, "./img/button_restart_small.bmp",
		restartRect, guiGameWindowButtonRestartAction);
	gameWindow->widgets[GAME_WINDOW_SAVE_BUTTON_INDEX] = createButton(gameWindow->rend, "./img/button_save_small.bmp",
		saveRect, guiGameWindowButtonSaveAction);
	gameWindow->widgets[3] = createButton(gameWindow->rend, "./img/button_load_small.bmp",
		loadRect, guiGameWindowButtonLoadAction);
	gameWindow->widgets[GAME_WINDOW_UNDO_BUTTON_INDEX] = createButton(gameWindow->rend, "./img/button_undo_small.bmp",
		undoRect, guiGameWindowButtonUndoAction);
	gameWindow->widgets[5] = createButton(gameWindow->rend, "./img/button_menu_small.bmp",
		menuRect, guiGameWindowButtonMenuAction);
	gameWindow->widgets[6] = createButton(gameWindow->rend, "./img/button_quit_small.bmp",
		quitRect, guiGameWindowButtonQuitAction);
	gameWindow->widgets[GAME_WINDOW_DISABLED_UNDO_BUTTON_INDEX] = createButton(gameWindow->rend, 
		"./img/button_undo_small_disabled.bmp",
		undoRect, guiGameWindowButtonUndoDisabledAction);
	gameWindow->widgets[GAME_WINDOW_SAVED_V_BUTTON_INDEX] = createButton(gameWindow->rend,
		"./img/button_saved_small.bmp",
		saveRect, guiGameWindowButtonSavedVAction);

	// check creation
	for (unsigned int i = 0; i < sizeof(gameWindow->widgets) / sizeof(gameWindow->widgets[0]); i++) {
		if (gameWindow->widgets[i] == NULL) return false;
	}

	return true;
}

GuiWindow * guiGameWindowCreate(SDL_Window * window, SDL_Renderer * rend, GameHandler * gh) {
	GuiWindow * result = malloc(sizeof(GuiWindow));
	GameWindow * gameWindow = calloc(sizeof(GameWindow), 1);

	if (gameWindow == NULL || result == NULL || gh == NULL) {
		free(result);
		free(gameWindow);
		printf("ERROR: malloc has failed.\n");
		return NULL;
	}

	result->data = gameWindow;
	gameWindow->window = window;
	gameWindow->rend = rend;
	gameWindow->gh = gh;
	gameWindow->exitAfterSave = false;
	gameWindow->menuAfterSave = false;

	// background texture
	gameWindow->bgTexture = guiTextureFromBMP(gameWindow->rend, "./img/game_window_bg.bmp");
	if (gameWindow->bgTexture == NULL) {
		guiGameWindowDestroy(result);
		printf("ERROR: background texture creation failed: %s\n", SDL_GetError());
		return NULL;
	}

	// create buttons
	if (!guiGameWindowCreateButtons(gameWindow)) {
		guiGameWindowDestroy(result);
		printf("ERROR: button creation failed: %s\n", SDL_GetError());
		return NULL;
	}

	result->destroy = guiGameWindowDestroy;
	result->handleEvent = guiGameWindowHandleEvent;
	result->draw = guiGameWindowDraw;
	return result;
}

void guiGameWindowDestroy(GuiWindow * src) {
	if (src == NULL) return;

	GameWindow * w = (GameWindow *)src->data;
	free(src);

	if (w == NULL) return;

	// destroy buttons
	for (unsigned int i = 0; i < sizeof(w->widgets) / sizeof(w->widgets[0]); i++) {
		if (w->widgets[i] != NULL) w->widgets[i]->destroy(w->widgets[i]);
	}

	// destroy bg texture
	if (w->bgTexture != NULL) SDL_DestroyTexture(w->bgTexture);

	// destroy game handler
	if (w->gh != NULL) gameHandlerDestroy(w->gh);

	free(w);
}

void guiGameWindowDraw(GuiWindow * w) {
	if (w == NULL) return;
	GameWindow * src = (GameWindow *)w->data;

	SDL_Rect bgRect = { .x = 0,.y = 0,.h = GUI_WINDOW_HEIGHT,.w = GUI_WINDOW_WIDTH };

	SDL_SetRenderDrawColor(src->rend, 255, 255, 255, 255);
	SDL_RenderClear(src->rend);
	SDL_RenderCopy(src->rend, src->bgTexture, NULL, &bgRect);

	// draw buttons
	for (unsigned int i = 0; i < sizeof(src->widgets) / sizeof(src->widgets[0]); i++) {
		// draw undo or disabled undo according to the history status
		if (i == GAME_WINDOW_UNDO_BUTTON_INDEX && arrayListIsEmpty(src->gh->game->history)) continue;
		if (i == GAME_WINDOW_DISABLED_UNDO_BUTTON_INDEX && !arrayListIsEmpty(src->gh->game->history)) continue;

		// draw the right save button 
		if (i == GAME_WINDOW_SAVE_BUTTON_INDEX && src->gh->gameIsSaved) continue;
		if (i == GAME_WINDOW_SAVED_V_BUTTON_INDEX && !src->gh->gameIsSaved) continue;

		src->widgets[i]->draw(src->widgets[i], src->rend);
	}

	SDL_RenderPresent(src->rend);
}

void guiGameWindowHandleEvent(GuiWindow * src, SDL_Event * e) {
	GameWindow * w = (GameWindow *)src->data;

	// handle save event
	if (e->type == SDL_USEREVENT && e->user.code == GUI_USEREVENT_SAVE) {
		guiPushUserEvent(GUI_USEREVENT_SAVE_FROM_GAME_WINDOW, NULL, NULL);
		return;
	}

	for (unsigned int i = 0; i < sizeof(w->widgets) / sizeof(w->widgets[0]); i++) {
		// let undo or disabled undo handle an event according to the history status
		if (i == GAME_WINDOW_UNDO_BUTTON_INDEX && arrayListIsEmpty(w->gh->game->history)) continue;
		if (i == GAME_WINDOW_DISABLED_UNDO_BUTTON_INDEX && !arrayListIsEmpty(w->gh->game->history)) continue;

		// dispatch the event to the right save button (save/saved)
		if (i == GAME_WINDOW_SAVE_BUTTON_INDEX && w->gh->gameIsSaved) continue;
		if (i == GAME_WINDOW_SAVED_V_BUTTON_INDEX && !w->gh->gameIsSaved) continue;

		w->widgets[i]->handleEvent(w->widgets[i], e);
	}
}