/* This example shows how to setup a very simple QUARK program.  The
 * dependencies involved in matrix multiplication are minimal, however
 * you can see how QUARK can easily enable parallelism for a tile
 * based code. 
 * 
 * In this example the calls to the tile based matmul routine are
 * replaced with calls to a routine that inserts tasks into the QUARK
 * runtime.  However, the overall tile based algorithm does not
 * change.
 * 
 * 
 * matmul ( &Ablk[NB*NB*i + NB*NB*BB*k], &Bblk[NB*NB*k + NB*NB*BB*j],
 * &Cblk[NB*NB*i + NB*NB*BB*j], NB);
 * 
 * matmul_quark_call ( quark, &Ablk[NB*NB*i + NB*NB*BB*k],
 * &Bblk[NB*NB*k + NB*NB*BB*j], &Cblk[NB*NB*i + NB*NB*BB*j], NB);
 *
 * Using this runtime, speedups for multiple threads can easily be
 * enabled.
*/


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <sys/time.h>
#include <string.h>

#include "quark.h"

/* This is the simple three loops version of matmul */
void matmul ( double *A, double *B, double *C, int NB ) 
{
    int i, j, k;
    double aij;
    
    for (j=0; j<NB; j++)
        for ( k=0; k<NB; k++)
            for ( i=0; i<NB; i++)
                C[i+j*NB] += A[i+k*NB] * B[k+j*NB];
}

/* Create a task wrapper to matmul, usable by the QUARK runtime.
 * Basically, you need to unpack the arguments from QUARK and call the
 * routine */
void matmul_quark_task( Quark *quark )
{
    double *A, *B, *C;
    int NB;
    quark_unpack_args_4( quark, A, B, C, NB );
    matmul( A, B, C, NB );
}


/* Create a call that will insert a matmul task into the QUARK
 * runtime.  Later, when dependencies are statisfied, the runtime will
 * execute this task.  The arguments to matmul are specified and
 * passed to QUARK here. */
void matmul_quark_call( Quark *quark, double *A, double *B, double *C, int NB )
{
    QUARK_Insert_Task( quark, matmul_quark_task, NULL,
                       sizeof(double)*NB*NB, A, INPUT,
                       sizeof(double)*NB*NB, B, INPUT,
                       sizeof(double)*NB*NB, C, INOUT,
                       sizeof(int), &NB, VALUE,
                       0 );
}

/* Print a matrix */
void matrix_print( char *label, double *A, int N ) 
{
    int i, j;
    printf("%s (intial 4x4 submatrix)\n", label);
    for (i=0; i<N && i<4; i++) {
        for (j=0; j<N && j<4; j++) {
            printf("%5.2f ", A[i + j*N] );
        }
        printf("\n");
    }            
}

/* Compare two matricies */
int matrix_compare( double *A, double *A2, int N ) 
{
    int i, j, nerr=0;
    double diff;
    for (i=0; i<N; i++) {
        for (j=0; j<N; j++) {
            diff = fabs( A[i*N+j] - A2[i*N+j] ); 
            if ( diff > 0.0001 ) {
                //printf("Error: [%d, %d] %5.1f != %5.1f (%f) \n", i, j, A[i*N + j], A2[i*N + j], diff );
                nerr++;
            }
        }            
    }
    return nerr;
}

/* Subtract two gettimeofday timevals */
double timeval_subtract (struct timeval *result, struct timeval *x, struct timeval *y )
{
    /* Perform the carry for the later subtraction by updating y. */
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }
    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;
    /* Return 1 if result is negative. */
    return (double)result->tv_sec + (double)result->tv_usec/1000000.0;
}

/* Convert from standard data layout to block data layout */
void std_to_bdl( double *A, double *Ablk, int N, int NB )
{
    int X, Y, x, y;
    int BB = N/NB;
    for (X = 0; X < BB; X++)
        for (Y = 0; Y < BB; Y++)
            for (x = 0; x < NB; x++)
                for (y = 0; y < NB; y++)
                    Ablk[Y*NB*NB + y + X*NB*NB*BB + x*NB] = A[Y*NB + y + X*NB*N + x*N];
}

/* Convert from block to standard data layout */
void bdl_to_std( double *A, double *Ablk, int N, int NB )
{
    int X, Y, x, y;
    int BB = N/NB;
    for (X = 0; X < BB; X++)
        for (Y = 0; Y < BB; Y++)
            for (x = 0; x < NB; x++)
                for (y = 0; y < NB; y++) {
                    A[Y*NB + y + X*NB*N + x*N] = Ablk[Y*NB*NB + y + X*NB*NB*BB + x*NB];
                }
}


/* Try various ways to do matmul and time them.  Tiled algorithms
 * running serially; multi-threaded QUARK runtime with tiled
 * algorithms; and direct serial computation over standard layout. */
int main_algorithm(int NB, int N, int THREADS)
{
    int i, j, k, nerr=0;
    int BB = N/NB;
    double *A = (double*)malloc(N*N*sizeof(double));
    double *Ablk = (double*)malloc(N*N*sizeof(double));
    double *B = (double*)malloc(N*N*sizeof(double));
    double *Bblk = (double*)malloc(N*N*sizeof(double));
    double *C_direct = (double*)malloc(N*N*sizeof(double));
    double *C = (double*)malloc(N*N*sizeof(double));
    double *Cblk = (double*)malloc(N*N*sizeof(double));
    double *C_quark = (double*)malloc(N*N*sizeof(double));
    double *C_quark_blk = (double*)malloc(N*N*sizeof(double));
    struct timeval tstart, tend, tdiff;
    double t_blk=0, t_quark=0, t_direct=0;

    // Initialize
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            A[i+j*N] = (double)1.0+i;
            B[i+j*N] = (double)2.0+i+j;
            C_quark[i+j*N] = C_direct[i+j*N] = C[i+j*N] = 3.0;
        }
    }

    matrix_print("Printing A", A, N);
    matrix_print("Printing B", B, N);
    matrix_print("Printing C before computation", C, N);

    // Move from F77 to BDL
    std_to_bdl( A, Ablk, N, NB );
    std_to_bdl( B, Bblk, N, NB );
    std_to_bdl( C, Cblk, N, NB );
    std_to_bdl( C_quark, C_quark_blk, N, NB );

    /* ORIGINAL TILED ROUTINE */
    /* This is the code for the serial tile-by-tile multiplication */
    printf("Doing matrix multiplication using serial tile-by-tile algorithm\n");
    gettimeofday( &tstart, NULL );
    for (i = 0; i < BB; i++)
        for (j = 0; j < BB; j++)
            for (k = 0; k < BB; k++)
                matmul ( &Ablk[NB*NB*i + NB*NB*BB*k], &Bblk[NB*NB*k + NB*NB*BB*j], &Cblk[NB*NB*i + NB*NB*BB*j], NB);
    gettimeofday( &tend, NULL );
    t_blk = timeval_subtract( &tdiff, &tend, &tstart );
    printf("Time taken: %f\n", tdiff.tv_sec + (double)tdiff.tv_usec/1000000 );
    bdl_to_std( C, Cblk, N, NB );
    matrix_print("Printing C produced by serial tile-algorithm after computation", C, N);
    printf("\n");

    /* QUARK PARALLEL TILED ROUTINE */
    /* This is the code for the QUARK runtime do do the parallel multi-threaded tile-by-tile algorithm */
    printf("Doing matrix multiplication using the multi-threaded QUARK runtime for a tile based algorithm\n");
    Quark *quark = QUARK_New(THREADS);
    gettimeofday( &tstart, NULL );
    for (i = 0; i < BB; i++)
        for (j = 0; j < BB; j++)
            for (k = 0; k < BB; k++) 
                matmul_quark_call ( quark, &Ablk[NB*NB*i + NB*NB*BB*k], &Bblk[NB*NB*k + NB*NB*BB*j], &C_quark_blk[NB*NB*i + NB*NB*BB*j], NB);
    QUARK_Barrier( quark );
    gettimeofday( &tend, NULL );
    t_quark = timeval_subtract( &tdiff, &tend, &tstart );
    printf("Time taken: %f\n", tdiff.tv_sec + (double)tdiff.tv_usec/1000000 );
    QUARK_Delete(quark);
    bdl_to_std( C_quark, C_quark_blk, N, NB );    
    matrix_print("Printing C produced by QUARK runtime after computation", C_quark, N);
    printf("\n");

    /* DIRECT COMPUTATION OVER STANDARD LAYOUT */
    /* Compute direct C if desired */
    printf("Doing matrix multiplication using direct loops (ie, view matrix as one big tile)\n");
    gettimeofday( &tstart, NULL );
    matmul ( A, B, C_direct, N );
    gettimeofday( &tend, NULL );
    t_direct = timeval_subtract( &tdiff, &tend, &tstart );
    printf("Time taken: %f\n", (double)(tdiff.tv_sec + (double)tdiff.tv_usec/1000000) );
    matrix_print("Printing C produced by direct matmul after computation", C_direct, N);
    printf("\n");

    /* Check for errors */
    printf("Comparing result matrices (direct versus QUARK)\n");
    nerr = matrix_compare( C_direct, C_quark, N );
    printf("Number of differences: %d\n", nerr);    
    printf("\n");    

    printf("Summary of time taken\n");
    printf("%-12s %-12s %-12s (%d threads)\n", "BigLoops", "SerialBlocks", "QUARKBlocks", THREADS);
    printf("%-12.5f %-12.5f %-12.5f\n", t_direct, t_blk, t_quark);
    
    free(A); free(Ablk); free(B); free(Bblk); free(C); free(Cblk); free(C_direct); free(C_quark); free(C_quark_blk);
    return 0;
}

/* Grab command line args and launch the matmul tests */
int main (int argc, char **argv)
{
    int NB, N, THREADS;
    if (argc != 4) {
        printf( "Usage: %s NB N THREADS\n", argv[0] );
        printf( "Usage: Note: N / NB must be an integer\n" );
        printf( "Usage: Assuming a simple test run, with the following parameters\n" );
        printf( "%s 200 800 2\n", argv[0] );
        NB = 200;
        N = 800;
        THREADS = 2;
    } else {
        NB = atoi(argv[1]); 
        N = atoi(argv[2]);
        THREADS = atoi(argv[3]); 
    }
    assert( N % NB == 0 );
    main_algorithm(NB, N, THREADS);
}
