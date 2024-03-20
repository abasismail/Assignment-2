#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define ARRAY_SIZE 1000
#define NUM_THREADS 4
#define MAX_VALUE 100

// Define a structure to hold bucket information
struct Bucket {
    int count;
    int* data;
};

// Global variables for the buckets and bucket size
struct Bucket buckets[NUM_THREADS];
int bucket_size = MAX_VALUE / NUM_THREADS;

// Comparison function for qsort
int compare(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

// Function to insert elements into buckets
void* insert_into_buckets(void* arg) {
    int thread_id = *((int*)arg);
    for (int i = 0; i < buckets[thread_id].count; i++) {
        int index = buckets[thread_id].data[i] / bucket_size;
        if (index == NUM_THREADS) {
            index--;
        }
        buckets[index].data[buckets[index].count++] = buckets[thread_id].data[i];
    }
    pthread_exit(NULL);
}

// Function to perform parallel Bucket Sort
void parallel_bucket_sort(int input_array[]) {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Initialize bucket counts and allocate memory for each bucket
    for (int i = 0; i < NUM_THREADS; i++) {
        buckets[i].count = 0;
        buckets[i].data = (int*)malloc(ARRAY_SIZE * sizeof(int));
    }

    // Insert elements into buckets based on their values
    for (int i = 0; i < ARRAY_SIZE; i++) {
        int index = input_array[i] / bucket_size;
        if (index == NUM_THREADS) {
            index--;
        }
        buckets[index].data[buckets[index].count++] = input_array[i];
    }

    // Create threads for inserting elements into buckets
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, insert_into_buckets, &thread_ids[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Sort each bucket using quicksort
    for (int i = 0; i < NUM_THREADS; i++) {
        qsort(buckets[i].data, buckets[i].count, sizeof(int), compare);
    }

    // Merge sorted buckets back into the original array
    int index = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        for (int j = 0; j < buckets[i].count; j++) {
            input_array[index++] = buckets[i].data[j];
        }
    }

    // Free memory allocated for buckets
    for (int i = 0; i < NUM_THREADS; i++) {
        free(buckets[i].data);
    }
}

int main() {
    int input_array[ARRAY_SIZE];

    // Initialize input array with random values
    srand(time(NULL));
    for (int i = 0; i < ARRAY_SIZE; i++) {
        input_array[i] = rand() % MAX_VALUE;
    }

    // Perform parallel Bucket Sort
    parallel_bucket_sort(input_array);

    // Print sorted array
    printf("Sorted Array:\n");
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("%d ", input_array[i]);
    }
    printf("\n");

    return 0;
}
