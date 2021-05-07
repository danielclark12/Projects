#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <zconf.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>
#include "../include/protocol.h"
#include <errno.h>

FILE *logfp;

int fileCount = 0;
char *fileNameArray[100];

int lenArr[20];

void createLogFile(void) {
    pid_t p = fork();
    if (p == 0)
        execl("/bin/rm", "rm", "-rf", "log", NULL);

    wait(NULL);
    mkdir("log", ACCESSPERMS);
    logfp = fopen("log/log_client.txt", "w");
}

void traverse(char * path) {

	struct dirent *reaDR;
	char *newPath;

	DIR *dr = opendir(path);

	if (dr == NULL) { //check if it could not be opened, return
		printf("could not open %s\n", path);
		return;
	}
	
	while ((reaDR = readdir(dr)) != NULL) {
		
		//creat newPath name, may be directory or file
		newPath = malloc(strlen(path) + strlen(reaDR->d_name) + 3); //plus 2 for the null chars
		sprintf(newPath, "%s", "\0");
		strcat(newPath, path);
		strcat(newPath, "/");
		strcat(newPath, reaDR->d_name);
		strcat(newPath, "\0");

		if (strcmp(reaDR->d_name, ".") == 0 || strcmp(reaDR->d_name, "..") == 0) {
			//skip if . or .. dir
			continue;
		} else if (reaDR->d_type == DT_DIR) {
			//check if it's a directory, recusive call
			traverse(newPath);
		} else {
			fileCount++;
			fileNameArray[fileCount-1] = newPath;
		}
	} //while

	if (fileCount < 100) { //checks if max files have been given
		fileNameArray[fileCount] = '\0';
	} else if (fileCount == 0) {
		printf("no files given. exiting.\n");
		return;
	}

	closedir(dr);
}

void parse(char * inputFileName) {

    FILE *fp = fopen(inputFileName, "r");; // load file to fp
    
    if(fp == NULL){ // Error checking
        printf("Error: NULL file.\n");
        return;
    }

    char c;
    int count = 0;
    
    while((c = fgetc(fp)) != EOF){ // get each char in line until EOF
        char space = ' ';
        char newLine = '\n';
        if(c == space || c == newLine ){ // check if current char is a ' ' or newline
            lenArr[count-1] = lenArr[count-1] + 1; // array index is key, value is val
            count = 0;
        }
        else{
            count++;
        }
    }

    if(count != 0) {
        lenArr[count-1] = lenArr[count-1] + 1; // array index is key, value is val
        count = 0;
    }

    fclose(fp);
}

void sendRequest(int sockfd, int reqID, int clientID) {
    request[RQS_RQS_CODE_NUM] = reqID;
    request[RQS_CLIENT_ID] = clientID;
    for(int j=RQS_DATA; j<22; j++) {
        request[j] = 0;
    }
    
    write(sockfd, request, sizeof(request));
}

int main(int argc, char *argv[]) {

    // process input arguments
    if (argc < 5) {
        printf("USAGE: ./client <Folder Name> <# of Clients> <Server IP> <Server Port>\n");
        exit(1);
    }
    int size = strlen(argv[1]);
    char dir[size];
    strcpy(dir, argv[1]);
    
    int nClients = strtol(argv[2],NULL,10);

    size = strlen(argv[3]);
    char server_ip[size];
    strcpy(server_ip, argv[3]);

    unsigned int server_port = strtol(argv[4], NULL, 10);

    // create log file
    createLogFile();

    traverse(dir);

    // spawn client processes
    

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(server_port);
    address.sin_addr.s_addr = inet_addr(server_ip);

    pid_t pid;

    for(int i = 1; i <= nClients; i++) {
        pid = fork();
        if(pid == 0) {
            //child
            
            //set to 0
            for(int i = 0; i < 20; i++) {
                lenArr[i] = 0;
            }
            int sockfd = socket(AF_INET , SOCK_STREAM , 0);
            if (connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0) {
                fprintf(logfp, "[%d] open connection\n", i);
                if (i <= fileCount) {
                    //there is a file for the client to parse
                    parse(fileNameArray[i-1]);
                
                    request[RQS_RQS_CODE_NUM] = UPDATE_WSTAT;
                    request[RQS_CLIENT_ID] = i; //only 1 client, default to client ID 1
                    int l = 0;
                    for (int j = RQS_DATA; j < 22; j++) {
                        request[j] = *(lenArr+l);
                        l++;
                    } //since GET_WSTAT doesn't need data, just fill with 0's

                    write(sockfd, request, sizeof(request));
                    read(sockfd, response, sizeof(response));
                    fprintf(logfp, "[%d] UPDATE_WSTAT: %d\n", i, 1);
                    
                    //close connection
                    close(sockfd);
                    fprintf(logfp, "[%d] close connection (successful execution)\n", i);
                }
            } else {
                perror("Connection failed!\n");
            }
            sockfd = socket(AF_INET , SOCK_STREAM , 0);
            if (connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0) {
                //GET MY UPDATES
                fprintf(logfp, "[%d] open connection\n", i);
                sendRequest(sockfd, GET_MY_UPDATES, i);
                write(sockfd, request, sizeof(request));
                read(sockfd, response, sizeof(response));
                fprintf(logfp, "[%d] GET_MY_UPDATES: %d %d\n", i, response[RSP_RSP_CODE_NUM], response[RSP_DATA]);

                //close connection
                close(sockfd);
                fprintf(logfp, "[%d] close connection (successful execution)\n", i);
            } else {
                perror("Connection failed!\n");
            }
            sockfd = socket(AF_INET , SOCK_STREAM , 0);
            if (connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0) {
                //GET ALL UPDATES
                fprintf(logfp, "[%d] open connection\n", i);
                sendRequest(sockfd, GET_ALL_UPDATES, i);
                write(sockfd, request, sizeof(request));
                read(sockfd, response, sizeof(response));
                fprintf(logfp, "[%d] GET_ALL_UPDATES: %d %d\n", i, response[RSP_RSP_CODE_NUM], response[RSP_DATA]);
                
                //close connection
                close(sockfd);
                fprintf(logfp, "[%d] close connection (successful execution)\n", i);
            } else {
                perror("Connection failed!\n");
            }
            sockfd = socket(AF_INET , SOCK_STREAM , 0);
            if (connect(sockfd, (struct sockaddr *) &address, sizeof(address)) == 0) {
                //GET HISTOGRAM
                sendRequest(sockfd, GET_WSTAT, i);
                read(sockfd, response, sizeof(response));
                fprintf(logfp, "[%d] GET_WSTAT: %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                                  i, response[RSP_RSP_CODE_NUM], response[2], response[3], response[4], response[5], response[6],
                                     response[7], response[8], response[9], response[10], response[11], response[12], response[13],
                                     response[14], response[15], response[16], response[17], response[18], response[19], response[20],
                                     response[21]);
                
                //close connection
                close(sockfd);
                fprintf(logfp, "[%d] close connection (successful execution)\n", i);
            } else {
                perror("Connection failed!\n");
            }
            break;
        } else if(pid == -1) {
            //fork() failed
            fprintf(stderr, "ERROR[%d]: %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
            
        } else {
            //parent
        }
    }//for()

    // wait for all client processes to terminate
    pid_t terminated_pid;
    while((terminated_pid = wait(NULL)) > 0) {
        printf("Terminated a child process (pid: %d)\n", terminated_pid);
    }

    // close log file
    fclose(logfp);

    return EXIT_SUCCESS;

}
