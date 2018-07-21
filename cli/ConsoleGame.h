#ifndef CONSOLE_GAME_H_
#define CONSOLE_GAME_H_

#include <stdio.h>
#include "Parser.h"
#include "GameHandler.h"

/*
This module encapsulates the CLI game.
*/

#define GAME_MODE_ONE_PLAYER 1
#define GAME_MODE_TWO_PLAYERS 2
#define GAME_DIFFICULTY_MIN_LEVEL 1
#define GAME_DIFFICULTY_MAX_LEVEL 5

typedef enum return_msg_e {
	CgMsgQuit,
	CgMsgStart,
	CgMsgMallocError,
	CgMsgReset,
	CgMsgLoad
} CgMsg;

typedef enum error_type_e {
	CgErrCmdInvalid,
} CgErrorType;

/*
Runs a chess console game.
*/
void consoleGameRun();

#endif