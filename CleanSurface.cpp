/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CleanSurface.cpp
 * Author: mfiore
 * 
 * Created on May 4, 2016, 5:18 PM
 */

#include "CleanSurface.h"

CleanSurface::CleanSurface() {
    agent_name_ = "agent";
    surface_name_ = "surface";

    std::vector<string> locations{"table", "surface1", "surface2", "surface3", "surface"};
    std::vector<string> statuses{"none", "cleaned", "glued", "completed"};

    agent_loc_var_ = agent_name_ + "_isAt";
    surface_status_var_ = surface_name_ + "_status";

    variables.push_back(agent_loc_var_);
    variables.push_back(surface_status_var_);

    std::map<string, std::vector < string>> var_values;
    var_values[agent_loc_var_] = locations;
    var_values[surface_status_var_] = statuses;


    this->varValues = var_values;

    actions.push_back(agent_name_ + "_move_" + surface_name_);
    actions.push_back(agent_name_ + "_clean_" + surface_name_);

    this->actions = actions;

    parameters.push_back(agent_name_);
    parameters.push_back(surface_name_);

    vector<string> par_var;
    par_var.push_back(agent_loc_var_);
    parameter_variables[agent_name_] = par_var;
    variable_parameter[par_var[0]] = agent_name_;


    par_var.clear();
    par_var.push_back(surface_status_var_);
    parameter_variables[surface_name_] = par_var;
    variable_parameter[par_var[0]] = surface_name_;
}

CleanSurface::CleanSurface(const CleanSurface& orig) {

}

CleanSurface::~CleanSurface() {

}

void CleanSurface::assignParametersFromActionName(string action_name) {
    vector<string> action_parts = StringOperations::stringSplit(action_name, '_');
    std::map<string, string> instance;
    if (action_parts.size() > 0) {
        instance[agent_name_] = action_parts[0];
    }
    if (action_parts.size() > 2) {
        instance[surface_name_] = action_parts[2];
    }
    assignParameters(instance);
}

string CleanSurface::getParametrizedAction(string action_name) {
    vector<string> action_parts = StringOperations::stringSplit(action_name, '_');
    stringstream param_action_name;

    param_action_name << "agent_" << action_parts[1];
    if (action_parts.size() > 2) {
        param_action_name << "_surface";
    }
    return param_action_name.str();

}
std::map<VariableSet, double> CleanSurface::transitionFunction(VariableSet state, string action) {
    VarStateProb future_beliefs;

    vector<string> action_parameters = MdpBasicActions::getActionParameters(action);
    string action_name = action_parameters[1];

    if (action_name == "wait") {
        future_beliefs[state] = 1;
    } else if (action_name == "clean"
            && state.set[surface_status_var_] == "none"
            && state.set[agent_loc_var_] == surface_name_) {
        state.set[surface_status_var_] = "cleaned";
        future_beliefs[state]=1;
    } else if (action_name == "move") {
        future_beliefs = MdpBasicActions::applyMove(agent_loc_var_, action_parameters[2], state);
    }
    else {
        future_beliefs[state]=1;
    }
    return future_beliefs;
}

bool CleanSurface::isStartingState(VariableSet state) {
    if (state.set[surface_status_var_] == "none") return true;
    return false;

}

int CleanSurface::rewardFunction(VariableSet state, string action) {
    if (state.set[surface_status_var_] == "none"
            && state.set[agent_loc_var_ ] == surface_name_
            && action == agent_name_ + "_clean_" + surface_name_) {
        return 100;
    } else return 0;

}

bool CleanSurface::isGoalState(VariableSet state) {
    if (state.set[surface_status_var_] == "cleaned") return true;
    return false;

}
