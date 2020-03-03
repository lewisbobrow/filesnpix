/*
* readaline.c
* Lewis Bobrow, Lawrence Xu, January 18th 2020
* HW1
*
* Reads a line from an input location
*/

#include <stdio.h>
#include <stdlib.h>
#include "readaline.h"
#include "assert.h"
#include "string.h"


/* READ A LINE
* I: inputfd, a file pointer. datapp, pointer to where line will be returned
* O: size of line read (includes \0)
* Purpose: Reads a line from a file, adding a null terminator to the end.
           Returns the size of the line read, datapp receives the line read.
*/
size_t  readaline (FILE *inputfd, char **datapp)
{
    /* when eof is reached - base case*/
    if (feof(inputfd)) {
        *datapp = NULL;
        return 0;
    }

    /* null arguments exception */
    assert(inputfd != NULL && datapp != NULL);

    /* creating space */
    int max = 200;
    char *line = calloc(max, sizeof(char));

    /* check if memory allocation failed */
    assert(line != NULL);

    int i = 0;
    while (i < max) {

        /* if max size reached, realloc */
        if (i == max - 1) {
            max *= 2;
            line = realloc(line, max);
            assert(line != NULL);
        }

        line[i] = fgetc(inputfd);

        /* checking for end of the line */
        if (line[i] == '\n') {
            i++;
            break;
        } else if (feof(inputfd)) {
            i++;
            break;
        }
        i++;
    }

   /* adding null terminator */
    line[i] = '\0';

    *datapp = line;

    return i;
}
