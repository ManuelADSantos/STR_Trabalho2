#include "timestamps.h"
#include "time.h"
#include "stdio.h"

struct timespec sub_timestamp(struct timespec begin, struct timespec end)
{
    struct timespec result;

    result.tv_sec = end.tv_sec - begin.tv_sec;
    result.tv_nsec = end.tv_nsec - begin.tv_nsec;

    if (result.tv_nsec < 0)
    {
        result.tv_sec -= 1;
        result.tv_nsec += 1e9;
    }

    return result;
}

struct timespec sum_timestamp(struct timespec begin, struct timespec end)
{
    struct timespec result;

    result.tv_sec = end.tv_sec + begin.tv_sec;
    result.tv_nsec = end.tv_nsec + begin.tv_nsec;

    if (result.tv_nsec > 1e9)
    {
        result.tv_sec += 1;
        result.tv_nsec -= 1e9;
    }

    return result;
}

// Returns time in milliseconds
double time_between_timestamp(struct timespec begin, struct timespec end)
{
    struct timespec calc;
    calc = sub_timestamp(begin, end);
    double result = (calc.tv_sec) * 1e3 + (calc.tv_nsec) / 1e6;

    return result;
}

void print_timestamp(struct timespec timestamp)
{
    printf("sec = %ld || nsec = %ld\n\n", timestamp.tv_sec, timestamp.tv_nsec);
}