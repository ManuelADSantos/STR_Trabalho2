#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h> //clock_gettime

// Store LiDAR points in a struct

struct point_cloud
{          // coordinates of a point c -> x[c], y[c], z[c]
    int n; // number of points
    double *x;
    double *y;
    double *z;
};

typedef struct point_cloud point_cloud;

// Read LiDAR points from a file
point_cloud read_points(char *filename)
{
    FILE *fp;
    int pts;
    double *x, *y, *z;
    struct point_could *pontos;

    pontos = malloc(sizeof(pontos)); // allocate memory for the struct
    pontos->n = 0;

    // Open file
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error opening file");
        exit(1);
    }

    return pontos;
}