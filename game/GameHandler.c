#include "GameHandler.h"

GameHandler * gameHandlerNewGame(GhSettings settings) {
	GameHandler * gh = malloc(sizeof(GameHandler));
	if (gh == NULL) return NULL;

	gh->settings.gameMode = settings.gameMode;
	gh->settings.difficultyLevel = settings.difficultyLevel;
	gh->settings.userColor = settings.userColor;

	gh->gameIsSaved = false;
	
	// the size of the history allows using the minimax algorithm
	gh->game = gameCreate(GH_DEFAULT_HISTORY_SIZE + gh->settings.difficultyLevel);
	if (gh->game == NULL) return NULL;

	return gh;
}

bool gameHandlerRestartGame(GameHandler * gh) {
	Game * prevGame = gh->game;

	gh->game = gameCreate(GH_DEFAULT_HISTORY_SIZE + gh->settings.difficultyLevel);
	if (gh->game == NULL) {
		gh->game = prevGame;
		return false;
	}

	gh->gameIsSaved = false;

	gameDestroy(prevGame);
	return true;
}

bool gameHandlerIsUserTurn(GameHandler * gh) {
	ChessPlayer currentPlayer;

	if (gh->settings.gameMode == GameModeMultiPlayer) return true;

	currentPlayer = gameGetCurrentPlayer(gh->game);

	return (currentPlayer == White && gh->settings.userColor == UserColorWhite) ||
		(currentPlayer == Black && gh->settings.userColor == UserColorBlack);
}

void gameHandlerDestroy(GameHandler * gh) {
	if (gh == NULL) return;

	gameDestroy(gh->game);
	free(gh);
}

GhSettings gameHandlerGetDefaultSettings() {
	GhSettings settings;

	settings.gameMode = GameModeSinglePlayer;
	settings.difficultyLevel = GameDifficultyEasy;
	settings.userColor = UserColorWhite;

	return settings;
}

void gameHandlerGameElementAddedToHistory(GameHandler * gh) {
	if (arrayListSize(gh->game->history) > GH_GAME_HISTORY_SIZE) {
		arrayListRemoveFirst(gh->game->history);
	}
}

void gameHandlerComputerTurn(GameHandler * gh) {
	Move suggestMove = minimaxSuggestMove(gh->game, gh->settings.difficultyLevel);
	gameSetMove(gh->game, suggestMove.oldSquare, suggestMove.newSquare);
	gameHandlerGameElementAddedToHistory(gh);
}

void gameHandlerPrintGameSettingsToFileHandler(FILE * fh, GhSettings settings) {
	char * difficulty;

	fprintf(fh, "SETTINGS:\n");
	fprintf(fh, "GAME_MODE: %s\n", (settings.gameMode == GameModeSinglePlayer) ? "1-player" : "2-player");

	if (settings.gameMode == GameModeSinglePlayer) {
		switch (settings.difficultyLevel) {
		case GameDifficultyAmateur:
			difficulty = "amateur";
			break;
		case GameDifficultyEasy:
			difficulty = "easy";
			break;
		case GameDifficultyModerate:
			difficulty = "moderate";
			break;
		case GameDifficultyHard:
			difficulty = "hard";
			break;
		case GameDifficultyExpert:
			difficulty = "expert";
			break;
		}

		fprintf(fh, "DIFFICULTY: %s\n", difficulty);
		fprintf(fh, "USER_COLOR: %s\n", settings.userColor == UserColorBlack ? "black" : "white");
	}
}

bool gameHandlerSaveGame(GameHandler * gh, char * path) {
	FILE * fh = fopen(path, "w");

	if (fh == NULL) return false;
	
	fprintf(fh, "%s\n", gameGetCurrentPlayerColorText(gh->game));
	gameHandlerPrintGameSettingsToFileHandler(fh, gh->settings);
	gamePrintGameBoardToFileHandler(fh, gh->game);

	if (fclose(fh) == 0) { // in success, fclose == 0
		gh->gameIsSaved = true;
		return true;
	}

	return false;
}

/*
Convert difficulty text to enum.
*/
static GhGameDifficultyLevel gameHandlerDifficultyTextToEnum(char * difficulty) {
	if (strcmp(difficulty, "easy") == 0) return GameDifficultyEasy;
	if (strcmp(difficulty, "moderate") == 0) return GameDifficultyModerate;
	if (strcmp(difficulty, "hard") == 0) return GameDifficultyHard;
	if (strcmp(difficulty, "expert") == 0) return GameDifficultyExpert;
	else return GameDifficultyAmateur;
}

GameHandler * gameHandlerLoadGame(char * path) {
	bool success = false, currentPlayerBlack = false;
	int numberOfPlayers, i, lineNum;
	char line[GH_SAVE_FILE_MAX_LINE_LENGTH];
	char difficulty[GH_MAX_DIFFICULTY_LENGTH], userColor[GH_MAX_USER_COLOR_LENGTH];
	GameHandler * gh = NULL;
	GhSettings settings = gameHandlerGetDefaultSettings();
	FILE * fh = fopen(path, "r");

	if (fh == NULL) return NULL;

	// this loop (actually, one iteration) is used to prevent repeating the error handling code
	do {
		// read first line - current player
		if (fscanf(fh, "%s\n", line) != 1) break;

		// set the current player. White always starts, so just change to black if it's the black player
		if (strcmp(line, "black") == 0) currentPlayerBlack = true;

		// SETTINGS line
		if (fgets(line, GH_SAVE_FILE_MAX_LINE_LENGTH - 1, fh) == NULL) break;

		// number of players
		if (fscanf(fh, "GAME_MODE: %d-player\n", &numberOfPlayers) != 1) break;
		settings.gameMode = (numberOfPlayers == 1) ? GameModeSinglePlayer : GameModeMultiPlayer;

		if (numberOfPlayers == 1) {
			// difficulty
			if (fscanf(fh, "DIFFICULTY: %s\n", difficulty) != 1) break;
			settings.difficultyLevel = gameHandlerDifficultyTextToEnum(difficulty);

			// user color
			if (fscanf(fh, "USER_COLOR: %s\n", userColor) != 1) break;
			settings.userColor = (strcmp(userColor, "white") == 0) ? UserColorWhite : UserColorBlack;
		}

		// create the game handler
		gh = gameHandlerNewGame(settings);

		if (gh == NULL) break;

		// check if need to switch players
		if (currentPlayerBlack) gameChangePlayer(gh->game);

		// set board!
		for (i = BOARD_ROWS_NUMBER - 1; i >= 0; i--) {
			if (fscanf(fh, "%d| %c %c %c %c %c %c %c %c |\n", 
				&lineNum, 
				&gh->game->gameBoard[i][0], &gh->game->gameBoard[i][1],
				&gh->game->gameBoard[i][2], &gh->game->gameBoard[i][3],
				&gh->game->gameBoard[i][4], &gh->game->gameBoard[i][5],
				&gh->game->gameBoard[i][6], &gh->game->gameBoard[i][7]) 
				!= BOARD_COLUMNS_NUMBER + 1) break;
		}

		// finished scanning all lines successfully
		if (i == -1) success = true;
	} while (0);

	fclose(fh);
	
	if (!success) {
		gameHandlerDestroy(gh);
		return NULL;
	}

	gh->gameIsSaved = true;
	gh->game->isWhiteKingChecked = gameBoardKingIsChecked(gh->game->gameBoard, White);
	gh->game->isBlackKingChecked = gameBoardKingIsChecked(gh->game->gameBoard, Black);

	return gh;
}