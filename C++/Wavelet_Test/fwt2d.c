#include <stdio.h>
#include <math.h>
#include "fwt.h"

void fwt2d (
        double s0[N][N],
        unsigned int s_len,
        double p[],
        double q[],
        unsigned int sup,
        double s1[N2][N2],
        double w1v[N2][N2],
        double w1h[N2][N2],
        double w1d[N2][N2]){
        
    double s1x[N][N2], w1x[N][N2];
    double s1xt[N2][N], w1xt[N2][N];
    double s1t[N2][N2], w1ht[N2][N2];
    double w1vt[N2][N2], w1dt[N2][N2];
    unsigned int i, j;
    
    for (j=0; j < s_len; j++){
        fwt1d(s0[j], s_len, p, q, sup, s1x[j], w1x[j]);
    }
    
    for (j=0; j < s_len; j++){
        for (i=0; i < s_len/2; i++){
            s1xt[i][j] = s1x[j][i];
            w1xt[i][j] = w1x[j][i];
        }
    }
    
    for (j=0; j < s_len/2; j++){
        fw1d(s1xt[j], s_len, p, q, sup, s1t[j], w1ht[j]);
        fw1d(w1xt[j], s_len, p, q, sup, w1vt[j], w1dt[j]);
    }
    
    for (j=0; j < s_len/2; j++){
        for (i=0; i < s_len/2; i++){
            s1[i][j] = s1t[j][i];
            w1h[i][j] = w1ht[j][i];
            w1v[i][j] = w1vt[j][i];
            w1d[i][j] = w1dt[j][i];
        }
    }
    
}