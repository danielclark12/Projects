#include "reducer.h"

/**
 * Write final word count to files.
 * The file should be in a corresponding folder in output/FinalData/ 
 */
void writeFinalDSToFiles(void) {

    // loop for each index in our array of lengths
    for(int i = 1; i < 21; i++){
       
        // convert current index integer to string
        int length = snprintf(NULL, 0, "%d", i);
        char *wordLengthStr = malloc(length + (sizeof(char) * 5));
        sprintf(wordLengthStr, "/%d", i); // convert to string in decimal mode, include /
        strcat(wordLengthStr,".txt");

        char *directory = malloc(strlen(finalDir) + strlen(wordLengthStr));
        sprintf(directory, "%s", finalDir); //append intermediateDir
        strcat(directory, wordLengthStr); //append /INDEX/

        length = i;
        int num_words = totalArr[i];
        
        if(num_words != 0){ // array has a value at this spot
            
            // convert data we are writing to string:
            int size = snprintf(NULL, 0, "%d %d", length, num_words);
            char *numberWords = malloc(sizeof(size));
            sprintf(numberWords, "%d %d", length, num_words);

            // write to the file: writeLineToFile(char *filepath, char *line);
            writeLineToFile(directory,numberWords);
        }

        // free all of our strings for next iteration
        free(directory);
        //free(tempDirectory);
    
    } // end for loop

}


/**
 * Read lines from files, and calculate a total count for a specific word length
 */
void reduce(char * intermediateFileName) {

    FILE *fp = getFilePointer(intermediateFileName); // load file to fp

    if(fp == NULL){ // Error checking
        printf("Error: NULL file.");
    }

    int wordLen;
    int wordCount;

    fscanf(fp, "%d %d", &wordLen, &wordCount);

    totalArr[wordLen] = totalArr[wordLen] + wordCount;

    fclose(fp);

}

int main(int argc, char *argv[]) {

	// initialize 
	reducerID = strtol(argv[1], NULL, 10);
	int nReducers = strtol(argv[2], NULL, 10);

    //getReducerTasks function returns a list of intermediate file names that this reducer should process
    char *myTasks[MaxNumIntermediateFiles] = {NULL};
    int nTasks = getReducerTasks(nReducers, reducerID, intermediateDir, &myTasks[0]);

    int tIdx;
    for (tIdx = 0; tIdx < nTasks; tIdx++) {
        printf("reducer[%d] - %s\n", reducerID, myTasks[tIdx]);
        reduce(myTasks[tIdx]);
        free(myTasks[tIdx]);
    }

    writeFinalDSToFiles();

	return EXIT_SUCCESS;
}