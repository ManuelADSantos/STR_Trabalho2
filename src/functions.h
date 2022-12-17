#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <time.h> //clock_gettime
#include <string.h>

struct point_struct *read_points(char *filename);
void decrease_points(struct point_struct *points);
void road_detection(struct point_struct *points);
void saveToFile(char *filename, struct point_struct *points, int size);
void divider();