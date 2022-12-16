#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <time.h> //clock_gettime
#include "timestamps.h"
#include <string.h>

// valores do enunciado
#define MIN_VALUE 10.0
#define TOO_FAR_X 30.0
#define TOO_FAR_Y 10.0

/*
    NÓS DA LISTA LIGADA
*/
struct node
{
    double x;
    double y;
    double z;
    struct node *next;
};

// Store LiDAR points in a struct
struct point_struct
{
    int n; // number of points
    struct node *head;
};

// Read LiDAR points from a file
struct point_struct read_points(char *filename) // recieve a file name and return a struct with the points
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

    struct point_struct *points;

    points = (struct point_struct *)malloc(sizeof(struct point_struct)); // allocate memory for the struct
    points->n = 0;

    // Open file
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error opening file");
        exit(1);
    }

    // Count lines in file
    while (getline(&buffer, &line, fp) >= 0)
    {
        points->n++;
    }
    free(buffer);
    buffer = NULL;
    fseek(fp, SEEK_SET, 0); // rewind file pointer to the beginning of the file

    struct node *now_pt, *prev_pt;

    for (int i = 0; i < points->n; i++)
    {
        struct node *pt = (struct node *)malloc(sizeof(struct node)); // allocate memory for the node of linked list

        if (i == 0)
            points->head = pt;
        else
            prev_pt->next = pt;

        fscanf(fp, "%lf %lf %lf", &pt->x, &pt->y, &pt->z);

        // printf("Point %d of %d: %lf %lf %lf\n", i + 1, points->n, pt->x, pt->y, pt->z);

        // check if the point is valid
        if (pt->x > maxX) // x
            maxX = pt->x;

        if (pt->x < minX)
            minX = pt->x;

        if (pt->y > maxY) // y
            maxY = pt->y;

        if (pt->y < minY)
            minY = pt->y;

        if (pt->z > maxZ) // z
            maxZ = pt->z;

        if (pt->z < minZ)
            minZ = pt->z;

        // sum up the points
        sumX += pt->x;
        sumY += pt->y;
        sumZ += pt->z;

        meanX = sumX / points->n;
        meanY = sumY / points->n;
        meanZ = sumZ / points->n;

        prev_pt = pt;
    }
    // calculate standard deviation
    /*
     *  σ=√[ [1/(N-1)] * ∑(Xi−μ)^2 ]
     *  where μ is the mean of the data set, and N is the number of data points.
     */

    now_pt = points->head;

    for (int i = 0; i < points->n; i++) // ∑(Xi−μ)^2
    {
        if (now_pt->next == NULL)
            break;

        stdX += pow(now_pt->x - meanX, 2);
        stdY += pow(now_pt->y - meanY, 2);
        stdZ += pow(now_pt->z - meanZ, 2);

        now_pt = now_pt->next;
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
    printf("Results:\n");
    printf("Number of points: %d\n", points->n);
    printf("Max X: %lf, Min X: %lf, Std X: %lf, Mean X: %lf\n", maxX, minX, stdX, meanX);
    printf("Max Y: %lf, Min Y: %lf, Std Y: %lf, Mean Y: %lf\n", maxY, minY, stdY, meanY);
    printf("Max Z: %lf, Min Z: %lf, Std Z: %lf, Mean Z: %lf\n", maxZ, minZ, stdZ, meanZ);

    fclose(fp);

    return *points;
}

/*
struct node
{
    double x;
    double y;
    double z;
    struct node *next;
};

// Store LiDAR points in a struct
struct point_struct
{
    int n; // number of points
    struct node *head;
};
*/

int main(int argc, char *argv[])
{
    char f1[] = "point_cloud1.txt";

    struct point_struct points1;

    points1 = read_points(f1);
    int n = points1.n;
    printf("\n");
    printf("Number of points: %d\n", n);
    printf("%lf\n", points1.head->x);
    printf("2: %lf\n", points1.head->next->x);

    printf("atribuicao:\n\n");

    struct node first;
    struct node second;

    first = *points1.head;
    printf("first: %lf\n", first.x);

    return 0;
}