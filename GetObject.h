/* 
 * File:   GetObject.h
 * Author: mfiore
 *
 * Created on January 14, 2016, 12:40 PM
 */

#ifndef GETOBJECT_H
#define	GETOBJECT_H

#include "Hmdp.h"

#include <string>
#include <vector>
#include "MdpBasicActions.h"

using namespace std;
class GetObject: public Hmdp {
public:
    GetObject();
    GetObject(const GetObject& orig);
    virtual ~GetObject();
    
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

#endif	/* GETOBJECT_H */


