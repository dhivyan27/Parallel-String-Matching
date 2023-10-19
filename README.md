# Parallel Boyer-Moore String Matching Algorithm

This program utilizes a parallel implementation of the Boyer-Moore string matching algorithm to search for query strings within a given text. 

## Features:
1. Efficient string matching with the Boyer-Moore algorithm.
2. Parallel processing to speed up the search.
3. Capability to handle large texts and queries.

## Dependencies:
- Ensure you have an appropriate C/C++ compiler installed.
- This program may use specific parallel processing libraries; ensure they are set up on your system.

## Usage:

### Step 1: Setup

Clone the repository to your local machine:

```bash
git clone [Repository URL]
cd [Repository Directory]
```

### Step 2: Compilation

Compile the source files:

```bash
gcc kmp.c -o Out
```
### Step 3: Choose your query and text

Replace the placeholder lines in the source code:

```c
char* book_content = ReadFromFile("Shakespeare.txt", &file_size);
```

and

```c
ProcessQueryFileAndSearch("single_query.txt", book_content);
```

with your preferred text and query file. For example:

```c
char* book_content = ReadFromFile("my_large_text.txt", &file_size);
```

and

```c
ProcessQueryFileAndSearch("my_queries.txt", book_content);
```

### Step 4: Run

Execute the compiled program:

```bash
./Out
```

The program will output the positions of the found query strings within the text.

## Troubleshooting:
- Ensure all the required libraries are installed and linked correctly.
- If working with large files, ensure your system has sufficient memory.
- If the program is too slow, consider increasing the number of threads (if the implementation supports it).


## License:

This project is licensed under the MIT License.
