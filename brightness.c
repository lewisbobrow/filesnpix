/*
* brightness.c
* Lewis Bobrow, Lawrence Xu, January 24th 2020
* HW1/Lab1
*
* Finds the average brightness of an image
*/

#include <stdio.h>
#include <stdlib.h>
#include "pnmrdr.h"
#include "assert.h"

int main(int argc, char *argv[])
{
    Pnmrdr_T reader;

    /* open file for reading */
    if (argc == 1) { /*no arguments*/
        reader = Pnmrdr_new(stdin);

    } else if (argc == 2) {
        FILE *fp = fopen(argv[1], "rb");
        reader = Pnmrdr_new(fp);

    } else {
        fprintf(stderr, "ERROR: %s has too many files requested!\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* process file */
    Pnmrdr_mapdata imageData = Pnmrdr_data(reader);

    /* throw error if type isn't right */
    assert(imageData.type == Pnmrdr_gray);

    /*loop through and get brightness of each pixel*/
    int size = imageData.height * imageData.width;
    /* throw error if size isn't right */
    assert(size > 0);


    int i;
    int sum = 0;
    for (i = 0; i < size; i++) {
        sum += Pnmrdr_get(reader);
    }

    /* return average */
    double result;
    result = (double)sum / imageData.denominator;

    printf("%.3f\n", result/size);
    Pnmrdr_free(&reader);

    exit(EXIT_SUCCESS);

    return 0;
}
