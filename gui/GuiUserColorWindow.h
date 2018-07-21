#ifndef USER_COLOR_WINDOW_H
#define USER_COLOR_WINDOW_H

#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include <stdbool.h>
#include "GuiButton.h"
#include "GuiHelpers.h"
#include "GuiWindow.h"

typedef struct user_color_window_t {
	SDL_Window * window;
	SDL_Renderer * rend;
	GuiWidget *  widgets[3]; // white, black, back
	SDL_Texture * bgTexture;
} UserColorWindow;

GuiWindow * guiUserColorWindowCreate(SDL_Window * window, SDL_Renderer * rend);
void guiUserColorWindowDraw(GuiWindow * src);
void guiUserColorWindowDestroy(GuiWindow * src);
void guiUserColorWindowHandleEvent(GuiWindow * src, SDL_Event * e);

#endif