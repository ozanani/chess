#ifndef SAVE_LOAD_WINDOW_H
#define SAVE_LOAD_WINDOW_H

#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include <stdbool.h>
#include "GuiButton.h"
#include "GuiSaveSlotButton.h"
#include "GuiHelpers.h"
#include "GuiWindow.h"
#include "GameHandler.h"

/*
This module represents the save-load window. It dynamically loads the correct page.
The number of saving slots is changeable using the first definition here. When changing the number of slots,
relevant images have to be created in the img directory with the following names:
button_slotX - switching X with the relevant number
button_slotX_empty - represents an empty slot (e.g not saved)

By default, 5 slots are shown, but 7 slots exist in the img directory.
*/

// number of saving slots
#define GUI_NUMBER_OF_SAVE_SLOTS 7

#define GUI_BACK_BUTTON_INDEX (GUI_NUMBER_OF_SAVE_SLOTS)
#define GUI_NEXT_SLOTS_BUTTON_INDEX (GUI_NUMBER_OF_SAVE_SLOTS + 1)
#define GUI_PREV_SLOTS_BUTTON_INDEX (GUI_NUMBER_OF_SAVE_SLOTS + 2)

#define GUI_NEXT_SLOTS_BUTTON_HEIGHT 133
#define GUI_NEXT_SLOTS_BUTTON_WIDTH 84

#define GUI_MAX_PATH_LENGTH 50
#define HEIGHT_GAP_BETWEEN_SLOTS 98

typedef struct save_load_window_t {
	SDL_Window * window;
	SDL_Renderer * rend;
	GuiWidget *  widgets[GUI_NUMBER_OF_SAVE_SLOTS + 3]; // back, next slots, prev slots, slots
	SDL_Texture * bgTexture;

	int curPage; // slot paging
	bool saveRequested; // or load
	bool isSlotEmpty[GUI_NUMBER_OF_SAVE_SLOTS];

	GameHandler * gh; // game handler is needed for saving
} SaveLoadWindow;

/*
@param savedRequested - whether the user wants to save (true) or load (false)
@param gh - the game handler to save (if it's save only)
*/
GuiWindow * guiSaveLoadWindowCreate(SDL_Window * window, SDL_Renderer * rend, bool saveRequested, GameHandler * gh);
void guiSaveLoadWindowDraw(GuiWindow * src);
void guiSaveLoadWindowDestroy(GuiWindow * src);
void guiSaveLoadWindowHandleEvent(GuiWindow * src, SDL_Event * e);

#endif