/* 
 * File:   TakeObject.h
 * Author: mfiore
 *
 * Created on January 14, 2016, 12:40 PM
 */

#ifndef TAKEOBJECT_H
#define	TAKEOBJECT_H

#include "Hmdp.h"

#include <string>
#include <vector>
#include "MdpBasicActions.h"

using namespace std;
class TakeObject: public Hmdp {
public:
    TakeObject();
    TakeObject(const TakeObject& orig);
    virtual ~TakeObject();
    
    void assignParametersFromActionName(string action_name);
    string getDeparametrizedAction(string action_name);

private:

    std::map<VariableSet, double> transitionFunction(VariableSet state, string action);
    int rewardFunction(VariableSet state, string action);
    bool isGoalState(VariableSet state);
    bool isStartingState(VariableSet state);
    
    string agent_loc_var_;
    string object_loc_var_;
    string agent_name_;
    string object_name_;
    
};

#endif	/* TAKEOBJECT_H */

