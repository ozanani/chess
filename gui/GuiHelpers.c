#include "GuiHelpers.h"

SDL_Texture * guiTextureFromBMP(SDL_Renderer * rend, char * imagePath) {
	SDL_Surface * surface = SDL_LoadBMP(imagePath);
	SDL_Texture * texture = NULL;

	if (surface == NULL) {
		printf("ERROR: loading BMP failed: %s\n", SDL_GetError());
		return NULL;
	}
	
	texture = SDL_CreateTextureFromSurface(rend, surface);
	SDL_FreeSurface(surface);

	if (texture == NULL) printf("ERROR: texture creation failed: %s\n", SDL_GetError());

	return texture;
}

void guiPushUserEvent(GuiUserEventCode code, void * data1, void * data2) {
	SDL_Event e;
	SDL_memset(&e, 0, sizeof(e));
	
	e.type = SDL_USEREVENT;
	e.user.code = code;
	e.user.data1 = data1;
	e.user.data2 = data2;
	
	SDL_PushEvent(&e);
}

SDL_Texture * guiTransparentTextureFromBMP(SDL_Renderer * rend, char * imagePath, Uint8 r, Uint8 g, Uint8 b) {
	SDL_Surface * surface = SDL_LoadBMP(imagePath);
	SDL_Texture * texture = NULL;

	if (surface == NULL) {
		printf("ERROR: loading BMP failed: %s\n", SDL_GetError());
		return NULL;
	}

	// set a specific color to transparent
	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, r, g, b));

	texture = SDL_CreateTextureFromSurface(rend, surface);
	SDL_FreeSurface(surface);

	if (texture == NULL) printf("ERROR: texture creation failed: %s\n", SDL_GetError());

	return texture;
}

void guiShowMessageBox(const char * title, const char * msg) {
	// the if statements catches en error
	if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
		title,
		msg,
		NULL) < 0) {
		printf("ERROR: message box creation failed: %s\n", SDL_GetError());
		printf("The message:\n%s: %s\n", title, msg);
	}
}

bool guiDoesFileExist(const char *fname) {
	FILE *file;
	if ((file = fopen(fname, "r")))
	{
		fclose(file);
		return 1;
	}
	return 0;
}