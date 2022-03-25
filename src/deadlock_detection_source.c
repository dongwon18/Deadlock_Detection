/**
 * Copyright (C) 2020 by Dongwon Kim, All rights reserved.
 *
 * Project 2 of OS: Deadlock Detection
 *
 * Summery of this code:
 *    1. get input file or create random input file
 *    2. get information from input file
 *    3. input file contains no. of process, resource, resource unit, allocated
 *       resource unit and asked resource unit by process
 *    4. using the information, detect deadlock.
 *    5. print the result(list of deadlocked process, state of the system)
 *
 * Written by Dongwon Kim on November 06, 2020
 *
 *  Modification History :
 *    1. Written by Dongwon Kim on Nov 06, 2020
 *
 *
 *  Compiler Used : gcc version 7.3.0, 32-bit
 *  Encoding : UTF - 8
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define RAND_RANGE 10   //  range for no. of process and resource in random input
#define UNIT_RANGE 20   //  range for no. of resource unit in random input
#define RANDOM_INPUT 1  //  if 1 random input is used

/**
 * function prototype
 */
void detection(int **allocate, int **request, int *r_unit, int *finish, int n, int m);
void find_deadlock(int *finish, int n);
void matrix_print(int **matrix, int n, int m);
int *create_finish(int n);
int *R_info(FILE *infile, int *n, int *m);
void get_input(FILE *infile, int n, int m, int **request, int **allocate);
int **create_matrix(int row, int column);
void free_matrix(int row, int column, int **matrix);
void get_R_remain(int *r_unit, int **allocate, int n, int m);
FILE *rand_input(void);

/**
 * if RANDOM_INPUT = 0, input1.txt is the input file
 * if RANDOM_INPUT = 1, input.txt is generated and used for input file
 *
 * return 0: normal termination
 *       -1: dynamic memory allocation failed
 *       -2: file opening failed
 *       -3: number of unit and given information is not matched
 *       -4: allocate more than total no. of resource unit
 */
int main(){
    srand(time(NULL));
    int n, m;
    FILE *infile;

    //  get input file
    if(RANDOM_INPUT == 0){
        printf("--Not random input (default: input1.txt)--\n\n");
        infile = fopen("input1.txt", "r");
    }else{
        printf("--Random input file--\n\n");
        infile = rand_input();
    }

    if(infile == NULL){
        printf("file opening failed\n");
        exit(-2);
    }
    //  get total no. of resource unit
    int *r_unit = R_info(infile, &n, &m);
    //  check if process[i] is finished
    int *finish = create_finish(n);

    //  matrix for request, allocate information
    int **request = create_matrix(n, m);
    int **allocate = create_matrix(n, m);
    //  get information from input file
    get_input(infile, n, m, request, allocate);

    //  print the given matrix
    printf("request\n");
    matrix_print(request, n, m);
    printf("allocate\n");
    matrix_print(allocate, n, m);
    //  calculate no. of available resource unit
    get_R_remain(r_unit, allocate, n, m);

    //  deadlock detection
    detection(allocate, request, r_unit, finish, n, m);

    //  print the result as matrix
    printf("result: final allocation\n");
    matrix_print(allocate, n, m);
    printf("result: remain request\n");
    matrix_print(request, n, m);

    //  find deadlocked process and define the state of the system
    find_deadlock(finish, n);

    //  free dynamic memory
    free_matrix(n, m, request);
    free_matrix(n, m, allocate);
    free(finish);
    free(r_unit);
    //  close file
    fclose(infile);

    return 0;
}

/**
 * if finish is 1, the process is terminated(not in detection scope)
 * if the process is blocked, impossible becomes 1
 * if impossible is 0 (the process is not blocked)
 *    return the resource and mark finish[i] = 1, change = 1
 *    find unblocked process from the first
 * if all process(whose finish = 0) is scanned and change = 0,
 *    means all remain processes are blocked.
 *    or all processes are terminated.
 */
void detection(int **allocate, int **request, int *r_unit, int *finish, int n, int m){
    int impossible = 0;
    int change = 0;
    while(1){
        change = 0;
        for(int i = 0; i < n; i++){
            // if process[i] is not terminated
            if(finish[i] != 1){
                // check if the request is equal or small than available
                for(int j = 0; j < m; j++){
                    if(r_unit[j] < request[i][j]){
                        impossible = 1;
                    }
                }
                // if the process is not blocked
                if(impossible == 0){
                    finish[i] = 1;
                    // return resource
                    for(int j = 0; j < m; j++){
                        r_unit[j] += allocate[i][j];
                        request[i][j] = 0;
                        allocate[i][j] = 0;
                    }
                    change = 1;
                }
            }
            impossible = 0;
            // if change is 1, scan from the first again
            if(change == 1){
                break;
            }
        }
        // if all processes are scanned and change not occurred,
        // detection finished
        if(change == 0){
            break;
        }
    }
}

/**
 * make random input.txt
 * used when RANDOM_INPUT = 1
 */
FILE *rand_input(void){
    FILE *infile = fopen("input.txt", "w+");
    if(infile == NULL){
        printf("file opening failed\n");
        exit(-2);
    }

    // random no. of processes and resource
    int n = rand() % (RAND_RANGE + 1);
    int m = rand() % (RAND_RANGE + 1);
    int unit[m];
    int remain[m];
    int **allocate = create_matrix(n, m);
    fprintf(infile, "%d, %d, ", n, m);

    // random no. of resource unit
    for(int i = 0; i < m; i++){
        if(i == m - 1){
            unit[i] = rand() % (UNIT_RANGE + 1);
            remain[i] = unit[i];
            fprintf(infile, "%d\n", unit[i]);
        }else{
            unit[i] = rand() % (UNIT_RANGE + 1);
            remain[i] = unit[i];
            fprintf(infile, "%d, ", unit[i]);
        }
    }

    //  allocate matrix
    int r;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            // to make total allocated unit is equal or less than total unit no.
            r = rand() % (remain[j] + 1);
            allocate[i][j] = r;
            if(j == m - 1){
                fprintf(infile,"%d\n", r);
            }else{
                fprintf(infile,"%d, ", r);
            }
            remain[j] -= r;
        }
    }

    //  require matrix
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            if(j == m - 1){
                fprintf(infile,"%d\n", rand() % (unit[j] - allocate[i][j]+ 1));
            }else{
                fprintf(infile,"%d, ", rand() % (unit[j] - allocate[i][j] + 1));
            }
        }
    }

    //  set file pointer to the start
    free_matrix(n, m, allocate);
    fseek(infile, 0, SEEK_SET);

    return infile;
}

/**
 * if finish[i] = 0, means the process is deadlocked
 * print the list of deadlocked process
 * find if the system is in deadlock state or not
 */
void find_deadlock(int *finish, int n){
    int num_deadlock = 0;
    // print the list of deadlocked process
    printf("Deadlocked process: \n");
    for(int i = 0; i < n; i++){
        if(finish[i] != 1){
            num_deadlock++;
            printf("P%d ", i);
        }
    }

    // find if the state is deadlock state
    if(num_deadlock == 0){
        printf("none\n");
        printf("Not in Deadlock state\n");
    }else{
        printf("\n");
        printf("The system is in Deadlock state\n");
    }
}

/**
 * get available no. of unit and store it in r_unit
 * if total allocated no. of unit > total no. of unit, return -4 and terminate
 */
void get_R_remain(int *r_unit, int **allocate, int n, int m){
    printf("Available resource \n");
    for(int i = 0; i < m; i++){
        for(int j = 0; j < n; j++){
            r_unit[i] -= allocate[j][i];
        }
        if(r_unit[i] < 0){
            printf("cannot allocate more than total unit number\n");
            exit(-4);
        }
        printf("%d ", r_unit[i]);
    }
    printf("\n\n");
}

/**
 * print n x m matrix
 */
void matrix_print(int **matrix, int n, int m){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * return int pointer array finish
 * initialize finish[i] to 0
 */
int *create_finish(int n){
    int *finish = (int *)malloc(sizeof(int) * n);
    if(finish == NULL){
        printf("dynamic memory allocation failed\n");
        exit(-1);
    }
    for(int i = 0; i < n; i++){
        finish[i] = 0;
    }
    return finish;
}

/**
 * get the first line of the input file
 * n = no. of process
 * m = no. of resource
 * resource = no. of resource unit
 * return resource
 */
int *R_info(FILE *infile, int *n, int *m){
    // get n, m
    fscanf(infile, "%d, %d, ", n, m);
    printf("# of P: %d # of R: %d\n", *n, *m);

    // create resource
    int *resource = (int *)malloc(sizeof(int) * (*m));
    if(resource == NULL){
        printf("dynamic memory allocation failed\n");
        exit(-1);
    }

    // get no. of resource unit
    int num = 0;
    printf("number of resource unit\n");
    for(int i = 0; i < *m; i++){
        if(i == *m - 1){
            num += fscanf(infile, "%d\n", (&resource[i]));
        }else{
            num += fscanf(infile, "%d, ", (&resource[i]));
        }
        printf("%d ", resource[i]);
    }
    printf("\n");

    // check if no. of resource unit and given data is not matched
    if(num != *m){
        printf("reading the number of resource unit failed\n");
        exit(-3);
    }

    return resource;
}

/**
 * get allocate, request matrix from input file
 * first, get allocate data
 * next, get request data

 * input file format
 *    R0 R1 R2 ...
 * P0
 * P1
 * P2
 * ...
 *
 * if given data and no. of resource unit is not matched, return -3 and terminate
 */
void get_input(FILE *infile, int n, int m, int **request, int **allocate){
    int read = 0;
    // get allocate data
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            if(j == m - 1){
                read += fscanf(infile, "%d\n", &(allocate[i][j]));
            }else{
                read += fscanf(infile, "%d, ", &(allocate[i][j]));
            }
        }
        // check given data is following the format
        if(read != m){
            printf("reading allocation data failed\n");
            exit(-3);
        }
        read = 0;
    }

    // get request data
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            if(j == m - 1){
                read += fscanf(infile, "%d\n", &(request[i][j]));
            }else{
                read += fscanf(infile, "%d, ", &(request[i][j]));
            }
        }
        // check given data is following the format
        if(read != m){
            printf("reading request data failed\n");
            exit(-3);
        }
        read = 0;
    }
}

/**
 * create row x column matrix by dynamic memory allocation
 * initialize the matrix to 0
 * return created matrix
 */
int **create_matrix(int row, int column){
    int **matrix = (int **)malloc(sizeof(int *)* row);
    if(matrix == NULL){
        printf("dynamic memory allocation failed\n");
        exit(-1);
    }

    for(int i = 0; i < row; i++){
        matrix[i] = (int *)malloc(sizeof(int) * column);
        if(matrix[i] == NULL){
            printf("dynamic memory allocation failed\n");
            exit(-1);
        }
    }

    //  initialize to 0
    for(int i = 0; i < row; i++){
        for(int j = 0; j < column; j++){
            matrix[i][j] = 0;
        }
    }

    return matrix;
}

/**
 * return allocated memory of 2D matrix
 */
void free_matrix(int row, int column, int **matrix){
    for(int i = 0; i < row; i++){
        free(matrix[i]);
    }
    free(matrix);
}
