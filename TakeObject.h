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

using namespace std;
class TakeObject: public Hmdp {
public:
    TakeObject();
    TakeObject(const TakeObject& orig);
    virtual ~TakeObject();
private:

    std::map<VariableSet, double> transitionFunction(VariableSet state, string action);
    int rewardFunction(VariableSet state, string action);
    bool isGoalState(VariableSet state);
    bool isStartingState(VariableSet state);
    
};

#endif	/* TAKEOBJECT_H */

