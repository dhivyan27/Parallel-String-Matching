#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h> 
#include <time.h>

// Function prototypes

char* ReadFromFile(char* pFilename, long* pOutSize);

bool KMPSearch(char* pat, char* txt);

void ProcessQueryFileAndSearch(char* queryFilename, char* book_content);


int main() {
    long file_size;
    struct timespec start, end;
    double time_taken;

    printf("Reading the text file...\n");
    clock_gettime(CLOCK_MONOTONIC, &start);

    char* book_content = ReadFromFile("Shakespeare.txt", &file_size);
    if (book_content == NULL) {
        printf("Could not read the file content.\n");
        return 1;
    }

    // Process the query file and search each word in the text.
    ProcessQueryFileAndSearch("single_query.txt", book_content);

    free(book_content);

    // Get the clock current time again and calculate overall time
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_taken = (end.tv_sec - start.tv_sec) * 1e9;
    time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
    printf("Overall time (Including read and search)(s): %lf\n", time_taken);

    return 0;
}


char* ReadFromFile(char* pFilename, long* pOutSize) {
    FILE* file = fopen(pFilename, "r");
    if (file == NULL) {
        printf("Could not open the file.\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = (char*)malloc(file_size + 1);
    if (content == NULL) {
        printf("Memory allocation error.\n");
        fclose(file);
        return NULL;
    }

    fread(content, 1, file_size, file);
    content[file_size] = '\0';
    fclose(file);

    *pOutSize = file_size;
    return content;
}


void ProcessQueryFileAndSearch(char* queryFilename, char* book_content) {
    FILE* queryFile = fopen(queryFilename, "r");
    if (!queryFile) {
        printf("Error opening the query file.\n");
        return;
    }

    char line[256];
    
    struct timespec startSearch, endSearch;
    double search_time;

    // Start the timer for all the searches
    clock_gettime(CLOCK_MONOTONIC, &startSearch);

    while (fgets(line, sizeof(line), queryFile) != NULL) {
        char* word = strtok(line, " ");
        
        if (word && KMPSearch(word, book_content)) {
            printf("'%s' found in text!\n", word);
        } else {
            printf("'%s' not found in text!\n", word);
        }
    }

    // End the timer after all searches are done
    clock_gettime(CLOCK_MONOTONIC, &endSearch);
    search_time = (endSearch.tv_sec - startSearch.tv_sec) * 1e9;
    search_time = (search_time + (endSearch.tv_nsec - startSearch.tv_nsec)) * 1e-9;
    printf("Total time taken for all KMP searches(s): %lf\n", search_time);

    fclose(queryFile);
}



// Function to compute array that holds the longest suffix that is also a prefix

void compute_LPS_Array(char* pattern, int* lps_arr, int M) {

    int index = 0;
    lps_arr[0] = 0;

    for (int i = 1; i < M;) {
        if (pattern[i] == pattern[index]) {
            lps_arr[i] = index + 1;
            index++;
            i++;
        } else {
            if (index != 0) {
                index = lps_arr[index - 1];
            } else {
                lps_arr[i] = 0;
                i++;
            }
        }
    }
}

// KMP pattern search algorithm
bool KMPSearch(char* pat, char* txt) {

    // calculating the lengths and storing in variables will optimise as we don't need to constantly calculate throughout the algorithm

    int M = strlen(pat); 
    int N = strlen(txt);

    int lps_arr[M];  //declare array on integers with length M

    compute_LPS_Array(pat, lps_arr, M);

    int i = 0;  // index for txt
    int j = 0;  // index for pat

    while (i < N && j < M) {
        if (txt[i] == pat[j]) {
            i++;
            j++;
        } else {
            if (j != 0) {
                j = lps_arr[j - 1];
            } else {
                i++;
            }
        }
    }
    return (j == M);
}
