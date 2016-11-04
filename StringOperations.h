/* 
 * File:   StringOperations.h
 * Author: mfiore
 *
 * Created on December 30, 2015, 4:03 PM
 */

#ifndef STRINGOPERATIONS_H
#define	STRINGOPERATIONS_H

#include <string>
#include <vector>
#include <sstream>

class StringOperations {
public:

    static std::vector<double> & splitElems(const std::string &s, char delim, std::vector<double> &elems) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(stod(item));
        }
        return elems;
    }

    static std::vector<double> split(const std::string &s, char delim) {
        std::vector<double> elems;
        splitElems(s, delim, elems);
        return elems;
    }

    static std::vector<std::string> & stringSplitElems(const std::string &s, char delim, std::vector<std::string> &elems) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }

    static std::vector<std::string> stringSplit(const std::string &s, char delim) {
        std::vector<std::string> elems;
        stringSplitElems(s, delim, elems);
        return elems;
    }
    
    static string addToString(string s, string to_add, char delim) {
        if (s=="") {
            s=s+to_add;
        }
        else {
            s=s+delim+to_add;
        }
        return s;
    }

};



#endif	/* STRINGOPERATIONS_H */

