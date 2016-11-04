/* 
 * File:   Wait.cpp
 * Author: mfiore
 * 
 * Created on March 21, 2016, 10:46 AM
 */

#include "Wait.h"

Wait::Wait() {
    agent_loc_var_ = "agent_isAt";
    variables_.push_back(agent_loc_var_);

    std::vector<string> locations{"table", "surface1", "surface2", "surface3"};
    agent_name_ = "agent";

    std::map<string, std::vector<string> > var_values;
    var_values[agent_loc_var_] = locations;

    this->var_values_ = var_values;

    std::vector<string> actions;
    actions.push_back(agent_name_ + "_wait");

    this->actions_ = actions;

    vector<string> par_var;
    parameters_.push_back(agent_name_);
    par_var.clear();
    par_var.push_back(agent_loc_var_);
    parameter_variables_[agent_name_] = par_var;
    variable_parameter_[par_var[0]] = agent_name_;

    parameter_action_place_[0]=agent_name_;

    name_="agent_wait";
}

Wait::Wait(const Wait& orig) {
}

Wait::~Wait() {
}

std::map<VariableSet, double> Wait::transitionFunction(VariableSet state, string action) {

    std::map<VariableSet,double> future_beliefs;
    future_beliefs[state]=1;
    
    return future_beliefs;
}

int Wait::rewardFunction(VariableSet state, string action) {
     return 0;
}

bool Wait::isGoalState(VariableSet state) {
    return false;
}

bool Wait::isStartingState(VariableSet state) {
    return true;
}