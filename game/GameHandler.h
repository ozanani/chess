#ifndef GAME_HANDLER_H_
#define GAME_HANDLER_H_

#include "Minimax.h"

#define GH_DEFAULT_HISTORY_SIZE 7 // 3 for each user, 1 for threat checking
#define GH_GAME_HISTORY_SIZE 6 // 3 for each user
#define GH_SAVE_FILE_MAX_LINE_LENGTH 40
#define GH_MAX_DIFFICULTY_LENGTH 10
#define GH_MAX_USER_COLOR_LENGTH 10

/*
This module is responsible of handling a game, both in GUI and CLI modes.
*/

/*
Default: Single mode
*/
typedef enum game_mode_e {
	GameModeSinglePlayer,
	GameModeMultiPlayer
} GhGameMode;

/*
Default:  Easy
*/
typedef enum difficulty_level_e {
	GameDifficultyAmateur = 1,
	GameDifficultyEasy = 2,
	GameDifficultyModerate = 3,
	GameDifficultyHard = 4,
	GameDifficultyExpert = 5
} GhGameDifficultyLevel;

/*
Default: white
Single mode only.
*/
typedef enum user_color_e {
	UserColorWhite,
	UserColorBlack
} GhUserColor;

/*
Game settings.
*/
typedef struct gh_settings_t {
	GhGameMode gameMode;
	GhGameDifficultyLevel difficultyLevel;
	GhUserColor userColor;
} GhSettings;

/*
Game handler struct - the game and its' settings.
*/
typedef struct gh_t {
	GhSettings settings;
	Game * game;

	bool gameIsSaved;
} GameHandler;

/*
Creates a new game instance with the given settings.
@param GhSettings the game settings
@return the game handler instance or NULL if malloc has failed
*/
GameHandler * gameHandlerNewGame(GhSettings settings);

/*
Restarts the game with the current settings.
@param gh the game handler
@return
True iff restart succeeded
*/
bool gameHandlerRestartGame(GameHandler * gh);

/*
Destroys the game handler instance and frees all memory.
@param gh the game handler to destroy
*/
void gameHandlerDestroy(GameHandler * gh);

/*
Returns the default settings of the game.
*/
GhSettings gameHandlerGetDefaultSettings();

/*
Returns true iff it's a user turn (and not the computer turn).
*/
bool gameHandlerIsUserTurn(GameHandler * gh);

/*
Manages the game history in order to keep place for game-handling operations.
*/
void gameHandlerGameElementAddedToHistory(GameHandler * gh);

/*
Makes a computer's move.
*/
void gameHandlerComputerTurn(GameHandler * gh);

/*
Saves the game to the specified path.
@param gh the game handler
@param path the path to save to
@return
true iff the game has been successfully saved
*/
bool gameHandlerSaveGame(GameHandler * gh, char * path);

/*
Print the game settings to the file handler (stdout or file on disk).
@param fh the file
@param settings the game settings to print
*/
void gameHandlerPrintGameSettingsToFileHandler(FILE * fh, GhSettings settings);

/*
Loads a game from the specified path.
Assumes the file is valid and is in the right format.
@param path the file path
@return 
gameHandler if the game was successfully restored
otherwise NULL
*/
GameHandler * gameHandlerLoadGame(char * path);

#endif