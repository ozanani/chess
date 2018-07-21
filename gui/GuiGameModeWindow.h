#ifndef GAME_MODE_WINDOW_H
#define GAME_MODE_WINDOW_H

#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include <stdbool.h>
#include "GuiButton.h"
#include "GuiHelpers.h"
#include "GuiWindow.h"

typedef struct game_mode_window_t {
	SDL_Window * window;
	SDL_Renderer * rend;
	GuiWidget *  widgets[3]; // one player, two player, back
	SDL_Texture * bgTexture;
} GameModeWindow;

GuiWindow * guiGameModeWindowCreate(SDL_Window * window, SDL_Renderer * rend);
void guiGameModeWindowDraw(GuiWindow * src);
void guiGameModeWindowDestroy(GuiWindow * src);
void guiGameModeWindowHandleEvent(GuiWindow * src, SDL_Event * e);

#endif