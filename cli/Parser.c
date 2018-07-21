#include "Parser.h"

/*
* Checks if the string is an integer number.
* Assumes str is of length 1024 at most, and str is not NULL.
*
* @return true iff the string represents an integer number.
*/
static bool isIntegerNumber(const char* str) {
	int i = 0;

	// check first digit
	if (!isdigit(str[i])) {
		if (str[i] != '-' && str[i] != '+')
			return false;

		// negative number or starts with +
		if (!isdigit(str[++i]))
			return false;
	}

	for (; i < MAX_LINE_LENGTH; i++) {
		if (str[i] == '\0')
			return true;
		if (!isdigit(str[i]))
			return false;
	}

	// no null terminator
	return false;
}

bool parserIsInt(const char* str) {
	char str_copied[MAX_LINE_LENGTH + 1], *token;
	long long result;

	if (str == NULL) return false;

	// copy the line to prevent a case where str is not null terminated
	strncpy(str_copied, str, MAX_LINE_LENGTH);
	str_copied[MAX_LINE_LENGTH] = '\0';

	// get the number token
	token = strtok(str_copied, SEP);

	if (token == NULL) {
		return false;
	}

	if (!isIntegerNumber(token))
		return false;

	// see that there are no more token
	token = strtok(NULL, SEP);

	if (token != NULL) {
		return false;
	}

	// check if the number can be converted to long
	result = strtoll(str, NULL, 10);

	// if the number is 0, the check isn't ok
	//if (!result)
	//	return false;

	// the number is not in the integer range
	if (!(INT_MIN <= result && result <= INT_MAX))
		return false;

	return true;
}

/*
Sets the correct command type.
Assuming cmd and cmdToken are not null.
*/
static void setCommandType(Command * cmd, char * token) {

	if (strcmp(token, CMD_GAME_MODE) == 0) cmd->cmdType = CMD_TYPE_GAME_MODE;
	else if (strcmp(token, CMD_DIFFICULTY) == 0) cmd->cmdType = CMD_TYPE_DIFFICULTY;
	else if (strcmp(token, CMD_USER_COLOR) == 0) cmd->cmdType = CMD_TYPE_USER_COLOR;
	else if (strcmp(token, CMD_LOAD) == 0) cmd->cmdType = CMD_TYPE_LOAD;
	else if (strcmp(token, CMD_DEFAULT) == 0) cmd->cmdType = CMD_TYPE_DEFAULT;
	else if (strcmp(token, CMD_PRINT_SETTINGS) == 0) cmd->cmdType = CMD_TYPE_PRINT_SETTINGS;
	else if (strcmp(token, CMD_QUIT) == 0) cmd->cmdType = CMD_TYPE_QUIT;
	else if (strcmp(token, CMD_START) == 0) cmd->cmdType = CMD_TYPE_START;
	else if (strcmp(token, CMD_MOVE) == 0) cmd->cmdType = CMD_TYPE_MOVE;
	else if (strcmp(token, CMD_GET_MOVES) == 0) cmd->cmdType = CMD_TYPE_GET_MOVES;
	else if (strcmp(token, CMD_SAVE) == 0) cmd->cmdType = CMD_TYPE_SAVE;
	else if (strcmp(token, CMD_UNDO) == 0) cmd->cmdType = CMD_TYPE_UNDO;
	else if (strcmp(token, CMD_RESET) == 0) cmd->cmdType = CMD_TYPE_RESET;

	else cmd->cmdType = CMD_TYPE_INVALID_LINE;
}

/*
Parses the token as coordinates of cell in the board. Fills the arguments with the correct
data in case of success and returns true, or returns false if the token is not a valid cell representation.
Assumes non of the arguments is null.
*/
static bool parseCellArgs(int *row, char *col, char *token) {
	int ret;
	unsigned int len, commaCount = 0;
	char *tokenPtr;

	len = strlen(token);

	// we need to count the commas, because more than one comma is an invalid command
	for (unsigned int i = 1; i < len - 1; i++) {
		if (token[i] == ',') commaCount++;
	}

	// check structure
	if (!(token[0] == '<' && token[len - 1] == '>' && commaCount == 1)) return false;

	// if the length neq 5, the structure is valid but the args are not.
	if (len != 5) {
		*row = -1;
		*col = 'Z';
		return true;
	}

	ret = sscanf(token, "<%d,%c>", row, col);

	switch (ret) {
	case 2: // 2 variables have been filled with data

		// check that the string is not followed by invalid character
		tokenPtr = strstr(token, ">");
		if (tokenPtr[1] != '\0') return false;

		break;

	case EOF:
		printf(ERROR_LIB_FUNC, "sscanf");
		return false; // to prevent FALLTHROUGH warning
	default: // invalid format
		break;
	}

	return true;
}

/*
Sets the command arguments after its type has already been checked and set. 
Assumes both cmd and token are not null, and cmd->validArg is false.
This function should be called after calling setCommandType, and when cmd is not invalid command.
*/
static void setCommandArguments(Command *cmd, char *token) {
	token = strtok(NULL, SEP);

	// check if second token is valid param
	if (token == NULL) return;
		
	switch (cmd->cmdType) {
	case CMD_TYPE_INVALID_LINE:
		break;

	case CMD_TYPE_GAME_MODE:
	case CMD_TYPE_DIFFICULTY:
	case CMD_TYPE_USER_COLOR:
		if (!parserIsInt(token)) return;

		cmd->arg1 = atoi(token);
		break;

	case CMD_TYPE_LOAD:
	case CMD_TYPE_SAVE:
		cmd->path = token;
		break;

	case CMD_TYPE_MOVE:
		if (!parseCellArgs(&cmd->arg1, &cmd->arg2, token)) return;

		// second token - CMD_MOVE_CELLS_SEPERATOR
		token = strtok(NULL, SEP);
		if (token == NULL || strcmp(token, CMD_MOVE_CELLS_SEPERATOR) != 0) return;

		token = strtok(NULL, SEP);
		if (token == NULL) return;

		if (!parseCellArgs(&cmd->arg3, &cmd->arg4, token)) return;
		break;

	case CMD_TYPE_GET_MOVES:
		if (!parseCellArgs(&cmd->arg1, &cmd->arg2, token)) return;
		break;

	default:
		break;
	}

	cmd->validArg = true;
}

Command parserParseLine(char* str) {
	Command cmd;
	char *token;
	
	// set default values
	cmd.cmdType = CMD_TYPE_INVALID_LINE;
	cmd.validArg = false;

	// check if the string is null
	if (str == NULL) return cmd;

	// get the first token - command string
	token = strtok(str, SEP);
	if (token == NULL) return cmd;

	setCommandType(&cmd, token);
	if (cmd.cmdType == CMD_TYPE_INVALID_LINE) return cmd;

	setCommandArguments(&cmd, token);

	return cmd;
}