#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h> //clock_gettime
#define MIN_VALUE 10.0
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

    double maxX, minX, maxY, minY, maxZ, minZ;
    maxX = maxY = maxZ = 0.0;
    minX = minY = minZ = MIN_VALUE;

    double stdX, stdY, stdZ;
    stdX = stdY = stdZ = 0.0;

    double sumX, sumY, sumZ;
    sumX = sumY = sumZ = 0.0;

    double meanX, meanY, meanZ;

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

    for (int i = 0; i < points->n; i++)
    {
        fscanf(fp, "%lf %lf %lf", &points->x[i], &points->y[i], &points->z[i]);
        // check if the point is valid
        if (points->x[i] > maxX) // x
        {
            maxX = points->x[i];
        }
        if (points->x[i] < minX)
        {
            minX = points->x[i];
        }
        if (points->y[i] > maxY) // y
        {
            maxY = points->y[i];
        }
        if (points->y[i] < minY)
        {
            minY = points->y[i];
        }
        if (points->z[i] > maxZ) // z
        {
            maxZ = points->z[i];
        }
        if (points->z[i] < minZ)
        {
            minZ = points->z[i];
        }
        // sum up the points
        sumX += points->x[i];
        sumY += points->y[i];
        sumZ += points->z[i];

        meanX = sumX / points->n;
        meanY = sumY / points->n;
        meanZ = sumZ / points->n;
    }
    // calculate standard deviation
    /*
     *  σ=√[ [1/(N-1)] * ∑(Xi−μ)^2 ]
     *  where μ is the mean of the data set, and N is the number of data points.
     */

    for (int i = 0; i < points->n; i++) // ∑(Xi−μ)^2
    {
        stdX += pow(points->x[i] - meanX, 2);
        stdY += pow(points->y[i] - meanY, 2);
        stdZ += pow(points->z[i] - meanZ, 2);
    }
    // divide by N-1
    stdX = stdX / ((points->n) - 1);
    stdY = stdY / ((points->n) - 1);
    stdZ = stdZ / ((points->n) - 1);

    // take the square root
    stdX = sqrt(stdX / points->n);
    stdY = sqrt(stdY / points->n);
    stdZ = sqrt(stdZ / points->n);

    // return points;
}

int main(int argc, char *argv[])
{
    printf("Hello world!");

    return 0;
}