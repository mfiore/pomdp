/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DrinkWater.h
 * Author: mfiore
 *
 * Created on August 12, 2016, 11:22 AM
 */

#ifndef DRINKWATER_H
#define DRINKWATER_H

#include "Mdp.h"

#include <string>
#include <vector>
#include "MdpBasicActions.h"

using namespace std;
class DrinkWater: public Mdp {
public:
    DrinkWater();
    DrinkWater(const DrinkWater& orig);
    virtual ~DrinkWater();
    
private:

    std::map<VariableSet, double> transitionFunction(VariableSet state, string action);
    int rewardFunction(VariableSet state, string action);
    bool isGoalState(VariableSet state);
    bool isStartingState(VariableSet state);
    
    string agent_loc_var_;
    string bottle_loc_var_;
    string glass_loc_var_;
    string glass_capacity_var_;
    string bottle_capacity_var_;
    string has_drunk_var_;
    
    string agent_name_;
    string glass_name_;
    string bottle_name_;
    
    
    
};
#endif /* DRINKWATER_H */

