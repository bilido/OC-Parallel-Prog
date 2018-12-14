#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

static const long Num_To_Add = 1000000000;
static const double Scale = 10.0 / RAND_MAX;

long add_serial(const char *numbers) {
    long sum = 0;
    for (long i = 0; i < Num_To_Add; i++) {
        sum += numbers[i];
    }
    return sum;
}

long add_parallel(const char *numbers) {
    long global_sum = 0;
    long sum; // local sum

#pragma omp parallel num_threads(omp_get_max_threads()) \
    private(sum) shared(global_sum)
    {
        sum = 0;

    #pragma omp for schedule(static, 1)
        for(long i=0; i< Num_To_Add; i++){
            sum += numbers[i];
        }

    #pragma omp critical
        global_sum += sum;

    }

   return global_sum;
}

int main() {
    // Due to limitations with how rand() works, we cannot parallelize this generation
    char *numbers = malloc(sizeof(long) * Num_To_Add);
    for (long i = 0; i < Num_To_Add; i++) {
        numbers[i] = (char) (rand() * Scale);
    }

    struct timeval start, end;

    printf("Timing sequential...\n");
    gettimeofday(&start, NULL);
    long sum_s = add_serial(numbers);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    printf("Timing parallel...\n");
    gettimeofday(&start, NULL);
    long sum_p = add_parallel(numbers);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    printf("Sum serial: %ld\nSum parallel: %ld", sum_s, sum_p);

    free(numbers);
    return 0;
}

