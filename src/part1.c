#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <time.h> //clock_gettime

#define MIN_VALUE 10.0
#define TOO_FAR_X 20.0
#define TOO_FAR_Y 20.0

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
    stdX = sqrt(stdX);
    stdY = sqrt(stdY);
    stdZ = sqrt(stdZ);

    // print the results
    // printf("Number of points: %d\n", points->n);
    printf("Max X: %lf, Min X: %lf, Std X: %lf, Mean X: %lf\n", maxX, minX, stdX, meanX);
    printf("Max Y: %lf, Min Y: %lf, Std Y: %lf, Mean Y: %lf\n", maxY, minY, stdY, meanY);
    printf("Max Z: %lf, Min Z: %lf, Std Z: %lf, Mean Z: %lf\n", maxZ, minZ, stdZ, meanZ);

    fclose(fp);

    return *points;
}

// pre-processing the points
void decrease_points(point_struct *points)
{
    int nr = points->n;
    printf("Number of points: %d\n", nr);
    int i = 0;

    while (i < nr)
    {
        if (points->x[i] > TOO_FAR_X || abs(points->y[i]) > TOO_FAR_Y) // remove points that are too far away
        {
            if (i < (nr - 1))
            {
                points->x[i] = points->x[i + 1];
                points->y[i] = points->y[i + 1];
                points->z[i] = points->z[i + 1];
                points->n--;
            }
            else
            {
                points->n--;
            }
            i++;
        }
        else if (points->x[i] <= 2.0 && (i > 0)) // remove points that are behind the car
        {
            if (i < (nr - 1))
            {
                points->x[i] = points->x[i + 1];
                points->y[i] = points->y[i + 1];
                points->z[i] = points->z[i + 1];
                points->n--;
            }
            else
            {
                points->n--;
            }
            i++;
        }
        else
        {
            i++;
        }
    }
}

void road_detection(point_struct *points)
{
    double minZ = INT_MIN;
    double maxZ = INT_MAX;
    int size = points->n;
    int search[size];
    int delete[size];

    for (int i = 0; i < size; i++)
    {
        search[i] = i;
    }
}

int main(int argc, char *argv[])
{
    char f1[] = "point_cloud1.txt";
    char f2[] = "point_cloud2.txt";
    char f3[] = "point_cloud3.txt";

    point_struct points1, points2, points3;

    int initial_size, after_process_size;

    // f1
    printf("Reading points from file 1\n");

    points1 = read_points(f1);
    initial_size = points1.n;
    printf("Initial size: %d\n", initial_size);

    decrease_points(&points1);
    after_process_size = points1.n;
    printf("After process size: %d\n", after_process_size);

    // f2
    printf("\n\nReading points from file 2\n\n");

    points2 = read_points(f2);
    initial_size = points2.n;
    printf("Initial size: %d\n", initial_size);

    decrease_points(&points2);
    after_process_size = points2.n;
    printf("After process size: %d\n", after_process_size);

    // f3
    printf("\n\nReading points from file 3\n\n");

    points3 = read_points(f3);
    initial_size = points3.n;
    printf("Initial size: %d\n", initial_size);

    decrease_points(&points3);
    after_process_size = points3.n;
    printf("After process size: %d\n", after_process_size);

    return 0;
}