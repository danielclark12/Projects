#include "mapper.h"

/**
 * Write word count that is stored in a intermeidate data structure to files.
 * The file should be in a corresponding folder in output/IntermediateData/
 */
void writeInterDSToFiles(void) {

    // convert the current mapperID to string
    int length = snprintf(NULL, 0, "%d", mapperID); //gets length of int
    char *string_from_MapID = malloc(length); 
    sprintf(string_from_MapID, "%d", mapperID);

    // concatenate strings for correct m_mapperID file name
    char* file_name = malloc((sizeof(char) * 5) + strlen(string_from_MapID)); // up to 8 chars in the file name
    sprintf(file_name, "m");
    strcat(file_name,string_from_MapID);
    strcat(file_name,".txt"); // file now has correct name EX: m_1.txt

    // loop for each index in our array of lengths
    for(int i = 1; i < 21; i++){
       
        // convert current index integer to string
        length = snprintf(NULL, 0, "%d", i);
        char *index_from_int = malloc(length);
        sprintf(index_from_int, "/%d/", i); // convert to string in decimal mode, include /

        char *directory = malloc(strlen(intermediateDir) + strlen(index_from_int) + strlen(file_name));
        sprintf(directory, "%s", intermediateDir); //append intermediateDir
        strcat(directory, index_from_int); //append /INDEX/
        strcat(directory, file_name); //append text file

        length = i;
        int num_words = lenArr[i];
        
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
    free(file_name);

}
/**
 * Read lines from files, and count words by word length
 */
void map(char * inputFileName) {


    FILE *fp = getFilePointer(inputFileName); // load file to fp

    char *line = NULL;
    size_t len = 0;

    if(fp == NULL){ // Error checking
        printf("Error: NULL file.");
    }

    char c;
    int count = 0;
    while((c = fgetc(fp)) != EOF){ // get each char in line until EOF
        //ssize_t line = getLineFromFile(&fp,&line,len); // get the current line
        char space = ' ';
        char newLine = '\n';
        if(c == space || c == newLine ){ // check if current char is a ' ' or newline
            lenArr[count] = lenArr[count] + 1; // array index is key, value is val
            count = 0;
        }
        else{
            count++;
        }
    }

    if(count != 0) {
        lenArr[count] = lenArr[count] + 1; // array index is key, value is val
        count = 0;
    }

    fclose(fp);
}

int main(int argc, char *argv[]) { // inputs: mapperID, nMapper, InputFileDir

    mapperID = strtol(argv[1], NULL, 10);
    int nMappers = strtol(argv[2], NULL, 10);
    inputFileDir = argv[3];

    //getMapperTasks function returns a list of input file names that this mapper should process
    char *myTasks[MaxNumInputFiles] = {NULL};
    int nTasks = getMapperTasks(nMappers, mapperID, inputFileDir, &myTasks[0]);

    int tIdx;
    for (tIdx = 0; tIdx < nTasks; tIdx++) {
        printf("mapper[%d] - %s\n", mapperID, myTasks[tIdx]);
        map(myTasks[tIdx]);
        free(myTasks[tIdx]);
    }

    writeInterDSToFiles();

    return EXIT_SUCCESS;
}
