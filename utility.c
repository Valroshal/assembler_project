/*
This file contains auxiliary functions for the first and second pass. All methods are declared in assembler.h file.
Valeria Kabishcher
 ID 206663932
Asaf Badichi
ID 036243301
*/


/* Includes  */
#include "assembler.h"
#include <ctype.h>
#include <stdlib.h>

/*  Methods  */
extern const command g_cmdArr[];
extern labelInfo g_labelArr[];
extern int g_labelNum;
lineInfo *g_entryLines[MAX_LABELS_NUM];
extern int g_entryLabelsNum;
extern const reg g_regArr[];

/*The function returns the pointer of the label with needed name in array of labels.
* In case there is no such name the function will return NULL*/
labelInfo *locateLabelPointer(char *nameOfLabel)
{
	int i=0;

	if (nameOfLabel == '\0')
    {
        return NULL;
    }
    else
	{
        while(i<g_labelNum)
		{
			if (strcmp(nameOfLabel, g_labelArr[i].name) != 0)
			{
                i++;
			}
            else
            {
                return &g_labelArr[i];
            }
		}
	}
    return NULL;
}

/*the function checks if command exists. It returns the ID of the command by command name in command array, if command doesn't exist, function will return -1. */
int comID(char *com)
{
	int i = 0;

	while (g_cmdArr[i].name)
	{
		if (strcmp(com, g_cmdArr[i].name) != 0)
		{
            i++;
		}
        else
        {
            return i; /*index in array of commands*/
        }
	}
	return -1; /*does not exist*/
}

/* Removes spaces(whitespace, tabs, etc) from the beginnig of the text.  ???????????????????????????????????????????????????????*/
void cutFromLeftStr(char **stPointer)
{
	/* Return if string is NULL, no string to verify */
	if (stPointer == NULL || **stPointer == '\0')
	{
		return;
	}
    else
    {
	/* if there is a spase on the begining move forward till finding char, point out the char*/
	while (isspace(**stPointer))
    {
		++*stPointer;
	}
    }
}

/* Removes all the spaces from the edges of the string stPointer is pointing to. ?????????????????????????/*/
void cutEdgesStr(char **stPointer)
{
	char *endOfSt;

	/* Return if string is NULL, no string to verify */
	if (stPointer == NULL || **stPointer == '\0')
	{
		return;
	}

	cutFromLeftStr(stPointer);

	/*Set endOfSt to point to the last char in string, before '\0' */
	endOfSt = *stPointer + strlen(*stPointer) - 1;

	/* Remove spaces from the end */
	while (isspace(*endOfSt) && endOfSt != *stPointer)
	{
		*endOfSt-- = '\0';
	}
}
/*Set pointer to the first char in string*/
/*"findFirstChar" func made minor changes need more*/
/* if *finSign isn't NULL, function makes pointer to the last char after the symbol.????????????? */
char *findFirstCh(char *str, char **finSign)
{
	char *startCh = str;
	char *endCh = NULL;

	/* cut from the biginning */
	cutFromLeftStr(&startCh);

	/* Find the end of the first word */
	endCh = startCh;
	while (*endCh != '\0' && !isspace(*endCh))
	{
		endCh++;
	}

	/* Add '\0' at the end of the string, if doesn't exist  */
	if (*endCh != '\0')
	{
		*endCh = '\0';
		endCh++;
	}

	/* makes pointer to the last char after the symbol */
	if (finSign != NULL)
	{
		*finSign = endCh;
	}
	return startCh;
}

/* check if string is only 1 word long. if so return true , else return false*/
bool wordWithoutSpaces(char *str)
{
    cutFromLeftStr(&str); /*remove space at the beginning*/
    int i;
    for(i=0; i<strlen(str); i++)
    {
        if(isspace(*str))
        {
            return FALSE;
        }
    }
    return TRUE;
}

/*check if str contains only white spaces, if yes returns true, else return false*/
bool spacesAndTabs(char *str) 
{ 
    int count = 0; /*count spaces*/
    int length = strlen(str); 
    for (int i = 0; i < length; i++) { 
        int c = str[i]; 
        if (isspace(c)) 
            count++; 
    } 
    if(count == length) /*quantity of spaces equal to string length*/
        return TRUE;
    else
        return FALSE;
} 

/* If label is a correct label name this function returns true, else false.*/

bool correctLabel(char *sLabel, int numOfLine, bool isError) //minor changes
{
	int i = 1;
    /*Verify that string of label is not empty*/
	if (*sLabel == '\0')
	{
        if (isError) printf("Document line is %d. Error at line %d in function \"%s\". Label is empty.",numOfLine,((__LINE__)-3),__func__);	
        return FALSE;
    }
    else
        /*Verify that label not longer than maximum label length*/
        if (strlen(sLabel) > MAX_LABEL_LENGTH)
        {
            if (isError) printf("Document line is %d. Error at line %d in function \"%s\". Label name longer than needed. Valid length is %d.",numOfLine,((__LINE__)-3),__func__, MAX_LABEL_LENGTH);
            return FALSE;
        }

        /* Verify if the 1st char is at the start of the line, without spaces. */
        if (isspace(*sLabel))
        {
            if (isError) printf("Document line is %d. Error at line %d in function \"%s\". Label must start at the start of the line.",numOfLine,((__LINE__)-3),__func__);		return FALSE;
            return FALSE;
        }
        /* If the first char isn't a letter error and false */
        if (!isalpha(*sLabel))
        {
            if (isError) printf("Document line is %d. Error at line %d in function \"%s\". \"%s\" is illegal label - first char must be a letter.",numOfLine,((__LINE__)-3),__func__, sLabel);
            return FALSE;
        }
        /* Verify all characters, all of them need to bee chars and nums, if not - return false and error. */
        while( i < strlen(sLabel))
        {
            if (!isalnum(sLabel[i]))
            {
                if (isError) printf("Document line is %d. Error at line %d in function \"%s\". \"%s\" is illegal label - only letters and numbers .",numOfLine,((__LINE__)-3),__func__, sLabel);
                return FALSE;
            }
            i++;
        }
 
        /* Verify, label can't be the name of register, if it is return false and error */
        if (isRegister(sLabel, NULL)) /* NULL since we don't have to save the register number */
        {
            if (isError) printf("Document line is %d. Error at line %d in function \"%s\". \"%s\" is illegal label - don't use a name of a register.",numOfLine,((__LINE__)-3),__func__, sLabel);
            return FALSE;
        }

        /* Verify, label can't be the name of command, if it is return false and error */
        if(strcmp(sLabel, g_cmdArr[i].name) == 0) //if (comID(sLabel) != -1) 
        {
            if (isError) printf("Document line is %d. Error at line %d in function \"%s\". \"%s\" is illegal label - don't use a name of a command.",numOfLine,((__LINE__)-3),__func__, sLabel);
            return FALSE;
        }
	return TRUE;
}
/*boolean to verify if label exists*/
bool labelExist(char *label)
{
	if (locateLabelPointer(label)== NULL) 
	{
		return FALSE;
	}

	return TRUE;
}
/* boolean to verify if label exists in the array of entry lines . */
bool entryLabel(char *nameOfLabel)
{
	int i = 0;
	if ((nameOfLabel!= NULL) && (nameOfLabel[0] == '\0')) /*if string is empty. changed here*/
	{
		return FALSE;
	}
	else
	{
		for (i = 0; i < g_entryLabelsNum; i++)
		{
			if (strcmp(nameOfLabel, g_entryLines[i]->lineStr) == 0)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}
/* Verify if label has the name of register and set the value to be the register value. */
bool isRegister(char *str, int *value)
{
    
	int i=0;
    while  (g_regArr[i].name != NULL )
    {
        if (strcmp (str,g_regArr[i].name)==0 && value != NULL)
        {
           *value =  g_regArr[i].value;
           return TRUE;
        }
        i++;
    }
	return FALSE;
     
}
/* Return a bool that checks if the line is comment or empty, if not returns false. */
/* Update line->isError to be TRUE, if the char ';' exists but not at the beginning of the line */
bool isCommentOrEmpty(lineInfo *line) 
{
	char *textStartPoint = line->lineStr; /* line->lineStr not changed */

	if (*line->lineStr != ';')
	{
		cutFromLeftStr(&textStartPoint);/*go forward, to the next char*/
	
		if (*textStartPoint == '\0') /*empty*/
		{
		return TRUE;
		}
		if (*textStartPoint == ';')
		{
            /*comment is wrong, char ';' exists but placed not at the beginning*/
            printf("Document line is %d. Error at line %d in function \"%s\". Comments must start with ';' at the start of the line.",line->lineNum,((__LINE__)-3),__func__);
			line->isError = TRUE; /*was isError in original*/
			return TRUE;
		}
		return FALSE;
    }
	else {
	/* the line is comment */
		return TRUE;
	}
}
/*set pointer to the beginning of the first operand, set the end of the operand to '\0'*/
/*If operand is not NULL, point at the char after the operand */
char *findFOperand(char *line, char **finOp, bool *commaFind)
{
	if(spacesAndTabs(line))
	{
		*commaFind = FALSE; 
	}
	else 
	{
		/* find comma if exists */
		char *last = strchr(line, ',');
		if (NULL == last)
		{
			*commaFind = FALSE;
		}
		else
		{
			*commaFind = TRUE;
			*last = '\0';
			last++;
		}
        /*If finOp is not NULL set it to point to the char after the operand, if it's the end of the line - set point */
		if (finOp)
		{
			if (last)
			{
				*finOp = last;
			}
			else
			{
				*finOp = strchr(line, '\0');
			}
		}
	}

	cutEdgesStr(&line);
	return line;
}
/*Verify that command is directive*/
bool isDirective(char *com) /*new command, changed*/
{
	if(*com == '.')
	{
		return TRUE;
	}
	return FALSE;
}
/* Verify if string is correct, per specification enclosed in quotes, return true if so , and remove the quotes*/
bool correctString(char **str, int numOfLine)
{
    /* check that string has value, else return false, changed, need more changes*/
    if (**str == '\0')
	{
		printf("Document line is %d. Error at line %d in function \"%s\". No parameter.",numOfLine,((__LINE__)-3),__func__);
        return FALSE;
    }
	/* check if the string is enclosed in quotes(""), if not - string not correct, return false */
	if ((*str)[0] != '"' && (*str)[strlen(*str) - 1] != '"')
	{
        printf("Document line is %d. Error at line %d in function \"%s\". The parameter for .string must be enclosed in quotes.",numOfLine,((__LINE__)-3),__func__);
        return FALSE;
		
	}
	else
	{
        /* the string has quotes and it is legal, remove the quotes and return true */
		(*str)[strlen(*str) - 1] = '\0';
		++*str;
    }
    return TRUE;
}

/*Verify that number is correct per specifiction, save it in *value */
bool correctNum(char *stNumber, int bitNumber, int numOfLine, int *value)
{
	char *fin_num;
    /*check that fin_num is in range of maximum of positive and negative number */
	int biggestNum = (1 << bitNumber) - 1;

	if (spacesAndTabs(stNumber))
	{
		printf("Document line is %d. Error at line %d in function \"%s\". Empty parameter.",numOfLine,((__LINE__)-3),__func__);
		return FALSE;
	}

	*value = strtol(stNumber, &fin_num, 0);

	/* Check if fin_num is at the end of the string */
	if (*fin_num)
	{
        printf("Document line is %d. Error at line %d in function \"%s\".\"%s\" isn't a valid number.",numOfLine,((__LINE__)-3),__func__, stNumber);
		return FALSE;
	}

	/* Check if the number is small enough to fit into 1 memory word 
	(if the absolute value of number is smaller than 'biggestNum' */
	if (*value > biggestNum || *value < -biggestNum)
	{
        printf("Document line is %d. Error at line %d in function \"%s\". \"%s\" is too %s, must be between %d and %d.",numOfLine,((__LINE__)-3),__func__,stNumber, (*value > 0) ? "big" : "small", -biggestNum, biggestNum);
		return FALSE;
	}

	return TRUE;
}



int binarytodec(long num)

{
    int decimal_val = 0, base = 1, rem;
    while (num > 0)
    {
        rem = num % 10;
        decimal_val = decimal_val + rem * base;
        num = num / 10 ;
        base = base * 2;
    	}
    return decimal_val;
}

int str2int(int t[])
{
    long i, res = 0;
    for (i = 0; i < 6; i++)
    {
        res = 10 * res + t[i];
    }
return res;
} 

int intToBase64(unsigned int in, char *ans)
{  
const char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int len_digitis =12;
    int bits_first[6];
    int bits_second[6];
    int out_digit[12];
    int i = 0;
    int j = 0;
    int u = 0;
    unsigned int mask = 1U << (len_digitis-1);
    for (u = 0; u < len_digitis; u++) 
        {
            out_digit[u] = (in & mask) ? 1 : 0;
            in <<= 1;
        }
        while( i< 12)
        {
            if(i<6)
            {
            for(j=0; j<6; j++)
                    {
                bits_first[j] = out_digit[i];
                    i++;
                    }
            }
        else
            {
            for(j=0; j<6; j++)
                {
                        bits_second[j] = out_digit[i];
                        i++;
                }
            }
        }
    long one = str2int(bits_first);
    long one1 = binarytodec(one);
    long two = str2int(bits_second);
    long two2 = binarytodec(two);
    char a = b64chars[one1];
    char b = b64chars[two2];
    ans[0] = a;
    ans[1] = b;
    ans[2] = '\0';
    return 0;
}

void printStrWithZeros(char *str, int strMinWidth)
{
	int i, numOfZeros = strMinWidth - strlen(str);

	for (i = 0; i < numOfZeros; i++)
	{
		printf("0");
	}
	printf("%s", str);
}

/* Prints a number in base 64 in the file. */
void fprintfBase64(FILE *file, int num, int strMinWidth)
{
	int numOfZeros, i;
	/* 2 chars are enough to represent 12 bits in base 64, and the last char is \0. */
	char buf[3] = { 0 }; 

	intToBase64(num, buf);  /*intToBase64(num, buf);*/

	/* Add zeros first, to make the length at least strMinWidth */
	numOfZeros = strMinWidth - strlen(buf);
	for (i = 0; i < numOfZeros; i++)
	{
		fprintf(file, "0");
	}
	fprintf(file, "%s", buf);
}
/* Prints a number in base 10 in the file. */
void fprintfBase10(FILE *file, int num, int strMinWidth)
{
	int numOfZeros, i;
	/* 4 chars are enough to represent 12 bits in base 10, and the last char is \0. */
	char buf[5] = { 0 }; 

	sprintf(buf, "%d",num); 

	/* Add zeros first, to make the length at least strMinWidth */
	numOfZeros = strMinWidth - strlen(buf);
	for (i = 0; i < numOfZeros; i++)
	{
		fprintf(file, "0");
	}
	fprintf(file, "%s", buf);
}