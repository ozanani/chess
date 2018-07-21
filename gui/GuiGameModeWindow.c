#include "GuiGameModeWindow.h"

static void guiGameModeWindowButtonBackAction() {
	guiPushUserEvent(GUI_USEREVENT_BACK, 0, NULL);
}

static void guiGameModeWindowButtonOnePlayerAction() {
	guiPushUserEvent(GUI_USEREVENT_GAME_MODE_CHOSEN, (void *)GameModeSinglePlayer, NULL);
}

static void guiGameModeWindowButtonTwoPlayersAction() {
	guiPushUserEvent(GUI_USEREVENT_GAME_MODE_CHOSEN, (void *)GameModeMultiPlayer, NULL);
}

/*
True iff buttons created successfully.
*/
static bool guiGameModeWindowCreateButtons(GameModeWindow * gameModeWindow) {
	SDL_Rect onePlayerRect = { .x = 341,.y = 250,.h = GUI_BUTTON_HEIGHT,.w = GUI_BUTTON_WIDTH };
	SDL_Rect twoPlayersRect = { .x = 341,.y = 348,.h = GUI_BUTTON_HEIGHT,.w = GUI_BUTTON_WIDTH };
	SDL_Rect backRect = { .x = 341,.y = 602,.h = GUI_BUTTON_HEIGHT,.w = GUI_BUTTON_WIDTH };

	// create
	gameModeWindow->widgets[0] = createButton(gameModeWindow->rend, "./img/button_one_player.bmp",
		onePlayerRect, guiGameModeWindowButtonOnePlayerAction);
	gameModeWindow->widgets[1] = createButton(gameModeWindow->rend, "./img/button_two_players.bmp",
		twoPlayersRect, guiGameModeWindowButtonTwoPlayersAction);
	gameModeWindow->widgets[2] = createButton(gameModeWindow->rend, "./img/button_back.bmp",
		backRect, guiGameModeWindowButtonBackAction);

	// check creation
	for (unsigned int i = 0; i < sizeof(gameModeWindow->widgets) / sizeof(gameModeWindow->widgets[0]); i++) {
		if (gameModeWindow->widgets[i] == NULL) return false;
	}

	return true;
}

GuiWindow * guiGameModeWindowCreate(SDL_Window * window, SDL_Renderer * rend) {
	GuiWindow * result = malloc(sizeof(GuiWindow));
	GameModeWindow * gameModeWindow = calloc(sizeof(GameModeWindow), 1);

	if (gameModeWindow == NULL || result == NULL) {
		free(result);
		free(gameModeWindow);
		printf("ERROR: malloc has failed.\n");
		return NULL;
	}

	result->data = gameModeWindow;
	gameModeWindow->window = window;
	gameModeWindow->rend = rend;

	// background texture
	gameModeWindow->bgTexture = guiTextureFromBMP(gameModeWindow->rend, "./img/game_mode_bg.bmp");
	if (gameModeWindow->bgTexture == NULL) {
		guiGameModeWindowDestroy(result);
		printf("ERROR: background texture creation failed: %s\n", SDL_GetError());
		return NULL;
	}

	// create buttons
	if (!guiGameModeWindowCreateButtons(gameModeWindow)) {
		guiGameModeWindowDestroy(result);
		printf("ERROR: button creation failed: %s\n", SDL_GetError());
		return NULL;
	}

	result->destroy = guiGameModeWindowDestroy;
	result->handleEvent = guiGameModeWindowHandleEvent;
	result->draw = guiGameModeWindowDraw;
	return result;
}

void guiGameModeWindowDestroy(GuiWindow * src) {
	if (src == NULL) return;

	GameModeWindow * w = (GameModeWindow *)src->data;
	free(src);

	if (w == NULL) return;

	// destroy buttons
	for (unsigned int i = 0; i < sizeof(w->widgets) / sizeof(w->widgets[0]); i++) {
		if (w->widgets[i] != NULL) w->widgets[i]->destroy(w->widgets[i]);
	}

	// destroy bg texture
	if (w->bgTexture != NULL) SDL_DestroyTexture(w->bgTexture);

	free(w);
}

void guiGameModeWindowDraw(GuiWindow * w) {
	if (w == NULL) return;
	GameModeWindow * src = (GameModeWindow *)w->data;

	SDL_Rect bgRect = { .x = 0,.y = 0,.h = GUI_WINDOW_HEIGHT,.w = GUI_WINDOW_WIDTH };

	SDL_SetRenderDrawColor(src->rend, 255, 255, 255, 255);
	SDL_RenderClear(src->rend);
	SDL_RenderCopy(src->rend, src->bgTexture, NULL, &bgRect);

	// draw buttons
	for (unsigned int i = 0; i < sizeof(src->widgets) / sizeof(src->widgets[0]); i++) {
		src->widgets[i]->draw(src->widgets[i], src->rend);
	}

	SDL_RenderPresent(src->rend);
}

void guiGameModeWindowHandleEvent(GuiWindow * src, SDL_Event * e) {
	GameModeWindow * w = (GameModeWindow *)src->data;

	for (unsigned int i = 0; i < sizeof(w->widgets) / sizeof(w->widgets[0]); i++) {
		w->widgets[i]->handleEvent(w->widgets[i], e);
	}
}