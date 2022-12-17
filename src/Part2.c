// =========================================================================
//                                PART 2
// =========================================================================
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <time.h> //clock_gettime
#include "timestamps.h"
#include "functions.h"
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

// Semaphores
sem_t f1Tof2;
sem_t f2Tof3;

// Linked list node
struct node
{
    double x;
    double y;
    double z;
    struct node *next;
};

struct timespec zeroHour;

struct point_struct *fromF1, *fromF2, *fromF3;

// thread responsável por ler os pontos do ficheiro dado
void *thread_read(void *arguments)
{
    double auxBegin, auxEnd, auxTarget;
    double regists[9];
    struct timespec target, end, sleep, begin;
    struct timespec frequency = {0, 1e8}; // 100ms

    // LiDAR takes 100ms to give a file with points
    clock_gettime(CLOCK_MONOTONIC, &begin); //  begin (when it started)
    sleep = sub_timestamp(zeroHour, begin);

    // wait until the first file is ready
    if (sleep.tv_nsec > 0)
    {
        printf("\nWaiting for the first file to be ready...\n");
        clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep, NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &begin);
    fromF1 = read_points("point_cloud1.txt");
    clock_gettime(CLOCK_MONOTONIC, &end);

    auxBegin = time_between_timestamp(zeroHour, begin);

    target = sum_timestamp(begin, frequency);             // target time = begin + 100ms (frequency)
    auxTarget = time_between_timestamp(zeroHour, target); // target time = begin + 100ms (frequency)

    auxEnd = time_between_timestamp(zeroHour, end);

    sem_post(&f1Tof2);

    regists[0] = auxBegin;
    regists[1] = auxTarget;
    regists[2] = auxEnd;

    //  ============== FILE 2 ==============
    divider();
    clock_gettime(CLOCK_MONOTONIC, &begin); //  begin (when it started)
    sleep = sub_timestamp(zeroHour, begin);

    // wait until the first file is ready
    if (sleep.tv_nsec > 0)
    {
        printf("\nWaiting for the second file to be ready...\n");
        clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep, NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &begin);
    fromF1 = read_points("point_cloud2.txt");
    clock_gettime(CLOCK_MONOTONIC, &end);

    auxBegin = time_between_timestamp(zeroHour, begin);

    target = sum_timestamp(begin, frequency); // target time = begin + 100ms (frequency)
    auxTarget = time_between_timestamp(zeroHour, target);

    auxEnd = time_between_timestamp(zeroHour, end);

    sem_post(&f1Tof2);

    regists[3] = auxBegin;
    regists[4] = auxTarget;
    regists[5] = auxEnd;

    clock_gettime(CLOCK_MONOTONIC, &begin); //  begin (when it started)
    sleep = sub_timestamp(zeroHour, begin);

    // ================= FILE 3 =================
    divider();
    if (sleep.tv_nsec > 0)
    {
        printf("\nWaiting for the third file to be ready...\n");
        clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep, NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &begin);
    fromF1 = read_points("point_cloud3.txt");
    clock_gettime(CLOCK_MONOTONIC, &end);

    auxBegin = time_between_timestamp(zeroHour, begin);

    target = sum_timestamp(begin, frequency);             // target time = begin + 100ms (frequency)
    auxTarget = time_between_timestamp(zeroHour, target); // target time = begin + 100ms (frequency)

    auxEnd = time_between_timestamp(zeroHour, end);

    sem_post(&f1Tof2);

    regists[6] = auxBegin;
    regists[7] = auxTarget;
    regists[8] = auxEnd;

    printf("\n\n");
    printf(" ==== Thread 1 ==== \n");
    printf("                 BEGIN                   TARGET                   END     \n");
    printf("point_cloud1    %lf               %lf               %lf           \n", regists[0], regists[1], regists[2]);
    printf("point_cloud2    %lf               %lf               %lf           \n", regists[3], regists[4], regists[5]);
    printf("point_cloud3    %lf               %lf               %lf           \n", regists[6], regists[7], regists[8]);

    return NULL;
}

// =======================================================================

// thread responsável por pre processar  os pontos
void *thread_preprocess(void *arguments)
{
    divider();

    double auxBegin, auxEnd, auxTarget;
    double regists[9];
    struct timespec begin, target, end, sleep;
    struct timespec frequency = {0, 1e8};

    sem_wait(&f1Tof2);

    // ================= FILE 1 =================
    clock_gettime(CLOCK_MONOTONIC, &begin);
    fromF2 = fromF1;
    decrease_points(fromF2);
    clock_gettime(CLOCK_MONOTONIC, &end);
    sem_post(&f2Tof3);

    auxBegin = time_between_timestamp(zeroHour, begin);
    target = sum_timestamp(begin, frequency); // target time = begin + 100ms (frequency)
    auxTarget = time_between_timestamp(zeroHour, target);
    auxEnd = time_between_timestamp(zeroHour, end);

    regists[0] = auxBegin;
    regists[1] = auxTarget;
    regists[2] = auxEnd;

    target = sum_timestamp(begin, frequency);
    sleep = sub_timestamp(end, target);
    clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep, NULL);

    sem_wait(&f1Tof2);
    clock_gettime(CLOCK_MONOTONIC, &begin);
    fromF2 = fromF1;
    decrease_points(fromF2);
    clock_gettime(CLOCK_MONOTONIC, &end);

    auxBegin = time_between_timestamp(zeroHour, begin);

    target = sum_timestamp(begin, frequency);             // target time = begin + 100ms (frequency)
    auxTarget = time_between_timestamp(zeroHour, target); // target time = begin + 100ms (frequency)

    auxEnd = time_between_timestamp(zeroHour, end);

    sem_post(&f2Tof3);

    regists[3] = auxBegin;
    regists[4] = auxTarget;
    regists[5] = auxEnd;

    target = sum_timestamp(begin, frequency);
    sleep = sub_timestamp(end, target);
    clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep, NULL);

    sem_wait(&f1Tof2);
    clock_gettime(CLOCK_MONOTONIC, &begin);
    fromF2 = fromF1;
    decrease_points(fromF2);
    clock_gettime(CLOCK_MONOTONIC, &end);

    auxBegin = time_between_timestamp(zeroHour, begin);

    target = sum_timestamp(begin, frequency);             // target time = begin + 100ms (frequency)
    auxTarget = time_between_timestamp(zeroHour, target); // target time = begin + 100ms (frequency)

    auxEnd = time_between_timestamp(zeroHour, end);

    sem_post(&f2Tof3);

    regists[6] = auxBegin;
    regists[7] = auxTarget;
    regists[8] = auxEnd;

    divider();
    printf("\n\n");
    printf(" ==== Thread 2 ==== \n");
    printf("                 BEGIN                   TARGET                   END     \n");
    printf("point_cloud1    %lf               %lf               %lf           \n", regists[0], regists[1], regists[2]);
    printf("point_cloud2    %lf               %lf               %lf           \n", regists[3], regists[4], regists[5]);
    printf("point_cloud3    %lf               %lf               %lf           \n", regists[6], regists[7], regists[8]);

    return NULL;
}

// =======================================================================

// thread responsável por processar os pontos
void *thread_process(void *arguments)
{
    double auxBegin, auxEnd, auxTarget;
    double regists[9];
    struct timespec begin, target, end, sleep;
    struct timespec frequency = {0, 1e8};

    sem_wait(&f2Tof3);

    // ================= FILE 1 =================
    clock_gettime(CLOCK_MONOTONIC, &begin);
    fromF3 = fromF2;
    decrease_points(fromF3);
    clock_gettime(CLOCK_MONOTONIC, &end);

    target = sum_timestamp(begin, frequency);
    sleep = sub_timestamp(end, target);
    clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep, NULL);

    auxBegin = time_between_timestamp(zeroHour, begin);
    target = sum_timestamp(begin, frequency); // target time = begin + 100ms (frequency)
    auxTarget = time_between_timestamp(zeroHour, target);
    auxEnd = time_between_timestamp(zeroHour, end);

    regists[0] = auxBegin;
    regists[1] = auxTarget;
    regists[2] = auxEnd;

    sem_wait(&f2Tof3);
    clock_gettime(CLOCK_MONOTONIC, &begin);
    fromF3 = fromF2;
    decrease_points(fromF3);
    clock_gettime(CLOCK_MONOTONIC, &end);

    auxBegin = time_between_timestamp(zeroHour, begin);
    target = sum_timestamp(begin, frequency); // target time = begin + 100ms (frequency)
    auxTarget = time_between_timestamp(zeroHour, target);
    auxEnd = time_between_timestamp(zeroHour, end);

    regists[3] = auxBegin;
    regists[4] = auxTarget;
    regists[5] = auxEnd;

    target = sum_timestamp(begin, frequency);
    sleep = sub_timestamp(end, target);
    clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep, NULL);

    sem_wait(&f2Tof3);
    clock_gettime(CLOCK_MONOTONIC, &begin);
    fromF3 = fromF2;
    decrease_points(fromF2);
    clock_gettime(CLOCK_MONOTONIC, &end);

    auxBegin = time_between_timestamp(zeroHour, begin);
    target = sum_timestamp(begin, frequency); // target time = begin + 100ms (frequency)
    auxTarget = time_between_timestamp(zeroHour, target);
    auxEnd = time_between_timestamp(zeroHour, end);

    regists[6] = auxBegin;
    regists[7] = auxTarget;
    regists[8] = auxEnd;

    divider();
    printf("\n\n");
    printf(" ==== Thread 3 ==== \n");
    printf("                 BEGIN                   TARGET                   END     \n");
    printf("point_cloud1    %lf               %lf               %lf           \n", regists[0], regists[1], regists[2]);
    printf("point_cloud2    %lf               %lf               %lf           \n", regists[3], regists[4], regists[5]);
    printf("point_cloud3    %lf               %lf               %lf           \n", regists[6], regists[7], regists[8]);

    return NULL;
}
// ================================== MAIN ==================================
int main(int argc, char *argv[])
{

    pthread_t threads[3];
    int thread_args[3];
    short i;

    // vamos inicializar os semáforos
    sem_init(&f1Tof2, 0, 0);
    sem_init(&f2Tof3, 0, 0);

    // vamos obter o tempo inicial
    clock_gettime(CLOCK_MONOTONIC, &zeroHour);

    // vamos criar todas as threads, uma a uma
    printf("IN MAIN: Creating thread 1. \n");
    threads[0] = 0;
    thread_args[0] = 0;
    int result = pthread_create(&threads[0], NULL, thread_read, &thread_args[0]);
    printf("result: %d", result);

    printf("IN MAIN: Creating thread 2. \n");
    threads[1] = 1;
    thread_args[1] = 1;
    pthread_create(&threads[1], NULL, thread_preprocess, &thread_args[1]);

    printf("IN MAIN: Creating thread 3. \n");
    threads[2] = 2;
    thread_args[2] = 2;
    pthread_create(&threads[2], NULL, thread_process, &thread_args[2]);

    printf("IN MAIN: All threads are created. \n");

    // vamos aguardar que todas as threads terminem
    for (i = 0; i < 3; i++)
    {
        pthread_join(threads[i], NULL);
    }

    divider();
    printf("\nIN MAIN: All threads ended\n");

    // vamos apagar os semáforos criados
    printf("\nIN MAIN: Destroying semaphores. \n");
    sem_destroy(&f1Tof2);
    sem_destroy(&f2Tof3);

    return 0;
}
