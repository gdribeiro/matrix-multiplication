#include <stdio.h>
#include <stdlib.h>

// Fulfills the matrix with ramdom numbers
void rand_matrix(int m, int n, int *mat);
// Builds the identity matrix
int identity_matrix(int m, int n);
// Prints the matrix on the screen
void show_matrix(int m, int n, int *mat);
// Writes the matrix in the file
void writeFileMat(int *mat, FILE *file, int m, int n);

int main(int argc, char *argv[]) {
    // Size of the matrix to be generated ramdomly
    // Matrix of size m*n and k*l or Lines and Columns
    int m, n, k, l;
    // Loop control variables
    int i, j;
    // Files where the matrices will be saved
    FILE *matFileOne;
    FILE *matFileTwo;
    // Open the two files in writing mode
    matFileOne = fopen("./in1.txt", "w");
    // If file not oppened prints a message
    if (matFileOne == NULL) {
        printf("Couldn't open ./in1.txt\n");
        return (0);
    }
    matFileTwo = fopen("./in2.txt", "w");
    // If file not oppened prints a message
    if (matFileTwo == NULL) {
        printf("Couldn't open ./in2.txt\n");
        return (0);
    }

    if (argc != 5) {
        printf("Insert the 4 dimensions for the Matrices!\n");
        exit(1);
    }

    // Convert the parameters to int to give the matrix's dimensions
    m = atoi((const char *)argv[1]);
    n = atoi((const char *)argv[2]);
    k = atoi((const char *)argv[3]);
    l = atoi((const char *)argv[4]);

    // Matrix pointer
    int *mat;
    // Allocate memory to the matrix or exits in case of error
    if (!(mat = (int *)malloc(m * n * sizeof(int)))) {
        printf("Error occurred while allocating memory for the matrix!\n");
        exit(1);
    } else
        // printf("Matrix allocated successfully!\n");

    // Generates the Matrices
    //
    rand_matrix(m, n, mat);
    // show_matrix(m, n, mat);
    writeFileMat(mat, matFileOne, m, n);
    rand_matrix(m, n, mat);
    // show_matrix(k, l, mat);
    writeFileMat(mat, matFileTwo, k, l);

    // Identity matrix. Always with the dimensions m*n, the first two arguments
    identity_matrix(m, n);
    // Show matrix
    // show_matrix(m, n, mat);

    // frees the allocated space in memory for the matrix
    free(mat);
    // Close files
    fclose(matFileOne);
    fclose(matFileTwo);

    exit(0);
}

// Fulfills the matrix with ramdom numbers
void rand_matrix(int m, int n, int *mat) {
    // Loop variables
    int i = 0, j = 0;
    // Fulfill the matrix with ramdom numbers
    // The variable i commands the line iteration
    for (i = 0; i < m; i++) {
        // The variable j commands the columns iteration
        for (j = 0; j < n; j++) {
            // Assign ramdom numbers to the Matrix's m*n position
            *(mat + (i * n) + j) = rand() % 10;
        }
    }
    // printf("Ramdom Matrix with dimentions %d x %d was created successfully!\n",
          //  m, n);
}

// Builds the identity matrix
int identity_matrix(int m, int n) {
    FILE *file;
    file = fopen("./identity.txt", "w");
    if (file == NULL) {
        printf("Couldn't open ./identity.txt\n");
        return (0);
    }
    // Loop variables
    int i = 0, j = 0;

    // Write here the code
    fprintf(file, "LINHAS = %d\n", m);
    fprintf(file, "COLUNAS = %d\n", n);

    // Creates the identity matrix
    // i iterates the lines
    for (i = 0; i < m; i++) {
        for (j = 0; j < n; j++) {
            // Space insertion to separate the numbers in teh file
            if ((j != 0)) fprintf(file, " ");
            if (i == j) {
                // print the Matrix's m*n element in the file
                fprintf(file, "1");
            } else
                fprintf(file, "0");
        }
        if (i < (m - 1)) fprintf(file, "\n");
        // Break the lines for vizualization on the screen
        // fprintf(file, '\n');
    }

    fflush(file);
    fclose(file);
    // printf(
    //     "Identity Matrix with dimentions %d x %d was created successfully!\n",
    //     m, n);
}

// Prints the matrix on the screen
void show_matrix(int m, int n, int *mat) {
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

void writeFileMat(int *mat, FILE *file, int m, int n) {
    // Write here the code
    fprintf(file, "LINHAS = %d\n", m);
    fprintf(file, "COLUNAS = %d\n", n);

    // Loop variables
    int i = 0, j = 0;
    // write the matrix's content in the file
    // i iterates the lines
    for (i = 0; i < m; i++) {
        // j iterates the columns
        for (j = 0; j < n; j++) {
            // Space insertion to separate the numbers in teh file
            if ((j != 0)) fprintf(file, " ");
            // print the Matrix's m*n element in the file
            fprintf(file, "%d", *(mat + (i * n) + j));
        }
        if (i < (m - 1)) fprintf(file, "\n");
        // Break the lines for vizualization on the screen
    }
    fflush(file);
}
