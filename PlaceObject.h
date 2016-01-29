/* 
 * File:   PlaceObject.h
 * Author: mfiore
 *
 * Created on January 14, 2016, 3:53 PM
 */

#ifndef PLACEOBJECT_H
#define	PLACEOBJECT_H

#include "Hmdp.h"
#include <string>
#include <vector>

using namespace std;
class PlaceObject:public Hmdp {
public:
    PlaceObject();
    PlaceObject(const PlaceObject& orig);
    virtual ~PlaceObject();
private:

    std::map<VariableSet, double> transitionFunction(VariableSet state, string action);
    int rewardFunction(VariableSet state, string action);
    bool isGoalState(VariableSet state);
    bool isStartingState(VariableSet state);

    string agent_loc_var_;
    string object_loc_var_;
    string goal_loc_var_;
    string agent_name_;
    string object_name_;
    string goal_location_;
    

};

#endif	/* PLACEOBJECT_H */

