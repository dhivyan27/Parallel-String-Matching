#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h> 
#include <time.h>
#include <omp.h>


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
    int M = strlen(pat);
    int N = strlen(txt);
    int lps_arr[M];
    compute_LPS_Array(pat, lps_arr, M);
    
    bool found = false;

    #pragma omp parallel
    {
        int thread_num = omp_get_thread_num();
        int num_threads = omp_get_num_threads();

        // Each thread checks a segment of the text
        int start = (N / num_threads) * thread_num;
        int end = (N / num_threads) * (thread_num + 1);
        
        // We add overlap equal to the pattern length - 1 to ensure no missed matches
        if(thread_num != num_threads - 1) end += M - 1;

        int i = start;
        int j = 0;

        while (i < end && j < M) {
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

        // If a thread finds a match, it sets the shared variable "found" to true
        if (j == M) {
            #pragma omp critical
            {
                found = true;
            }
        }
    }

    return found;
}
