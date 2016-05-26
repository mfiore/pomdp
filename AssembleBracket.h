/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AssembleBracket.h
 * Author: mfiore
 *
 * Created on May 4, 2016, 4:51 PM
 */

#ifndef ASSEMBLEBRACKET_H
#define ASSEMBLEBRACKET_H

#include "Hmdp.h"
#include <string>
#include "CleanSurface.h"
#include "GlueSurface.h"
#include "AttachBracket.h"
#include <map>
#include <boost/lexical_cast.hpp>

using namespace std;

class AssembleBracket : public Hmdp {
public:
    AssembleBracket();
    AssembleBracket(const AssembleBracket& orig);
    virtual ~AssembleBracket();

    std::map<VariableSet, double> transitionFunction(VariableSet state, string action);
    bool isStartingState(VariableSet state);
    int rewardFunction(VariableSet state, string action);
    bool isGoalState(VariableSet state);
    
    void assignParametersFromActionName(string action_name);


private:
    string agent_loc_var_;
    string bracket_loc_var_;
    string glue_loc_var_;
    string surface_status_var_;

    string bracket_name_;
    string glue_name_;
    string agent_name_;
    string surface_name_;
};


#endif /* ASSEMBLEBRACKET_H */

