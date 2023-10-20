#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int LENGTH = 100;

long int findSize(char file_name[])
{
    FILE *fp = fopen(file_name, "r+");
    if (fp == NULL)
    {
        printf("FILE DOESN't EXIST!\n");
        return -1;
    }

    fseek(fp, 0L, SEEK_END);

    long int res = ftell(fp);
    res = res / (1024 * 1024 * 1024);
    fclose(fp);

    return res;
}

int merge(char **list, int leftStart, int leftEnd, int rightStart, int rightEnd)
{

    int leftLength = leftEnd - leftStart;
    int rightLength = rightEnd - rightStart;

    char *leftHalf = (char *)malloc(leftLength * sizeof(char *));

    char *rightHalf = (char *)malloc(rightLength * sizeof(char *));

    int r = 0;
    int l = 0;
    int i = 0;

    for (i = leftStart; i < leftEnd; i++, l++)
    {

        leftHalf[l] = (char *)malloc(LENGTH * sizeof(char));
        strcpy(leftHalf[l], list[i]);
    }

    for (i = rightStart; i < rightEnd; i++, r++)
    {

        rightHalf[r] = (char *)malloc(LENGTH * sizeof(char));
        strcpy(rightHalf[r], list[i]);
    }

    for (i = leftStart, r = 0, l = 0; l < leftLength && r < rightLength; i++)
    {
        if (strcmp(leftHalf[l], rightHalf[r]) < 0)
        {
            strcpy(list[i], leftHalf[l++]);
        }
        else
        {
            strcpy(list[i], rightHalf[r++]);
        }
    }

    for (; l < leftLength; i++, l++)
    {
        strcpy(list[i], leftHalf[l]);
    }
    for (; r < rightLength; i++, r++)
    {
        strcpy(list[i], rightHalf[r]);
    }
    l = 0;
    r = 0;
    for (i = leftStart; i < leftEnd; i++, l++)
    {
        free(leftHalf[l]);
    }

    for (i = rightStart; i < rightEnd; i++, r++)
    {

        free(rightHalf[r]);
    }

    free(leftHalf);

    free(rightHalf);

    return 0;
}

int mergeSort_r(int left, int right, char **list)
{
    if (right - left <= 1)
    {
        return 0;
    }

    int leftStart = left;
    int leftEnd = (left + right) / 2;
    int rightStart = leftEnd;
    int rightEnd = right;

    mergeSort_r(leftStart, leftEnd, list);

    mergeSort_r(rightStart, rightEnd, list);

    merge(list, leftStart, leftEnd, rightStart, rightEnd);
}

int mergeSort(char **list, int length)
{
    mergeSort_r(0, length, list);
    return 0;
}

//CHanging values as per the file size 10000000 for 1 gb and 40000000 for 4GB
int NUMBER_OF_RECORDS = 10000000;
//int NUMBER_OF_RECORDS = 40000000;

int main(int argc, char **argv)
{
    char *inputFile;
    char *outputFile;
    int numThreads;
    struct timeval start, end;
    double executionTime;

    if (argc != 4)
    {
        fprintf("Error in arguments");
        return 1;
    }

    // Read arguments
    inputFile = argv[1];
    outputFile = argv[2];
    numThreads = atoi(argv[3]);

    char temp;
    char *Strings = (char *)malloc(NUMBER_OF_RECORDS * sizeof(char *));
    char array_i[100];

    FILE *fp[2];
    gettimeofday(&start, NULL);
    fp[0] = fopen(inputFile, "r");
    for (int j = 0; j < NUMBER_OF_RECORDS; j++)
    {
        fscanf(fp[0], "%[^\n]%*c", array_i);
        Strings[j] = (char *)malloc(LENGTH * sizeof(char));

        strcpy(Strings[j], array_i);
    }

    mergeSort(Strings, NUMBER_OF_RECORDS);

    fp[1] = fopen(outputFile, "w+");
    for (int j = 0; j < NUMBER_OF_RECORDS; j++)
    {
        fprintf(fp[1], "%s\n", Strings[j]);
    }
    gettimeofday(&end, NULL);
    executionTime = ((double)end.tv_sec - (double)start.tv_sec) + ((double)end.tv_usec - (double)start.tv_usec) / 1000000.0;

    printf("input file: %s\n", inputFile);
    printf("output file: %s\n", outputFile);
    printf("number of threads: %d\n", numThreads);
    printf("execution time: %lf\n", executionTime);

    return 0;
}

/*

TRIED CODE FOR EXTERNAL SORTING


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NUM 5000000
#define LEN 100

int merge(char **list, int left_start, int left_end, int right_start, int right_end)
{

    int left_length = left_end - left_start;
    int right_length = right_end - right_start;

    char *left_half = (char *)malloc(left_length * sizeof(char *));

    char *right_half = (char *)malloc(right_length * sizeof(char *));

    // char left_half[left_length][LEN];
    // char right_half[right_length][LEN];

    int r = 0;
    int l = 0;
    int i = 0;

    for (i = left_start; i < left_end; i++, l++)
    {

        left_half[l] = (char *)malloc(LEN * sizeof(char));
        strcpy(left_half[l], list[i]);
    }

    for (i = right_start; i < right_end; i++, r++)
    {

        right_half[r] = (char *)malloc(LEN * sizeof(char));
        strcpy(right_half[r], list[i]);
    }

    for (i = left_start, r = 0, l = 0; l < left_length && r < right_length; i++)
    {
        if (strcmp(left_half[l], right_half[r]) < 0)
        {
            strcpy(list[i], left_half[l++]);
        }
        else
        {
            strcpy(list[i], right_half[r++]);
        }
    }

    for (; l < left_length; i++, l++)
    {
        strcpy(list[i], left_half[l]);
    }
    for (; r < right_length; i++, r++)
    {
        strcpy(list[i], right_half[r]);
    }
    l = 0;
    r = 0;
    for (i = left_start; i < left_end; i++, l++)
    {
        free(left_half[l]);
    }

    for (i = right_start; i < right_end; i++, r++)
    {

        free(right_half[r]);
    }

    free(left_half);

    free(right_half);

    return 0;
}

int mergeSort_r(int left, int right, char **list)
{ // Overloaded portion
    if (right - left <= 1)
    {
        return 0;
    }

    int left_start = left;
    int left_end = (left + right) / 2;
    int right_start = left_end;
    int right_end = right;

    mergeSort_r(left_start, left_end, list);

    mergeSort_r(right_start, right_end, list);

    merge(list, left_start, left_end, right_start, right_end);
}

int mergeSort(char **list, int length)
{ // First part
    mergeSort_r(0, length, list);
    return 0;
}

char *file[] = {"file1.txt", "file2.txt",
                "res1.txt", "res2.txt"};

void splitData()
{
    // Change this
    int count = 10000000;

    // char *Strings = (char *)malloc(NUM * sizeof(char *));
    char array_i[100];
    int line = 0;
    // DATA data[MAX_DATA_SIZE];

    FILE *fp[3];
    int i, val;
    fp[2] = fopen("test-file.txt", "r");
    // printf("This is line 40");
    //     if (!fp[2]) {
    //             printf("splitData-fopen failed- %s\n", filename);
    //             exit(0);
    //     }

    fp[0] = fopen(file[0], "w+");
    fp[1] = fopen(file[1], "w+");

    if (!fp[0] || !fp[1])
    {
        fcloseall();
        printf("splitData - fopen failed\n");
        exit(1);
    }

    // printf("This is line 54");
    // char array_i[100];
    puts("before fscan 1/n");
    for (i = 0; i < count / 2; i++)
    {
        // puts("after iteration");
        fscanf(fp[2], "%[^\n]%*c", array_i);
        // Strings[i] = (char *)malloc(LEN * sizeof(char));
        // printf("%s\n", array_i);
        // strcpy(Strings[j], array_i);
        fprintf(fp[0], "%s\n", array_i);
        // puts("after iteration");
        //  free();

        // read(data, line, fp[2]);
        // strcpy(data[line].id, strtok(data[line].id, " "));
        // fprintf(fp[0], "%s, %s, %s\n", data[line].id, data[line].key, data[line].value);
    }
    // printf("This is line 62");

    for (i = count / 2; i < count; i++)
    {
        fscanf(fp[2], "%[^\n]%*c", array_i);
        // Strings[i] = (char *)malloc(LEN * sizeof(char));
        //  printf("%s", array_i);
        // strcpy(Strings[j], array_i);
        fprintf(fp[1], "%s\n", array_i);

        // read(data, line, fp[2]);
        // strcpy(data[line].id, strtok(data[line].id, " "));
        // fprintf(fp[1], "%s, %s, %s\n", data[line].id, data[line].key, data[line].value);
    }
    fcloseall();
    return;
}

int main()
{
    char temp;
    int i = 0;

    char *Strings = (char *)malloc(NUM * sizeof(char *));
    char array_i[100];
    FILE *fp[3];
    // for (int k = 0; k < NUM; k++)
    // {

    // }

    // printf("Please enter %d strings, one per line:\n", NUM);
    // for (i; i < 5; i++)
    // {
    //     fgets(&Strings[i][0], LEN, stdin);
    // }
    puts("before split");
    // splitData();

    puts("after split");
    fp[0] = fopen("test-file.txt", "r");
    for (int j = 0; j < NUM; j++)
    {
        fscanf(fp[0], "%[^\n]%*c", array_i);
        Strings[j] = (char *)malloc(LEN * sizeof(char));
        // printf("%s\n", array_i);
        // printf("%s\n", Strings[j]);
        // printf("%d",j);
        strcpy(Strings[j], array_i);
        printf("%s\n", Strings[j]);
    }

    // printf("%s", &Strings[NUM - 1]);
    // puts("\nHere are the strings in the order you entered:");
    // for (i; i < NUM; i++)
    // {
    //     printf("%s\n", Strings[i]);
    // }

    mergeSort(Strings, NUM);
    // i = 0;
    // puts("\nHere are the strings in alphabetical order");
    // for (i; i < NUM; i++)
    //{
    //     printf("%s\n", Strings[i]);
    // }

    i = 0;
    fp[1] = fopen("file1.txt", "w+");
    for (int j = 0; j < NUM; j++)
    {
        fprintf(fp[1], "%s\n", Strings[j]);
        printf("%s", Strings[j]);
    }

    fp[2] = fopen("file2.txt", "w+");
    for (int j = 0; j < NUM; j++)
    {
        fscanf(fp[0], "%[^\n]%*c", array_i);
        // Strings[j] = (char *)malloc(LEN * sizeof(char));
        //  printf("%s", array_i);
        strcpy(Strings[j], array_i);
    }

    mergeSort(Strings, NUM);

    i = 0;
    // fp2 = fopen("file2.txt", "w+");
    for (int j = 0; j < NUM; j++)
    {
        fprintf(fp[2], "%s\n", Strings[j]);
    }

    fcloseall();

    return 0;
}
*/
