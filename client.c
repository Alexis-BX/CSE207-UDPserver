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
    int terminate = 0;  // run until told otherwise
    // Arguments parsing
    if (argc<3) {
        printf("Syntax: ./client <ipaddr> <port>\n");
        exit(1);
    }
    int port = atoi(argv[2]);
    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(struct sockaddr_in));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);

    // Socket creation
    int mysocket = socket(AF_INET, SOCK_DGRAM, 0); 
	int tmp = inet_pton(AF_INET, argv[1], &dest.sin_addr);
	if(tmp <= 0) {printf("Prob with server address \r\n"); return 1;}

	tmp = connect(mysocket, (struct sockaddr *)&dest, sizeof(dest));
	if (tmp < 0) {printf("Connection failed \r\n"); return 1;}

    // Greetings message
    char *send_buf_t = "Hello \r\n";
	tmp = sendto(mysocket, send_buf_t, sizeof(send_buf_t), 0, (const struct sockaddr *) &dest, sizeof(dest));
	if (tmp < 0) {printf("Send failed... \r\n"); return 1;}
    //free(send_buf_t);

	int gridSize = 3;
	int i, j;

    // Loop 
    while (! terminate) {
		
		// Receive message from server
		char *recv_buf_t;
		recv_buf_t = malloc(sizeof(char) * B_SIZE);
		bzero(recv_buf_t, sizeof(char) * B_SIZE);
		
		// Problem here
		printf("Waiting for server response\n");
		recvfrom(mysocket, recv_buf_t, sizeof(recv_buf_t), 0, (struct sockaddr *) &dest, (socklen_t *)sizeof(struct sockaddr_in));

        if (tmp<0){printf("Recieve failed... \r\n"); return 1;}
		
        // Process message (and maybe interact with user? send message(s) to server?)
        show_bytes(recv_buf_t, sizeof(recv_buf_t));

        char *tmpC = malloc(sizeof(char));
        strncpy(tmpC, recv_buf_t, 1);
        int typeI = atoi(tmpC);
        recv_buf_t += 2;

        switch(typeI){
            case FYI: 
				strncpy(tmpC, recv_buf_t, 1);
				recv_buf_t++;
				gridSize = atoi(tmpC);

				for (i = 0; i < gridSize; i++) {
					for (j = 0; j < gridSize; j++) {
						printf(recv_buf_t + (i*gridSize) + j);
						printf(" ");
					}
					printf("\n");
				}
				break;

            case MYM:
                printf("Please entre column row: \n");
                send_buf_t = malloc(sizeof(char)*5);
				send_buf_t = MOV+"0";
                scanf("%s",send_buf_t+2);

                tmp = sendto(mysocket, send_buf_t, sizeof(send_buf_t), 0, (const struct sockaddr *) &dest, sizeof(dest));
                if (tmp < 0) {printf("Send failed... \r\n"); return 1;}
                break;

            case END:
                strncpy(tmpC, recv_buf_t, 1);
                if(atoi(tmpC)==0){
                    printf("Draw\r\n");
                }
                else if (atoi(tmpC)==1){
					printf("O won!\r\n");
                }
				else {
					printf("X won!\r\n");
				}
				terminate = 1;
				break;

			case TXT:
                printf(recv_buf_t);
                break;
        }
		free(recv_buf_t);
    }
    // Terminate
    return 0;
}