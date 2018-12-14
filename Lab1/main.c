#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

// Returns a random value between -1 and 1
double getRand() {
    return (double) rand() * 2 / (double) (RAND_MAX) - 1;
}

long double Calculate_Pi_Sequential(long long number_of_tosses) {
    long long int number_of_hits_in_circle = 0;
    double x, y, distance_squared;

    for (int toss = 0; toss < number_of_tosses; toss++) {
        x = getRand();
        y = getRand();
        distance_squared = x*x + y*y;
        if (distance_squared <= 1)
            number_of_hits_in_circle += 1;
    }

    double pi_estimate = 4*number_of_hits_in_circle/((double) number_of_tosses);

    return pi_estimate;
}

long double Calculate_Pi_Parallel(long long number_of_tosses) {

    long long int number_of_hits_in_circle = 0;
    double x, y, distance_squared;

#pragma omp parallel for num_threads(number_of_tosses) \
     reduction(+: number_of_hits_in_circle) private(x, y, distance_squared)
        for (long toss = 0; toss < number_of_tosses; toss++) {
            x = getRand();
            y = getRand();
            distance_squared = x * x + y * y;
            if (distance_squared <= 1)
                number_of_hits_in_circle += 1;
        }

    double pi_estimate = 4*number_of_hits_in_circle/((double) number_of_tosses);

    return pi_estimate;
}

int main() {
    struct timeval start, end;

    long long int num_tosses = 10000000;

    printf("Timing sequential...\n");
    gettimeofday(&start, NULL);
    long double sequential_pi = Calculate_Pi_Sequential(num_tosses);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    printf("Timing parallel...\n");
    gettimeofday(&start, NULL);
    long double parallel_pi = Calculate_Pi_Parallel(num_tosses);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    // This will print the result to 10 decimal places
    printf("π = %.10Lf (sequential)", sequential_pi);
    printf("π = %.10Lf (parallel)", parallel_pi);

    return 0;
}
