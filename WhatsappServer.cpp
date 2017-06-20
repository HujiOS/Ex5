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
//#include "Constants.h"

#define ERR -1

#include <bfd.h>
#include <iostream>
#include "Constants.h"

#define STDIN 0

using namespace std;
static vector<string> rmsgs;
static map<int, vector<string>> msgs;
static map<int, string> socket_to_nic;
static map<string, int> nic_to_socket;
static map<string, vector<string>> groups;

static vector<int> clientSocks;
static vector<int> deletedSockes;
static map<int, string> clientNickNames;
//the thread function

void sendMsg(int socketId, string msg) {
    return;
}


int main(int argc, char *argv[]) {
    int socket_desc, new_sock, sd, valread;
    struct sockaddr_in address;
    string msg;
    fd_set active_fd_set;

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
                handleSysErr("accept", errno);
            }
            clientSocks.push_back(new_sock);
        }
        if (FD_ISSET(STDIN, &active_fd_set)) {
            std::string line;
            std::getline(std::cin, line);
            cout << line << endl;
        }

        // check sockets for new input
        for (int i = 0; i < clientSocks.size(); ++i) {
            sd = clientSocks[i];
            if (FD_ISSET(sd, &active_fd_set)) {
                msg = readMessage(sd);
                if (msg == LEAVE_MESSAGE) {
                    // TODO Handle Exit
                    deletedSockes.push_back(i);
                    close(sd);
                    continue;
                } else if(msg == ERR_MSG){
                    handleSysErr("read", errno);
                } else {
                    //TODO Handle Message, send it to Gal.
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


