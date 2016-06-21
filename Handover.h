/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Handover.h
 * Author: theworld
 *
 * Created on June 17, 2016, 12:01 PM
 */

#ifndef HANDOVER_H
#define HANDOVER_H

#include "Hmdp.h"

#include <string>
#include <vector>
#include "MdpBasicActions.h"

using namespace std;
class Handover: public Hmdp {
public:
    Handover();
    Handover(const Handover& orig);
    virtual ~Handover();
    
//    void assignParametersFromActionName(string action_name);
//    string getDeparametrizedAction(string action_name);

private:

    std::map<VariableSet, double> transitionFunction(VariableSet state, string action);
    int rewardFunction(VariableSet state, string action);
    bool isGoalState(VariableSet state);
    bool isStartingState(VariableSet state);
    
    string agent1_loc_var_;
    string agent2_loc_var_;
    string object_loc_var_;
    string agent1_name_;
    string agent2_name_;
    string object_name_;
    
};

#endif /* HANDOVER_H */

