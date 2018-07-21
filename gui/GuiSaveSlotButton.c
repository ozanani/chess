#include "GuiSaveSlotButton.h"
#include <stdio.h>
#include <stdlib.h>

GuiWidget* createSaveSlotButton(
	SDL_Renderer* renderer,
	const char* image,
	SDL_Rect location,
	int slotNum,
	bool saveRequested,
	GameHandler * gh,
	void(*action)(int slotNum, bool saveRequested, GameHandler * gh))
{
	// allocate data
	GuiWidget* res = (GuiWidget*)malloc(sizeof(GuiWidget));
	if (res == NULL)
		return NULL;

	GuiSaveSlotButton* data = (GuiSaveSlotButton*)malloc(sizeof(GuiSaveSlotButton));
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
	data->slotNum = slotNum;
	data->saveRequested = saveRequested;
	data->gh = gh;
	res->destroy = destroySaveSlotButton;
	res->draw = drawSaveSlotButton;
	res->handleEvent = handleSaveSlotButtonEvent;
	res->data = data;
	return res;
}

void destroySaveSlotButton(GuiWidget* src)
{
	GuiSaveSlotButton* button = (GuiSaveSlotButton*)src->data;
	SDL_DestroyTexture(button->texture);
	free(button);
	free(src);
}

void handleSaveSlotButtonEvent(GuiWidget* src, SDL_Event* e)
{
	if (e->type == SDL_MOUSEBUTTONUP) {
		GuiSaveSlotButton* button = (GuiSaveSlotButton*)src->data;
		SDL_Point point = { .x = e->button.x,.y = e->button.y };

		if (SDL_PointInRect(&point, &button->location)) {
			button->action(button->slotNum, button->saveRequested, button->gh);
		}
	}
}

void drawSaveSlotButton(GuiWidget* src, SDL_Renderer* render)
{
	GuiSaveSlotButton* button = (GuiSaveSlotButton*)src->data;
	SDL_RenderCopy(render, button->texture, NULL, &button->location);
}
