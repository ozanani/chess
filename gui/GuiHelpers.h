#ifndef GUI_HANDLER_H
#define GUI_HANDLER_H

#include <SDL.h>
#include <SDL_video.h>
#include "GameHandler.h"

/*
Common helpers for the gui modules.
*/

#define GUI_WINDOW_WIDTH 1024
#define GUI_WINDOW_HEIGHT 768

#define GUI_BUTTON_HEIGHT 72
#define GUI_BUTTON_WIDTH 343

#define GUI_BUTTON_SMALL_HEIGHT 54
#define GUI_BUTTON_SMALL_WIDTH 226

typedef enum user_event_code_e {
	GUI_USEREVENT_NEW_GAME,
	GUI_USEREVENT_LOAD_FROM_WELCOME_WINDOW,
	GUI_USEREVENT_LOAD_FROM_GAME_WINDOW,
	GUI_USEREVENT_SAVE_FROM_GAME_WINDOW,
	GUI_USEREVENT_BACK,
	GUI_USEREVENT_GAME_MODE_CHOSEN,
	GUI_USEREVENT_DIFFICULTY_CHOSEN,
	GUI_USEREVENT_COLOR_CHOSEN,
	GUI_USEREVENT_MSGBOX,
	GUI_USEREVENT_COMPUTER_TURN,
	GUI_USEREVENT_WELCOME_WINDOW,
	GUI_USEREVENT_RESTART,
	GUI_USEREVENT_UNDO, 
	GUI_USEREVENT_SAVE,
	GUI_USEREVENT_SLOTS_PAGE_CHANGED,
	GUI_USEREVENT_LOAD_SLOT_CHOSEN,
	GUI_USEREVENT_SAVE_SLOT_CHOSEN,
	GUI_USEREVENT_QUIT_FROM_GAME_WINDOW,
	GUI_USEREVENT_MENU_FROM_GAME_WINDOW

} GuiUserEventCode;

SDL_Texture * guiTextureFromBMP(SDL_Renderer * rend, char * imagePath);

/*
Creates a transparent texture from the specified bmp. The user has to specify the color that 
he want to make transparent.
@param rend the renderer
@param imagePath the image
@param r, g, b the color
@return a texture, or NULL if failed
*/
SDL_Texture * guiTransparentTextureFromBMP(SDL_Renderer * rend, char * imagePath, Uint8 r, Uint8 g, Uint8 b);

void guiPushUserEvent(GuiUserEventCode code, void * data1, void * data2);

/*
Displays a simple message box. If the call to SDL fails, prints the msg in the console.
@param title the title
@param msg the msg
*/
void guiShowMessageBox(const char * title, const char * msg);

/*
Returns true iff the file with the given file name exists.
@param fname the file name
@return true iff the file exists
*/
bool guiDoesFileExist(const char *fname);

#endif