#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int getWordCount(const char* text, int length)
{
    int wordCount = 0;
    if(length >= 2) //Due to quotes;
    {
        wordCount += 1;
    }
    int i = 0;
    for(i = 0; i < length; i++){
	if(' ' == text[i])
        {
	    wordCount++;
        }
    }
    return wordCount;
}

int getNameColumn(char* line) {
    int name_column = -1;
    int iter = 0;
    const char* token;
    printf("Line: %s", line);
    //printf("Got here\n");
    //for (tok = strtok(line, ","); tok && *tok; tok = strtok(NULL, ",\n"))
    token = strtok(line, ",");
   
    /* walk through other tokens */
    while(token != NULL) {
       //printf( " %s\n", token );
       iter++; 
       token = strtok(NULL, ",");
       if (strcmp(token, "\"name\"") == 0) {
	 name_column = iter + 1;
         break;
       }
    }
    return name_column;
}

const char* getfield(char* line, int num)
{
    const char* tok;
    //for (tok = strtok(line, ","); tok && *tok; tok = strtok(NULL, ",\n"))
    for (tok = strtok(line, ","); ; tok = strtok(NULL, ",\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
    	printf("Invalid number of arguments\n");
        exit(0);
    }
    printf("Input file: %s\n", argv[1]);
    FILE* stream = fopen(argv[1], "r");

    char line[1024];
    float lines = 0;
    float lenTotal = 0;
    int name_column = -1;
    while (fgets(line, 1024, stream))
    {   
	/*
	char* tmp = strdup(line);
	const char* out = getfield(tmp, 12);
        float nextLength = strlen(out);
        lines++;
        printf("Text: %s\n", out);
        printf("Length: %f\n", nextLength);
        
        float wC = getWordCount(out, nextLength);
        printf("Word Count: %f\n", wC);
        int aveCPW = (nextLength - (wC-1))/wC;
	printf("Chars per word: %d\n", aveCPW);
        char* wordTwoGuess = (char *) malloc(aveCPW+1 * sizeof(char));
        memcpy(wordTwoGuess, &out[aveCPW + 1], aveCPW);
        printf("Guess at second word: %s\n", wordTwoGuess);
        */
        lines++;
	if (lines == 1) {
           //printf(line);
           char* first_line = strdup(line);
	   name_column = getNameColumn(first_line);
	   free(first_line);
           printf("NAME COLUMN: %d\n", name_column);
        }
        /*
        if(wC == 2)
	{
      	  printf("%c\n", out[2000048]);
	}

	if(wC > 5)
	{
	  free(tmp);
	}
        lenTotal += nextLength;
        // NOTE strtok clobbers tmp
        free(tmp);
        */
    }

    //printf("Average Tweet Length: %f\n", lenTotal/lines);
}
