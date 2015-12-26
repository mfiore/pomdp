/* 
 * File:   RewardInput.h
 * Author: mfiore
 *
 * Created on October 9, 2014, 3:44 PM
 */

#ifndef REWARDINPUT_H
#define	REWARDINPUT_H

#include "string"
#include <map>
using namespace std;

class RewardInput {
public:
    RewardInput();
    RewardInput(const RewardInput& orig);
    virtual ~RewardInput();

    inline bool operator<(const RewardInput& b) const {
        map<string, string>::const_iterator i;
        i = state.begin();
        while (i != state.end()) {
            if (b.state.find(i->first) != b.state.end()) {
                if (b.state.at(i->first) == "*" || i->second == "*") {
                    i++;
                } else if (i->second < b.state.at(i->first)) {
                    return true;
                } else if (i->second > b.state.at(i->first)) {
                    return false;
                } else {
                    i++;
                }
            } else {
                i++;
            }
        }
        return false;

    }

    inline bool operator==(const RewardInput& b) const {
        if (state.size() != b.state.size()) return false;
        map<string, string>::const_iterator i;
        i = state.begin();
        while (i != state.end()) {
            if (b.state.find(i->first) != b.state.end()) {
                if (i->second != b.state.at(i->first) && i->second != "*" && b.state.at(i->first) != "*") return false;
                i++;
            } else return false;
        }
    }
    map<string, string> state;


private:

};

#endif	/* REWARDINPUT_H */

