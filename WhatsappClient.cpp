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
    string helloMsg = "Hello";
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
    int r = send(sock, helloMsg.c_str(), helloMsg.size(),0);


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
            else if(send(sock, line.c_str(), line.size(), 0) != line.size()){
                handleSysErr("send",errno);
            }
        }
        if(FD_ISSET(sock, &active_fd_set)){
            if ((readVal = read(sock, message, 1024)) == 0) {
                // client disconnected.
                if(close(sock) < 0){
                    handleSysErr("close", errno);
                }
            } else if(readVal > 0){
                // got new msg from sd.
                message[readVal] = '\0';
                std::cout << message << std::endl;
                std::flush(std::cout);
            } else{
                handleSysErr("read", errno);
            }
        }
    }

    close(sock);
    return 0;
}



void handleSysErr(string errCall, int errNu){
    std::cout<< "ERROR: "<<errCall<<" "<< errNu <<"."<<std::endl;
    exit(1);
}