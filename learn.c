#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include "mpi.h" 

int displayMatrix(float** p, const char * matrixName, int row, int col){
	int i , j;
	printf("--%s-", matrixName);
	for( i = 0; i<row; i++){
		for( j = 0; j<col; j++){
			printf("%8.2f\n", p[i][j]);
		}
		printf("\n");
	}
}

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
createTdArray(sizeof(float), 4, 4, &A);
displayMatrix(A, "A", 4, 4)