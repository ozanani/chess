#ifndef PARSER_H_
#define PARSER_H_

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include "ChessGlobalDefinitions.h"

#define MAX_LINE_LENGTH 1024
#define NULL_CHARACTER '\0'
#define CMD_MOVE_CELLS_SEPERATOR "to"

// commands
#define CMD_GAME_MODE "game_mode"
#define CMD_DIFFICULTY "difficulty"
#define CMD_USER_COLOR "user_color"
#define CMD_LOAD "load"
#define CMD_DEFAULT "default"
#define CMD_PRINT_SETTINGS "print_settings"
#define CMD_QUIT "quit"
#define CMD_START "start"
#define CMD_MOVE "move"
#define CMD_GET_MOVES "get_moves"
#define CMD_SAVE "save"
#define CMD_UNDO "undo"
#define CMD_RESET "reset"

//a type used to represent a command
typedef enum {
	// all states commands
	CMD_TYPE_QUIT,
	CMD_TYPE_INVALID_LINE,
	
	// setting state commands
	CMD_TYPE_GAME_MODE,
	CMD_TYPE_DIFFICULTY,
	CMD_TYPE_USER_COLOR,
	CMD_TYPE_LOAD,
	CMD_TYPE_DEFAULT,
	CMD_TYPE_PRINT_SETTINGS,
	CMD_TYPE_START,

	// game state commands
	CMD_TYPE_MOVE,
	CMD_TYPE_GET_MOVES,
	CMD_TYPE_SAVE,
	CMD_TYPE_UNDO,
	CMD_TYPE_RESET
} COMMAND_TYPE;

// encapsulation of a parsed line
typedef struct command_t {
	COMMAND_TYPE cmdType;
	bool validArg; // is set to true if the line contains valid arguments
	int arg1;
	char arg2;
	int arg3;
	char arg4;

	char * path;
} Command;

/**
* Checks if a specified string represents a valid integer. It is recommended
* to use this function prior to calling the standard library function atoi.
*
* @return
* true if the string represents a valid integer, and false otherwise.
*/
bool parserIsInt(const char* str);

/**
* Parses a specified line. If the line is a command which has an integer
* argument then the argument is parsed and is saved in the field arg and the
* field validArg is set to true. In any other case then 'validArg' is set to
* false and the value 'arg' is undefined
*
* @return
* A parsed line such that:
*   cmd - contains the command type, if the line is invalid then this field is
*         set to CMD_TYPE_INVALID_LINE
*   validArg - is set to true if the command is add_disc and the integer argument
*              is valid
*   arg      - the integer argument in case validArg is set to true
*/
Command parserParseLine(char* str);

#endif
