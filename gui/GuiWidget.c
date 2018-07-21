#include "GuiWidget.h"

void destroyWidget(GuiWidget* src)
{
	if (src != NULL) src->destroy(src);
}