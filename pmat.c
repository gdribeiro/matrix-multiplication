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
bool verbose = false;
// File names
char *matFileOne;
char *matFileTwo;
char *outFileTxt;
char *timeLogFile;
int procNumber;

typedef struct {
  int seg_id;
  int seg_size;
  int *mat;
  int lines, columns;
} matrix;
matrix matrixOne, matrixTwo, matrixOut;

// To divide the job of multiplication
typedef struct {
  int line_start;
  int line_stop;
} matrixSlice;

// Divides the matrix among the process
void divideMatrix(matrixSlice *lines);
// Create shared memory space for output matrix
void allocOutMatrix(matrix *matrixOne, matrix *matrixTwo, matrix *matrixOut);
// Check if the matrices' sizes are compatible
void checkSizes(matrix *matrixOne, matrix *matrixTwo);
// Does what it says!!
void show_matrix(matrix *matt);
// Create memory space and load matrices from files
void loadMatrix(matrix *m, char *matFile);
// Parse input
void parseInput(int argc, char *argv[]);
// Multiply the matrices
void multiplyMatrices(matrixSlice *ptr);
// Saves matrix to file
void writeOutMatrix(matrix *mat);
// Return a time stamp to calculate the execution time
unsigned long int getTimeStamp();
void timeLog(struct timespec startClock, struct timespec stopClock);

int main(int argc, char *argv[]) {
  struct timespec startClock, stopClock;
  unsigned long int cpu_time_used;

  // Parses the input arguments
  parseInput(argc, argv);
  loadMatrix(&matrixOne, matFileOne);
  loadMatrix(&matrixTwo, matFileTwo);
  checkSizes(&matrixOne, &matrixTwo);
  allocOutMatrix(&matrixOne, &matrixTwo, &matrixOut);

  // Will be passed to eatch process
  matrixSlice *lines = (matrixSlice *)malloc(procNumber * sizeof(matrixSlice));
  divideMatrix(lines);

  // Create process pids
  pid_t *procs = (pid_t *)malloc((procNumber - 1) * sizeof(pthread_t));

  // Get start time
  if (ENABLE_TIME_COUNT) {
    clock_gettime(CLOCK_REALTIME, &startClock);
  }
  for (int i = 0; i < procNumber - 1; i++) {
    procs[i] = fork();
    if (procs[i] == -1) {
      printf("Error creating processes!");
    } else if (procs[i] == 0) {
      // Do the job with the child process and exits
      multiplyMatrices(&lines[i]);
      exit(0);
    }
  }
  // The parent process also multiplies
  multiplyMatrices(&lines[procNumber - 1]);
  // Join all the threads
  for (int i = 0; i < procNumber - 1; i++) {
    // Waits for all the children processes to finish
    waitpid(procs[i], 0, 0);
  }
  // Get the stop time
  if (ENABLE_TIME_COUNT) {
    clock_gettime(CLOCK_REALTIME, &stopClock);
    timeLog(startClock, stopClock);
  }

  writeOutMatrix(&matrixOut);

  // frees
  free(procs);
  free(lines);
  shmdt(matrixOne.mat);
  shmdt(matrixTwo.mat);
  shmdt(matrixOut.mat);
  shmctl(matrixOne.seg_id, IPC_RMID, NULL);
  shmctl(matrixTwo.seg_id, IPC_RMID, NULL);
  shmctl(matrixOut.seg_id, IPC_RMID, NULL);

  exit(0);
}

void multiplyMatrices(matrixSlice *ptr) {
  matrixSlice *matLines = (matrixSlice *)ptr;
  int i = matLines->line_start;
  int m = matLines->line_stop + 1;
  int n = matrixOne.columns;
  int l = matrixTwo.columns;
  int k = matrixTwo.lines;
  int j, t, sum;
  int *mat1 = matrixOne.mat;
  int *mat2 = matrixTwo.mat;
  int *matOut = matrixOut.mat;

  for (i = matLines->line_start; i < m; i++) {
    for (j = 0; j < l; j++) {
      sum = 0;
      for (t = 0; t < n || t < k; t++) {
        sum = sum + ((*(mat1 + (i * n) + t)) * (*(mat2 + (t * l) + j)));
      }
      *(matrixOut.mat + (i * l) + j) = sum;
    }
  }
}

void writeOutMatrix(matrix *mat) {
  int m = mat->lines;
  int l = mat->columns;
  FILE *outFile = fopen(outFileTxt, "w");
  if (outFile == NULL) {
    printf("Error opening the out.txt file.");
    exit(1);
  }
  fprintf(outFile, "LINHAS = %d\n", m);
  fprintf(outFile, "COLUNAS = %d\n", l);
  // Writes multiplied matrix in the out.txt file
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < l; j++) {
      // Space insertion to separate the numbers in teh file
      if ((j != 0)) fprintf(outFile, " ");
      fprintf(outFile, "%d", *(matrixOut.mat + (i * l) + j));
    }
    if (i < (m - 1)) fprintf(outFile, "\n");
    fflush(outFile);
  }
  // Closes file
  fclose(outFile);
}

void timeLog(struct timespec startClock, struct timespec stopClock) {
  unsigned long int cpu_time_used;
  // Returns the value in microeconds u
  cpu_time_used =
      ((1000000000 *
        (unsigned long int)(stopClock.tv_sec - startClock.tv_sec)) +
       (unsigned long int)(stopClock.tv_nsec - startClock.tv_nsec)) /
      1000;
  if (verbose) {
    printf("CPU Seconds : %lu\n", stopClock.tv_sec - startClock.tv_sec);
    printf("CPU nSeconds: %lu\n", stopClock.tv_nsec - startClock.tv_nsec);
    printf("CPU time:     %lu us\n", cpu_time_used);
  }
  //
  FILE *timeFile = fopen(timeLogFile, "a");
  if (timeFile != NULL) {
    printf("File opened!\n");
    fprintf(timeFile, "%lu\n", cpu_time_used);
    fflush(timeFile);
    fclose(timeFile);
  }
}

void divideMatrix(matrixSlice *lines) {
  // lpt - Lines Per Thread
  int lpt = matrixOne.lines / procNumber;
  int resto = matrixOne.lines % procNumber;
  if (matrixOne.lines < procNumber) {
    printf("Can't do more Process than lines in the matrix!");
    exit(0);
  }
  for (int i = 0; i < procNumber; i++) {
    lines[i].line_start = i * lpt;
    lines[i].line_stop = (i * lpt) + (lpt - 1);
  }
  // If there are not an equal number of lines for each process
  // The odd ones are appended to the last process
  if (resto) {
    lines[procNumber - 1].line_stop = lines[procNumber - 1].line_stop + resto;
  }
}

void allocOutMatrix(matrix *matrixOne, matrix *matrixTwo, matrix *matrixOut) {
  matrixOut->lines = matrixOne->columns;
  matrixOut->columns = matrixTwo->lines;
  // Determine the size of the segment
  matrixOut->seg_size = matrixOne->lines * matrixTwo->columns * sizeof(int);
  // Allocate shared memory segment
  matrixOut->seg_id =
      shmget(IPC_PRIVATE, (const int)matrixOut->seg_size, S_IRUSR | S_IWUSR);
  // Attach the shared memory segment
  matrixOut->mat = (int *)shmat(matrixOut->seg_id, NULL, 0);
}
// Create memory space and load matrices from files
void loadMatrix(matrix *m, char *matFile) {
  int i = 0, j = 0, t = 0;
  int lines, columns;
  char buff[30];
  FILE *file;

  // Opens file
  file = fopen(matFile, "r");
  if (file == NULL) {
    printf("Error: Couldn't open the file %s", matFile);
    exit(1);
  }

  // Gets the matrix's size
  fscanf(file, "%s", buff);
  fscanf(file, "%s", buff);
  fscanf(file, "%d", &lines);
  fscanf(file, "%s", buff);
  fscanf(file, "%s", buff);
  fscanf(file, "%d", &columns);

  m->lines = lines;
  m->columns = columns;

  // The input matrices are also with shared memmory
  m->seg_size = lines * columns * sizeof(int);
  // Allocate shared memory segment
  m->seg_id = shmget(IPC_PRIVATE, (const int)m->seg_size, S_IRUSR | S_IWUSR);
  // Attach the shared memory segment
  m->mat = (int *)shmat(m->seg_id, NULL, 0);

  // Lines
  for (i = 0; i < lines; i++) {
    if (feof(file)) {
      printf("Error: Cannot read the file!");
      exit(1);
    }
    // Columns
    for (j = 0; j < columns; j++) {
      fscanf(file, "%d", &(*(m->mat + (i * columns) + j)));
    }
  }
  fclose(file);
}
// Parses the input arguments
void parseInput(int argc, char *argv[]) {
  // Defaults
  matFileOne = "./in1.txt";
  matFileTwo = "./in2.txt";
  outFileTxt = "./out.txt";
  timeLogFile = "./time.log";
  procNumber = 1;

  for (int i = 1; i < argc - 1; i++) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
        // numver of threads or process
        case 'n':
          procNumber = atoi((const char *)argv[i + 1]);
          break;
        // output file
        case 'o':
          outFileTxt = argv[i + 1];
          break;
        // input files
        case 'i':
          matFileOne = argv[i + 1];
          matFileTwo = argv[i + 2];
          break;
        // time logging file
        case 'l':
          timeLogFile = argv[i + 1];
          break;
        case 'v':
          verbose = true;
          break;
        default:
          break;
      }
    }
  }
  if (argc < 2) {
    printf("Running default options!\n");
    printf(
        "For more options use:\n-i <input file 1> <input file 2>\n-o <output "
        "file>\n"
        "-n <number of process>\n-v for verbose execution\n");
  }
  if (verbose) {
    printf("Imput matrices' files: %s and %s\n", matFileOne, matFileTwo);
    printf("Output matrix's file: %s\n", outFileTxt);
    printf("Time log file: %s\n", timeLogFile);
    printf("Number of process: %d\n", procNumber);
  }
}

void show_matrix(matrix *matt) {
  int m = matt->lines;
  int n = matt->columns;
  int *mat = matt->mat;

  // Loop variables
  int i = 0, j = 0;
  // Show the matrix's content
  // i iterates the lines
  for (i = 0; i < m; i++) {
    // j iterates the columns
    for (j = 0; j < n; j++) {
      // print the Matrix's m*n element
      printf(" %d ", *(mat + (i * n) + j));
    }
    // Break the lines for vizualization on the screen
    printf("\n");
  }
}

void checkSizes(matrix *matrixOne, matrix *matrixTwo) {
  if (matrixOne->columns != matrixTwo->lines) {
    printf("Erros: Matrices sizes are not compatible!\n");
    exit(0);
  }
}
