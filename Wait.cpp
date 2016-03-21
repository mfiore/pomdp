/* 
 * File:   Wait.cpp
 * Author: mfiore
 * 
 * Created on March 21, 2016, 10:46 AM
 */

#include "Wait.h"

Wait::Wait() {
    agent_loc_var_ = "agent_isAt";
    variables.push_back(agent_loc_var_);

    std::vector<string> locations{"table", "bed", "counter", "shelf"};
    agent_name_ = "agent";

    std::map<string, std::vector<string> > var_values;
    var_values[agent_loc_var_] = locations;

    this->varValues = var_values;

    std::vector<string> actions;
    actions.push_back(agent_name_ + "_wait");

    this->actions = actions;

    vector<string> par_var;
    parameters.push_back(agent_name_);
    par_var.clear();
    par_var.push_back(agent_loc_var_);
    parameter_variables[agent_name_] = par_var;
    variable_parameter[par_var[0]] = agent_name_;
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