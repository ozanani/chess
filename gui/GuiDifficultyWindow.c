#include "GuiDifficultyWindow.h"

static void guiDifficultyWindowButtonBackAction() {
	guiPushUserEvent(GUI_USEREVENT_BACK, (void *)0, NULL);
}

static void guiDifficultyWindowButtonAmateurAction() {
	guiPushUserEvent(GUI_USEREVENT_DIFFICULTY_CHOSEN, (void *)GameDifficultyAmateur, NULL);
}

static void guiDifficultyWindowButtonEasyAction() {
	guiPushUserEvent(GUI_USEREVENT_DIFFICULTY_CHOSEN, (void *)GameDifficultyEasy, NULL);
}

static void guiDifficultyWindowButtonModerateAction() {
	guiPushUserEvent(GUI_USEREVENT_DIFFICULTY_CHOSEN, (void *)GameDifficultyModerate, NULL);
}

static void guiDifficultyWindowButtonHardAction() {
	guiPushUserEvent(GUI_USEREVENT_DIFFICULTY_CHOSEN, (void *)GameDifficultyHard, NULL);
}

static void guiDifficultyWindowButtonExpertAction() {
	guiPushUserEvent(GUI_USEREVENT_DIFFICULTY_CHOSEN, (void *)GameDifficultyExpert, NULL);
}

/*
True iff buttons created successfully.
*/
static bool guiDifficultyWindowCreateButtons(DifficultyWindow * difficultyWindow) {
	// level rects
	SDL_Rect amateurRect = { .x = 341,.y = 152,.h = GUI_BUTTON_HEIGHT,.w = GUI_BUTTON_WIDTH };
	SDL_Rect easyRect = { .x = 341,.y = 250,.h = GUI_BUTTON_HEIGHT,.w = GUI_BUTTON_WIDTH };
	SDL_Rect moderateRect = { .x = 341,.y = 348,.h = GUI_BUTTON_HEIGHT,.w = GUI_BUTTON_WIDTH };
	SDL_Rect hardRect = { .x = 341,.y = 446,.h = GUI_BUTTON_HEIGHT,.w = GUI_BUTTON_WIDTH };
	SDL_Rect expertRect = { .x = 341,.y = 544,.h = GUI_BUTTON_HEIGHT,.w = GUI_BUTTON_WIDTH };

	SDL_Rect backRect = { .x = 341,.y = 666,.h = GUI_BUTTON_HEIGHT,.w = GUI_BUTTON_WIDTH };

	// create
	difficultyWindow->widgets[0] = createButton(difficultyWindow->rend, "./img/button_amateur.bmp",
		amateurRect, guiDifficultyWindowButtonAmateurAction);
	difficultyWindow->widgets[1] = createButton(difficultyWindow->rend, "./img/button_easy.bmp",
		easyRect, guiDifficultyWindowButtonEasyAction);
	difficultyWindow->widgets[2] = createButton(difficultyWindow->rend, "./img/button_moderate.bmp",
		moderateRect, guiDifficultyWindowButtonModerateAction);
	difficultyWindow->widgets[3] = createButton(difficultyWindow->rend, "./img/button_hard.bmp",
		hardRect, guiDifficultyWindowButtonHardAction);
	difficultyWindow->widgets[4] = createButton(difficultyWindow->rend, "./img/button_expert.bmp",
		expertRect, guiDifficultyWindowButtonExpertAction);

	difficultyWindow->widgets[5] = createButton(difficultyWindow->rend, "./img/button_back.bmp",
		backRect, guiDifficultyWindowButtonBackAction);

	// check creation
	for (unsigned int i = 0; i < sizeof(difficultyWindow->widgets) / sizeof(difficultyWindow->widgets[0]); i++) {
		if (difficultyWindow->widgets[i] == NULL) return false;
	}

	return true;
}

GuiWindow * guiDifficultyWindowCreate(SDL_Window * window, SDL_Renderer * rend) {
	GuiWindow * result = malloc(sizeof(GuiWindow));
	DifficultyWindow * difficultyWIndow = calloc(sizeof(DifficultyWindow), 1);

	if (difficultyWIndow == NULL || result == NULL) {
		free(result);
		free(difficultyWIndow);
		printf("ERROR: malloc has failed.\n");
		return NULL;
	}

	result->data = difficultyWIndow;
	difficultyWIndow->window = window;
	difficultyWIndow->rend = rend;

	// background texture
	difficultyWIndow->bgTexture = guiTextureFromBMP(difficultyWIndow->rend, "./img/difficulty_bg.bmp");
	if (difficultyWIndow->bgTexture == NULL) {
		guiDifficultyWindowDestroy(result);
		printf("ERROR: background texture creation failed: %s\n", SDL_GetError());
		return NULL;
	}

	// create buttons
	if (!guiDifficultyWindowCreateButtons(difficultyWIndow)) {
		guiDifficultyWindowDestroy(result);
		printf("ERROR: button creation failed: %s\n", SDL_GetError());
		return NULL;
	}

	result->destroy = guiDifficultyWindowDestroy;
	result->handleEvent = guiDifficultyWindowHandleEvent;
	result->draw = guiDifficultyWindowDraw;
	return result;
}

void guiDifficultyWindowDestroy(GuiWindow * src) {
	if (src == NULL) return;

	DifficultyWindow * w = (DifficultyWindow *)src->data;
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

void guiDifficultyWindowDraw(GuiWindow * w) {
	if (w == NULL) return;
	DifficultyWindow * src = (DifficultyWindow *)w->data;

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

void guiDifficultyWindowHandleEvent(GuiWindow * src, SDL_Event * e) {
	DifficultyWindow * w = (DifficultyWindow *)src->data;

	for (unsigned int i = 0; i < sizeof(w->widgets) / sizeof(w->widgets[0]); i++) {
		w->widgets[i]->handleEvent(w->widgets[i], e);
	}
}