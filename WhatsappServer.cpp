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

#include <bfd.h>
#include <iostream>
#include "Constants.h"

#define ERR_MSG string("ERROR:")
#define STDIN 0
#define MSG_END string(".\n")
#define GROUP_SUCC(group) string("Group ") +string("\"")+ group + string("\"")+string(" was created successfully") + MSG_END
#define GROUP_ERR(group) ERR_MSG + string(" failed to create group ") + string("\"") + group + string("\"") + MSG_END


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

int create_group(string group_name, vector<string> user_names)
{
    if(groups.find(group_name) != groups.end()) return ERR;
    if(nic_to_socket.find(group_name) != nic_to_socket.end()) return ERR;

    sort(user_names.begin(), user_names.end());
    user_names.erase(unique(user_names.begin(), user_names.end()), user_names.end());

    if(user_names.size() <= 1) return ERR;

    groups[group_name] = user_names;

    return SUCCESS;
}

int send_to_user(string user, string msg)
{
    if(nic_to_socket.find(user) == nic_to_socket.end()) return ERR;
    if(sendMsg(nic_to_socket[user], msg) != SUCCESS) return ERR;
    return SUCCESS;
}

int send_to_target(string name, string msg)
{
    if(groups.find(name) != groups.end())
    {
        vector<string> send_to = groups[name];
        for(string user: send_to)
        {
            if(send_to_user(user, msg) != SUCCESS) return ERR;
        }
        return SUCCESS;
    }
    return send_to_user(name, msg);
}

int parse_incoming(int sid, string s)
{
    cmatch m;
    vector<string> tokens = parse_delim(s, ' ');
    vector<string> names;

    string msg;
    string s1 = tokens[0];
    switch(msgs_to_enum[s1])
    {
        case CREATE_GROUP:
            names = parse_delim(tokens[2], ',');
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
            msg = socket_to_nic[sid] + string(":") + tokens[2];
            if(send_to_target(tokens[1], msg) != SUCCESS) {
                cerr <<socket_to_nic[sid]<< ": " <<ERR_MSG << " failed to send " <<
                     tokens[2] << " to " << tokens[1] << MSG_END;
                sendMsg(sid, ERR_MSG + string(" failed to send") + MSG_END);
                return ERR;
            }

            sendMsg(sid, string("Sent successfully") + MSG_END);
            cout << msg << " was sent successfully to " << tokens[1] << MSG_END;
            return SUCCESS;
        case HELLO:


        case WHO:

        case EXIT:
        default:
            break;
    }
}


int main(int argc, char *argv[]) {
    int socket_desc, new_sock, sd, valread;
    struct sockaddr_in address;
    string msg;
    fd_set active_fd_set;
    if(argc < 2){
        std::cerr << "Usage: whatsappServer portNum" << endl;
        exit(1);
    }

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
                    parse_incoming(sd, msg);
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


