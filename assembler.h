/*
General header file for the assembly. 
Contains macros, data structures and methods declaration.

=======================
By Lotem Ben Yaakov
ID 206663932
=======================
*/

#ifndef ASSEMBLER_H
	#define ASSEMBLER_H

#include <stdio.h>
#include <string.h>

/* ======== Macros ======== */
/* Utilities */
#define FOREVER				for(;;)
#define BYTE_SIZE			8
#define FALSE				0
#define TRUE				1

/* Given Constants */
#define MAX_DATA_NUM		1000
#define FIRST_ADDRESS		100 
#define MAX_LINE_LENGTH		80
#define MAX_LABEL_LENGTH	30
#define MEMORY_WORD_LENGTH	12
#define MAX_REGISTER_DIGIT	7

/* Defining Constants */
#define MAX_LINES_NUM		1000
#define MAX_LABELS_NUM		MAX_LINES_NUM 

/* ======== Data Structures ======== */
typedef unsigned int bool; /* Only get TRUE or FALSE values */

/* === First Read  === */

/* Labels Management */
typedef struct
{
	int address;					/* The address it contains */
	char name[MAX_LABEL_LENGTH];	/* The name of the label */					
	bool isExtern;					/* Extern flag */
	bool isData;					/* Data flag (.data or .string) */
} labelInfo;

/* Directive And Commands */
typedef struct 
{
	char *name;
	void (*parseFunc)();
} directive;

typedef struct
{
	char *name;
	unsigned int opcode : 4;
	int numOfParams;
} command;

typedef struct
{
	char *name;
	int value;
} reg;

/* Operands */
typedef enum { NUMBER =1, LABEL = 3,  REGISTER = 5, INVALID = -1 } opType;

typedef struct
{
	int value;				/* Value */
	char *str;				/* String */
	opType type;			/* Type */
	int address;			/* The adress of the operand in the memory */
} operandInfo;

/* Line */
typedef struct
{
	int lineNum;				/* The number of the line in the file */
	int address;				/* The address of the first word in the line */
	char *originalString;		/* The original pointer, allocated by malloc */
	char *lineStr;				/* The text it contains (changed while using parseLine) */
	bool isError;				/* Represent whether there is an error or not */
	labelInfo *label;			/* A poniter to the lines label in labelArr */

	char *commandStr;			/* The string of the command or directive */

	/* Command line */
	const command *cmd;			/* A pointer to the command in g_cmdArr */
	operandInfo op1;			/* The 1st operand */
	operandInfo op2;			/* The 2nd operand */
} lineInfo;

/* === Second Read  === */

typedef enum { ABSOLUTE = 0, EXTENAL = 1, RELOCATABLE = 2 }AREType;

/* Memory Word */

typedef struct /* 12 bits */
{
	unsigned int ARE : 2;

	union /* 10 bits */
	{
		/* Commands (only 10 bits) */
		struct
		{
			unsigned int dest : 3;		/* Destination op addressing method ID */
            unsigned int opcode : 4;	/* Command ID */
			unsigned int src : 3;		/* Source op addressing method ID */
		} cmdBits;

		/* Registers (only 10 bits) */
		struct
		{
			unsigned int destBits : 5;
			unsigned int srcBits : 5;
		} regBits;

		/* Other operands */
		int value : 12; /* (12 bits) */

	} valueBits; /* End of 10 bits union */

} memoryWord;


/* ======== Methods Declaration ======== */
/* utility.c methods */
int comID(char *com);
labelInfo *locateLabelPointer(char *nameOfLabel);
void cutFromLeftStr(char **stPointer);
void cutEdgesStr(char **stPointer);
char *findFirstCh(char *str, char **finSign);
bool wordWithoutSpaces(char *str);
bool spacesAndTabs(char *str);
bool correctLabel(char *sLabel, int numOfLine, bool isError);
bool labelExist(char *label);
bool entryLabel(char *nameOfLabel);
bool isRegister(char *str, int *value);
bool isCommentOrEmpty(lineInfo *line);
char *findFOperand(char *line, char **finOp, bool *commaFind);
bool isDirective(char *com);
bool correctString(char **str, int numOfLine);
bool correctNum(char *stNumber, int bitNumber, int numOfLine, int *value);
int intToBase64(unsigned int in, char *ans);
int intToBase64(unsigned int in, char *ans);
void fprintfBase64(FILE *file, int num, int strMinWidth);
void fprintfBase10(FILE *file, int num, int strMinWidth);

/* firstRead.c methods */
int firstFileRead(FILE *file, lineInfo *linesArr, int *linesFound, int *IC, int *DC);

/* secondRead.c methods */
int secondFileRead(int *memoryArr, lineInfo *linesArr, int lineNum, int IC, int DC);

/* main.c methods */
void printError(int lineNum, const char *format, ...);

#endif