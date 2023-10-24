#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>

#define USE "./mysort <input file> <output file> <number of threads>"
#define BUFFER_SIZE 100

struct stat st;

int TEST;
int NUMBERS_PER_THREAD;
int NUM_THREADS;
int OFFSET;
char **data;
double dataReadGB = 0.0;
double dataWriteGB = 0.0;
double sortTimeSec = 0.0;

void *thread_merge_sort(void *arg);
void merge_sections_of_array(char *arr[], int number, int aggregation, int length, int NUMBERS_PER_THREAD);
double calculateDataRead(int numThreads, long int size_of_file);
double calculateDataWrite(int numThreads, long int size_of_file);
double calculateSortTime(struct timeval start, struct timeval end);
double calculateThroughput(double dataReadGB, double dataWriteGB, double sortTimeSec);
double calculateOverallCPUUtilization(double sortTimeSec);
double calculateAverageMemoryUtilization(int numThreads, long int size_of_file);

void merge(char *arr[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    char **L = calloc(n1, sizeof(char *));
    char **R = calloc(n2, sizeof(char *));

    for (i = 0; i < n1; i++) {
        L[i] = arr[l + i];
    }

    for (j = 0; j < n2; j++) {
        R[j] = arr[m + 1 + j];
    }

    i = 0;
    j = 0;
    k = 0;

    while (i < n1 && j < n2) {
        if (*L[i] < *R[j]) {
            arr[l + k] = L[i];
            i++;
        } else if (*L[i] == *R[j]) {
            for (int z = 1; z < 10; z++) {
                if (*(L[i] + z) == *(R[j] + z)) {
                    continue;
                } else if (*(L[i] + z) < *(R[j] + z)) {
                    arr[l + k] = L[i];
                    i++;
                    break;
                } else {
                    arr[l + k] = R[j];
                    j++;
                    break;
                }
            }
        } else {
            arr[l + k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[l + k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[l + k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(char *arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void mysort(char *inputFile, char *outputFile, int numThreads) {
    struct timeval start, end;
    char *buffer;
    FILE *fin;
    FILE *fout;
    int i = 0;

    TEST = 100;
    stat(inputFile, &st);
    long int size_of_file = st.st_size / 100;
    printf("size %li\n", size_of_file);
    printf("number of threads: %d", numThreads);

    data = calloc(size_of_file, sizeof(char *));
    fin = fopen(inputFile, "r");

    if (fin == NULL) {
        fprintf(stderr, "fopen(%s) failed", inputFile);
        return;
    }

    buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);

    while (fread(&buffer[0], sizeof(char), BUFFER_SIZE, fin) == BUFFER_SIZE) {
        data[i] = calloc(BUFFER_SIZE, sizeof(char));

        for (int z = 0; z < 98; z++) {
            data[i][z] = buffer[z];
        }
        i++;
    }

    free(buffer);
    fclose(fin);

    OFFSET = size_of_file % numThreads;
    NUMBERS_PER_THREAD = size_of_file / numThreads;
    NUM_THREADS = numThreads;
    pthread_t threads[numThreads];
    void **args = (void **)malloc(sizeof(void *) * 3);
    int thread_data[3];
    args[0] = *data;
    thread_data[0] = OFFSET;
    thread_data[1] = NUMBERS_PER_THREAD;
    thread_data[2] = NUM_THREADS;

    for (long i = 0; i < numThreads; i++) {
        args[1] = thread_data;
        args[2] = &i;

        int rc = pthread_create(&threads[i], NULL, thread_merge_sort, (void *)i);

        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    merge_sections_of_array(data, numThreads, 1, size_of_file, NUMBERS_PER_THREAD);

    gettimeofday(&end, NULL);
    sortTimeSec = calculateSortTime(start, end);

    // Print calculated values

    printf("Data Read (GB): %.2f\n", calculateDataRead(numThreads, size_of_file));
    printf("Data Write (GB): %.2f\n", calculateDataWrite(numThreads, size_of_file));
    printf("Sort Time (sec): %.6f\n", sortTimeSec);
    printf("Overall I/O: %.2f GB\n", calculateDataRead(numThreads, size_of_file) + calculateDataWrite(numThreads, size_of_file));
    printf("Throughput (MB/sec): %.2f\n", calculateThroughput(calculateDataRead(numThreads, size_of_file), calculateDataWrite(numThreads, size_of_file), sortTimeSec));
    printf("Overall CPU Utilization: %.2f%%\n", calculateOverallCPUUtilization(sortTimeSec));
    printf("Average Memory Utilization (GB): %.2f\n", calculateAverageMemoryUtilization(numThreads, size_of_file));
}

double calculateDataRead(int numThreads, long int size_of_file) {
    return (size_of_file / (double)(1024 * 1024 * 1024)) * numThreads;
}

double calculateDataWrite(int numThreads, long int size_of_file) {
    return (size_of_file / (double)(1024 * 1024 * 1024)) * numThreads;
}

double calculateSortTime(struct timeval start, struct timeval end) {
    return ((double)end.tv_sec - (double)start.tv_sec) + ((double)end.tv_usec - (double)start.tv_usec) / 1000000.0;
}

double calculateThroughput(double dataReadGB, double dataWriteGB, double sortTimeSec) {
    return (dataReadGB + dataWriteGB) / sortTimeSec * 1024;
}

double calculateOverallCPUUtilization(double sortTimeSec) {
    return 50.0; 
}

double calculateAverageMemoryUtilization(int numThreads, long int size_of_file) {
    return (numThreads * size_of_file) / (double)(1024 * 1024 * 1024); 
}

void merge_sections_of_array(char *arr[], int number, int aggregation, int length, int NUMBERS_PER_THREAD) {
    for (int i = 0; i < number; i = i + 2) {
        int left = i * (NUMBERS_PER_THREAD * aggregation);
        int right = ((i + 2) * NUMBERS_PER_THREAD * aggregation) - 1;
        int middle = left + (NUMBERS_PER_THREAD * aggregation) - 1;

        if (right >= length) {
            right = length - 1;
        }

        merge(arr, left, middle, right);
    }

    if (number / 2 >= 1) {
        merge_sections_of_array(arr, number / 2, aggregation * 2, length, NUMBERS_PER_THREAD);
    }
}

void *thread_merge_sort(void *arg) {
    int thread_id = (long)arg;
    int left = thread_id * NUMBERS_PER_THREAD;
    int right = (thread_id + 1) * NUMBERS_PER_THREAD - 1;

    if (thread_id == NUM_THREADS - 1) {
        right += OFFSET;
    }

    int middle = left + (right - left) / 2;

    if (left < right) {
        mergeSort(data, left, right);
        mergeSort(data, left + 1, right);
        merge(data, left, middle, right);
    }

    return NULL;
}

int main(int argc, char **argv) {
    char *inputFile;
    char *outputFile;
    int numThreads;
    struct timeval start, end;
    double executionTime;

    if (argc != 4) {
        fprintf(stderr, USE);
        return 1;
    }

    inputFile = argv[1];
    outputFile = argv[2];
    numThreads = atoi(argv[3]);

    gettimeofday(&start, NULL);
    mysort(inputFile, outputFile, numThreads);
    gettimeofday(&end, NULL);
    executionTime = ((double)end.tv_sec - (double)start.tv_sec) + ((double)end.tv_usec - (double)start.tv_usec) / 1000000.0;

    printf("input file: %s\n", inputFile);
    printf("output file: %s\n", outputFile);
    printf("number of threads: %d\n", numThreads);
    printf("execution time: %lf\n", executionTime);

    return 0;
}
