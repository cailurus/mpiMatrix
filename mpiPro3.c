/**********************************************************************************************
* Matrix Multiplication Program using MPI.
*
* Viraj Brian Wijesuriya - University of Colombo School of Computing, Sri Lanka.
* 
* Works with any type of two matrixes [A], [B] which could be multiplied to produce a matrix [c].
*
* Master process initializes the multiplication operands, distributes the muliplication 
* operation to worker processes and reduces the worker results to construct the final output.
*  
************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#define NUM_ROWS_A 1000 //rows of input [A]
#define NUM_COLUMNS_A 1000 //columns of input [A]
#define NUM_ROWS_B 1000 //rows of input [B]
#define NUM_COLUMNS_B 1000 //columns of input [B]
#define MASTER_TO_SLAVE_TAG 1 //tag for messages sent from master to slaves
#define SLAVE_TO_MASTER_TAG 4 //tag for messages sent from slaves to master
void makeAB(); //makes the [A] and [B] matrixes
void printArray(); //print the content of output matrix [C];
int rank; //process rank
int size; //number of processes
int i, j, k; //helper variables
long calTime;
double mat_a[NUM_ROWS_A][NUM_COLUMNS_A]; //declare input [A]
double mat_b[NUM_ROWS_B][NUM_COLUMNS_B]; //declare input [B]
double mat_result[NUM_ROWS_A][NUM_COLUMNS_B]; //declare output [C]
/*
double start_time; //hold start time
double start1_time; // hold the start time of input part
double start2_time; // hold the start time of the outout part
double start3_time; // hold the start time of doing calculation
double start4_time; // hold the end time of doing calculation
double end_time; // hold end time
*/
int low_bound; //low bound of the number of rows of [A] allocated to a slave
int upper_bound; //upper bound of the number of rows of [A] allocated to a slave
int portion; //portion of the number of rows of [A] allocated to a slave
MPI_Status status; // store status of a MPI_Recv
MPI_Request request; //capture request of a MPI_Isend
FILE *fa;
FILE *fb;
FILE *fc;
FILE *fd;
FILE *ff;

int main(int argc, char *argv[])
{
    calTime = 0;
    MPI_Init(&argc, &argv); //initialize MPI operations
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //get the rank
    MPI_Comm_size(MPI_COMM_WORLD, &size); //get number of processes
    /* master initializes work*/
    struct timeval start_time, start1_time, start2_time, start3_time, start4_time, end_time;
    if (rank == 0) {
        //printf("this is my first rank 0 : %i\n", rank);
        gettimeofday(&start1_time, NULL);
        //start1_time = MPI_Wtime();
        makeAB();
        gettimeofday(&start_time, NULL);
        //start_time = MPI_Wtime();
        for (i = 1; i < size; i++) {//for each slave other than the master
            portion = (NUM_ROWS_A / (size - 1)); // calculate portion without master
            low_bound = (i - 1) * portion;
            if (((i + 1) == size) && ((NUM_ROWS_A % (size - 1)) != 0)) {//if rows of [A] cannot be equally divided among slaves
                upper_bound = NUM_ROWS_A; //last slave gets all the remaining rows
            } else {
                upper_bound = low_bound + portion; //rows of [A] are equally divisable among slaves
            }
            //send the low bound first without blocking, to the intended slave
            MPI_Isend(&low_bound, 1, MPI_INT, i, MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD, &request);
            //next send the upper bound without blocking, to the intended slave
            MPI_Isend(&upper_bound, 1, MPI_INT, i, MASTER_TO_SLAVE_TAG + 1, MPI_COMM_WORLD, &request);
            //finally send the allocated row portion of [A] without blocking, to the intended slave
            MPI_Isend(&mat_a[low_bound][0], (upper_bound - low_bound) * NUM_COLUMNS_A, MPI_DOUBLE, i, MASTER_TO_SLAVE_TAG + 2, MPI_COMM_WORLD, &request);
        }
    }
    //broadcast [B] to all the slaves
    MPI_Bcast(&mat_b, NUM_ROWS_B*NUM_COLUMNS_B, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    /* work done by slaves*/
    if (rank > 0) {
        //printf("this is my rank %i\n", rank);
        //receive low bound from the master
        MPI_Recv(&low_bound, 1, MPI_INT, 0, MASTER_TO_SLAVE_TAG, MPI_COMM_WORLD, &status);
        //next receive upper bound from the master
        MPI_Recv(&upper_bound, 1, MPI_INT, 0, MASTER_TO_SLAVE_TAG + 1, MPI_COMM_WORLD, &status);
        //finally receive row portion of [A] to be processed from the master
        MPI_Recv(&mat_a[low_bound][0], (upper_bound - low_bound) * NUM_COLUMNS_A, MPI_DOUBLE, 0, MASTER_TO_SLAVE_TAG + 2, MPI_COMM_WORLD, &status);
        gettimeofday(&start3_time, NULL);
        for (i = low_bound; i < upper_bound; i++) {//iterate through a given set of rows of [A]
            for (j = 0; j < NUM_COLUMNS_B; j++) {//iterate through columns of [B]
                for (k = 0; k < NUM_ROWS_B; k++) {//iterate through rows of [B]
                    mat_result[i][j] += (mat_a[i][k] * mat_b[k][j]);
                }
            }
        }
        gettimeofday(&start4_time, NULL);
        //send back the low bound first without blocking, to the master
        MPI_Isend(&low_bound, 1, MPI_INT, 0, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD, &request);
        //send the upper bound next without blocking, to the master
        MPI_Isend(&upper_bound, 1, MPI_INT, 0, SLAVE_TO_MASTER_TAG + 1, MPI_COMM_WORLD, &request);
        //finally send the processed portion of data without blocking, to the master
        MPI_Isend(&mat_result[low_bound][0], (upper_bound - low_bound) * NUM_COLUMNS_B, MPI_DOUBLE, 0, SLAVE_TO_MASTER_TAG + 2, MPI_COMM_WORLD, &request);
    }
    /* master gathers processed work*/
    if (rank == 0) {
        //printf("this is my second rank 0 : %i\n", rank);
        for (i = 1; i < size; i++) {// untill all slaves have handed back the processed data
            //receive low bound from a slave
            MPI_Recv(&low_bound, 1, MPI_INT, i, SLAVE_TO_MASTER_TAG, MPI_COMM_WORLD, &status);
            //receive upper bound from a slave
            MPI_Recv(&upper_bound, 1, MPI_INT, i, SLAVE_TO_MASTER_TAG + 1, MPI_COMM_WORLD, &status);
            //receive processed data from a slave
            MPI_Recv(&mat_result[low_bound][0], (upper_bound - low_bound) * NUM_COLUMNS_B, MPI_DOUBLE, i, SLAVE_TO_MASTER_TAG + 2, MPI_COMM_WORLD, &status);
        }
        
        //printArray();
        gettimeofday(&start2_time, NULL);
        //start2_time = MPI_Wtime();
        outputArray();
        gettimeofday(&end_time, NULL);
        //end_time = MPI_Wtime();
        /*printf("\nInput time = %f\n", start_time - start1_time);
        printf("\nCalculation time = %f\n", start4_time - start3_time);
        printf("\nOutput time = %f\n", end_time - start2_time);
        printf("\nRunning time = %f\n", end_time - start1_time);

        printf("\nstart 4 = %f\n", start4_time);
        printf("\nstart 3 = %f\n", start3_time);
        printf("\nstart 2 = %f\n", start2_time);
        printf("\nstart 1 = %f\n", start1_time);
        printf("\nstart = %f\n", start_time);
        printf("\nend = %f\n", end_time);
        
        printf("\nInput Microseconds:%lu\n", (start_time.tv_sec - start1_time.tv_sec)*1000000+start_time.tv_usec - start1_time.tv_usec);
        printf("\nCalculation Microseconds:%lu\n", (start4_time.tv_sec - start3_time.tv_sec)*1000000+start4_time.tv_usec - start3_time.tv_usec);
        printf("\nOutput Microseconds:%lu\n", (end_time.tv_sec - start2_time.tv_sec)*1000000+end_time.tv_usec - start2_time.tv_usec);
        printf("\nRunning Microseconds:%lu\n", (end_time.tv_sec - start1_time.tv_sec)*1000000+end_time.tv_usec - start1_time.tv_usec);
        */
        fd = fopen("statistics", "a+");
        //fprintf(fd, "dimension is %d\n", NUM_COLUMNS_A);
        //input, calculation, output, running

        fprintf(fd, "%lu, %lu, %lu, %lu\n", (start_time.tv_sec - start1_time.tv_sec)*1000000+start_time.tv_usec - start1_time.tv_usec,

            (end_time.tv_sec - start1_time.tv_sec)*1000000+end_time.tv_usec - start1_time.tv_usec - 
            (end_time.tv_sec - start2_time.tv_sec)*1000000-end_time.tv_usec + start2_time.tv_usec - 
            (start_time.tv_sec - start1_time.tv_sec)*1000000-start_time.tv_usec + start1_time.tv_usec,

            (end_time.tv_sec - start2_time.tv_sec)*1000000+end_time.tv_usec - start2_time.tv_usec, 
            (end_time.tv_sec - start1_time.tv_sec)*1000000+end_time.tv_usec - start1_time.tv_usec);
        fclose(fd);
    }
    MPI_Finalize(); //finalize MPI operations
    //printf("%i\n", rank);
    //fprintf(fd, "%lu\n", (start4_tlsime.tv_sec - start3_time.tv_sec)*1000000+start4_time.tv_usec - start3_time.tv_usec);
    if(rank != 0){
        ff = fopen("calTime", "a+");
        fprintf(ff, "%lu\n", (start4_time.tv_sec - start3_time.tv_sec)*1000000+start4_time.tv_usec - start3_time.tv_usec);
        fclose(ff);
    }
    return 0;
}// end of main 
void makeAB()
{
    char *token;
    int arr[3];

        //arguments for read line by line
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    fa = fopen("testMatrix1", "r");
    fb = fopen("testMatrix2", "r");
//    gettimeofday(&beginTime, NULL);

    // all set to zero
    for (i = 0; i < NUM_ROWS_A; i++) {
        for (j = 0; j < NUM_COLUMNS_A; j++) {
            mat_a[i][j] = 0;
        }
    }
    for (i = 0; i < NUM_ROWS_B; i++) {
        for (j = 0; j < NUM_COLUMNS_B; j++) {
            mat_b[i][j] = 0;
        }
    }

    // read all non-zero elements from input files
    while((read = getline(&line, &len, fa)) != -1){
        int x = 0;
        token = strtok(line, " ");
        while(token != NULL){
            arr[x++] = atoi(token);
            token = strtok(NULL, " ");
        }
        mat_a[arr[0]][arr[1]] = arr[2];
    }

    while((read = getline(&line, &len, fb)) != -1){
        int x = 0;
        token = strtok(line, " ");
        while(token != NULL){
            arr[x++] = atoi(token);
            token = strtok(NULL, " ");
        }
        mat_b[arr[0]][arr[1]] = arr[2];
    }
    fclose(fa);
    fclose(fb);
}
void outputArray()
{
    fc = fopen("result", "w");
    for (i = 0; i < NUM_ROWS_A; i++) {
        for (j = 0; j < NUM_COLUMNS_B; j++)
            fprintf(fc, "%8.2f  ", mat_result[i][j]);
        fprintf(fc, "%s\n", "\n");
    }
    fclose(fc);
}
void printArray()
{
    for (i = 0; i < NUM_ROWS_A; i++) {
        printf("\n");
        for (j = 0; j < NUM_COLUMNS_A; j++)
            printf("%8.2f  ", mat_a[i][j]);
    }
    printf("\n\n\n");
    for (i = 0; i < NUM_ROWS_B; i++) {
        printf("\n");
        for (j = 0; j < NUM_COLUMNS_B; j++)
            printf("%8.2f  ", mat_b[i][j]);
    }
    printf("\n\n\n");
    for (i = 0; i < NUM_ROWS_A; i++) {
        printf("\n");
        for (j = 0; j < NUM_COLUMNS_B; j++)
            printf("%8.2f  ", mat_result[i][j]);
    }
    printf("\n\n");
}