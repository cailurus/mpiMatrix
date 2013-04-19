/*
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tdarray.h"

int createTdArray(int nsize, int X, int Y, void *** parray)
{
    void ** tdarray = NULL;
    char * tmparray = NULL;
    int i = 0;
    *parray = NULL;
    if (!(tdarray = (void **)malloc(sizeof(float) * Y))) {
        printf("********************\n");
        return MNOMEMORY;
    }
    if (!(tmparray = (char *)malloc(nsize * (X * Y)))) {
        FREE(tdarray);
        printf("++++++++++++++++++++\n");
        return MNOMEMORY;
    }
    memset(tmparray, 0x00, nsize * (X * Y));
    for (i = 0; i < Y; i++)
        tdarray[i] = (tmparray + (i * X) * nsize);
    *parray = tdarray;
    return SUCCESS;
}

void freeTdArray(void *** parray)
{
    if (*parray) {
        FREE((*parray)[0]);
        FREE((*parray));
    }
}

