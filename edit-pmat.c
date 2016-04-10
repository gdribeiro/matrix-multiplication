#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>  // To support pid_t
#include <sys/wait.h>   // To support wait
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>

// Enables time counting if true or not if false
#define ENABLE_TIME_COUNT true

// File names
char *matFileOne;
char *matFileTwo;
char *outFileTxt;
char *timeLogFile;

// Matrices' pointer
int *mat1 = NULL;
int *mat2 = NULL;
int *matOut = NULL;
// Matrices's sizes
int m, n, k, l;

int procNumber;

// Shared memory variables
int segment_id;
int segment_size;

// Argument passed to the new created processes
// To divide the job of multiplication
typedef struct str_thdata {
  int line_begin;
  int line_final;
} thdata;


// Parse files and create the memory space for each input matrix
int *parseFiles(char* matFile);
// Parse input
void parseInput(int argc, char *argv[]);
// Multiply the matrices
void multiplyMatrices(thdata *ptr);
// Return a time stamp to calculate the execution time
unsigned long int getTimeStamp();

int main(int argc, char *argv[]) {
  // Timing
  // struct timespec startClock, stopClock;
  // unsigned long int cpu_time_used;
  //
  // int i = 0, j = 0, t = 0;
  // int threads_number;
  // FILE *in1, *in2, *outFile;
  // FILE *avgFile;
  // char *buff;
  // pid_t *Thds;


  // Parses the input arguments
  parseInput(argc, argv);


  printf("Imput matrices' files: %s and %s\n",matFileOne, matFileTwo);
  printf("Output matrix's file: %s\n", outFileTxt);
  printf("Time log file: %s\n",timeLogFile);
  printf("Number of process: %d\n",procNumber);



  exit(0);
}

void multiplyMatrices(thdata *ptr) {
  thdata *lines;
  lines = (thdata *)ptr;

  int i = 0, j = 0, t = 0, sum = 0, m = 0;

  i = lines->line_begin;
  m = lines->line_final + 1;

  printf("%d %d \n", lines->line_begin, lines->line_final);
  for (i = 0; i < m; i++) {
    for (j = 0; j < l; j++) {
      sum = 0;
      for (t = 0; t < n || t < k; t++) {
        sum = sum + ((*(mat1 + (i * n) + t)) * (*(mat2 + j + (t * l))));
      }
      *(matOut + (i * l) + j) = sum;
    }
  }
}

int *parseFiles(char* matFile) {
  int i = 0, j = 0, t = 0;
  FILE *in;
  char *buff;
  int *mat = NULL;
  // Shared memory variables
  int seg_id;
  int seg_size;

  // Opens file
  in = fopen(matFile, "r");
  if (in == NULL) {
    printf("Error: Opening the file!");
    exit(1);
  }
  // Gets the matrix's size
  fscanf(in, "%s", &buff);
  fscanf(in, "%s", &buff);
  fscanf(in, "%d", &m);
  fscanf(in, "%s", &buff);
  fscanf(in, "%s", &buff);
  fscanf(in, "%d", &n);
  // Prints Size
  printf("m= %d x n= %d", m, n);

  // The input matrices are also with shared memmory
  seg_size = m * n * sizeof(int);
  // Allocate shared memory segment
  seg_id = shmget(IPC_PRIVATE, (const int)seg_size, S_IRUSR | S_IWUSR);
  // Attach the shared memory segment
  mat = (int*)shmat(seg_id, NULL, 0);

  // Lines
  for (i = 0; i < m; i++) {
    if (feof(in)) {
      printf("Error while reading file!");
      exit(1);
    }
    // Columns
    for (j = 0; j < n; j++) {
      fscanf(in, "%d", &(*(mat + (i * n) + j)));
    }
  }
  fclose(in);
}

// Parses the input arguments
void parseInput(int argc, char *argv[]) {
  // Defaults
  matFileOne = "./in1.txt";
  matFileTwo = "./in2.txt";
  outFileTxt = "./out.txt";
  timeLogFile = "./avarage.txt";
  procNumber = 1;

  for (int i = 1; i < argc-1; i++) {
    if(argv[i][0] == '-'){
      switch (argv[i][1]) {
        // numver of threads or process
        case 'n':
          procNumber = atoi((const char*) argv[i+1]);
          break;
        // output file
        case 'o':
          outFileTxt = argv[i+1];
          break;
        // input files
        case 'i':
          matFileOne = argv[i+1];
          matFileTwo = argv[i+2];
          break;
        // time logging file
        case 'l':
          timeLogFile = argv[i+1];
          break;
        default:
        break;
      }
    }
  }
  if (argc < 2) {
    printf("Running default options!\n");
  }
  printf("Imput matrices' files: %s and %s\n",matFileOne, matFileTwo);
  printf("Output matrix's file: %s\n", outFileTxt);
  printf("Time log file: %s\n",timeLogFile);
  printf("Number of process: %d\n",procNumber);
  printf("For more options use: -i <input file 1> <input file 2> -o <output file> -n <number of process>\n");
}
