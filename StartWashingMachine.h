/* 
 * File:   GetLaundry.h
 * Author: mfiore
 *
 * Created on December 22, 2015, 12:18 PM
 */

#ifndef STARTWASHINGMACHINE_H
#define	STARTWASHINGMACHINE_H

#include "Hmdp.h"

#include <string>
#include <vector>

using namespace std;

class StartWashingMachine: public Hmdp {
public:
    StartWashingMachine();
    StartWashingMachine(const StartWashingMachine& orig);
    virtual ~StartWashingMachine();
    
    std::map<VariableSet, double> transitionFunction(VariableSet state, string action);
    int rewardFunction(VariableSet state, string action);
    bool isGoalState(VariableSet state);
    bool isStartingState(VariableSet state);
    string laundryHelper(string laundry_isAt,string human_isAt,string box_isAt,string action,string laundry_name);

private:

};

#endif	/* STARTWASHINGMACHINE_H */

