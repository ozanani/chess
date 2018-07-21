#include "GuiButton.h"
#include <stdio.h>
#include <stdlib.h>

GuiWidget* createButton(
	SDL_Renderer* renderer,
	const char* image,
	SDL_Rect location,
	void(*action)(void))
{
	// allocate data
	GuiWidget* res = (GuiWidget*)malloc(sizeof(GuiWidget));
	if (res == NULL)
		return NULL;

	GuiButton* data = (GuiButton*)malloc(sizeof(GuiButton));
	if (data == NULL) {
		free(res);
		return NULL;
	}

	// we use the surface as a temp var
	SDL_Surface* surface = SDL_LoadBMP(image);
	if (surface == NULL) {
		free(res);
		free(data);
		return NULL;
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (texture == NULL) {
		free(res);
		free(data);
		SDL_FreeSurface(surface);
		return NULL;
	}

	// surface is not needed after texture is created
	SDL_FreeSurface(surface);

	// store button & widget details
	data->texture = texture;
	data->location = location;
	data->action = action;
	res->destroy = destroyButton;
	res->draw = drawButton;
	res->handleEvent = handleButtonEvent;
	res->data = data;
	return res;
}

void destroyButton(GuiWidget* src)
{
	GuiButton* button = (GuiButton*)src->data;
	SDL_DestroyTexture(button->texture);
	free(button);
	free(src);
}

void handleButtonEvent(GuiWidget* src, SDL_Event* e)
{
	if (e->type == SDL_MOUSEBUTTONUP) {
		GuiButton* button = (GuiButton*)src->data;
		SDL_Point point = { .x = e->button.x,.y = e->button.y };

		if (SDL_PointInRect(&point, &button->location)) {
			button->action();
		}
	}
}

void drawButton(GuiWidget* src, SDL_Renderer* render)
{
	GuiButton* button = (GuiButton*)src->data;
	SDL_RenderCopy(render, button->texture, NULL, &button->location);
}
