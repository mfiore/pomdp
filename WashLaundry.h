/* 
 * File:   WashLaundry.h
 * Author: mfiore
 *
 * Created on December 22, 2015, 9:45 PM
 */

#ifndef WASHLAUNDRY_H
#define	WASHLAUNDRY_H

#include "Hmdp.h"
#include <vector>
#include <string>
#include "GetLaundry.h"
#include "StartWashingMachine.h"

class WashLaundry : public Hmdp {
public:
    WashLaundry();
    WashLaundry(const WashLaundry& orig);
    virtual ~WashLaundry();

    std::map<VariableSet, double> transitionFunction(VariableSet state, string action);
    bool isStartingState(VariableSet state);
    int rewardFunction(VariableSet state, string action);
    bool isGoalState(VariableSet state);
private:

};

#endif	/* WASHLAUNDRY_H */

