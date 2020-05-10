# UDP server

The goal of this project was to code the client and the server of an online game of TicTacToe in C using UPD packets. 

### Compilation

`cc -g  -Wall -o common.o -c common.c`

To compile the client:

`cc -g  -Wall -o client.o -c client.c; cc -g -o client common.o client.o`

To compile the server:

`cc -g  -Wall -o server.o -c server.c; cc -g -o server common.o server.o`
