#include "mapreduce.h"

/*
GROUP #: 45
Sean Mark (markx086)
Daniel Clark-Orive (clar2161)
Sangwoo Park (park2335)
*/


int main(int argc, char *argv[]) {

	//TODO: number of argument check

	int nMappers 	= strtol(argv[1], NULL, 10);
	int nReducers 	= strtol(argv[2], NULL, 10);
    inputFileDir = argv[3];

    if(!isValidDir(inputFileDir)) {
        exit(EXIT_FAILURE);
    }

	bookeepingCode();

	// TODO: spawn mappers
	pid_t map_id;

	for(int i = 1; i <= nMappers; i++){
        map_id = fork();
        int length = snprintf(NULL, 0, "%d", i); //gets length of int
        char *mID = malloc(length); 
        sprintf(mID, "%d", i);
        if(map_id == 0){ // child process
            char *const arguments[] = {"mapper", mID, argv[1], inputFileDir, NULL};
            //printf("%s\n", arguments);
            execv("mapper", arguments); // run mapper
            printf("exec not called correctly. Exiting. \n");
            exit(1);
	    }
        free(mID);
	}

	// TODO: wait for all children to complete execution
	pid_t terminated_map;
    while((terminated_map = wait(NULL)) > 0) {
        printf("Terminated a child process (pid: %d)\n", terminated_map);
    }

	// TODO: spawn reducers
    if(!isValidDir(intermediateDir)) {
        exit(EXIT_FAILURE);
    }
    
	pid_t reduce_id;
    for(int i = 1; i <= nReducers; i++){
        reduce_id = fork();
        int length = snprintf(NULL, 0, "%d", i); //gets length of int
        char *rID = malloc(length); 
        sprintf(rID, "%d", i);
        if(reduce_id == 0){ // child process
            char *const arguments[] = {"reducer", rID, argv[2], intermediateDir, NULL};
            execv("reducer", arguments); // run mapper
            printf("exec not called correctly. Exiting. \n");
            exit(1);
	    }
	}

	// TODO: wait for all children to complete execution
    pid_t terminated_reduce;
    while((terminated_reduce = wait(NULL)) > 0) {
        printf("Terminated a child process (pid: %d)\n", terminated_reduce);
    }
	return EXIT_SUCCESS;
}
