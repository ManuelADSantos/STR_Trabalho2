#ifndef TIMESTAMPS
#define TIMESTAMPS "timestamps.h"

struct timespec sub_timestamp(struct timespec begin, struct timespec end);

struct timespec sum_timestamp(struct timespec begin, struct timespec end);

double time_between_timestamp(struct timespec begin, struct timespec end);

void print_timestamp(struct timespec timestamp);

double return_milli(struct timespec timestamp);

#endif