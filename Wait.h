/* 
 * File:   Wait.h
 * Author: mfiore
 *
 * Created on March 21, 2016, 10:46 AM
 */

#ifndef WAIT_H
#define	WAIT_H

#include "Hmdp.h"
#include <string>
#include <vector>

using namespace std;
class Wait: public Hmdp {
public:
    Wait();
    Wait(const Wait& orig);
    virtual ~Wait();
private:
    std::map<VariableSet, double> transitionFunction(VariableSet state, string action);
    int rewardFunction(VariableSet state, string action);
    bool isGoalState(VariableSet state);
    bool isStartingState(VariableSet state);

    string agent_loc_var_;
    string agent_name_;
};

#endif	/* WAIT_H */

