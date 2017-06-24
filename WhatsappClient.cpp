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

static string nickname;
static string ip;
static string port;
void handleSysErr(string,int);
int main(int argc , char *argv[])
{
    int sock, readVal;
    struct sockaddr_in server;
    char message[1024];
    int y = 1;
    fd_set active_fd_set;
    if(argc < 4){
        std::cerr << "Usage: whatsappClient clientName serverAddress serverPort" << endl;
        exit(1);
    }
    nickname = string(argv[1]);
    ip = string(argv[2]);
    port = string(argv[3]);

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        handleSysErr("socket",errno);
    }
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons( stoi(port) );
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        handleSysErr("connect",errno);
        return 1;
    }
    sendMsg(sock, HELLO_MSG + " " + nickname);


    cout << "Connected successfully." << endl;

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
            vector<string> tokens = parse_delim(line, ' ');

            switch(is_msg_legal(line, nickname))
            {
                case BAD_HELLO:
                    cerr << "Illegal username bro"<< endl;
                    break;
                case BAD_COMMAND:
                    cerr << ERROR_MSG <<" Invalid input."<< endl;
                    break;
                case BAD_GROUP:
                    cerr << GROUP_ERR(tokens[1]);
                    break;
                case BAD_EXIT:
                    cerr << "impossible" << endl;
                    break;
                case BAD_SEND:
                    cerr << SEND_ERR_CLIENT;
                    break;
                case BAD_WHO:
                    cerr << WHO_ERR;
                    break;
                case SUCCESS:
                    sendMsg(sock, line);
                    break;
                default:
                    cerr << "Whoops. Got an unknown err message" << endl;
                    break;
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