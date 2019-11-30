#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_CHAR 1024
#define TOPKTWEETS 10
#define HEADER_LINENO 1
//A struct object to record each distinct user within the CSV file
//The object works more like a map with the userName mapped to the corresponding number of tweets
struct TweeterCount {   
   int numTweets;
   char* tweeterName;
};

//A method to extract a particular field content provided the entire field string
//For example: "UserName_ABC" is the input and the output will be UserName_ABC (without the quotes and extra spaces)
char* getContent(const char* token) {

    //if empty token, return NULL
    if (strlen(token) == 0) {
        return NULL;
    }

    static char content[MAX_CHAR];  //We used a static char array as a content string to extract the contents. MAX_CHAR SIZE is 1024 since we assume max_field size can be so
    bool appendToContent = false;   //A boolean to notify when to add contents into the string
    int numQuotes = 0;  //An integer to count the number of quotes (") in the string. To check for open quotes
    int lastQuoteLoc = -1; //An index variable to find where the last Quote (closing quote) lies in
    int iter = 0;   //an index variable for the content

    //A simple loop to run through the token string to find the number of quotes
    for (int i = 0; i < strlen(token); i++) {
        if (token[i] == '"') {
            numQuotes++;
            lastQuoteLoc = i;
        }
    }

    //if the count of number of quotes is 1, then it is incorrect number of quotes
    if (numQuotes == 1) {
        printf("Incorrect number of quotes: %s\n", token);
        return NULL;
    }
    //if the opening has 0 quotes and we have to start adding right from the beginning
    if (numQuotes == 0) {
        appendToContent = true;
    }

    //run through the token again
    for (int i = 0; i < strlen(token); i++) {
        //if this is the last quote location, no need to run through
        if (i == lastQuoteLoc) {
            break;
        }

        //if the append is set to true now, continue to append
        if (appendToContent == true) {
            char token_char = token[i];
            content[iter] = token_char;
            iter++;
        }

        if (token[i] == '"') {
            appendToContent = true;
        }
    }

    content[iter] = '\0'; //null terminate the string
    return content;
}

//run through the particular line and find the col number of that field matching within the line
int getFieldColumn(char* line, char* field) {
    int name_column = 0; //look through that particular field Column
    int num_name_cols = 0;  //checking to see if that field is repeated more than once
    int iter = 0; //run through the count of the number of seperate fields inside the line
    char* token;    //to extract the field string by seperation
    char* content = "";
   
    while ((token = strsep(&line, ","))) {
        iter++; //count the num_fields
        if (token == NULL) {    //if the token is NULL, ignore
            break;
        }

        content = getContent(token); //extract the field content first
        if (content == NULL) {  //if nothing, ignore this field alone
            continue;
        }

        if (strcmp(content, field) == 0) { //if the field is found
            name_column = iter + 1; //find the col number
            num_name_cols++;    //add the number of such columns
            if (num_name_cols != 1) {   //if more than 1 such columns
                return -1; // checks that the field column appears once
            }
        }
    }
    return name_column - 1; //return that particular column
}

//extracts that particular field given the column number (the reverse of the previous function logically)
char* getField(char* line, int num) {
    char* tok;
    int iter = 0;

    while ((tok = strsep(&line, ","))) {
        if (iter++ == num - 1) {    //if found that requested col num, then return that field content
            return getContent(tok);
        }
    }
    return NULL; //else, return null
}

//find the number of columns within the given line
int getNumCols(char* line) {
    int num_cols = 0;
    for (int i = 0; i <  strlen(line); i++) {
        if (line[i] == ',') { //techincally, whenever there is a comma, then it marks a new column
            num_cols++;
        }
    }
    return num_cols;
}

//a function which just adds a tweeter into the arrays of TweeterCount objects. Takes in the tweetername
//and initializes the number of tweets by the tweeterName as 1.
void addTweeter(struct TweeterCount TweeterCountPtr[], int num_tweeters, char* tweeterName) {
    struct TweeterCount newTweeter = {.numTweets = 1};  //Init numTweets by that user as 1 (add new Tweeter)
    newTweeter.tweeterName = (char *) malloc(MAX_CHAR);
    strcpy(newTweeter.tweeterName, tweeterName);    //Set the name of that object
    TweeterCountPtr[num_tweeters - 1] = newTweeter; //Append object into the array
    return;
}

//look into the array of tweeterCount objects and find if there's a tweeter with the given tweeterName
int searchAndUpdateTweeter(struct TweeterCount TweeterCountPtr[], int num_tweeters, char* tweeterName) {
    for (int i = 0; i < num_tweeters; i++) {
        if (strcmp(TweeterCountPtr[i].tweeterName, tweeterName) == 0) { //if found the tweeterName
            TweeterCountPtr[i].numTweets = TweeterCountPtr[i].numTweets + 1;    //increment the number of tweets by 1
            return 0; //found, so return 0
        }
    }
    return -1; //not found, return -1
}

//print the top ten tweeters
//logic explained below
void printTopTenTweeters(struct TweeterCount TweeterCountPtr[], int num_tweeters) {
    int end_of_iter = TOPKTWEETS;
    if (num_tweeters < TOPKTWEETS) {
        end_of_iter = num_tweeters; //limit the max_tweeters to print if the number of tweeters in the file is < 10
    }

    //run thru the "max num of tweeter" times
    for (int j = 0; j < end_of_iter; j++) {
        int max_tweets = -1;    //set the max_tweet count to find highest number of tweeters
        int index = -1; //the index (within the array) of that user within highest tweets
        for (int i = 0; i < num_tweeters; i++) {
            if (TweeterCountPtr[i].numTweets > max_tweets) {
                max_tweets = TweeterCountPtr[i].numTweets; //find the highest tweeter
                index = i;
            }
        }
        printf("%s: %d\n", TweeterCountPtr[index].tweeterName, TweeterCountPtr[index].numTweets); //print result
        TweeterCountPtr[index].numTweets = -1; //set that entry to -1 since we need to find the next highest tweeter
    }
}

//find number of lines in the file
int getLineCount(char* filename){
    FILE* stream2 = fopen(filename,"r");
    int num_lines = 0;
    char line[MAX_CHAR];    //to read the line (just dummy in this function)
    while(fgets(line,MAX_CHAR,stream2)) {   //read in the line in the file
        num_lines++; //even counts the header line
    }

    return num_lines - 1; //exclude the header line
}


int main(int argc, char** argv) {
    if (argc != 2) {    //if there is no argument for the filename to parse, quit the program
        printf("Invalid number of arguments\n");
        exit(0);
    }

    if (access(argv[1], F_OK) == -1) {//If the file doesn't exist
        printf("File provided does not exist.\n");
        exit(0);
    }

    FILE* stream = fopen(argv[1], "r");
    int num_lines = getLineCount(argv[1]); //get the number of lines in the file
    char line[MAX_CHAR];    //to read in every file
    float lines = 0; //used to check the lines count inside the program
    int name_column = -1; //the name column in the file
    int num_cols_in_header = -1; //look through the num columns in the header
    int num_cols_in_file = -1; //look through the num columns in the file
    struct TweeterCount TweeterCountPtr[num_lines]; //array of all tweeter objects
    int num_tweeters = 0; //number of distinct tweeters

    //if the number of lines in the file is less than 10, definetely there are less than 10 distinct tweeters
    if (num_lines < TOPKTWEETS) {  //10 since header is excluded from counting
        printf("Not enough tweets to rank top 10.\n");
        exit(0);
    }

    while (fgets(line, MAX_CHAR, stream)) {   //read every line of the file
        lines++; //count the lines
        if (lines == HEADER_LINENO) { //if the header line is being read in
            char* first_line = strdup(line);    //copy the line first
            name_column = getFieldColumn(first_line, "name"); //find the col number of field name
            if (name_column == -1) { //if not found
                printf("Invalid csv file.\n");
                exit(0);
            }
            num_cols_in_header = getNumCols(line); //get the number of columns in the header
        } else {
            num_cols_in_file = getNumCols(line); //find the number of columns in the file (every line is assumed to have same number of columns)
            if (num_cols_in_file != num_cols_in_header) { //if there's a mismatch in number of cols in the file
                printf("Invalid number of commas in file.\n");
                exit(0);
            }
            char* tweeterName = getField(line, name_column); //extract the tweeter name in every other line

            if (tweeterName == NULL) { //if not found, skip the line
                continue;
            }

            int searchAndUpdateResult = searchAndUpdateTweeter(TweeterCountPtr, num_tweeters, tweeterName); //look into the array first
            if (searchAndUpdateResult == -1) { //if not found in array
                num_tweeters++; //count the increase in number of tweeters
                addTweeter(TweeterCountPtr, num_tweeters, tweeterName);//add the tweeter
            }
        }
    }

    printTopTenTweeters(TweeterCountPtr, num_tweeters); //print top ten tweeters once we finish parsing and collecting within the file

}
