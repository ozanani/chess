#ifndef WIDGET_H_
#define WIDGET_H_

#include <SDL.h>
#include <SDL_video.h>

typedef struct widget_t GuiWidget;
struct widget_t {
	void(*draw)(GuiWidget*, SDL_Renderer*);
	void(*handleEvent)(GuiWidget*, SDL_Event*);
	void(*destroy)(GuiWidget*);
	void* data;
};

void destroyWidget(GuiWidget* src);

#endif
