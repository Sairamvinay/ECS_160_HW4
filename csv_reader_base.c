#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

int getWordCount(const char* text, int length)
{
    int wordCount = 0;
    if(length >= 2) { //Due to quotes;
        wordCount += 1;
    }
    int i = 0;
    for(i = 0; i < length; i++){
        if(' ' == text[i]) {
            wordCount++;
        }
    }
    return wordCount;
}

char* getContent(const char* token) {
    char* content = (char *)malloc(sizeof(char)*strlen(token));
    bool appendToContent = false;
    int numQuotes = 0;
    int lastQuoteLoc = -1;
    int iter = 0;

    for (int i = 0; i < strlen(token); i++) {
        if (token[i] == '"') {
            numQuotes++;
            lastQuoteLoc = i;
        }
    }

    if (numQuotes == 1) {
        printf("Incorrect number of quotes: %s\n", token);
        return NULL;
    }
    if (numQuotes == 0) {
        appendToContent = true;
    }

    for (int i = 0; i < strlen(token); i++) {
        if (i == lastQuoteLoc) {
            break;
        }

        if (appendToContent == true) {
            char token_char = token[i];
            content[iter] = token_char;
            iter++;
        }

        if (token[i] == '"') {
            appendToContent = true;
        }
    }
    return content;
}

int getFieldColumn(char* line, char* field) {
    int name_column = -1;
    int num_name_cols = 0;
    int iter = 0;
    const char* token;
    char* content = "";
    token = strtok(line, ",");
   
    /* walk through other tokens */
    while(token != NULL) {
        iter++;
        token = strtok(NULL, ",");
        if (token == NULL) {
            break;
        }

        content = getContent(token);
        if (content == NULL) {
            free(content);
            continue;
        }

        if (strcmp(content, field) == 0) {
            name_column = iter + 1;
            num_name_cols++;
            if (num_name_cols != 1) {
                name_column = -1; // checks that the field column appears once
            }
            free(content);
        }
    }
    return name_column;
}

const char* getfield(char* line, int num) {
    const char* tok;
    for (tok = strtok(line, ","); ; tok = strtok(NULL, ",\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}

int getNumCols(char* line) {
    int num_cols = 0;
    for (int i = 0; i <  strlen(line); i++) {
        if (line[i] == ',') {
            num_cols++;
        }
    }
    return num_cols;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("Invalid number of arguments\n");
        exit(0);
    }

    if (access(argv[1], F_OK) == -1) {
        printf("File provided does not exist.\n");
        exit(0);
    }

    printf("Input file: %s\n", argv[1]);
    FILE* stream = fopen(argv[1], "r");

    char line[1024];
    float lines = 0;
    int name_column = -1;
    int num_cols_in_header = -1;
    int num_cols_in_file = -1;

    while (fgets(line, 1024, stream)) {   
        lines++;
        if (lines == 1) {
            char* first_line = strdup(line);
            name_column = getFieldColumn(first_line, "negativereason_gold");
            num_cols_in_header = getNumCols(line);
            free(first_line);
            printf("NAME COLUMN: %d\n", name_column);
            printf("Number of columns: %d\n", num_cols_in_header);
        } else {
            num_cols_in_file = getNumCols(line);
            if (num_cols_in_file != num_cols_in_header) {
                printf("Invalid number of commas in file.\n");
                exit(0);
            }
        }
    }

}