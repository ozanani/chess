#include "GuiSaveLoadWindow.h"

/*
Sets path with the right slot path. Assumes path has enough space.
*/
static void getSlotPath(char * path, int slotNum) {
	sprintf(path, "./saves/slot%d.txt", slotNum);
}

static void guiSaveLoadWindowButtonSlotAction(int slotNum, bool saveRequested, GameHandler * gh) {
	char path[GUI_MAX_PATH_LENGTH];
	getSlotPath(path, slotNum);

	// save
	// error handling is in the event loop
	if (saveRequested) guiPushUserEvent(GUI_USEREVENT_SAVE_SLOT_CHOSEN, (void *)(gameHandlerSaveGame(gh, path)), NULL);

	// load
	// error handling is in the event loop
	else guiPushUserEvent(GUI_USEREVENT_LOAD_SLOT_CHOSEN, (void*)(gameHandlerLoadGame(path)), NULL);
}

static void guiSaveLoadWindowButtonBackAction() {
	guiPushUserEvent(GUI_USEREVENT_BACK, (void *)0, NULL);
}

static void guiSaveLoadWindowButtonNextSlotsAction() {
	// data1 indicates the value to add to the current page (+1 -> next page)
	guiPushUserEvent(GUI_USEREVENT_SLOTS_PAGE_CHANGED, (void *)1, NULL);
}

static void guiSaveLoadWindowButtonPrevSlotsAction() {
	guiPushUserEvent(GUI_USEREVENT_SLOTS_PAGE_CHANGED, (void *)-1, NULL);
}

/*
Returns the correct rectangle for the specified slot number.
*/
static SDL_Rect guiGetSlotRect(int slotNum) {
	SDL_Rect basicSlot = { .x = 341,.y = 152,.h = GUI_BUTTON_HEIGHT,.w = GUI_BUTTON_WIDTH };

	basicSlot.y += HEIGHT_GAP_BETWEEN_SLOTS * (slotNum % 5); // 5 slots in a PAGE

	return basicSlot;
}

/*
True iff buttons created successfully.
*/
static bool guiSaveLoadWindowCreateButtons(SaveLoadWindow * saveLoadWindow, bool saveRequested, GameHandler * gh) {
	char path[GUI_MAX_PATH_LENGTH];

	SDL_Rect backRect = { .x = 341,.y = 666,.h = GUI_BUTTON_HEIGHT,.w = GUI_BUTTON_WIDTH };

	SDL_Rect prevSlotsRect = { .x = 243,.y = 318,.h = GUI_NEXT_SLOTS_BUTTON_HEIGHT,.w = GUI_NEXT_SLOTS_BUTTON_WIDTH };
	SDL_Rect nextSlotsRect = { .x = 701,.y = 318,.h = GUI_NEXT_SLOTS_BUTTON_HEIGHT,.w = GUI_NEXT_SLOTS_BUTTON_WIDTH };
	
	// create slot buttons
	for (int i = 0; i < GUI_NUMBER_OF_SAVE_SLOTS; i++) {
		getSlotPath(path, i + 1);
		saveLoadWindow->isSlotEmpty[i] = !guiDoesFileExist(path);

		if (!saveLoadWindow->isSlotEmpty[i]) sprintf(path, "./img/button_slot%d.bmp", i + 1);
		else sprintf(path, "./img/button_slot%d_empty.bmp", i + 1);
		
		saveLoadWindow->widgets[i] = createSaveSlotButton(saveLoadWindow->rend,
			path,
			guiGetSlotRect(i),
			i + 1, 
			saveRequested, 
			gh, 
			guiSaveLoadWindowButtonSlotAction);
	}

	saveLoadWindow->widgets[GUI_BACK_BUTTON_INDEX] = createButton(saveLoadWindow->rend, "./img/button_back.bmp",
		backRect, guiSaveLoadWindowButtonBackAction);
	saveLoadWindow->widgets[GUI_NEXT_SLOTS_BUTTON_INDEX] = createButton(saveLoadWindow->rend, "./img/button_next_slots.bmp",
		nextSlotsRect, guiSaveLoadWindowButtonNextSlotsAction);
	saveLoadWindow->widgets[GUI_PREV_SLOTS_BUTTON_INDEX] = createButton(saveLoadWindow->rend, "./img/button_prev_slots.bmp",
		prevSlotsRect, guiSaveLoadWindowButtonPrevSlotsAction);

	// check creation
	for (unsigned int i = 0; i < sizeof(saveLoadWindow->widgets) / sizeof(saveLoadWindow->widgets[0]); i++) {
		if (saveLoadWindow->widgets[i] == NULL) return false;
	}

	return true;
}

GuiWindow * guiSaveLoadWindowCreate(SDL_Window * window, SDL_Renderer * rend, bool saveRequested, GameHandler * gh) {
	char * bgPath = saveRequested ? "./img/save_bg.bmp" : "./img/load_bg.bmp";
	GuiWindow * result = malloc(sizeof(GuiWindow));
	SaveLoadWindow * saveLoadWindow = calloc(sizeof(SaveLoadWindow), 1);

	if (saveLoadWindow == NULL || result == NULL) {
		free(result);
		free(saveLoadWindow);
		printf("ERROR: malloc has failed.\n");
		return NULL;
	}

	result->data = saveLoadWindow;
	saveLoadWindow->window = window;
	saveLoadWindow->rend = rend;
	saveLoadWindow->curPage = 0;
	saveLoadWindow->saveRequested = saveRequested;

	// set gamehandler in case of SAVE
	if (saveRequested) saveLoadWindow->gh = gh;
	else saveLoadWindow->gh = NULL;

	// background texture
	saveLoadWindow->bgTexture = guiTextureFromBMP(saveLoadWindow->rend, bgPath);
	if (saveLoadWindow->bgTexture == NULL) {
		guiSaveLoadWindowDestroy(result);
		printf("ERROR: background texture creation failed: %s\n", SDL_GetError());
		return NULL;
	}

	// create buttons
	if (!guiSaveLoadWindowCreateButtons(saveLoadWindow, saveRequested, saveLoadWindow->gh)) {
		guiSaveLoadWindowDestroy(result);
		printf("ERROR: button creation failed: %s\n", SDL_GetError());
		return NULL;
	}

	result->destroy = guiSaveLoadWindowDestroy;
	result->handleEvent = guiSaveLoadWindowHandleEvent;
	result->draw = guiSaveLoadWindowDraw;
	return result;
}

void guiSaveLoadWindowDestroy(GuiWindow * src) {
	if (src == NULL) return;

	SaveLoadWindow * w = (SaveLoadWindow *)src->data;
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

void guiSaveLoadWindowDraw(GuiWindow * w) {
	if (w == NULL) return;
	SaveLoadWindow * src = (SaveLoadWindow *)w->data;

	SDL_Rect bgRect = { .x = 0,.y = 0,.h = GUI_WINDOW_HEIGHT,.w = GUI_WINDOW_WIDTH };

	SDL_SetRenderDrawColor(src->rend, 255, 255, 255, 255);
	SDL_RenderClear(src->rend);
	SDL_RenderCopy(src->rend, src->bgTexture, NULL, &bgRect);

	// draw only the relevant slots for this page
	for (int i = src->curPage * 5; i < (src->curPage + 1) * 5 && i < GUI_NUMBER_OF_SAVE_SLOTS; i++) {
		src->widgets[i]->draw(src->widgets[i], src->rend);
	}

	// draw back button
	src->widgets[GUI_BACK_BUTTON_INDEX]->draw(src->widgets[GUI_BACK_BUTTON_INDEX], src->rend);

	// draw prev and next button on certain conditions
	if (src->curPage > 0) {
		src->widgets[GUI_PREV_SLOTS_BUTTON_INDEX]->draw(src->widgets[GUI_PREV_SLOTS_BUTTON_INDEX], src->rend);
	}

	if ((src->curPage + 1) * 5 < GUI_NUMBER_OF_SAVE_SLOTS) {
		src->widgets[GUI_NEXT_SLOTS_BUTTON_INDEX]->draw(src->widgets[GUI_NEXT_SLOTS_BUTTON_INDEX], src->rend);
	}

	SDL_RenderPresent(src->rend);
}

void guiSaveLoadWindowHandleEvent(GuiWindow * src, SDL_Event * e) {
	SaveLoadWindow * w = (SaveLoadWindow *)src->data;

	// current page changed
	if (e->type == SDL_USEREVENT && e->user.code == GUI_USEREVENT_SLOTS_PAGE_CHANGED) {
		if (e->user.data1 == (void *)1) w->curPage++;
		else w->curPage--;
		
		return;
	}

	// let only slots in this page handle the event
	for (int i = w->curPage * 5; i < (w->curPage + 1) * 5 && i < GUI_NUMBER_OF_SAVE_SLOTS; i++) {
		w->widgets[i]->handleEvent(w->widgets[i], e);
	}

	// draw back button
	w->widgets[GUI_BACK_BUTTON_INDEX]->handleEvent(w->widgets[GUI_BACK_BUTTON_INDEX], e);

	// draw prev and next button on certain conditions
	if (w->curPage > 0) {
		w->widgets[GUI_PREV_SLOTS_BUTTON_INDEX]->handleEvent(w->widgets[GUI_PREV_SLOTS_BUTTON_INDEX], e);
	}

	if ((w->curPage + 1) * 5 < GUI_NUMBER_OF_SAVE_SLOTS) {
		w->widgets[GUI_NEXT_SLOTS_BUTTON_INDEX]->handleEvent(w->widgets[GUI_NEXT_SLOTS_BUTTON_INDEX], e);
	}
}