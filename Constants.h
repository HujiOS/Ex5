//
// Created by bgalb on 6/18/17.
//

#ifndef EX5CLIENT_CONSTANTS_H
#define EX5CLIENT_CONSTANTS_H
using namespace std;
#include <string>
#include <vector>
#include <iterator>
#include <sstream>
#include <map>
#include <iostream>
#include <regex>
#include <math.h>
#include <stdlib.h>

#define ERR -1
#define SUCCESS 0
#define TIMEOUT -2
#define OKAY_MESSAGE "OK"
#define OKAY_LENGTH 2
#define LEAVE_MESSAGE "MOTHERFUCKERWANNALEAVEUS"
#define ERR_MSG "HOUSTONWEHAVEAPROBLEMSOPLEASEBEPATIENT"

#define BAD_GROUP -3
#define BAD_SEND -4
#define BAD_WHO -5
#define BAD_EXIT -6
#define BAD_HELLO -7
#define BAD_COMMAND -1


static const string HELLO_MSG = string("HELLOMADAFAKAWORLD");
enum msg_types{
CREATE_GROUP, SEND, WHO, EXIT, HELLO
};
static map<msg_types, string>enum_to_msgs = {{CREATE_GROUP, "create_group"}, {SEND,"send"},
                              {WHO,"who"}, {EXIT, "exit"}, {HELLO, HELLO_MSG}};
static map<string, msg_types>  msgs_to_enum = {{"create_group", CREATE_GROUP},
                                        {"send",SEND}, {"who", WHO}, {"exit",EXIT},
                                        {HELLO_MSG, HELLO}};

static const regex digit_nums_only("[\\w\\s]+");
/**
 * check if a given message is legal
 * @param s message to check
 * @return true if it's legal, false otherwise
 */

vector<string> parse_delim(string s, char delim)
{
    istringstream iss(s);
    vector<string> tokens;
    string t;
    while(std::getline(iss, t, delim)) {
        if(t.size()==0) continue;
        tokens.push_back(t);
    }

    return tokens;
}

void handleSysErr(string errCall, int errNu) {
    std::cout << "ERROR: " << errCall << " " << errNu << "." << std::endl;
    exit(1);
}

int is_msg_legal(string s, string sender)
{
    cmatch m;
    bool got_names = false;
    vector<string> tokens = parse_delim(s, ' ');
    vector<string> names;
    if(tokens.size() == 0 || msgs_to_enum.find(tokens[0]) == msgs_to_enum.end()) return BAD_COMMAND;
    string s1 = tokens[0];
    switch(msgs_to_enum[s1])
    {
        case CREATE_GROUP:
            if(tokens.size() < 3 || tokens.size() > 3) return BAD_GROUP;
            if (!regex_match(tokens[1].c_str(), m, digit_nums_only)) return BAD_GROUP;
             names = parse_delim(tokens[2], ',');
            for(string name: names)
            {
                if (!regex_match(name.c_str(), m, digit_nums_only)) return BAD_GROUP;
                got_names = true;
            }
            if(!got_names) return BAD_GROUP;
            return SUCCESS;
        case SEND:
            if(tokens.size() < 3 || tokens.size() > 3) return BAD_SEND;
            if(sender == tokens[1]) return BAD_SEND;
            if(regex_match(tokens[1].c_str(), m, digit_nums_only)) return SUCCESS;
            return BAD_SEND;
        case WHO:
            if (tokens.size()!=1) return BAD_WHO;
            return SUCCESS;
        case EXIT:
            if (tokens.size()!=1) return BAD_EXIT;
            return SUCCESS;
        case HELLO:
            if (tokens.size()!=2) return BAD_HELLO;
            if(regex_match(tokens[1].c_str(), m, digit_nums_only)) return SUCCESS;
            return BAD_HELLO;
        default:
            return BAD_COMMAND;
    }
}

int msgLengthToInt(string ml){
    if(ml.size() < 4){
        return -1;
    }
    return stoi(ml);
}

string intToMsgLength(int number){
    int ml = 4;
    string m = to_string(number);
    string mLength;
    ml -= m.size();
    mLength = string(ml,'0') + m;
    return mLength;
}

string readMessage(int socket) {
    int length = 4, currRead, readBytes = 0, chunks = 256, lr;
    char msgbuffer[256], msgLength[4];
    string word = "";
    while (readBytes < length) {
        if (readBytes == 0) {
            readBytes += read(socket, msgLength, 4);
            if (readBytes == 0) {
                return LEAVE_MESSAGE;
            } else if (readBytes < 0) {
                return ERR_MSG;
            }
            length = msgLengthToInt(msgLength);
        }
        currRead = read(socket, msgbuffer, max(chunks, chunks - length));
        if (currRead < 0) {
            return ERR_MSG;
        }
        readBytes += currRead;
        msgbuffer[currRead] = '\0';
        word += string(msgbuffer);
        bzero(msgbuffer, chunks);
    }
    send(socket, OKAY_MESSAGE, OKAY_LENGTH, 0);
    return word;
}
int confirmWithTimeout(int socket);
int sendMsg(int socket, string msg) {
    string fmsg = "";
    int confirmationCode;
    if (msg.size() > 9999) {
        // Error we cant send messages longer than 9999 bytes.
        msg = msg.substr(0, 9998); // we will trim it madafaka!$@
    }
    fmsg += intToMsgLength(msg.size());
    fmsg += msg;
    if(send(socket, fmsg.c_str(), fmsg.size(), 0) != fmsg.size()){
        handleSysErr("send",errno);
    }
    confirmationCode = confirmWithTimeout(socket);
    if(confirmationCode == TIMEOUT){
        cout << "Tried to send message, got timeout instead of okay :( " << endl;
        return TIMEOUT;
    } else if(confirmationCode == ERR){
        cout << "Tried to send message, got other message, not OK :(" << endl;
        return ERR;
    } else {
        cout << "Message sent successfuly" << endl;
        return SUCCESS;
    }
}

int sendMsg(int socket, string msg, bool force){
    if(force){
        while(sendMsg(socket, msg) < 0){}
    }
    sendMsg(socket, msg);
    return 0;
}



int confirmWithTimeout(int socket){
    fd_set set;
    struct timeval timeout;
    char tBuff[256];
    int rv, rb;
    FD_ZERO(&set); /* clear the set */
    FD_SET(socket, &set); /* add our file descriptor to the set */
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;
    rv = select(socket + 1 , &set, NULL, NULL, NULL);
    if(rv == -1){
        handleSysErr("select",errno);
    }
    if(rv == 0){
        return TIMEOUT;
    } else {
        rb = read(socket, tBuff, 2);
        if(rb < 0){
            handleSysErr("read", errno);
            return 0; // useless line..
        } else {
            tBuff[rb] = '\0';
            if(strcmp(tBuff, OKAY_MESSAGE)){
                return ERR;
            } else {
                return SUCCESS;
            }
        }
    }
}




#endif