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
    point_struct *points;

    points = malloc(sizeof(point_struct)); // allocate memory for the struct
    points->n = 0;

    // Open file
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error opening file");
        exit(1);
    }

    // count lines in file
    while (getline(&buffer, &line, fp) >= 0)
    {
        points->n++;
    }
    free(buffer);
    buffer = NULL;
    fseek(fp, SEEK_SET, 0); // rewind file pointer to the beginning of the file

    // allocate memory for the points
    points->x = (double *)malloc(points->n * sizeof(double));
    points->y = (double *)malloc(points->n * sizeof(double));
    points->z = (double *)malloc(points->n * sizeof(double));

    // return points;
}

int main(int argc, char *argv[])
{
    printf("Hello world!");

    return 0;
}