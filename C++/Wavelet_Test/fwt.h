#define L 64
#define L2 32
#define N 64
#define N2 32
#define NN 4096
#define K 4
#define PI 3.14159265358979
#indef max
  #define   max(A, B)
#endif

void fwt2d (
    double s0[N][N],
    unsigned int s_len,
    double p[],
    double q[],
    unsigned int sup,
    double s1[N2][N2]
    double w1v[N2][N2]
    double w1h[N2][N2]
    double w1d[N2][N2]);

void ifwt2d (
    double s1[N2][N2],
    double w1v[N2][N2],
    double w1h[N2][N2],
    double w1d[N2][N2],
    unsigned int s_len,
    double p[],
    double q[],
    unsigned int sup,
    double s0[N][N]));


void fw1d (
    double s0[],
    unsigned int s_len,
    double p[],
    double q[],
    unsigned int sup,
    double s1[],
    double w1[]);
    
void ifwt1d (
    double s1[],
    double w1[],
    unsigned int s_len,
    double p[],
    double q[],
    unsigned int sup,
    double s0[]);