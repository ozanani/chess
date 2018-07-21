#include "GuiUserColorWindow.h"

static void guiUserColorWindowButtonBackAction() {
	guiPushUserEvent(GUI_USEREVENT_BACK, 0, NULL);
}

static void guiUserColorWindowButtonWhiteAction() {
	guiPushUserEvent(GUI_USEREVENT_COLOR_CHOSEN, (void *)UserColorWhite, NULL);
}

static void guiUserColorWindowButtonBlackAction() {
	guiPushUserEvent(GUI_USEREVENT_COLOR_CHOSEN, (void *)UserColorBlack, NULL);
}

/*
True iff buttons created successfully.
*/
static bool guiUserColorWindowCreateButtons(UserColorWindow * userColorWindow) {
	SDL_Rect whiteRect = { .x = 341,.y = 250,.h = GUI_BUTTON_HEIGHT,.w = GUI_BUTTON_WIDTH };
	SDL_Rect blackRect = { .x = 341,.y = 348,.h = GUI_BUTTON_HEIGHT,.w = GUI_BUTTON_WIDTH };
	SDL_Rect backRect = { .x = 341,.y = 602,.h = GUI_BUTTON_HEIGHT,.w = GUI_BUTTON_WIDTH };

	// create
	userColorWindow->widgets[0] = createButton(userColorWindow->rend, "./img/button_white.bmp",
		whiteRect, guiUserColorWindowButtonWhiteAction);
	userColorWindow->widgets[1] = createButton(userColorWindow->rend, "./img/button_black.bmp",
		blackRect, guiUserColorWindowButtonBlackAction);
	userColorWindow->widgets[2] = createButton(userColorWindow->rend, "./img/button_back.bmp",
		backRect, guiUserColorWindowButtonBackAction);

	// check creation
	for (unsigned int i = 0; i < sizeof(userColorWindow->widgets) / sizeof(userColorWindow->widgets[0]); i++) {
		if (userColorWindow->widgets[i] == NULL) return false;
	}

	return true;
}

GuiWindow * guiUserColorWindowCreate(SDL_Window * window, SDL_Renderer * rend) {
	GuiWindow * result = malloc(sizeof(GuiWindow));
	UserColorWindow * userColorWindow = calloc(sizeof(UserColorWindow), 1);

	if (userColorWindow == NULL || result == NULL) {
		free(result);
		free(userColorWindow);
		printf("ERROR: malloc has failed.\n");
		return NULL;
	}

	result->data = userColorWindow;
	userColorWindow->window = window;
	userColorWindow->rend = rend;

	// background texture
	userColorWindow->bgTexture = guiTextureFromBMP(userColorWindow->rend, "./img/user_color_bg.bmp");
	if (userColorWindow->bgTexture == NULL) {
		guiUserColorWindowDestroy(result);
		printf("ERROR: background texture creation failed: %s\n", SDL_GetError());
		return NULL;
	}

	// create buttons
	if (!guiUserColorWindowCreateButtons(userColorWindow)) {
		guiUserColorWindowDestroy(result);
		printf("ERROR: button creation failed: %s\n", SDL_GetError());
		return NULL;
	}

	result->destroy = guiUserColorWindowDestroy;
	result->handleEvent = guiUserColorWindowHandleEvent;
	result->draw = guiUserColorWindowDraw;
	return result;
}

void guiUserColorWindowDestroy(GuiWindow * src) {
	if (src == NULL) return;

	UserColorWindow * w = (UserColorWindow *)src->data;
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

void guiUserColorWindowDraw(GuiWindow * w) {
	if (w == NULL) return;
	UserColorWindow * src = (UserColorWindow *)w->data;

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

void guiUserColorWindowHandleEvent(GuiWindow * src, SDL_Event * e) {
	UserColorWindow * w = (UserColorWindow *)src->data;

	for (unsigned int i = 0; i < sizeof(w->widgets) / sizeof(w->widgets[0]); i++) {
		w->widgets[i]->handleEvent(w->widgets[i], e);
	}
}