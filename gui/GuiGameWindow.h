#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include <stdbool.h>
#include "GuiButton.h"
#include "GuiHelpers.h"
#include "GuiGameBoard.h"
#include "GuiWindow.h"

#define GAME_WINDOW_UNDO_BUTTON_INDEX 4
#define GAME_WINDOW_DISABLED_UNDO_BUTTON_INDEX 7
#define GAME_WINDOW_SAVE_BUTTON_INDEX 2
#define GAME_WINDOW_SAVED_V_BUTTON_INDEX 8

typedef struct game_window_t {
	SDL_Window * window;
	SDL_Renderer * rend;
	GuiWidget *  widgets[9]; 
	SDL_Texture * bgTexture;

	GameHandler * gh;
	bool exitAfterSave; // indicates whether the program will exit after save
	bool menuAfterSave;
} GameWindow;

GuiWindow * guiGameWindowCreate(SDL_Window * window, SDL_Renderer * rend, GameHandler * gh);
void guiGameWindowDraw(GuiWindow * src);
void guiGameWindowDestroy(GuiWindow * src);
void guiGameWindowHandleEvent(GuiWindow * src, SDL_Event * e);

#endif