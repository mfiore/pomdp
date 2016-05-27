/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CleanSurface.h
 * Author: mfiore
 *
 * Created on May 4, 2016, 5:18 PM
 */

#ifndef CLEANSURFACE_H
#define CLEANSURFACE_H

#include "Hmdp.h"
#include <string>
#include <map>
#include <boost/lexical_cast.hpp>
#include"MdpBasicActions.h"

using namespace std;

class CleanSurface : public Hmdp {
public:
    CleanSurface();
    CleanSurface(const CleanSurface& orig);
    virtual ~CleanSurface();

    std::map<VariableSet, double> transitionFunction(VariableSet state, string action);
    bool isStartingState(VariableSet state);
    int rewardFunction(VariableSet state, string action);
    bool isGoalState(VariableSet state);
    void assignParametersFromActionName(string action_name);
    string getParametrizedAction(string action_name);


private:
    string agent_loc_var_;
    string surface_status_var_;

    string agent_name_;
    string surface_name_;
};



#endif /* CLEANSURFACE_H */

