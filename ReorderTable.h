/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ReorderTable.h
 * Author: mfiore
 *
 * Created on August 12, 2016, 2:25 PM
 */

#ifndef REORDERTABLE_H
#define REORDERTABLE_H

#include "Mdp.h"

#include <string>
#include <vector>
#include "MdpBasicActions.h"

using namespace std;
class ReorderTable: public Mdp {
public:
    ReorderTable();
    ReorderTable(const ReorderTable& orig);
    virtual ~ReorderTable();
    
private:

    std::map<VariableSet, double> transitionFunction(VariableSet state, string action);
    int rewardFunction(VariableSet state, string action);
    bool isGoalState(VariableSet state);
    bool isStartingState(VariableSet state);
    
    string agent_loc_var_;
    string bottle_loc_var_;
    string glass_loc_var_;
    
    string agent_name_;
    string glass_name_;
    string bottle_name_;

    string place_location_;
    
};
#endif /* REORDERTABLE_H */

