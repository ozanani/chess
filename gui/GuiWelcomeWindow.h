#ifndef WELCOME_WINDOW_H
#define WELCOME_WINDOW_H

#include <SDL.h>
#include <SDL_video.h>
#include <stdio.h>
#include <stdbool.h>
#include "GuiButton.h"
#include "GuiHelpers.h"
#include "GuiWindow.h"

/*
This is the main (i.e first to see) window of the game. 
*/

typedef struct welcome_window_t {
	SDL_Window * window;
	SDL_Renderer * rend;
	GuiWidget *  widgets[3]; // newGame, load, exit
	SDL_Texture * bgTexture;
} WelcomeWindow;

GuiWindow * guiWelcomeWindowCreate(SDL_Window * window, SDL_Renderer * rend);
void guiWelcomeWindowDraw(GuiWindow * src);
void guiWelcomeWindowDestroy(GuiWindow * src);
void guiWelcomeWindowHandleEvent(GuiWindow * src, SDL_Event * e);

#endif
