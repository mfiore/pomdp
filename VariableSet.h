/* 
 * File:   VariableSet.h
 * Author: mfiore
 *
 * Created on August 19, 2014, 1:04 PM
 * 
 * Implements an assignement of variables, with their specific comparisons 
 */

#ifndef VARIABLESET_H
#define	VARIABLESET_H

#include <vector>
#include <map>
#include <iostream>
#include <sstream>
using namespace std;

class VariableSet {
public:
    VariableSet(map<string, string> set);
    VariableSet();
    VariableSet(const VariableSet& orig);
    virtual ~VariableSet();

    //comparison operator

    inline bool operator<(const VariableSet& b) const {
        std::map<string, string>::const_iterator i;
        i = set.begin();
        map<string, string> other_set = b.set;
        while (i != set.end()) {
            if (other_set.find(i->first) != other_set.end()) {
                string v1 = other_set.at(i->first);
                string v2 = i->second;
                if (v1 < v2) {
                    return true;
                } else if (v1 > v2) {
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

    inline bool operator==(const VariableSet& b) const {
        map<string, string> other_set= b.set;
        if (set.size() != other_set.size()) return false;
        std::map<string, string>::const_iterator i;
        i = set.begin();
        while (i != set.end()) {
            if (other_set.find(i->first) != other_set.end()) {
                if (i->second != other_set.at(i->first)) return false;
                i++;
            } else return false;
        }

    }

    inline void operator=(const VariableSet &b) {
        if (b.set.size() > 0) {
            this->set = b.set;
        }
    }

    inline string toString() {
        stringstream ss;
        for (auto s : set) {
            ss << s.first << " " << s.second << "\n";
        }
        return ss.str();
    }
    std::map<string, string> set;

private:

};

#endif	

