#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "common.h"

#define B_SIZE 1024 * 5000

int main(int argc, char** argv) {
    int checkVictory(int i, int j, const char *grid, int playing, int gridSize);
    void getMYM(struct sockaddr_in * clientaddress, int socket, int *p);

    int terminate = 0;  // run until told otherwise
    // Arguments parsing
    if (argc<2) {
        printf("Syntax: ./server <port>\n");
        exit(1);
    }

	int tmp;
    int port = atoi(argv[1]);
    struct sockaddr_in server;
    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;

    // Socket creation
    int mysocket = socket(AF_INET, SOCK_DGRAM, 0); 
	if (mysocket < 0) { printf("Prob with server socket \r\n"); return 1; }

	tmp = bind(mysocket, (struct sockaddr *)&server, sizeof(server));
	if (tmp < 0) { printf("Bind error\n"); return 1; }

	char *recv_buf_t;
	recv_buf_t = malloc(sizeof(char) * B_SIZE);
	bzero(recv_buf_t, sizeof(char) * B_SIZE);

    // accept P1
	struct sockaddr_in clientaddress1;
	recvfrom(mysocket, recv_buf_t, sizeof(recv_buf_t), 0, (struct sockaddr *) &clientaddress1, (socklen_t *)sizeof(struct sockaddr_in));

    // Greetings message
    char *msg = "Hello and welcome. We are waiting for a second player. \r\n";
	send_msg(TXT, mysocket, strlen(msg), msg, &clientaddress1, sizeof(clientaddress1));
	printf("sent\n");

    // accept P2
	struct sockaddr_in clientaddress2;
	recvfrom(mysocket, recv_buf_t, sizeof(recv_buf_t), 0, (struct sockaddr *) &clientaddress2, (socklen_t *)sizeof(clientaddress2));

    msg = "You are O\n";
	send_msg(TXT, mysocket, strlen(msg), msg, &clientaddress1, sizeof(clientaddress1));
    msg = "You are X\n";
	send_msg(TXT, mysocket, strlen(msg), msg, &clientaddress2, sizeof(clientaddress2));

	int gridSize = 3;
	char * grid;
	grid = malloc(sizeof(char)*gridSize*gridSize);
	int i, j;
    int moves = 0;
    int victory = 0;
    int playing = 2;

    for (i=0; i<gridSize; i++){
        for (j=0; j<gridSize; j++){
            strncpy(grid+i*gridSize+j, ".", 1);
        }
    }

    // Loop 
    while (! terminate) {
        moves++;

        if (playing == 1){
            playing = 2;
        }
        else{
            playing = 1;
        } 

        int *p = NULL;
        if (playing == 1){
			send_msg(FYI, mysocket, strlen(grid), grid, &clientaddress1, sizeof(clientaddress2));
            getMYM(&clientaddress1, mysocket, p);
        }
		else{
			send_msg(FYI, mysocket, strlen(grid), grid, &clientaddress2, sizeof(clientaddress2));
            getMYM(&clientaddress2, mysocket, p);
        }            
        i = *(p);
        j = *(p+1);

        while (!strncmp(grid+i*gridSize+j, ".", 1)){
            if (playing == 1){
                getMYM(&clientaddress1, mysocket, p);
            }
            else{
                getMYM(&clientaddress2, mysocket, p);
            }            
            i = *(p);
            j = *(p+1);
        }
        
        if (playing == 1){
            strncpy(grid+i*gridSize+j, "O", 1);
        }
        else{
            strncpy(grid+i*gridSize+j, "X", 1);
        }  
        
        victory = checkVictory(i, j, grid, playing, gridSize);

        terminate = (victory) || (moves > (gridSize*gridSize));
    }
	char temp = playing;
	send_msg(END, mysocket, 5, &temp, &clientaddress1, sizeof(clientaddress1));
	send_msg(END, mysocket, 5, &temp, &clientaddress1, sizeof(clientaddress1));

    return 0;
}


int checkVictory(int i, int j, const char *grid, int playing, int gridSize){
    const char *objective;
    if (playing == 1){
        objective = "O";
    }
    else{
        objective = "X";
    }

    if (strncmp(grid+i*gridSize+(j)%3, objective, 1) && strncmp(grid+i*gridSize+(j+1)%3, objective, 1) && strncmp(grid+i*gridSize+(j+2)%3, objective, 1)){
        return playing;
    }
    if (strncmp(grid+((i)%3)*gridSize+j, objective, 1) && strncmp(grid+((i+1)%3)*gridSize+j, objective, 1) && strncmp(grid+((i+2)%3)*gridSize+j, objective, 1)){
        return playing;
    }
    if (i==j){
        if (strncmp(grid, objective, 1) && strncmp(grid+gridSize+1, objective, 1) && strncmp(grid+2*gridSize+2, objective, 1)){
            return playing;
        }
    }
    if(i == gridSize-j-1){
        if (strncmp(grid+2, objective, 1) && strncmp(grid+gridSize+1, objective, 1) && strncmp(grid+2*gridSize, objective, 1)){
            return playing;
        }
    }
    return 0;
}


void getMYM(struct sockaddr_in * clientaddress, int socket, int *p){
	send_msg(MYM, socket, 5*sizeof(char), " ", clientaddress, sizeof(clientaddress));

    char *recv_buf_t;
    recv_buf_t = malloc(sizeof(char)*10);
    bzero(recv_buf_t, sizeof(char)*10);
    
    recvfrom(socket, recv_buf_t, sizeof(recv_buf_t), 0, (struct sockaddr *) &clientaddress, (socklen_t *)sizeof(clientaddress));

    int i,j;
    char *tmpC = " ";
    strncpy(tmpC, recv_buf_t, 1);
    i = atoi(tmpC);
    recv_buf_t += 2;
    strncpy(tmpC, recv_buf_t, 1);
    j = atoi(tmpC);
    p[0] = i;
	p[1] = j;
}

