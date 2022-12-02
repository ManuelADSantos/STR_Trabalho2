#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h> //clock_gettime

// Store LiDAR points in a struct

typedef struct 
{          // coordinates of a point c -> x[c], y[c], z[c]
    int n; // number of points
    double *x;
    double *y;
    double *z;
} point_struct;

// Read LiDAR points from a file
point_struct read_points(char *filename)
{
    size_t line = 0;
    char *buffer = NULL;
    FILE *fp;
    double *x, *y, *z;
    point_struct pontos;

    // pontos = (struct point_cloud *)malloc(sizeof(struct point_cloud)); // allocate memory for the struct
    //  pontos.n = 0;
    
    // Open file
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error opening file");
        exit(1);
    }

    // count lines in file - number of lines = number of points
    while (getline(&buffer, &line, fp) >= 0)
    {
        pontos.n = pontos.n + 1;
    }

    // return pontos;
}

int main(int argc, char *argv[])
{
    printf("Hello world!");

    return 0;
}