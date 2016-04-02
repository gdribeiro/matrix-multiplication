#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h> // To support pid_t
#include <sys/wait.h> // To support wait
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>


// Filenames definitions
#define matFileOne "./in1.txt"
#define matFileTwo "./in2.txt"
#define outFileTxt "./out.txt"
#define avgFileTxt "./avarage.txt"
#define ENABLE_TIME_COUNT true
// Global variables
// Source matrices and resulting matrix
// Also the dimensions of the matrices
int* mat1 = NULL;
int* mat2 = NULL;
int* matout = NULL;
int m, n, k, l;

// Variables to control the shared memmory
int segment_id;
int segment_size;

typedef struct str_thdata
{
    int line_begin;
    int line_final;
} thdata;

// Multiply the matrices
void multiplyMatrices(thdata *ptr);
// Return a time stamp to calculate the execution time
unsigned long int getTimeStamp();

void main(int argc, char * argv[])
{
    // Variable to monitor the time required to multiply the matrices
    struct timespec startClock, stopClock;
    unsigned long int cpu_time_used;

    int i=0, j=0, t=0;
    int threads_number;
    FILE *in1, *in2, *outFile;
    FILE *avgFile;
    char* buff;
    pid_t *Thds;

    in1 = fopen(matFileOne, "r");
    in2 = fopen(matFileTwo, "r");
    // Texts and gets the desired number of threads
    if(argc != 2)
    {
        printf("Insert the desired number of Threads...\n");
        exit(1);
    }
    // Get the number of threads
    threads_number = atoi((const char*) argv[1]);

    //******************************************************//
    if(in1 == NULL || in2 == NULL)
    {
        printf("Error while opening the files!");
        exit (1);
    }
    // Read the fist two line to know the size
    // Read the string to advance the file pointer
    // Matrix 1
    fscanf(in1, "%s", &buff);
    fscanf(in1, "%s", &buff);
    fscanf(in1, "%d", &m);
    printf("m= %d ", m);
    fscanf(in1, "%s", &buff);
    fscanf(in1, "%s", &buff);
    fscanf(in1, "%d", &n);
    printf("n= %d \n", n);
    // Matrix 2
    fscanf(in2, "%s", &buff);
    fscanf(in2, "%s", &buff);
    fscanf(in2, "%d", &k);
    printf("k= %d ", k);
    fscanf(in2, "%s", &buff);
    fscanf(in2, "%s", &buff);
    fscanf(in2, "%d", &l);
    printf("l= %d \n", l);
    // Allocate Memory for the matrix acording to the dimensions
    if( !(mat1 = (int *) malloc( m * n * sizeof(int))) )
    {
        printf("Error occurred while allocating memory for the matrix 1!\n");
        exit (1);
    } else printf("Matrix %d * %d allocated successfully!\n", m, n);
    // Allocate Memory for the matrix acording to the dimensions
    if( !(mat2 = (int *) malloc( k * l * sizeof(int))) )
    {
        printf("Error occurred while allocating memory for the matrix 2!\n");
        exit (1);
    } else printf("Matrix %d * %d allocated successfully!\n", k, l);
    // Reads the numbers from the file to the matrixi 1
    // i iterates the lines
    for(i=0; i<m; i++)
    {
        if(feof(in1))
        {
            printf("Some Error occurred while reading the numbers from file.");
            exit (1);
        }

        // j iterates the columns
        for(j=0; j<n; j++)
        {
            // Save the m*n element from file to the Matrix's m*n element
            fscanf(in1, "%d", &(*(mat1+(i*n)+j)));
            //printf("%li\n", *(mat+(i*n)+j));
        }
    }
    // Reads the numbers from the file to the matrix 2
    // i iterates the lines
    for(i=0; i<k; i++)
    {
        if(feof(in2))
        {
            printf("Some Error occurred while reading the numbers from file.");
            exit (1);
        }

        // j iterates the columns
        for(j=0; j<l; j++)
        {
            // Save the m*n element from file to the Matrix's m*n element
            fscanf(in2, "%d", &(*(mat2+(i*l)+j)));
            //printf("%li\n", *(mat+(i*n)+j));
        }
    }
    fclose(in1);
    fclose(in2);

    //*****************************************************************//
    // MAGIC
    // Error message, if m different of n, so matrices are not multipliable
    if(!(n == k))
    {
        printf("Matrices cannot be multiplied, m*n x K*l -> m != l\n");
        exit (0);
    }


    //####################################################################
    // Determine the size of the segment
    segment_size = m * l * sizeof(int);
    // Allocate shared memory segment
    segment_id = shmget(IPC_PRIVATE,(const int) segment_size, S_IRUSR | S_IWUSR);
    // Attach the shared memory segment
    matout = (int *) shmat(segment_id, NULL, 0);

    // *************************************************************** //
    // Divided the work!
    // Creates an array to store each thrad's boundaries of multiplication
    thdata * lines = (thdata *) malloc(threads_number * sizeof(thdata));

    // lpt stands for Lines Per Thread
    int lpt = m / threads_number;
    int resto = m % threads_number;
    if (m < threads_number)
    {
        printf("There are more threads then lines in the matrix, insert a lower number!");
        exit(1);
    }

    for (i = 0; i < threads_number; i++)
    {
        lines[i].line_begin = i * lpt;
        lines[i].line_final = (i * lpt) + (lpt - 1);
    }
    if ( resto )
    {
        lines[threads_number-1].line_final = lines[threads_number-1].line_final + resto;
    }

    // Multiply the matrices
    // Dinamically creates the array of treads with the desired number
        Thds = (pid_t *) malloc((threads_number - 1) * sizeof(pthread_t));
    //**********************************************
    // Saves the beginning time to compute the cpu enlapsed time
    if (ENABLE_TIME_COUNT){
        clock_gettime(CLOCK_REALTIME, &startClock);
    }
    // Creates each Process and let them run
    for (i = 0; i < threads_number - 1; i++)
    {
       // forks the new process to do part of the processing
       Thds[i] = fork();
       // if any erros occurs while forking
       if(Thds[i] == -1)
       {
           printf("An error has occurred when creating a process");
       }
       else if(Thds[i] == 0)
       {
            // Do the job with the child process and exits
            multiplyMatrices(&lines[i]);
            exit(0);
       }
    }
    // the parent also does the work of multiplying
    multiplyMatrices(&lines[threads_number - 1]);

    // Join all the threads
    for (i = 0; i < threads_number - 1; i++)
    {
        // Waits for all the children processes to finish
        waitpid(Thds[i], 0, 0);
    }

    // ***************************
    // Here is the final messure of time
    if (ENABLE_TIME_COUNT){
        clock_gettime(CLOCK_REALTIME, &stopClock);
    }

    // Free the memory of the threads array
    if (Thds != NULL)   free(Thds);

    if (ENABLE_TIME_COUNT){
        // Returns the value in microeconds u
        cpu_time_used =  ( (1000000000 * (unsigned long int)(stopClock.tv_sec - startClock.tv_sec)) + (unsigned long int)(stopClock.tv_nsec - startClock.tv_nsec))/1000;
        printf("CPU time: %ld us \n", cpu_time_used);

        printf("CPU Seconds : %ld \n", stopClock.tv_sec - startClock.tv_sec);
        printf("CPU nSeconds: %ld \n", stopClock.tv_nsec - startClock.tv_nsec);

        avgFile = fopen(avgFileTxt,"a");
        if (avgFile != NULL)
        {
             fprintf(avgFile, "%ld\n", cpu_time_used);
             fflush(avgFile);
             fclose(avgFile);
        }
    }
    // DONE!!
    // ***************************************************************** //
    // Writes the multiplied matrix in the file out.txt
    outFile = fopen(outFileTxt, "w");
    if(outFile==NULL)
    {
        printf("Error opening the out.txt file.");
        exit(0);
    }

    fprintf(outFile, "LINHAS = %d\n", m);
    fprintf(outFile, "COLUNAS = %d\n", l);
    // Writes multiplied matrix in the out.txt file
    for(i=0; i<m; i++)
    {
        for(j=0; j<l; j++)
        {
            // Space insertion to separate the numbers in teh file
            if((j!=0)) fprintf(outFile, " ");
            fprintf(outFile, "%li", *(matout+(i*l)+j));
        }
        if(i<(m-1)) fprintf(outFile, "\n");
        fflush(outFile);
    }
    // Transfered to whithin the for that writes the file
    // maybe it can provide better performance for big matrices
    //fflush(outFile);

    // Closes file
    fclose(outFile);

    // Free the memmory utilized by the matrices
    free(mat1);
    free(mat2);
    //free(matout);
    // Detach the shared memory segment
    shmdt(matout);
    // Remove the shared memory segment
    shmctl(segment_id, IPC_RMID, NULL);
    exit(0);
}

void multiplyMatrices(thdata * ptr)
{
    thdata * lines;
    lines = (thdata *) ptr;

    int i=0, j=0, t=0, sum=0, m;

    i = lines->line_begin;
    m = lines->line_final +1;

    printf("%d %d \n", lines->line_begin, lines->line_final);
    for(i = 0; i < m; i++)
    {
        for(j = 0; j < l; j++)
        {
            sum = 0;
            for(t = 0; t < n || t < k; t++)
            {
                sum = sum + ( (*(mat1+(i*n)+t)) * (*(mat2+j+(t*l)) )  );
            }
            *(matout+(i*l)+j) = sum;
        }
    }
}
