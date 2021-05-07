#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <zconf.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include "../include/protocol.h"

struct threadArg {
    int clientFd;
    char *clientIP;
    int clientPort;
};

//initialize sem
sem_t sem_mutex;
sem_t sem_cond;


// use synchronization method for updating these
int histogram[21]; // max word length is 20
int client_status[21]; // max num of clients is 20...20 or 50?

void *read_data(void *args){
    struct threadArg *arg = args;
    
    int n = read(arg->clientFd, request, sizeof(request));
    if(n < 0){
        printf("close connection from %s:%d\n", arg->clientIP, arg->clientPort);
        close(arg->clientFd);
    }
    //wait on mutex
    sem_wait(&sem_mutex);
    
    if (request[RQS_RQS_CODE_NUM] > 4 || request[RQS_RQS_CODE_NUM] < 1 ||  request[RQS_CLIENT_ID] < 1){
        printf("REQUEST OR CLIENT ID ERROR\n");
        int response[3]; //response structure
        for(int i=RSP_RQS_CODE_NUM; i < 3; i++) {
            response[i] = 0;
        }
        write(arg->clientFd, response, sizeof(response));
    } else if(request[RQS_RQS_CODE_NUM] == UPDATE_WSTAT) {

        for(int i=RQS_DATA; i < 22; i++) {
            hist[i-RQS_DATA] = hist[i-RQS_DATA] + request[i];
        }
        clientUpdates[request[RQS_CLIENT_ID]-1]++;
        printf("[%d] UPDATE_WSTAT\n", request[RQS_CLIENT_ID]);
        int response[3];
        response[RSP_RQS_CODE_NUM] = request[RQS_RQS_CODE_NUM];
        response[RSP_RSP_CODE_NUM] = 1; //successful request
        response[RSP_DATA] = request[RQS_CLIENT_ID];
        write(arg->clientFd, response, sizeof(response));

    } else if(request[RQS_RQS_CODE_NUM] == GET_MY_UPDATES) {

        printf("[%d] GET_MY_UPDATES\n", request[RQS_CLIENT_ID]);

        int response[3];
        response[RSP_RQS_CODE_NUM] = request[RQS_RQS_CODE_NUM];
        response[RSP_RSP_CODE_NUM] = 1; //successful request
        response[RSP_DATA] = clientUpdates[request[RQS_CLIENT_ID]-1];
        
        write(arg->clientFd, response, sizeof(response));

    } else if(request[RQS_RQS_CODE_NUM] == GET_ALL_UPDATES) {

        printf("[%d] GET_ALL_UPDATES\n", request[RQS_CLIENT_ID]);

        int response[3];
        response[RSP_RQS_CODE_NUM] = request[RQS_RQS_CODE_NUM];
        response[RSP_RSP_CODE_NUM] = 1; //successful request
        response[RSP_DATA] = 0;
        for(int i=0; i<20; i++) {
            response[RSP_DATA] += clientUpdates[i];
        }

        write(arg->clientFd, response, sizeof(response));

    } else if(request[RQS_RQS_CODE_NUM] == GET_WSTAT) {

        printf("[%d] GET_WSTAT\n", request[RQS_CLIENT_ID]);

        int response[22]; //response structure
        response[RSP_RQS_CODE_NUM] = request[RQS_RQS_CODE_NUM];
        response[RSP_RSP_CODE_NUM] = 1; //successful request
        for (int i = RSP_DATA; i < 22; i++) {
            response[i] = hist[i-2]; 
        }
        write(arg->clientFd, response, sizeof(response));

    } else {
        printf("UNKNOWN ERROR\n");
        int response[3]; //response structure
        for(int i=RSP_RQS_CODE_NUM; i < 3; i++) {
            response[i] = 0;
        }
        write(arg->clientFd, response, sizeof(response));
    }
    //unlock mutex
    sem_post(&sem_mutex);
    sem_post(&sem_cond);
    
    close(arg->clientFd);
    free(arg);
    return NULL;
}

int main(int argc, char *argv[]) {

    // process input arguments
    unsigned int server_port = strtol(argv[1],NULL,10);

    // socket create and verification
    int sockfd, connfd = 0;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1){
        printf("socket creation failed\n");
        exit(EXIT_FAILURE);
    }

    //initialize num of client updates
    for(int i=0; i < 20; i++) {
        clientUpdates[i] = 0;
    }

    //initialize thread array
    pthread_t threads[MAX_CONCURRENT_CLIENTS];

    sem_init(&sem_mutex, 0, 1);
    sem_init(&sem_cond, 0, 0);

    // assign IP, PORT
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // double check
    servaddr.sin_port = htons(server_port);

    // Binding newly created socket to given IP
     if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) {
        printf("socket bind failed\n");
        exit(EXIT_FAILURE);
    }

    // Now server is ready to listen
    if(listen(sockfd,connfd) != 0){
        printf("Listen failed!\n");
        exit(EXIT_FAILURE);
    }
    else{
        printf("server is listening\n");
    }

    socklen_t length = 0;
    while (1) {
        // Accept the data packet from client
        struct sockaddr_in clientAddress;
        length = sizeof(clientAddress);

        // accept the data from client
        printf("accepting...\n");
        connfd = accept(sockfd, (struct sockaddr*) &clientAddress, &length);
        if(connfd < 0){
            printf("server accept failed!\n");
            exit(EXIT_FAILURE);
        }
        
        struct threadArg *arg = (struct threadArg *) malloc(sizeof(struct threadArg));
        arg->clientFd = connfd;
        arg->clientIP = inet_ntoa(clientAddress.sin_addr);
        arg->clientPort = clientAddress.sin_port;
        printf("open connection from %s:%d\n", arg->clientIP, arg->clientPort);
        
        int count = 0;
        int thread;
        thread = pthread_create(&threads[count], NULL, read_data, (void *) arg);
        if (thread != 0) {
            fprintf(stderr, "SERVER: thread failed to be created\n");
            exit(EXIT_FAILURE);
        }
        thread = pthread_detach(threads[count]);
        if (thread != 0) {
            fprintf(stderr, "SERVER: thread detach failed\n");
            exit(EXIT_FAILURE);
        }
        count++;
        if (count == MAX_CONCURRENT_CLIENTS) {
            count = 0;
        }

        

    }
    exit(EXIT_SUCCESS);
}
