/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   GlueSurface.cpp
 * Author: mfiore
 * 
 * Created on May 25, 2016, 2:06 PM
 */

#include "GlueSurface.h"

GlueSurface::GlueSurface() {
    agent_name_ = "agent";
    surface_name_ = "surface";
    glue_name_ = "gluebottle";

    std::vector<string> locations{"other_location", "surface"};
    std::vector<string> statuses{"other_status", "cleaned", "glued"};

    agent_loc_var_ = agent_name_ + "_isAt";
    surface_status_var_ = surface_name_ + "_status";
    glue_loc_var_ = glue_name_ + "_isAt";

    variables.push_back(agent_loc_var_);
    variables.push_back(surface_status_var_);
    variables.push_back(glue_loc_var_);

    std::map<string, std::vector < string>> var_values;
    var_values[agent_loc_var_] = locations;
    var_values[surface_status_var_] = statuses;
    var_values[glue_loc_var_].push_back("other_location");
    var_values[glue_loc_var_].push_back("other_agent");
    var_values[glue_loc_var_].push_back(agent_name_);

    this->varValues=var_values;
    abstract_states_[glue_loc_var_]["surface1"]="other_location";
    abstract_states_[glue_loc_var_]["surface2"]="other_location";
    abstract_states_[glue_loc_var_]["surface3"]="other_location";
    abstract_states_[glue_loc_var_]["table"]="other_location";
    abstract_states_[glue_loc_var_]["agent1"]="other_agent";
    abstract_states_[glue_loc_var_]["agent2"]="other_agent";
    
    abstract_states_[surface_status_var_]["none"]="other_status";
    abstract_states_[surface_status_var_]["completed"]="other_status";
    
    abstract_states_[agent_loc_var_]["surface1"]="other_location";
    abstract_states_[agent_loc_var_]["surface2"]="other_location";
    abstract_states_[agent_loc_var_]["surface3"]="other_location";
    abstract_states_[agent_loc_var_]["table"]="other_location";

    actions.push_back(agent_name_ + "_get_" + glue_name_);
    actions.push_back(agent_name_ + "_move_" + surface_name_);
    actions.push_back(agent_name_ + "_glue_" + surface_name_);
    hierarchy_map_[agent_name_ + "_get_" + glue_name_] = new GetObject();

    this->actions = actions;

    parameters.push_back(agent_name_);
    parameters.push_back(surface_name_);

    parameter_action_place[0] = agent_name_;
    parameter_action_place[2] = surface_name_;

    vector<string> par_var;
    par_var.push_back(agent_loc_var_);
    parameter_variables[agent_name_] = par_var;
    variable_parameter[par_var[0]] = agent_name_;


    par_var.clear();
    par_var.push_back(surface_status_var_);
    parameter_variables[surface_name_] = par_var;
    variable_parameter[par_var[0]] = surface_name_;

    name = "agent_glue_surface";

}

GlueSurface::GlueSurface(const GlueSurface& orig) {

}

GlueSurface::~GlueSurface() {
    delete hierarchy_map_[agent_name_ + "_get_" + glue_name_];
}

//void GlueSurface::assignParametersFromActionName(string action_name) {
//    vector<string> action_parts = StringOperations::stringSplit(action_name, '_');
//    std::map<string, string> instance;
//    if (action_parts.size() > 0) {
//        instance[agent_name_] = action_parts[0];
//    }
//    if (action_parts.size() > 2) {
//        instance[surface_name_] = action_parts[2];
//    }
//    assignParameters(instance);
//}

std::map<VariableSet, double> GlueSurface::transitionFunction(VariableSet state, string action) {
    VarStateProb future_beliefs;

    vector<string> action_parameters = MdpBasicActions::getActionParameters(action);
    string action_name = action_parameters[1];

    if (action_name == "wait") {
        future_beliefs[state] = 1;
    } else if (action_name == "glue"
            && state.set[surface_status_var_] == "cleaned"
            && state.set[agent_loc_var_] == surface_name_
            && state.set[glue_loc_var_] == agent_name_) {
        state.set[surface_status_var_] = "glued";
        future_beliefs[state] = 1;
    } else if (action_name == "move") {
        future_beliefs = MdpBasicActions::applyMove(agent_loc_var_, action_parameters[2], state);
    } else {
        future_beliefs[state] = 1;
    }
    return future_beliefs;
}

bool GlueSurface::isStartingState(VariableSet state) {
    if (state.set[surface_status_var_] == "cleaned") return true;
    return false;

}

int GlueSurface::rewardFunction(VariableSet state, string action) {
    if (state.set[surface_status_var_] == "cleaned"
            && state.set[agent_loc_var_ ] == surface_name_
            && action == agent_name_ + "_glue_" + surface_name_
            && state.set[glue_loc_var_] == agent_name_) {
        return 100;
    } else return 0;

}

bool GlueSurface::isGoalState(VariableSet state) {
    if (state.set[surface_status_var_ ] == "glued") return true;
    return false;

}
