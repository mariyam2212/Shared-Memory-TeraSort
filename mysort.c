#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#define USE "./mysort <input file> <output file> <number of threads>"
#define BUFFER_SIZE 100
 
 
struct stat st;
// Merges two subarrays of arr[].
// First subarray : arr[l..m]
// Second subarray : arr[m+1..r]
int test;
int NUMBERS_PER_THREAD;
int NUM_THREADS;
int OFFSET;
char **data;
void* thread_merge_sort(void* arg);
void merge_sections_of_array(char *arr[], int number, int aggregation,int length, int merge_sections_of_array);
void merge(char *arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;
 
    /* create temp arrays */
    char **L = calloc(n1,sizeof(char*));
    char **R = calloc(n2,sizeof(char*));
   // char *L[n1], *R[n2];
 
    /* Copy data to arrays L and R */
    for (i = 0; i < n1; i++){
        L[i] = arr[l + i];
       // printf("L1: %c\n",*L[i]);
    }
 
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];
 
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = 0; 
    while (i < n1 && j < n2) {
 
        if (*L[i] < *R[j]) {
            arr[l+k] = L[i];
            i++;
        }
        else if(*L[i] == *R[j]){    //checking if the elements are same
            for(int z=1;z<10;z++){
                if(*(L[i]+z) == *(R[j]+z)){
                    continue;
                }
                else if(*(L[i]+z) < *(R[j]+z)){
                    arr[l+k] = L[i];
                    i++;
                    break;
                }
                else{
                    arr[l+k] = R[j];
                    j++;
                    break;
                }
            }
        }
        else {
            arr[l+k] = R[j];
            j++;
        }
        k++;
    }
 
 
 
 
    /* Copy the remaining elements of L[], if there
    are any */
    while (i < n1) {
        arr[l+k] = L[i];
        i++;
        k++;
    }
 
    /* Copy the remaining elements of R[], if there
    are any */
    while (j < n2) {
        arr[l+k] = R[j];
        j++;
        k++;
    }
 
}
 
/* l is for left index and r is right index of the
sub-array of arr to be sorted */
void mergeSort(char *arr[], int l, int r)
{   
    if (l < r) {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l + (r - l) / 2;
 
        // Sort first and second halves
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
 
        merge(arr, l, m, r);
    }
}
void printArray(char *A[], int size)
{
    printf("print arr: \n");
    int i;
    for (i = 0; i < size; i++)
        {
            printf("i:%d %s\n",i,A[i]);
            printf("\n");
    }
}
// sorting function - reads and write the data
void mysort(char* inputFile, char* outputFile, int numThreads)
{
    char* buffer;
    FILE* fin;
    FILE* fout;
    int i=0;
    test = 100;
    // find size of the file
    stat(inputFile,&st);
    long int size_of_file = st.st_size/100;
    printf("size %li\n",size_of_file);
    printf("n_threads:%d",numThreads);
 
 
    // creating the array for sorting
     data = calloc(size_of_file,sizeof(char*));
    // Open input file
    fin = fopen(inputFile, "r");
    if (fin == NULL) {
        fprintf(stderr, "fopen(%s) failed", inputFile);
        return;
    }
 
    // Allocate memory for the buffer
    buffer = (char*) malloc(sizeof(char) * BUFFER_SIZE);
 
    // Store the data in data char array for sorting
 
 
 
     while (fread(&buffer[0], sizeof(char), BUFFER_SIZE, fin) == BUFFER_SIZE) {
        data[i] = calloc(BUFFER_SIZE,sizeof(char));
 
       for(int z=0;z<98;z++){
               data[i][z]=buffer[z];
       }
      // printf("i:%d %s\n",i,data[i]);
       i++;
   }
 
 
    // Clear buffer and close files
    free(buffer);
 
    fclose(fin);
 
   // printArray(data, size_of_file);
 
     OFFSET = size_of_file  % numThreads;
     NUMBERS_PER_THREAD = size_of_file / numThreads;
     NUM_THREADS=numThreads;
 
    pthread_t threads[numThreads];
    void **args = (void **) malloc(sizeof(void*) * 3);
    int thread_data[3];
    // create threads
    args[0]=*data;
    thread_data[0]=OFFSET;
    thread_data[1]=NUMBERS_PER_THREAD;
    thread_data[2]=NUM_THREADS;
    for (long i = 0; i < numThreads; i ++) {
    	//thread_data[3]=i;
    	args[1]=thread_data;
    	args[2]=&i;
        int rc = pthread_create(&threads[i], NULL, thread_merge_sort, (void *)i); 
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    // join threads
    for(int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }
 
   merge_sections_of_array(data, numThreads,1,size_of_file,NUMBERS_PER_THREAD);
 
    printf("\nSorted array is \n");
   // printArray(data, size_of_file);
 
    // Open output file
    fout = fopen(outputFile, "w");
    if (fout == NULL) {
        fprintf(stderr, "fopen(%s) failed", outputFile);
        return;
    }
   for(i=0;i<size_of_file;i++){
    for(int z=0;z<99;z++){
    	buffer[z]=data[i][z];
    	}
    fwrite(&buffer[0], sizeof(char), BUFFER_SIZE, fout);
    }
    // Clear the buffer and close files
    fclose(fout);
}
 
 /* merge locally sorted sections */
 void merge_sections_of_array(char *arr[], int number, int aggregation,int length,int NUMBERS_PER_THREAD) {
 
    for(int i = 0; i < number; i = i + 2) {
        int left = i * (NUMBERS_PER_THREAD * aggregation);
        int right = ((i + 2) * NUMBERS_PER_THREAD * aggregation) - 1;
        int middle = left + (NUMBERS_PER_THREAD * aggregation) - 1;
        if (right >= length) {
            right = length - 1;
        }
        merge(arr, left, middle, right);
    }
    if (number / 2 >= 1) {
        merge_sections_of_array(arr, number / 2, aggregation * 2,length,NUMBERS_PER_THREAD);
    }
}
 
/** assigns work to each thread to perform merge sort */
void *thread_merge_sort(void* arg)
{	
    int thread_id = (long)arg;
    printf("Offset: %d, NT: %d, NPT: %d, id:%d\n",OFFSET,NUM_THREADS,NUMBERS_PER_THREAD,thread_id);
 
 
    int left = thread_id * (NUMBERS_PER_THREAD);
    int right = (thread_id + 1) * (NUMBERS_PER_THREAD) - 1;
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
 
int main(int argc, char** argv) {
    char* inputFile;
    char* outputFile;
    int numThreads;
    struct timeval start, end;
    double executionTime;
 
    if (argc != 4) {
        fprintf(stderr, USE);
        return 1;
    }
 
    // Read arguments
    inputFile = argv[1];
    outputFile = argv[2];
    numThreads = atoi(argv[3]);
 
    // sorting execution and execution time measurement
    gettimeofday(&start, NULL);
    mysort(inputFile, outputFile, numThreads);
    gettimeofday(&end, NULL);
    executionTime = ((double) end.tv_sec - (double) start.tv_sec)
            + ((double) end.tv_usec - (double) start.tv_usec) / 1000000.0;
 
    printf("input file: %s\n", inputFile);
    printf("output file: %s\n", outputFile);
    printf("number of threads: %d\n", numThreads);
    printf("execution time: %lf\n", executionTime);
 
    return 0;
}