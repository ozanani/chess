#ifndef WINDOW_H_
#define WINDOW_H_

#include <SDL.h>
#include <SDL_video.h>

/*
This is a generic window which provides an interface for common actions. Each window is contained by this structure - 
the data property is the specific (sub-class) window.

data - the specific window data
draw - renders the window and present it on the screen
handleEvent - handle an SDL_Event
destroy - destroys the window and frees its resources
*/

typedef struct window_t GuiWindow;

struct window_t {
	void * data;
	void(*draw)(GuiWindow*);
	void(*handleEvent)(GuiWindow*, SDL_Event*); 
	void(*destroy)(GuiWindow*);
};

/*
Destroys a window.
@param w the window to destroy
*/
void destroyWindow(GuiWindow* w);

#endif /* WINDOW_H_ */
