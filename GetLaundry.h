/* 
 * File:   GetLaundry.h
 * Author: mfiore
 *
 * Created on December 22, 2015, 12:18 PM
 */

#ifndef GETLAUNDRY_H
#define	GETLAUNDRY_H

#include "Hmdp.h"

#include <string>
#include <vector>

using namespace std;

class GetLaundry: public Hmdp {
public:
    GetLaundry();
    GetLaundry(const GetLaundry& orig);
    virtual ~GetLaundry();
    
    std::map<VariableSet, double> transitionFunction(VariableSet state, string action);
    int rewardFunction(VariableSet state, string action);
    bool isGoalState(VariableSet state);
    bool isStartingState(VariableSet state);

private:

};

#endif	/* GETLAUNDRY_H */

