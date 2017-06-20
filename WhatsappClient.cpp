#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include <zconf.h>
#include <sstream>
#include <iostream>
#include "Constants.h"


#include <string>
#include <stdlib.h>

#define STDIN 0
using namespace std;
void handleSysErr(string,int);
int main(int argc , char *argv[])
{
    int sock, readVal;
    struct sockaddr_in server;
    char message[1024];
    int y = 1;
    fd_set active_fd_set;

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
    sendMessage(sock, HELLO_MSG+" myName");


    puts("Connected\n");

    //keep communicating with server
     // stdin
    while(1 == y)
    {
        FD_ZERO(&active_fd_set);
        FD_SET(sock, &active_fd_set);
        FD_SET(STDIN, &active_fd_set);
        string line;
        int ret = select(sock+1,&active_fd_set,NULL,NULL,NULL);
        if((ret<0) && (errno != EINTR)){
            handleSysErr("select",errno);
        }
        if(FD_ISSET(STDIN, &active_fd_set)){
            getline(std::cin, line);
            if(!is_msg_legal(line)){
                cout << "Message illegal!"<< endl;
            }
            else{
                sendMessage(sock, line);
            }
        }
        if(FD_ISSET(sock, &active_fd_set)){
            string msg = readMessage(sock);
            if(msg == ERR_MSG){
                handleSysErr("read", errno);
            }
            cout << "client received new message! " << msg << endl;
        }
    }

    close(sock);
    return 0;
}