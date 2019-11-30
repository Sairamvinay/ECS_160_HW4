#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

struct TweeterCount {   
   int numTweets;
   char* tweeterName;
};

char* getContent(const char* token) {
    if (strlen(token) == 0) {
        return NULL;
    }

    static char content[1024];
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

    content[iter] = '\0';
    return content;
}

int getFieldColumn(char* line, char* field) {
    int name_column = 0;
    int num_name_cols = 0;
    int iter = 0;
    char* token;
    char* content = "";
   
    while ((token = strsep(&line, ","))) {
        iter++;
        if (token == NULL) {
            break;
        }

        content = getContent(token);
        if (content == NULL) {
            continue;
        }

        if (strcmp(content, field) == 0) {
            name_column = iter + 1;
            num_name_cols++;
            if (num_name_cols != 1) {
                return -1; // checks that the field column appears once
            }
        }
    }
    return name_column - 1;
}

char* getField(char* line, int num) {
    char* tok;
    int iter = 0;

    while ((tok = strsep(&line, ","))) {
        if (iter++ == num - 1) {
            return getContent(tok);
        }
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

void addTweeter(struct TweeterCount TweeterCountPtr[], int num_tweeters, char* tweeterName) {
    struct TweeterCount newTweeter = {.numTweets = 1};
    newTweeter.tweeterName = (char *) malloc(1024);
    strcpy(newTweeter.tweeterName, tweeterName);
    TweeterCountPtr[num_tweeters - 1] = newTweeter;
    return;
}

int searchAndUpdateTweeter(struct TweeterCount TweeterCountPtr[], int num_tweeters, char* tweeterName) {
    for (int i = 0; i < num_tweeters; i++) {
        if (strcmp(TweeterCountPtr[i].tweeterName, tweeterName) == 0) {
            TweeterCountPtr[i].numTweets = TweeterCountPtr[i].numTweets + 1;
            return 0;
        }
    }
    return -1;
}

void printTopTenTweeters(struct TweeterCount TweeterCountPtr[], int num_tweeters) {
    int end_of_iter = 10;
    if (num_tweeters < 10) {
        end_of_iter = num_tweeters;
    }

    for (int j = 0; j < end_of_iter; j++) {
        int max_tweets = -1;
        int index = -1;
        for (int i = 0; i < num_tweeters; i++) {
            if (TweeterCountPtr[i].numTweets > max_tweets) {
                max_tweets = TweeterCountPtr[i].numTweets;
                index = i;
            }
        }
        printf("%s: %d\n", TweeterCountPtr[index].tweeterName, TweeterCountPtr[index].numTweets);
        TweeterCountPtr[index].numTweets = -1;
    }
}


int getLineCount(char* filename){
    FILE* stream2 = fopen(filename,"r");
    int num_lines = 0;
    char line[1024];
    while(fgets(line,1024,stream2)) {
        num_lines++; //even counts the header line
    }

    return num_lines - 1;
}


int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Invalid number of arguments\n");
        exit(0);
    }

    if (access(argv[1], F_OK) == -1) {
        printf("File provided does not exist.\n");
        exit(0);
    }

    FILE* stream = fopen(argv[1], "r");
    int num_lines = getLineCount(argv[1]);
    char line[1024];
    float lines = 0;
    int name_column = -1;
    int num_cols_in_header = -1;
    int num_cols_in_file = -1;
    struct TweeterCount TweeterCountPtr[num_lines];
    int num_tweeters = 0;

    if (num_lines < 10) {  //10 since header is excluded from counting
        printf("Not enough tweets to rank top 10.\n");
        exit(0);
    }

    while (fgets(line, 1024, stream)) {   
        lines++;
        if (lines == 1) {
            char* first_line = strdup(line);
            name_column = getFieldColumn(first_line, "name");
            if (name_column == -1) {
                printf("Invalid csv file.\n");
                exit(0);
            }
            num_cols_in_header = getNumCols(line);
        } else {
            num_cols_in_file = getNumCols(line);
            if (num_cols_in_file != num_cols_in_header) {
                printf("Invalid number of commas in file.\n");
                exit(0);
            }
            char* tweeterName = getField(line, name_column);

            if (tweeterName == NULL) {
                continue;
            }

            int searchAndUpdateResult = searchAndUpdateTweeter(TweeterCountPtr, num_tweeters, tweeterName);
            if (searchAndUpdateResult == -1) {
                num_tweeters++;
                addTweeter(TweeterCountPtr, num_tweeters, tweeterName);
            }
        }
    }

    printTopTenTweeters(TweeterCountPtr, num_tweeters);

}
