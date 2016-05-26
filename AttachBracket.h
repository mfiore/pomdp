/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AttachBracket.h
 * Author: mfiore
 *
 * Created on May 4, 2016, 5:18 PM
 */

#ifndef ATTACHBRACKET_H
#define ATTACHBRACKET_H

#include "Hmdp.h"
#include <string>
#include <map>
#include <boost/lexical_cast.hpp>
#include "GetObject.h"

using namespace std;

class AttachBracket : public Hmdp {
public:
    AttachBracket();
    AttachBracket(const AttachBracket& orig);
    virtual ~AttachBracket();

    std::map<VariableSet, double> transitionFunction(VariableSet state, string action);
    bool isStartingState(VariableSet state);
    int rewardFunction(VariableSet state, string action);
    bool isGoalState(VariableSet state);

    void assignParametersFromActionName(string action_name);

private:
    string agent_loc_var_;
    string surface_status_var_;
    string bracket_loc_var_;

    string agent_name_;
    string surface_name_;
    string bracket_name_;
};
#endif

