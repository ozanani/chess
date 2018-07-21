#include "GuiWindow.h"

void destroyWindow(GuiWindow * w) {
	if (w) w->destroy(w);
}