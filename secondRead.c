/*
This file gets the data structures from the first file read, and convert them into bits.

=======================
By Lotem Ben Yaakov
ID 206663932
=======================
*/

/* ======== Includes ======== */
#include "assembler.h"

/* ====== Externs ====== */
/* Use the commands list from firstRead.c */
extern const command g_cmdArr[];

/* Use the data from firstRead.c */
extern labelInfo g_labelArr[MAX_LABELS_NUM];
extern int g_labelNum;
lineInfo *g_entryLines[MAX_LABELS_NUM];
extern int g_entryLabelsNum;
extern int g_dataArr[MAX_DATA_NUM];

/* ====== Methods ====== */

/* Updates the addresses of all the data labels in g_labelArr. */
void updateDataLabelsAddress(int IC)
{
	int i;

	/* Search in the array for label with isData flag */
	for (i = 0; i < g_labelNum; i++)
	{
		if (g_labelArr[i].isData)
		{
			/* Increase the adress */
			g_labelArr[i].address += IC;
		}
	}
}

/* Returns if there is an illegal entry line in g_entryLines. */
int countIllegalEntries()
{
	int i, ret = 0;
	labelInfo *label;

	for (i = 0; i < g_entryLabelsNum; i++)
	{
		label = locateLabelPointer(g_entryLines[i]->lineStr);
		if (label)
		{
			if (label->isExtern)
			{
				printError(g_entryLines[i]->lineNum, "The parameter for .entry can't be an external label.");
				ret++;
			}
		}
		else
		{
			printError(g_entryLines[i]->lineNum, "No such label as \"%s\".", g_entryLines[i]->lineStr);
			ret++;
		}
	}

	return ret;
}



/* If the op is a label, this method is updating the value of the it to be the address of the label. */
/* Returns FALSE if there is an error, or TRUE otherwise. */
bool updateLableOpAddress(operandInfo *op, int lineNum)
{
	if (op->type == LABEL)
	{

			labelInfo *label = locateLabelPointer(op->str);

			/* Check if op.str is a real label name */
			if (label == NULL)
			{
				/* Print errors (legal name is illegal or not exists yet) */
				if (correctLabel(op->str, lineNum, TRUE))
				{
					printError(lineNum, "No such label as \"%s\"", op->str);
				}
				return FALSE;
			}
			op->value = label->address;
	}

	return TRUE;
}

/* Returns the int value of a memory word. */
int getNumFromMemoryWord(memoryWord memory)
{
	/* Create an int of "MEMORY_WORD_LENGTH" times '1', and all the rest are '0' */
	unsigned int mask = ~0;
	mask >>= (sizeof(int) * BYTE_SIZE - MEMORY_WORD_LENGTH);

	/* The mask makes sure we only use the first "MEMORY_WORD_LENGTH" bits */
	return mask & ((memory.valueBits.value << 2) + memory.ARE);
}

/* Returns the id of the addressing method of the opAREnd */
int getOpTypeId(operandInfo op)
{
	/* Check if the opAREnd have legal type */
	if (op.type != INVALID)
	{
		/* immediate = 1, LABEL = 3, REGISTER = 5 */
		return  (int)op.type;
	}

	return 0;
}

/* Returns a memory word which represents the command in a line. */
memoryWord getCmdMemoryWord(lineInfo line)
{
	memoryWord memory = { 0 };

	/* Update all the bits in the command word */
	memory.ARE = (AREType)ABSOLUTE; /* Commands are absolute */
	memory.valueBits.cmdBits.dest = getOpTypeId(line.op2);
	memory.valueBits.cmdBits.src = getOpTypeId(line.op1);
	memory.valueBits.cmdBits.opcode = line.cmd->opcode;
	return memory;
}

/* Returns a memory word which represents the opAREnd (assuming it's a valid opAREnd). */
memoryWord getOpMemoryWord(operandInfo op, bool isDest)
{
	memoryWord memory = { 0 };

	/* Check if it's a register or not */
	if (op.type == REGISTER)
	{
		memory.ARE = (AREType)ABSOLUTE; /* Registers are absolute */

		/* Check if it's the dest or src */
		if (isDest)
		{
			memory.valueBits.regBits.destBits = op.value;
		}
		else
		{
			memory.valueBits.regBits.srcBits = op.value;
		}
	}
	else
	{
		labelInfo *label = locateLabelPointer(op.str);

		/* Set ARE */	
		if (op.type == LABEL && label && label->isExtern)
		{
			memory.ARE = EXTENAL;
		}
		else
		{
			memory.ARE = (op.type == NUMBER) ? (AREType)ABSOLUTE : (AREType)RELOCATABLE;
		}

		memory.valueBits.value = op.value;
	}

	return memory;
}

/* Adds the value of memory word to the memoryArr, and increase the memory counter. */
void addWordToMemory(int *memoryArr, int *memoryCounter, memoryWord memory)
{
	/* Check if memoryArr isn't full yet */
	if (*memoryCounter < MAX_DATA_NUM)
	{
		/* Add the memory word and increase memoryCounter */
		memoryArr[(*memoryCounter)++] = getNumFromMemoryWord(memory);
	}
}

/* Adds a whole line into the memoryArr, and increase the memory counter. */
bool addLineToMemory(int *memoryArr, int *memoryCounter, lineInfo *line)
{
	bool foundError = FALSE;

	/* Don't do anything if the line is error or if it's not a command line */
	if (!line->isError && line->cmd != NULL)
	{
		/* Update the label opAREnds value */
		if (!updateLableOpAddress(&line->op1, line->lineNum) || !updateLableOpAddress(&line->op2, line->lineNum))
		{
			line->isError = TRUE;
			foundError = TRUE;
		}

		/* Add the command word to the memory */
		addWordToMemory(memoryArr, memoryCounter, getCmdMemoryWord(*line));

		if (line->op1.type == REGISTER && line->op2.type == REGISTER)
		{
			/* Create the memory word */
			memoryWord memory = { 0 };
			memory.ARE = (AREType)ABSOLUTE; /* Registers are absolute */
			memory.valueBits.regBits.destBits = line->op2.value;
			memory.valueBits.regBits.srcBits = line->op1.value;

			/* Add the memory to the memoryArr array */
			addWordToMemory(memoryArr, memoryCounter, memory);
		}
		else
		{
			/* Check if there is a source opAREnd in this line */
			if (line->op1.type != INVALID)
			{
				/* Add the op1 word to the memory */
				line->op1.address = FIRST_ADDRESS + *memoryCounter;
				addWordToMemory(memoryArr, memoryCounter, getOpMemoryWord(line->op1, FALSE));
				/* ^^ The FALSE param means it's not the 2nd op */
			}

			/*Check if there is a destination opAREnd in this line */
			if (line->op2.type != INVALID)
			{
				/* Add the op2 word to the memory */
				line->op2.address = FIRST_ADDRESS + *memoryCounter;
				addWordToMemory(memoryArr, memoryCounter, getOpMemoryWord(line->op2, TRUE));
				/* ^^ The TRUE param means it's the 2nd op */
			}
		}
	}

	return !foundError;
}

/* Adds the data from g_dataArr to the end of memoryArr. */
void addDataToMemory(int *memoryArr, int *memoryCounter, int DC)
{
	int i;
	/* Create an int of "MEMORY_WORD_LENGTH" times '1', and all the rest are '0' */
	unsigned int mask = ~0;
	mask >>= (sizeof(int) * BYTE_SIZE - MEMORY_WORD_LENGTH);

	/* Add each int from g_dataArr to the end of memoryArr */
	for (i = 0; i < DC; i++)
	{
		if (*memoryCounter < MAX_DATA_NUM)
		{
			/* The mask makes sure we only use the first "MEMORY_WORD_LENGTH" bits */
			memoryArr[(*memoryCounter)++] = mask & g_dataArr[i];
		}
		else
		{
			/* No more space in memoryArr */
			return;
		}
	}
}

/* Reads the data from the first read for the second time. */
/* It converts all the lines into the memory. */
int secondFileRead(int *memoryArr, lineInfo *linesArr, int lineNum, int IC, int DC)
{
	int errorsFound = 0, memoryCounter = 0, i;

	/* Update the data labels */
	updateDataLabelsAddress(IC);

	/* Check if there are illegal entries */
	errorsFound += countIllegalEntries();

	/* Add each line in linesArr to the memoryArr */
	for (i = 0; i < lineNum; i++)
	{
		if (!addLineToMemory(memoryArr, &memoryCounter, &linesArr[i]))
		{
			/* An error was found while adding the line to the memory */
			errorsFound++;
		}
	}

	/* Add the data from g_dataArr to the end of memoryArr */
	addDataToMemory(memoryArr, &memoryCounter, DC);

	return errorsFound;
}