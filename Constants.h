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

#define OKAY_MESSAGE "OK"
#define OKAY_LENGTH 2
#define LEAVE_MESSAGE "MOTHERFUCKERWANNALEAVEUS"
#define ERR_MSG "HOUSTONWEHAVEAPROBLEMSOPLEASEBEPATIENT"

static const string HELLO_MSG = string("HELLOMADAFAKAWORLD");
enum msg_types{
CREATE_GROUP, SEND, WHO, EXIT, HELLO
};
static map<msg_types, string>enum_to_msgs = {{CREATE_GROUP, "create_group"}, {SEND,"send"},
                              {WHO,"who"}, {EXIT, "exit"}, {HELLO, HELLO_MSG}};
static map<string, msg_types>  msgs_to_enum = {{"create_group", CREATE_GROUP},
                                        {"send",SEND}, {"who", WHO}, {"exit",EXIT},
                                        {HELLO_MSG, HELLO}};

static const regex digit_nums_only("[\w\s]+");
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

bool is_msg_legal(string s)
{
    cmatch m;
    vector<string> tokens = parse_delim(s, ' ');
    vector<string> names;
    if(tokens.size() < 0 || msgs_to_enum.find(tokens[0]) == msgs_to_enum.end()) return false;
    string s1 = tokens[0];
    switch(msgs_to_enum[s1])
    {
        case CREATE_GROUP:
            if(tokens.size() < 3) return false;
            if (!regex_match(tokens[1].c_str(), m, digit_nums_only)) return false;
             names = parse_delim(tokens[2], ',');
            for(string name: names)
            {
                if (!regex_match(name.c_str(), m, digit_nums_only)) return false;
            }

            //TODO: check if there are extra params?
            return true;
        case SEND:
            if(tokens.size() < 3) return false;
            return regex_match(tokens[1].c_str(), m, digit_nums_only);
        case WHO:
            return true;
        case EXIT:
            return true;
        default:
            return false;
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
    cout << "Message Received! " << word << endl;
    return word;
}

void sendMessage(int socket, string msg) {
    string fmsg = "";
    char tBuff[256];
    int rb;
    if (msg.size() > 9999) {
        // Error we cant send messages longer than 9999 bytes.
        msg = msg.substr(0, 9998); // we will trim it madafaka!$@
    }
    fmsg += intToMsgLength(msg.size());
    fmsg += msg;
    if(send(socket, fmsg.c_str(), fmsg.size(), 0) != fmsg.size()){
        handleSysErr("send",errno);
    }
    if((rb = read(socket, tBuff, 256)) < 0){
        handleSysErr("read",errno);
    }
    tBuff[rb] = '\0';
    if (strcmp(OKAY_MESSAGE,tBuff)) {
        cout << "Error receiveing the okay response.. check it out intenally..." << endl;
    }
}




#endif