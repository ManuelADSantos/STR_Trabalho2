/*

    COMPILAR: gcc part1.c timestamps.c -o part1 -Wall -lm

*/

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

void divider()
{
    printf("\n========================================\n");
}

struct node
{
    double x;
    double y;
    double z;
    struct node *next;
};

// Store LiDAR points in a struct
struct point_struct
{          // coordinates of a point c -> x[c], y[c], z[c]
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

    // count lines in file
    while (getline(&buffer, &line, fp) >= 0)
    {
        points->n++;
    }
    free(buffer);
    buffer = NULL;
    fseek(fp, SEEK_SET, 0); // rewind file pointer to the beginning of the file

    // // allocate memory for the points
    // points->x = (double *)malloc(points->n * sizeof(double));
    // points->y = (double *)malloc(points->n * sizeof(double));
    // points->z = (double *)malloc(points->n * sizeof(double));

    struct node *now_pt, *prev_pt;

    for (int i = 0; i < points->n; i++)
    {
        struct node *pt = (struct node *)malloc(sizeof(struct node)); // allocate memory for the struct

        if (i == 0)
            points->head = pt;
        else
            prev_pt->next = pt;

        fscanf(fp, "%lf %lf %lf", &pt->x, &pt->y, &pt->z);

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

// remove points that are too far away or behind the car
void decrease_points(struct point_struct *points) // pre-processing the points
{
    int nr = points->n;
    // printf("Number of points: %d\n", nr);
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
        else if (points->x[i] <= 2.0 && (i > 0))
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

/*
 * Car dimensions: 4 x 2
 * Remove points that are too close to the car
 * Grid: 30x20
 * hight: 1.5 <----
 * flat ground: Zmax - Zmin < 0.5 < -----
 */
void road_detection(struct point_struct *points)
{
    double minZ = INT_MIN;
    double maxZ = INT_MAX;

    int size = points->n;
    int search[size];
    int delete[size];
    int numberOfDeletes, aux = 0;

    double xx, yy;

    xx = 2.0;
    yy = 4.0;

    double gridX = 30.0;
    double gridY = 10.0;
    double offset = 0.2;

    int counter = -1;

    for (int i = 0; i < size; i++)
    {
        search[i] = i; // initialize the search array with the indexes
    }

    while (xx < gridX)
    {
        while (yy < gridY)
        {
            for (int i = 0; i < size; i++)
            {
                if ((points->x[search[i]] >= xx) && (points->x[search[i]] <= (xx + offset)) && (points->y[search[i]] >= yy) && (points->y[search[i]] <= (yy + offset)))
                {
                    if (points->z[search[i]] < minZ)
                    {
                        minZ = points->z[search[i]];
                    }
                    if (points->z[search[i]] > maxZ)
                    {
                        maxZ = points->z[search[i]];
                    }

                    counter++; // on the first iteration, it will go to 0
                    search[i] = search[counter];

                    delete[numberOfDeletes] = search[i];
                    numberOfDeletes++;
                }
            }

            if ((abs(maxZ - minZ) < 0.5) || abs(maxZ) > 1.5) // check z now
            {
                aux = numberOfDeletes;
            }
            else
            {
                numberOfDeletes = aux; // remove the current grid from deletes
            }

            // reset min and max
            minZ = INT_MIN;
            maxZ = INT_MAX;

            yy += offset;
        }
        xx += offset;
        yy = -10.0;
    }
    for (int i = 0; i < numberOfDeletes; i++)
    {
        points->x[delete[i]] = points->x[delete[i] + 1];
        points->y[delete[i]] = points->y[delete[i] + 1];
        points->z[delete[i]] = points->z[delete[i] + 1];
        points->n--;
    }
}

// ==== Save results to file ====
void saveToFile(char *filename, struct point_struct points, int size)
{ // open file for writing results
    FILE *outfile;
    char line[50];

    outfile = fopen(filename, "w");

    if (outfile == NULL)
    {
        fprintf(stderr, "\nError openening file\n");
        exit(1);
    }

    for (int i = 0; i < size; i++)
    {
        sprintf(line, "%lf %lf %lf\n", points.x[i], points.y[i], points.z[i]);
        fputs(line, outfile);
        if (ferror(outfile))
        {
            printf("Error writing in file!\n");
            exit(1);
        }
    }
    memset(line, 0, sizeof(line));

    fclose(outfile);
}

// ================================== MAIN ==================================

int main(int argc, char *argv[])
{
    char f1[] = "point_cloud1.txt";
    char f2[] = "point_cloud2.txt";
    char f3[] = "point_cloud3.txt";
    double calc = 0.0;

    struct point_struct points1, points2, points3;
    struct timespec start, end;
    int after_process_size1 = 0, after_process_size2 = 0, after_process_size3 = 0;

    // ================== FILE 1 ==================
    divider();
    printf("\n  Reading points from file 1 ... \n");

    // FUNCTION 1
    clock_gettime(CLOCK_MONOTONIC, &start);
    points1 = read_points(f1);
    clock_gettime(CLOCK_MONOTONIC, &end);
    calc = time_between_timestamp(start, end);
    printf("\nF1: Time to read points of file 1: %lf ms\n", calc);

    // FUNCTION 2
    clock_gettime(CLOCK_MONOTONIC, &start);
    decrease_points(&points1);
    clock_gettime(CLOCK_MONOTONIC, &end);
    calc = time_between_timestamp(start, end);
    printf("\nF2: Time to process points of file 1: %lf ms\n", calc);

    // FUNCTION 3
    clock_gettime(CLOCK_MONOTONIC, &start);
    road_detection(&points1);
    clock_gettime(CLOCK_MONOTONIC, &end);
    calc = time_between_timestamp(start, end);
    printf("\nF3: Time to process points of file 1: %lf ms\n", calc);
    after_process_size1 = points1.n;
    printf("\n === Number of points after process: %d === \n", after_process_size1);

    divider();

    // ================== FILE 2 ==================
    // FUNCTION 1
    printf("\n  Reading points from file 2 ... \n");
    clock_gettime(CLOCK_MONOTONIC, &start);
    points2 = read_points(f2);
    clock_gettime(CLOCK_MONOTONIC, &end);
    calc = time_between_timestamp(start, end);
    printf("\nF1: Time to read points of file 2: %lf ms\n", calc);

    // FUNCTION 2
    clock_gettime(CLOCK_MONOTONIC, &start);
    decrease_points(&points2);
    clock_gettime(CLOCK_MONOTONIC, &end);
    calc = time_between_timestamp(start, end);
    printf("\nF2: Time to process points of file 2: %lf ms\n", calc);

    // FUNCTION 3
    clock_gettime(CLOCK_MONOTONIC, &start);
    road_detection(&points2);
    clock_gettime(CLOCK_MONOTONIC, &end);
    calc = time_between_timestamp(start, end);
    printf("\nF3: Time to process points of file 2: %lf ms\n", calc);
    after_process_size2 = points2.n;
    printf("\n === Number of points after process: %d === \n", after_process_size2);

    divider();

    // ============ FILE 3 ============
    printf("\n  Reading points from file 3...\n");

    // FUNCTION 1
    clock_gettime(CLOCK_MONOTONIC, &start);
    points3 = read_points(f3);
    clock_gettime(CLOCK_MONOTONIC, &end);
    calc = time_between_timestamp(start, end);
    printf("\nF1: Time to read points of file 3: %lf ms\n", calc);

    // FUNCTION 2
    clock_gettime(CLOCK_MONOTONIC, &start);
    decrease_points(&points3);
    road_detection(&points3);
    clock_gettime(CLOCK_MONOTONIC, &end);
    calc = time_between_timestamp(start, end);
    printf("\nF2: Time to process points of file 3: %lf ms\n", calc);

    // FUNCTION 3
    clock_gettime(CLOCK_MONOTONIC, &start);
    road_detection(&points3);
    clock_gettime(CLOCK_MONOTONIC, &end);
    calc = time_between_timestamp(start, end);
    printf("\nF3: Time to process points of file 3: %lf ms\n", calc);
    after_process_size3 = points3.n;
    printf("\n === Number of points after process: %d === \n", after_process_size3);

    divider();

    char outf1[] = "results1.txt";
    char outf2[] = "results2.txt";
    char outf3[] = "results3.txt";

    saveToFile(outf1, points1, after_process_size1);
    printf("Results of file 1 saved\n");
    saveToFile(outf2, points2, after_process_size2);
    printf("Results of file 2 saved\n");
    saveToFile(outf3, points3, after_process_size3);
    printf("Results of file 3 saved\n");

    divider();

    // free memory
    free(points1);
    free(points2);
    free(points3);

    return 0;
}
