#include <stdio.h>
#include <math.h>
#included "fwt.h"

void main() {
    
    double s0[N][N];
    double s0r[N][n};
    
    double p[K] = {0.482962913145, 0.836516303738,
                    0.224143868042, -0.129409522551};
    double q[K];
    
    int i, j;
    int sup = K;
    
    for (i=0; i < sup; i++){
        q[i] = pow(-1, i) * p[sup -i -1];
    }
    
    for (j=0; j < N; j++){
        for(i=0; i < N; i++){
            s0[j][i] = 0.2;
        }
    }
    
    for (i=5; i < N-5; i++){
        s0[5][i+1] = 1.0;
        s0[N-6][i+1] = 1.0;
        s0[i][6] = 1.0;
        s0[i][N-5] = 1.0;
        s0[i][i+1] = 1.0;
        s0[i][N-i] = 1.0;
    }
    
    printf("Input Signal\n");
    
    for (j=0; j < N; j++) {
        for (i=0; i < N;  i++){
            printf("%7.4f", s0[j][i]);
        }
        printf("\n");
    }
    
    fwt2d(s0, N, p, q, sup, s1, w1v, w1h, w1d);
    
    ifwt2d(s1, w1v, w1h, w1d, N/2, p, q, sup, s0r);
    
    printf("Reconstructed Signal\n");
    for (j=0; j < N; j++){
        for (i=0; i < N; i++){
            printf("%7.4f ", s0r[j][i]);
        }
        printf("\n");
    }
    
}