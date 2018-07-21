#include "ConsoleGame.h"

GhSettings settings;
GameHandler * gh = NULL;
bool gameBoardChanged = false;

static void errorLibFunc(char * funcName) {
	printf(ERROR_LIB_FUNC, funcName);
}

static void consoleGameFreeAndExit() {
	gameHandlerDestroy(gh);
	exit(0);
}

/**
*  Gets a command from the user and removes trailing \n.
*  @param cmd the command
*  @return the command without trailing \n
*/
static char* getCommandFromUserStripped(char * cmd) {
	if (fgets(cmd, MAX_LINE_LENGTH + 1, stdin) == NULL) {
		return NULL;
	}

	for (int i = 0; i < MAX_LINE_LENGTH + 1 && cmd[i] != NULL_CHARACTER; i++) {
		if (cmd[i] == '\n') {
			cmd[i] = NULL_CHARACTER;
		}
	}

	return cmd;
}

static void consoleGameHandleCmdInvalid() {
	printf("ERROR: invalid command\n");
}

static void consoleGameHandleCmdGameMode(Command parsedCmd) {
	if (parsedCmd.validArg && (parsedCmd.arg1 == GAME_MODE_ONE_PLAYER || parsedCmd.arg1 == GAME_MODE_TWO_PLAYERS)) {
		if (parsedCmd.arg1 == GAME_MODE_ONE_PLAYER) settings.gameMode = GameModeSinglePlayer;
		else settings.gameMode = GameModeMultiPlayer;

		printf("Game mode is set to %s\n", (settings.gameMode == GameModeSinglePlayer) ? "1-player" : "2-player");
	}

	else printf("Wrong game mode\n");
}

static void consoleGameHandleCmdDifficultyLevel(Command parsedCmd) {
	char * difficulty;

	if (settings.gameMode != GameModeSinglePlayer) consoleGameHandleCmdInvalid();

	else if (parsedCmd.validArg && parsedCmd.arg1 >= GAME_DIFFICULTY_MIN_LEVEL && parsedCmd.arg1 <= GAME_DIFFICULTY_MAX_LEVEL) {
		switch (parsedCmd.arg1) {
		case 1:
			difficulty = "amateur";
			settings.difficultyLevel = GameDifficultyAmateur;
			break;
		case 2:
			difficulty = "easy";
			settings.difficultyLevel = GameDifficultyEasy;
			break;
		case 3:
			difficulty = "moderate";
			settings.difficultyLevel = GameDifficultyModerate;
			break;
		case 4:
			difficulty = "hard";
			settings.difficultyLevel = GameDifficultyHard;
			break;
		case 5:
			difficulty = "expert";
			settings.difficultyLevel = GameDifficultyExpert;
			break;
		}

		printf("Difficulty level is set to %s\n", difficulty);
	}

	else printf("Wrong difficulty level. The value should be between 1 to 5\n");
}

static void consoleGameHandleCmdUserColor(Command parsedCmd) {
	if (settings.gameMode != GameModeSinglePlayer) consoleGameHandleCmdInvalid();

	else if (parsedCmd.validArg && (parsedCmd.arg1 == 0 || parsedCmd.arg1 == 1)) {
		if (parsedCmd.arg1 == 0) settings.userColor = UserColorBlack;
		else settings.userColor = UserColorWhite;

		printf("User color is set to %s\n", settings.userColor == UserColorBlack ? "black" : "white");
	}

	else printf("Wrong user color. The value should be 0 or 1\n");
}

static void consoleGameHandleCmdDefault() {
	settings = gameHandlerGetDefaultSettings();

	printf("All settings reset to default\n");
}

static void consoleGameHandleCmdPrintSettings() {
	gameHandlerPrintGameSettingsToFileHandler(stdout, settings);
}

static void consoleGameHandleCmdQuit() {
	printf("Exiting...\n");
	consoleGameFreeAndExit();
}

static void consoleGameHandleCmdStart() {
	printf("Starting game...\n");
}

/*
True iff the game has been loaded successfully.
*/
static bool consoleGameHandleCmdLoad(Command parsedCmd) {
	if (!parsedCmd.validArg) {
		consoleGameHandleCmdInvalid();
		return false;
	}

	gh = gameHandlerLoadGame(parsedCmd.path);
	
	if (gh == NULL) {
		printf("Error: File doesn't exist or cannot be opened\n");
		return false;
	}

	return true;
}

static CgMsg consoleGameRunSettingsState(bool useDefaultSettings) {
	char cmd[MAX_LINE_LENGTH + 1];
	Command parsedCmd;
	bool gameLoaded = false;

	if (useDefaultSettings) settings = gameHandlerGetDefaultSettings();

	printf("Specify game settings or type 'start' to begin a game with the current settings:\n");

	do {
		// get command
		if (getCommandFromUserStripped(cmd) == NULL) { // EOF reached
			consoleGameHandleCmdQuit();
			return CgMsgQuit;
		}
		parsedCmd = parserParseLine(cmd);

		switch (parsedCmd.cmdType)
		{
		case CMD_TYPE_GAME_MODE:
			consoleGameHandleCmdGameMode(parsedCmd);
			break;
		case CMD_TYPE_DIFFICULTY:
			consoleGameHandleCmdDifficultyLevel(parsedCmd);
			break;
		case CMD_TYPE_USER_COLOR:
			consoleGameHandleCmdUserColor(parsedCmd);
			break;
		case CMD_TYPE_LOAD:
			// check if load succeeded. Work on the loaded game settings.
			if (consoleGameHandleCmdLoad(parsedCmd)) {
				gameLoaded = true;
				settings = gh->settings;
			}
			break;
		case CMD_TYPE_DEFAULT:
			consoleGameHandleCmdDefault();
			break;
		case CMD_TYPE_PRINT_SETTINGS:
			consoleGameHandleCmdPrintSettings();
			break;
		case CMD_TYPE_QUIT:
			consoleGameHandleCmdQuit();
			return CgMsgQuit;
		case CMD_TYPE_START:
			consoleGameHandleCmdStart();

			// if the game has been loaded, save the modified settings and run it.
			if (gameLoaded) {
				gh->settings = settings;
				return CgMsgLoad;
			}
			return CgMsgStart;
		case CMD_TYPE_INVALID_LINE:
		default:
			consoleGameHandleCmdInvalid();
			break;
		}
	} while (1);
}

static void consoleGameBoardChanged() {
	gameBoardChanged = true;
	gh->gameIsSaved = false;

	switch (gameCheckWinner(gh->game)) {
	case GAME_CHECK_WINNER_CONTINUE:
		break;
	case GAME_CHECK_WINNER_DRAW:
		printf("The game ends in a draw\n");
		consoleGameFreeAndExit();
		return;
	case GAME_CHECK_WINNER_CURRENT_PLAYER_LOSE:
		// the winner is NOT the current player
		printf("Checkmate! %s player wins the game\n", gameGetCurrentPlayer(gh->game) == White ? "black" : "white");
		consoleGameFreeAndExit();
		return;
	}

	if (gameIsCurrentPlayerChecked(gh->game)) printf("Check: %s king is threatened\n", gameGetCurrentPlayerColorText(gh->game));
}

/*
Takes arguments of cell and convert them to BoardSquare.
*/
static BoardSquare consoleGameConvertArgsToBoardSquare(int row, char col) {
	return (BoardSquare) { row - 1, col - 'A' };
}

static void consoleGameHandleCmdMove(Command parsedCmd) {
	if (!parsedCmd.validArg) {
		consoleGameHandleCmdInvalid();
		return;
	}
	
	// convert the characters into the corresponding int, and call setMove
	switch (gameSetMove(gh->game, consoleGameConvertArgsToBoardSquare(parsedCmd.arg1, parsedCmd.arg2), 
		consoleGameConvertArgsToBoardSquare(parsedCmd.arg3, parsedCmd.arg4))) {

	case GAME_INVALID_SQUARE:
		printf("Invalid position on the board\n");
		break;
	case GAME_INVALID_PIECE:
		printf("The specified position does not contain your piece\n");
		break;
	case GAME_MOVE_INVALID:
		printf("Illegal move\n");
		break;
	case GAME_MOVE_FAILED_KING_THREAT:
	case GAME_MOVE_FAILED_THREAT_CAPTURE:
		if (gameIsCurrentPlayerChecked(gh->game)) printf("Illegal move: king is still threatened\n");
		else printf("Illegal move: king will be threatened\n");
		break;
	case GAME_MOVE_SUCCESS_CAPTURE:
	case GAME_MOVE_SUCCESS:
		consoleGameBoardChanged();
		break;
	default:
		break;
	}

	gameHandlerGameElementAddedToHistory(gh);
}

static void gameConsolePrintBoardSquare(BoardSquare s) {
	printf("<%d,%c>", s.row + 1, s.col + 'A');
}

static void consoleGameHandleCmdGetMoves(Command parsedCmd) {
	MovesBoardWithTypes movesBoardWithTypes = { {BoardSquareInvalidMove} };

	if (!parsedCmd.validArg) {
		consoleGameHandleCmdInvalid();
		return;
	}

	switch (gameGetMovesWrapper(gh->game, consoleGameConvertArgsToBoardSquare(parsedCmd.arg1, parsedCmd.arg2),
		movesBoardWithTypes)) {
	case GAME_INVALID_SQUARE:
		printf("Invalid position on the board\n");
		break;
	case GAME_INVALID_PIECE:
		printf("The specified position does not contain a player piece\n");
		break;
	case GAME_SUCCESS:
		for (int i = 0; i < BOARD_ROWS_NUMBER; i++) {
			for (int j = 0; j < BOARD_COLUMNS_NUMBER; j++) {
				if (gameIsValidMove(movesBoardWithTypes[i][j])) {

					gameConsolePrintBoardSquare((BoardSquare) { i, j });

					// add the specific indicators
					switch (movesBoardWithTypes[i][j]) {
					case BoardSquareCaptureMove:
						printf("^");
						break;
					case BoardSquareThreatMove:
						printf("*");
						break;
					case BoardSquareCaptureAndThreatMove:
						printf("*^");
						break;
					default:
						break;
					}

					printf("\n");
				}
			}
		}
	default:
		break;
	}
}

static void consoleGameHandleCmdUndoMove() {
	HistoryElement histElem;

	// check if no history
	if (arrayListIsEmpty(gh->game->history)) {
		printf("Empty history, no move to undo\n");
		return;
	}

	// do twice if possible
	for (int i = 0; i < 2; i++) {
		// if no more history (only on the second undo) - break
		if (arrayListIsEmpty(gh->game->history)) break;

		// save history element and undo move
		histElem = arrayListGetLast(gh->game->history);
		gameUndoPrevMove(gh->game);

		// print the undo
		printf("Undo move for %s player: ", gameGetCurrentPlayerColorText(gh->game));
		gameConsolePrintBoardSquare(histElem.newSquare);
		printf(" -> ");
		gameConsolePrintBoardSquare(histElem.oldSquare);
		printf("\n");
	}
	
	gamePrintBoard(gh->game);
}

/*
Returns a string that the piece represents.
Assumes piece is one of the valid pieces.
*/
static char * consoleGameGetPieceText(char piece) {
	piece = tolower(piece);

	switch (piece) {
	case PIECE_BISHOP:
		return "bishop";
	case PIECE_KING:
		return "king";
	case PIECE_KNIGHT:
		return "knight";
	case PIECE_PAWN:
		return "pawn";
	case PIECE_QUEEN:
		return "queen";
	case PIECE_ROOK:
		return "rook";
	}

	return ""; // for compilation
}

static void consoleGameHandleComputerMove() {
	HistoryElement histElemComputerMove;

	// move and save the move
	gameHandlerComputerTurn(gh);
	histElemComputerMove = arrayListGetLast(gh->game->history);

	// print the move
	printf("Computer: move %s at ",
		consoleGameGetPieceText(gh->game->gameBoard[histElemComputerMove.newSquare.row][histElemComputerMove.newSquare.col]));
	gameConsolePrintBoardSquare(histElemComputerMove.oldSquare);
	printf(" to ");
	gameConsolePrintBoardSquare(histElemComputerMove.newSquare);
	printf("\n");

	consoleGameBoardChanged();
}

static void consoleGameHandleCmdSave(Command parsedCmd) {
	if (!parsedCmd.validArg) {
		consoleGameHandleCmdInvalid();
		return;
	}

	if (gameHandlerSaveGame(gh, parsedCmd.path)) printf("Game saved to: %s\n", parsedCmd.path);
	else printf("File cannot be created or modified\n");
}

/*
Settings state must be run before game state.
@param gameLoaded - indicates whether the game was loaded with the load command
*/
static CgMsg consoleGameRunGameState(bool gameLoaded) {
	char cmd[MAX_LINE_LENGTH + 1];
	Command parsedCmd;
	
	// game not loaded - create new game
	if (!gameLoaded) {
		gh = gameHandlerNewGame(settings);

		if (gh == NULL) {
			errorLibFunc("malloc");
			return CgMsgMallocError;
		}
	}

	// print the board at the start only if it's user turn
	if (gameHandlerIsUserTurn(gh)) gamePrintBoard(gh->game);

	do {
		// user turn
		if (gameHandlerIsUserTurn(gh)) {
			if (gameBoardChanged) {
				gamePrintBoard(gh->game);
				gameBoardChanged = false;
			}

			printf("Enter your move (%s player):\n", gameGetCurrentPlayerColorText(gh->game));

			if (getCommandFromUserStripped(cmd) == NULL) { // EOF reached
				consoleGameHandleCmdQuit();
				return CgMsgQuit;
			}
			parsedCmd = parserParseLine(cmd);

			switch (parsedCmd.cmdType) {
			case CMD_TYPE_MOVE:
				consoleGameHandleCmdMove(parsedCmd);
				break;
			case CMD_TYPE_GET_MOVES:
				consoleGameHandleCmdGetMoves(parsedCmd);
				break;
			case CMD_TYPE_UNDO:
				consoleGameHandleCmdUndoMove();
				break;
			case CMD_TYPE_RESET:
				printf("Restarting...\n");
				gameHandlerDestroy(gh);
				gh = NULL;
				return CgMsgReset;
			case CMD_TYPE_QUIT:
				consoleGameHandleCmdQuit();
				return CgMsgQuit;
			case CMD_TYPE_SAVE:
				consoleGameHandleCmdSave(parsedCmd);
				break;
			case CMD_TYPE_INVALID_LINE:
			default:
				consoleGameHandleCmdInvalid();
				break;
			}
		}

		// computer turn
		else {
			consoleGameHandleComputerMove();
		}

	} while (1);
	
}

void consoleGameRun() {
	bool useDefaultSettings = true; // first run - default settings

	printf(" Chess\n");
	printf("-------\n");

	do {
		switch (consoleGameRunSettingsState(useDefaultSettings)) {
		case CgMsgStart:
			useDefaultSettings = false; // default settings only on the first run
			consoleGameRunGameState(false); 
			break;
		case CgMsgLoad:
			useDefaultSettings = false;
			consoleGameRunGameState(true);
			break;
		case CgMsgQuit:
			break;
		default:
			break;
		}
	} while (1);

}