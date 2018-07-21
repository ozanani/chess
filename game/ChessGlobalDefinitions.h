#ifndef GLOBAL_DEFINITIONS_H_
#define GLOBAL_DEFINITIONS_H_

#define SEP " \t\r\n" // seperator
#define ERROR_LIB_FUNC "ERROR: function \"%s\" has failed."

/*
This struct is located in this file because it is needed in multiple files.
*/
typedef struct square_t {
	int row;
	int col;
} BoardSquare;

#endif