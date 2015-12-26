/* 
 * File:   TransitionElement.h
 * Author: mfiore
 *
 * Created on August 19, 2014, 12:42 PM
 */

#ifndef TRANSITIONELEMENT_H
#define	TRANSITIONELEMENT_H
#include <string>
#include <vector>
#include <map>
#include "VariableSet.h"
#include <iostream>
using namespace std;

class TransitionInput {
public:
    TransitionInput();
    TransitionInput(const TransitionInput& orig);
    virtual ~TransitionInput();

    inline bool operator<(const TransitionInput& b) const {
        if (var < b.var) return true;
        if (b.var<var) return false;
        return state<b.state;
    }

    inline bool operator==(const TransitionInput& b) const {
        if (var != b.var) return false;
        return state==b.state;
        }
    
    VariableSet state;
    string var;
private:

};

#endif	/* TRANSITIONELEMENT_H */

