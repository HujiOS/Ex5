#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include <malloc.h>
#include <string>
#include <vector>
#include <map>
#include "Constants.h"

#define ERR -1
#include <bfd.h>
#include <iostream>

#define STDIN 0

using namespace std;
static vector<string> rmsgs;
static map<int,vector<string>> msgs;
static map<int,string> socket_to_nic;
static map<string,int> nic_to_socket;
static map<string, vector<string>> groups;

static vector<int> clientSocks;
static vector<int> deletedSockes;
static map<int, string> clientNickNames;
//the thread function

void sendMsg(int socketId, string msg){
   return;
}
void handleSysErr(string errCall, int errNu){
    std::cout<< "ERROR: "<<errCall<<" "<< errNu <<"."<<std::endl;
    exit(1);
}

int main(int argc, char *argv[]) {
    int socket_desc, new_sock, sd, valread;
    char buffer[1025];
    fd_set active_fd_set;
    FD_ZERO(&active_fd_set);
    struct sockaddr_in address;

int incoming_message(int sid, string s)
{
    if(!is_msg_legal(s)) return ERR;
    vector<string> tokens = parse_delim(s, ' ');

    switch(msgs_to_enum[tokens[0]])
    {
        case CREATE_GROUP:

        case SEND:

        case WHO:

        case EXIT:

        default:
            break;
    }
}


int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;

    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8888);
    int yes = 1;
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("setsockopt");
    }

    //Bind
    if (bind(socket_desc, (struct sockaddr *) &address, sizeof(address)) < 0) {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc, 5);

    //Accept and incoming connection
    int k = sizeof(address);
    int retval;
    while (1 == yes) {
        int maxSocket = socket_desc;
        FD_ZERO(&active_fd_set);
        FD_SET(socket_desc, &active_fd_set);
        FD_SET(STDIN, &active_fd_set);
        // defining the io-sockets
        for (int i = 0; i < clientSocks.size(); ++i) {
            FD_SET(clientSocks[i], &active_fd_set);
            if (clientSocks[i] > maxSocket) maxSocket = clientSocks[i];
        }
        retval = select(maxSocket + 1, &active_fd_set, NULL, NULL, NULL);
        if ((retval < 0) && (errno != EINTR)) {
            printf("Select Error\n");
        }
        if (FD_ISSET(socket_desc, &active_fd_set)) {
            if ((new_sock = accept(socket_desc, (struct sockaddr *) &address, (socklen_t *) &k)) < 0) {
                perror("Error open new socket");
            }
            if (send(new_sock, "ok", 2, 0) != 2) {
                perror("Wrong length");
            }
            printf("New client connected \n");
            clientSocks.push_back(new_sock);
        }
        if (FD_ISSET(STDIN, &active_fd_set)){
            std::string line;
            std::getline( std::cin, line );
            cout << line;
        }

        // check sockets for new input
        for (int i = 0; i < clientSocks.size(); ++i) {
            sd = clientSocks[i];
            if (FD_ISSET(sd, &active_fd_set)) {
                if ((valread = read(sd, buffer, 1024)) == 0) {
                    // client disconnected.
                    close(sd);
                    deletedSockes.push_back(i);
                } else if(valread > 0){
                    // got new msg from sd.
                    buffer[valread] = '\0';
                    std::cout << buffer << " Received from socket number #" << sd << std::endl;
                    std::flush(std::cout);
                    send(sd, buffer, strlen(buffer), 0);
                } else {
                    handleSysErr("read", errno);
                }

            }
        }
        // erase deleted sockets
        while (deletedSockes.size() != 0) {
            clientSocks.erase(clientSocks.begin() + deletedSockes.back());
            deletedSockes.pop_back();
        }
    }

    //Free the socket pointer
    return 0;
}

