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
    //分配指针数组
    if (!(tdarray = (void **)malloc(sizeof(float) * Y))) {
        printf("********************\n");
        return MNOMEMORY;
    }
    //分配实际数组空间
    if (!(tmparray = (char *)malloc(nsize * (X * Y)))) {
        FREE(tdarray);
        printf("++++++++++++++++++++\n");
        return MNOMEMORY;
    }
    //初始化内存
    memset(tmparray, 0x00, nsize * (X * Y));
    //指针数组赋值
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

