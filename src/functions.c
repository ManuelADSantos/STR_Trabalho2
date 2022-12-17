/*
    COMPILE: gcc -c functions.c timestamps.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <time.h> //clock_gettime
#include <string.h>

// valores do enunciado
#define MIN_VALUE 10.0
#define TOO_FAR_X 15.0
#define TOO_FAR_Y 10.0

// Aesthetic divider
void divider()
{
    printf("\n========================================\n");
}

// Linked list node
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
struct point_struct *read_points(char *filename) // recieve a file name and return a pointer to a struct with the points
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

    return points;
}

// remove points that are too far away or behind the car
void decrease_points(struct point_struct *points) // pre-processing the points
{
    struct node *now_pt = points->head;
    struct node *prev_pt, *aux_pt;

    while (1)
    {
        if (now_pt == NULL)
            break;

        // remove points that are too far away or behind the car
        if ((now_pt->x < 0.0) || ((now_pt->x <= 2.0) && (abs(now_pt->y <= 0.2))) || (now_pt->x > TOO_FAR_X) || (abs(now_pt->y) > TOO_FAR_Y) || now_pt->y > 1.9 || (now_pt->y < -3 && now_pt->x > 2 && now_pt->x < 4 && now_pt->z > -0.8))
        {
            if (points->head == now_pt)
            {
                points->head = now_pt->next;
            }
            else
            {
                prev_pt->next = now_pt->next;
            }

            points->n--;
            aux_pt = now_pt->next;
            // free(now_pt);
            now_pt = aux_pt;
        }
        else
        {
            prev_pt = now_pt;
            now_pt = now_pt->next;
        }
    }
}

// /*
//  * Car dimensions: 4 x 2
//  * Remove points that are too close to the car
//  * Grid: 30x20
//  * hight: 1.5 <----
//  * flat ground: Zmax - Zmin < 0.5 < -----
//  */
void road_detection(struct point_struct *points)
{

    /*
    x = cellsize * ( i - (colums/2) ) + startX
    y = cellsize * ( j - (rows/2) ) + startY

    i = floor ((( x - startX + (cellsize/2)) / cellsize ) + (cols/2))
    J = floor ((( y - startY + (cellsize/2)) / cellsize ) + (rows/2))

    */

    double minZ = 0.0;
    double maxZ = 1.5;

    struct node *now_pt;
    struct node *prev_pt, *aux_pt;

    int invalid = 0;

    double xx, yy;

    xx = 0.0;
    yy = 0.0;

    double gridX = 20.0;
    double gridY = 20.0;
    double offset = 0.5;

    while (xx < gridX)
    {
        while (yy < gridY)
        {
            now_pt = points->head;
            while (1)
            {
                if (now_pt == NULL)
                    break;

                // search for min and max values of z in the current grid cell
                if (((now_pt->x >= xx) && (now_pt->x <= (xx + offset))) && ((now_pt->y + 10.0 >= yy) && (now_pt->y + 10.0 <= (yy + offset))))
                {
                    if (now_pt->z < minZ)
                    {
                        minZ = now_pt->z;
                    }
                    if (now_pt->z > maxZ)
                    {
                        maxZ = now_pt->z;
                    }
                }

                now_pt = now_pt->next;
            }
            // mark the existence of invalid points
            if (abs(maxZ - minZ) < 0.1 || maxZ > 1.5)
                invalid = 1;

            if (invalid == 1)
            {
                now_pt = points->head; // reset the pointer to the head of the list

                while (1)
                {
                    if (now_pt == NULL)
                        break;
                    // remove unwanted points
                    if (((now_pt->x >= xx) && (now_pt->x <= (xx + offset))) && ((now_pt->y + 10.0 >= yy) && (now_pt->y + 10.0 <= (yy + offset))))
                    {
                        if (points->head == now_pt)
                        {
                            points->head = now_pt->next;
                        }
                        else
                        {
                            prev_pt->next = now_pt->next;
                        }

                        points->n--; // decrease the number of points
                        aux_pt = now_pt->next;
                        free(now_pt);
                        now_pt = aux_pt;
                    }
                    else
                    {
                        prev_pt = now_pt;
                        now_pt = now_pt->next;
                    }
                }
            }
            // reset min and max
            minZ = 0.0;
            maxZ = 1.5;

            yy += offset;
            invalid = 0;
        }
        xx += offset;
        yy = 0.0;
    }
}

// ==== Save results to file ====
void saveToFile(char *filename, struct point_struct *points, int size)
{ // open file for writing results
    FILE *outfile;
    char line[50];
    struct node *pt = points->head;

    outfile = fopen(filename, "w");

    if (outfile == NULL)
    {
        fprintf(stderr, "\nError openening file\n");
        exit(1);
    }

    for (int i = 0; i < size; i++)
    {
        sprintf(line, "%lf %lf %lf\n", pt->x, pt->y, pt->z);
        fputs(line, outfile);
        if (ferror(outfile))
        {
            printf("Error writing in file!\n");
            exit(1);
        }
        if (pt->next == NULL)
        {
            break;
        }
        else
        {
            pt = pt->next;
        }
    }
    memset(line, 0, sizeof(line));

    fclose(outfile);
}