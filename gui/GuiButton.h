#ifndef BUTTON_H_
#define BUTTON_H_

#include "GuiWidget.h"
#include <SDL.h>
#include <SDL_video.h>

/*
A button widget.
*/

typedef struct button_t {
	SDL_Renderer* render;
	SDL_Texture* texture;
	SDL_Rect location;
	void(*action)(void);
} GuiButton;

GuiWidget* createButton(
	SDL_Renderer* renderer,
	const char* image,
	SDL_Rect location,
	void(*action)(void));
void destroyButton(GuiWidget* src);
void handleButtonEvent(GuiWidget* src, SDL_Event* event);
void drawButton(GuiWidget*, SDL_Renderer*);

#endif 
