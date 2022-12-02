#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h> //clock_gettime

// Store LiDAR points in a struct

struct point_cloud // coordinates of a point c -> x[c], y[c], z[c]
{
    int n; // number of points
    double *x;
    double *y;
    double *z;
};
