/*
 *
 */
#include <stdlib.h>


#define SUCCESS 0 /*No error*/
#define MFAILED 1 /*General failure*/
#define MNOMEMORY 2 /*Out of memory*/

#define FREE(name) if (NULL != name) free(name);name = NULL;

int createTdArray(int, int, int, void ***); 
void freeTdArray(void ***);
