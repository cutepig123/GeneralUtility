#include <stdio.h>
#include <stdlib.h>

void compute(int DIM) {

    int ntasks = 0;

    for (int k = 0; k < DIM; ++k) {

        ++ntasks; // potrf: write A[k,k]
        
        for (int m = k+1; m < DIM; ++m) {
            ++ntasks; // trsm: read A[k,k] write A[m,k]
        }

        for (int m = k+1; m < DIM; ++m) {
            ++ntasks; // syrk: read A[k,k] write A[m,k]

            for (int n = k+1; n < m; ++n) {
                ++ntasks; // gemm: read A[m,k], A[n,k] write A[m,n]
            }
        }
    }

    printf("n: %d num_tasks: %d\n", DIM, ntasks);
}

int main(int argc, char *argv[]) {

    int n = 5;

    if (argc == 2) {
        n = atoi(argv[1]);
    }

    compute(n);
    return 0;
}
