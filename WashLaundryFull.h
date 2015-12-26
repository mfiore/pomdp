/* 
 * File:   WashLaundryFull.h
 * Author: mfiore
 *
 * Created on December 25, 2015, 10:01 PM
 */

#ifndef WASHLAUNDRYFULL_H
#define	WASHLAUNDRYFULL_H


#include "Pomdp.h"
#include <vector>
#include <string>
#include "GetLaundry.h"
#include "StartWashingMachine.h"

class WashLaundryFull : public Pomdp {
public:
    WashLaundryFull();
    WashLaundryFull(const WashLaundryFull& orig);
    virtual ~WashLaundryFull();

    std::map<VariableSet, double> transitionFunction(VariableSet state, string action);
    int rewardFunction(VariableSet state, string action);
    bool isGoalState(VariableSet state);
    bool isStartingState(VariableSet state);
private:

};

#endif	/* WASHLAUNDRYFULL_H */

