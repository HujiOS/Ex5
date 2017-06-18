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


enum msg_types{
CREATE_GROUP, SEND, WHO, EXIT, HELLO
};
static map<msg_types, string>enum_to_msgs = {{CREATE_GROUP, "create_group"}, {SEND,"send"},
                              {WHO,"who"}, {EXIT, "exit"}, {HELLO, "hello"}};
static map<string, msg_types>  msgs_to_enum = {{"create_group", CREATE_GROUP},
                                        {"send",SEND}, {"who", WHO}, {"exit",EXIT},
                                        {"hello", HELLO}};

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





#endif