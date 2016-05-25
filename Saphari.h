/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Saphari.h
 * Author: mfiore
 *
 * Created on May 4, 2016, 4:31 PM
 */

#ifndef SAPHARI_H
#define SAPHARI_H


#include "Hmdp.h"
#include <string>
#include "AssembleBracket.h"
#include <map>
#include <boost/lexical_cast.hpp>

using namespace std;

class Saphari : public Hmdp {
public:
    Saphari();
    Saphari(const Saphari& orig);
    virtual ~Saphari();

    std::map<VariableSet, double> transitionFunction(VariableSet state, string action);
    bool isStartingState(VariableSet state);
    int rewardFunction(VariableSet state, string action);
    bool isGoalState(VariableSet state);

private:
    string agent_loc_var_;
    vector<string> bracket_loc_var_;
    string glue_loc_var_;
    vector<string> surface_status_var_;
 
    
    string bracket_name_;
    string glue_name_;
    string agent_name_;
    string surface_name_;

};

#endif /* SAPHARI_H */

