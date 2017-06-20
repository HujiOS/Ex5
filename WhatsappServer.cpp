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

#include <bfd.h>
#include <iostream>

#define ERR_MSG string("ERROR:")
#define STDIN 0
#define MSG_END string(".\n")
#define GROUP_SUCC(group) string("Group ") +string("\"")+ group + string("\"")+string(" was created successfully") + MSG_END
#define GROUP_ERR(group) ERR_MSG + string(" failed to create group ") + string("\"") + group + string("\"") + MSG_END


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

int sendMsg(int socketId, string msg){
   return 8;
}

int create_group(string group_name, vector<string> user_names)
{
    if(groups.find(group_name) != groups.end()) return ERR;
    if(nic_to_socket.find(group_name) != groups.end()) return ERR;

    sort(user_names.begin(), user_names.end());
    user_names.erase(unique(user_names.begin(), user_names.end()), user_names.end());

    if(user_names.size() <= 1) return ERR;

    groups[group_name] = user_names;

    return SUCCESS;
}

int send_to_user(string user, string msg)
{

}

int send_to_target(string name, string msg)
{

}

int parse_incoming(int sid, string s)
{
    cmatch m;
    vector<string> tokens = parse_delim(s, ' ');

    string s1 = tokens[0];
    switch(msgs_to_enum[s1])
    {
        case CREATE_GROUP:
            vector<string> names = parse_delim(tokens[2], ',');
            names.push_back(socket_to_nic[sid]);
            if(create_group(tokens[1], names) != SUCCESS)
            {
                sendMsg(sid, GROUP_ERR(tokens[1]));
                cerr << socket_to_nic[sid] << ":" << GROUP_ERR(tokens[1]);
                return ERR;
            }
            sendMsg(sid, GROUP_SUCC(tokens[1]));
            cout << socket_to_nic[sid] << ":" << GROUP_SUCC(tokens[1]);
            //TODO: anything else?
            return SUCCESS;

        case SEND:
            if(send_to_target(tokens[1], tokens[2]) != SUCCESS) return false;
        case HELLO:


        case WHO:

        case EXIT:
        default:
    }
}


void handleSysErr(string errCall, int errNu){
    std::cout<< "ERROR: "<<errCall<<" "<< errNu <<"."<<std::endl;
    exit(1);
}


int main(int argc , char *argv[])
{
    int socket_desc, new_sock, sd, valread;
    struct sockaddr_in address;
    char buffer[1025];
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


