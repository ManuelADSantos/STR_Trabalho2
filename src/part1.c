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

// Read LiDAR points from a file
struct point_cloud read_points(char *filename)
{
    FILE *fp;
    int pts;
    double *x, *y, *z;
    struct point_could *pontos;

    pontos = (struct point_cloud *)malloc(sizeof(struct point_cloud)); // allocate memory for the struct
    pontos.n = 0;

    // Open file
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error opening file");
        exit(1);
    }

    return pontos;
}

int main(int argc, char *argv[])
{
    printf("Hello world!");

    return 0;
}