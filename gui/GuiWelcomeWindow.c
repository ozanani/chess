#include "GuiWelcomeWindow.h"

static void guiWelcomeWindowButtonQuitAction() {
	SDL_Event e = { .type = SDL_QUIT };
	SDL_PushEvent(&e);
}

static void guiWelcomeWindowButtonNewgameAction() {
	guiPushUserEvent(GUI_USEREVENT_NEW_GAME, 0, NULL);
}

static void guiWelcomeWindowButtonLoadAction() {
	guiPushUserEvent(GUI_USEREVENT_LOAD_FROM_WELCOME_WINDOW, 0, NULL);
}

/*
True iff buttons created successfully.
*/
static bool guiWelcomeWindowCreateButtons(WelcomeWindow * welcomeWindow) {
	SDL_Rect newGameRect = { .x = 341,.y = 250,.h = GUI_BUTTON_HEIGHT,.w = GUI_BUTTON_WIDTH };
	SDL_Rect loadRect = { .x = 341,.y = 348,.h = GUI_BUTTON_HEIGHT,.w = GUI_BUTTON_WIDTH };
	SDL_Rect quitRect = { .x = 341,.y = 602,.h = GUI_BUTTON_HEIGHT,.w = GUI_BUTTON_WIDTH };

	// create
	welcomeWindow->widgets[0] = createButton(welcomeWindow->rend, "./img/button_new_game.bmp",
		newGameRect, guiWelcomeWindowButtonNewgameAction);
	welcomeWindow->widgets[1] = createButton(welcomeWindow->rend, "./img/button_load.bmp",
		loadRect, guiWelcomeWindowButtonLoadAction);
	welcomeWindow->widgets[2] = createButton(welcomeWindow->rend, "./img/button_quit.bmp",
		quitRect, guiWelcomeWindowButtonQuitAction);

	// check creation
	for (unsigned int i = 0; i < sizeof(welcomeWindow->widgets) / sizeof(welcomeWindow->widgets[0]); i++) {
		if (welcomeWindow->widgets[i] == NULL) return false;
	}

	return true;
}

GuiWindow * guiWelcomeWindowCreate(SDL_Window * window, SDL_Renderer * rend) {
	GuiWindow * result = malloc(sizeof(GuiWindow));
	WelcomeWindow * welcomeWindow = calloc(sizeof(WelcomeWindow), 1);

	if (welcomeWindow == NULL || result == NULL) {
		free(result);
		free(welcomeWindow);
		printf("ERROR: malloc has failed.\n");
		return NULL;
	}

	result->data = welcomeWindow;
	welcomeWindow->window = window;
	welcomeWindow->rend = rend;
	
	// background texture
	welcomeWindow->bgTexture = guiTextureFromBMP(welcomeWindow->rend, "./img/welcome_window_bg.bmp");
	if (welcomeWindow->bgTexture == NULL) {
		guiWelcomeWindowDestroy(result);
		printf("ERROR: background texture creation failed: %s\n", SDL_GetError());
		return NULL;
	}

	// create buttons
	if (!guiWelcomeWindowCreateButtons(welcomeWindow)) {
		guiWelcomeWindowDestroy(result);
		printf("ERROR: button creation failed: %s\n", SDL_GetError());
		return NULL;
	}

	result->destroy = guiWelcomeWindowDestroy;
	result->handleEvent = guiWelcomeWindowHandleEvent;
	result->draw = guiWelcomeWindowDraw;
	return result;
}

void guiWelcomeWindowDestroy(GuiWindow * src) {
	if (src == NULL) return;

	WelcomeWindow * ww = (WelcomeWindow *)src->data;
	free(src);

	if (ww == NULL) return;

	// destroy buttons
	for (unsigned int i = 0; i < sizeof(ww->widgets) / sizeof(ww->widgets[0]); i++) {
		if (ww->widgets[i] != NULL) ww->widgets[i]->destroy(ww->widgets[i]);
	}

	// destroy bg texture
	if (ww->bgTexture != NULL) SDL_DestroyTexture(ww->bgTexture);

	free(ww);
}

void guiWelcomeWindowDraw(GuiWindow * w) {
	if (w == NULL) return;
	WelcomeWindow * src = (WelcomeWindow *)w->data;

	SDL_Rect bgRect = { .x = 0, .y = 0, .h = GUI_WINDOW_HEIGHT, .w = GUI_WINDOW_WIDTH };	

	SDL_SetRenderDrawColor(src->rend, 255, 255, 255, 255);
	SDL_RenderClear(src->rend);
	SDL_RenderCopy(src->rend, src->bgTexture, NULL, &bgRect);

	// draw buttons
	for (unsigned int i = 0; i < sizeof(src->widgets) / sizeof(src->widgets[0]); i++) {
		src->widgets[i]->draw(src->widgets[i], src->rend);
	}

	SDL_RenderPresent(src->rend);
}

void guiWelcomeWindowHandleEvent(GuiWindow * src, SDL_Event * e) {
	WelcomeWindow * ww = (WelcomeWindow *)src->data;

	for (unsigned int i = 0; i < sizeof(ww->widgets) / sizeof(ww->widgets[0]); i++) {
		ww->widgets[i]->handleEvent(ww->widgets[i], e);
	}
}