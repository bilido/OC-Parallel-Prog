#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>
#include <memory.h>

static const long Num_To_Sort = 1000000000;

// Sequential version of your sort
// If you're implementing the PSRS algorithm, you may ignore this section

void swap(long* valA, long* valB){
    long temp = *valA;
    *valA = *valB;
    *valB = temp;
}

// QuickSort Algorithm implementation Reference: https://www.geeksforgeeks.org/quick-sort/

/* This function takes last element as pivot, places
   the pivot element at its correct position in sorted
    array, and places all smaller (smaller than pivot)
   to left of pivot and all greater elements to right
   of pivot */
long partition (int* arr[], int low, int high){
    // pivot (Element to be placed at right position)
    long pivot = arr[high];
    int i = (low - 1); // Index of smaller element
    for (int j=low; j <= high - 1; j++){
        // If current element is smaller than or
        // equal to pivot
        if (arr[j] <= pivot){
            i++;    // increment index of smaller element
            swap(&arr[i],&arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);

    return (i + 1);
}

void quickSort_s(int* arr, int low, int high){
    if(low < high){
        /* pi is partitioning index, arr[pi] is now
           at right place */
        long pi = partition(arr, low, high);

        quickSort_s(arr, low, pi - 1);  // Before pi
        quickSort_s(arr, pi + 1, high); // After pi
    }
}

void sort_s(int* arr) {

    quickSort_s(arr, 0, Num_To_Sort - 1);
}

void quickSort_p(int* arr, int low, int high){
    if(low < high){
        long pi = partition(arr, low, high);

        #pragma  omp parallel sections
        {
            #pragma omp section
            quickSort_p(arr, low, pi - 1);

            #pragma omp section
            quickSort_p(arr, pi + 1, high);
        }

    }
}

// Parallel version of your sort
void sort_p(int *arr) {

    long num_to_sort_p = sizeof(arr) / sizeof(arr[0]);
    quickSort_p(arr, 0, num_to_sort_p - 1);
}

int main() {
    int *arr_s = malloc(sizeof(int) * Num_To_Sort);
    long chunk_size = Num_To_Sort / omp_get_max_threads();
#pragma omp parallel num_threads(omp_get_max_threads())
    {
        int p = omp_get_thread_num();
        unsigned int seed = (unsigned int) time(NULL) + (unsigned int) p;
        long chunk_start = p * chunk_size;
        long chunk_end = chunk_start + chunk_size;
        for (long i = chunk_start; i < chunk_end; i++) {
            arr_s[i] = rand_r(&seed);
        }
    }

    // Copy the array so that the sorting function can operate on it directly.
    // Note that this doubles the memory usage.
    // You may wish to test with slightly smaller arrays if you're running out of memory.
    int *arr_p = malloc(sizeof(int) * Num_To_Sort);
    memcpy(arr_p, arr_s, sizeof(int) * Num_To_Sort);

    struct timeval start, end;

    printf("Timing sequential...\n");
    gettimeofday(&start, NULL);
    sort_s(arr_s);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    free(arr_s);

    printf("Timing parallel...\n");
    gettimeofday(&start, NULL);
    sort_p(arr_p);
    gettimeofday(&end, NULL);
    printf("Took %f seconds\n\n", end.tv_sec - start.tv_sec + (double) (end.tv_usec - start.tv_usec) / 1000000);

    free(arr_p);

    return 0;
}

