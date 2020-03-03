/*
* simlines.c
* Lewis Bobrow, Lawrence Xu, January 24th 2020
* HW1/Lab1
*
* Finds similar lines and prints them out
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "readaline.h"
#include "list.h"
#include "table.h"
#include "atom.h"
#include "assert.h"


char* stripLine(char *line, size_t letterLength);
void insertLine(char *line,
                char *filename,
                int lineNum,
                Table_T table,
                size_t size,
                int *matchp);

void printMatches(const void *key, void **value, void *cl);
void printLine(char *file, int line);

/* struct that holds the filename and line number of line read.
*  will be stored in the List
*/
typedef struct LineData {
    char *filename;
    int lineNum;
} Data;

int main (int argc, char *argv[])
{
    Table_T table = Table_new(0, NULL, NULL);

    /* to keep track of number of matches */
    int matches = 0;

    /* reads lines from each file */
    int i;
    for (i = 1; i < argc; i++) {

        FILE *fp = fopen(argv[i], "r");
        assert(fp != NULL);

        int j = 1;
        char *cur = NULL;

        do {
            /* create store for readaline */
            size_t len = readaline(fp, &cur);
            if (cur == NULL) {
                break;
            }

            /* insert into Table */
            insertLine(cur, argv[i], j, table, len, &matches);

            j++;
        } while (cur != NULL);

        fclose(fp);
    }

    /* print matches */
    Table_map(table, printMatches, &matches);

    /* free */
    Table_free(&table);

    exit(EXIT_SUCCESS);

    return 0;
}

/* stripLine
* I: line, the string that needs to be stripped, letterLength, the size of the
     string.
* O: returns the string stripped of its symbols and extra spaces
* Purpose: cleans the string of extra symbols and spaces to be inserted into
*           the table when converted into an atom
*/
char* stripLine(char *line, size_t letterLength)
{
    char *newLine;
    newLine = calloc(letterLength, sizeof(char));
    /* check if memory allocation failed */
    assert(newLine != NULL);

    int i;
    int anumCounter = 0;
    bool word = false;
    for (i = 0; i < (int)letterLength; i++) {

        if (isalnum(line[i]) || line[i] == 95) {
            word = true;
        } else {
            /* if prev char is alphanumeric, put space */
            if (word > 0) {
                newLine[anumCounter] = ' ';
                anumCounter++;
            }
            word = false;
        }

        if (word) {
            newLine[anumCounter] = line[i];
            anumCounter++;
        }
    }

    /* remove space at end */
    if (anumCounter > 1) {
        if (newLine[anumCounter - 1] == ' ') {
            newLine[anumCounter - 1] = '\0';
        }
    }

    /* free the line, which was malloc'd in readaline (as datapp) */
    free(line);

    return newLine;
}


/* insertLine
* I: line, line read in by readaline
     filename, name of the file holding the line
     linenumber, linebumber of the line in the file
     table, table that holds all the lines - the database
     size, size of the line read by readaline
* O: nothing
* Purpose: strips the line, converts it into an atom, and inserts it
            into the table
*/
void insertLine(char *line,
                char *filename,
                int lineNum,
                Table_T table,
                size_t size,
                int *matchp)
{
    /* check if line is empty */
    if (size <= 1) {
        /* no use for line anymore, free it (from readaline as datapp) */
        free(line);
        return;
    }

    /* clear symbols */
    char *stripped = stripLine(line, size);

    /* case for if line is ONLY symbols */
    if (strlen(stripped) <= 1) {
        free(stripped);
        return;
    }

    /* make into atom */
    const char *atomLine = Atom_string(stripped);

    /* atom made, line can be freed */
    free(stripped);

    /* create struct to be inserted into List in the table */
    Data *lineInfo = (Data*) malloc(sizeof(Data));
    /* checks if memory allocation failed */
    assert(lineInfo != NULL);

    lineInfo->filename = filename;
    lineInfo->lineNum = lineNum;

    /* insert into table */
    if ((Table_get(table, atomLine)) == NULL) {
        /* list not found, create one */
        List_T lineList = List_list(lineInfo, NULL);
        Table_put(table, atomLine, lineList);

    } else {
        /* line already found, add in */
        List_T existList = Table_get(table, atomLine);
        if (List_length(existList) == 1) {
            *matchp += 1;
        }

        existList = List_push(existList, lineInfo);

        /* update table with list */
        Table_put(table, atomLine, existList);

        /* update matches */
        *matchp += 1;
    }
}

/* printMatches
* Purpose: a MAP FUNCTION. Used to print out all the matching lines
           in correct format. Intended to only be used by Table_map().
*/
void printMatches(const void *key, void **value, void *cl)
{
    List_T tempList = *value;
    int *matches = cl;

    /* check if matches exist */
    if (List_length(tempList) > 1) {
        /* printing specified output */
        const char *tempKey = key;
        printf("%s\n", tempKey);

        /* reverse order of list */
        tempList = List_reverse(tempList);
        /* add a Null so List_length works */
        tempList = List_append(tempList, NULL);

        while (List_length(tempList) != 0) {
            Data *tempLine;
            tempList = List_pop(tempList, (void**)&tempLine);

            printLine(tempLine->filename, tempLine->lineNum);
            free(tempLine);

            /* after one Data has been popped, reduce matches */
            *matches -= 1;

            printf("\n");
        }
        if (*matches > 0) {
            printf("\n");
        }

    } else if (List_length(tempList) == 1) {
        /* free lists that only hold one struct */
        Data *lonelyLine;
        tempList = List_pop(tempList, (void**)&lonelyLine);

        free(lonelyLine);
    }

    List_free(&tempList);
}

/* printLine
* I: file, the name of the file
     line, the linenumber
* O: nothing
* Purpose: Prints out the filename & linenumber for the matching line in the
           required format.
*/
void printLine(char *file, int line)
{
    if (strlen(file) > 20) {
        printf("%s %d", file, line);
    } else {
        /* first print filename */
        printf("%s", file);

        /* print spaces */
        int spaces = 21 - (int)strlen(file);
        for (int i = 0; i < spaces; i++) {
            printf(" ");
        }

        /* print linenumber */
        char lineHolder[7];
        sprintf(lineHolder, "%d", line);
        printf("%7s", lineHolder);
    }
}
