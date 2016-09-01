/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AssembleBracket.cpp
 * Author: mfiore
 * 
 * Created on May 4, 2016, 4:51 PM
 */

#include "AssembleBracket.h"

AssembleBracket::AssembleBracket() {
    agent_name_ = "agent";
    bracket_name_ = "bracket";
    glue_name_ = "gluebottle";
    surface_name_ = "surface";

    std::vector<string> locations{"surface"};
    std::vector<string> statuses{"none", "cleaned", "glued", "completed"};

    agent_loc_var_ = agent_name_ + "_isAt";
    bracket_loc_var_ = bracket_name_ + "_isAt";
    glue_loc_var_ = glue_name_ + "_isAt";
    surface_status_var_ = surface_name_ + "_status";

    variables_.push_back(agent_loc_var_);
    variables_.push_back(bracket_loc_var_);
    variables_.push_back(surface_status_var_);
    variables_.push_back(glue_loc_var_);

    std::map<string, std::vector < string>> var_values;
    var_values[agent_loc_var_] = locations;
    var_values[agent_loc_var_].push_back("other_location");
    //    var_values[glue_loc_var_] = locations;
    var_values[glue_loc_var_].push_back(agent_name_);
    var_values[glue_loc_var_].push_back("other_location");
    var_values[glue_loc_var_].push_back("other_agent");
    //    var_values[bracket_loc_var_]=locations;
    var_values[bracket_loc_var_].push_back(agent_name_);
    var_values[bracket_loc_var_].push_back("other_location");
    var_values[bracket_loc_var_].push_back(surface_name_);
    //    var_values[bracket_loc_var_].push_back("table");
    var_values[bracket_loc_var_].push_back("other_agent");
    var_values[surface_status_var_] = statuses;

    this->var_values_ = var_values;

    abstract_states_[glue_loc_var_]["surface1"] = "other_location";
    abstract_states_[glue_loc_var_]["surface2"] = "other_location";
    abstract_states_[glue_loc_var_]["surface3"] = "other_location";
    abstract_states_[glue_loc_var_]["table"] = "other_location";
    abstract_states_[glue_loc_var_]["agent1"] = "other_agent";
    abstract_states_[glue_loc_var_]["agent2"] = "other_agent";

    abstract_states_[bracket_loc_var_]["surface1"] = "other_location";
    abstract_states_[bracket_loc_var_]["surface2"] = "other_location";
    abstract_states_[bracket_loc_var_]["surface3"] = "other_location";
    abstract_states_[bracket_loc_var_]["table"] = "other_location";
    abstract_states_[bracket_loc_var_]["agent1"] = "other_agent";
    abstract_states_[bracket_loc_var_]["agent2"] = "other_agent";

    abstract_states_[agent_loc_var_]["surface1"] = "other_location";
    abstract_states_[agent_loc_var_]["surface2"] = "other_location";
    abstract_states_[agent_loc_var_]["surface3"] = "other_location";
    abstract_states_[agent_loc_var_]["table"] = "other_location";

    actions_.push_back(agent_name_ + "_clean_" + surface_name_);
    actions_.push_back(agent_name_ + "_glue_" + surface_name_);
    actions_.push_back(agent_name_ + "_apply_" + bracket_name_ + "_" + surface_name_);
    actions_.push_back(agent_name_ + "_get_" + bracket_name_);
    actions_.push_back(agent_name_ + "_get_" + glue_name_);
    actions_.push_back(agent_name_ + "_move_" + surface_name_);
    //    hierarchy_map_[agent_name_ + "_clean_" + surface_name_] = new CleanSurface();
    //    hierarchy_map_[agent_name_ + "_glue_" + surface_name_] = new GlueSurface();
    //    hierarchy_map_[agent_name_ + "_apply_" + bracket_name_ + "_" + surface_name_] = new AttachBracket();
    hierarchy_map_[agent_name_ + "_get_" + bracket_name_] = new GetObject();
    hierarchy_map_[agent_name_ + "_get_" + glue_name_] = new GetObject();

    this->actions_ = actions_;

    parameters_.push_back(agent_name_);
    parameters_.push_back(bracket_name_);
    parameters_.push_back(surface_name_);

    parameter_action_place_[0] = agent_name_;
    parameter_action_place_[2] = bracket_name_;
    parameter_action_place_[3] = surface_name_;

    vector<string> par_var;
        par_var.push_back(agent_loc_var_);
        parameter_variables_[agent_name_] = par_var;
        variable_parameter_[par_var[0]] = agent_name_;

    par_var.clear();
    par_var.push_back(bracket_loc_var_);
    parameter_variables_[bracket_name_] = par_var;
    variable_parameter_[par_var[0]] = bracket_name_;

    par_var.clear();
    par_var.push_back(surface_status_var_);
    parameter_variables_[surface_name_] = par_var;
    variable_parameter_[par_var[0]] = surface_name_;

    name_ = "agent_assemble_bracket_surface";
}

AssembleBracket::AssembleBracket(const AssembleBracket& orig) {

}

AssembleBracket::~AssembleBracket() {
    //    delete hierarchy_map_[agent_name_ + "_clean_" + surface_name_];
    //    delete hierarchy_map_[agent_name_ + "_glue_" + surface_name_];
    //    delete hierarchy_map_[agent_name_ + "_apply_" + bracket_name_ + "_" + surface_name_];

}


//void AssembleBracket::assignParametersFromActionName(string action_name) {
//    vector<string> action_parts = StringOperations::stringSplit(action_name, '_');
//    std::map<string, string> instance;
//    if (action_parts.size() > 0) {
//        instance[agent_name_] = action_parts[0];
//    }
//    if (action_parts.size() > 2) {
//        instance[bracket_name_] = action_parts[2];
//    }
//    if (action_parts.size() > 3) {
//        instance[surface_name_] = action_parts[3];
//    }
//    assignParameters(instance);
//}

std::map<VariableSet, double> AssembleBracket::transitionFunction(VariableSet state, string action) {
    VarStateProb future_beliefs;

    string glue_location = state.set.at(glue_loc_var_);
    string agent_location = state.set.at(agent_loc_var_);
    string surface_status = state.set.at(surface_status_var_);
    string bracket_location = state.set.at(bracket_loc_var_);

    vector<string> action_parameters = MdpBasicActions::getActionParameters(action);
    string action_name = action_parameters[1];

    if (action == agent_name_ + "_wait") {
        future_beliefs[state] = 1;
    } else if (action_name == "glue" && glue_location == agent_name_ && agent_location == surface_name_ &&
            surface_status == "cleaned") {
        state.set[surface_status_var_] = "glued";
        future_beliefs[state] = 1;
    } else if (action_name == "clean" && agent_location == surface_name_ && surface_status == "none") {
        state.set[surface_status_var_] = "cleaned";
        future_beliefs[state] = 1;
    } else if (action_name == "apply"  && agent_location == surface_name_ && surface_status == "glued" 
            && bracket_location==agent_name_) {
        state.set[surface_status_var_] = "completed";
        state.set[bracket_loc_var_]=surface_name_;
        future_beliefs[state] = 1;
    } else if (action_name == "move") {
        future_beliefs = MdpBasicActions::applyMove(agent_loc_var_, action_parameters[2], state);
    } else {
        future_beliefs[state] = 1;
    }


    return future_beliefs;

}

bool AssembleBracket::isStartingState(VariableSet state) {
    return state.set[surface_status_var_] != "completed";
    //            && state.set[bracket_loc_var_] == "table";

}

int AssembleBracket::rewardFunction(VariableSet state, string action) {
    if (state.set[surface_status_var_] == "glued" && action == agent_name_ + "_apply_" + bracket_name_ + "_" + surface_name_) {
        return 100;
    } else return 0;

}

bool AssembleBracket::isGoalState(VariableSet state) {
    if (state.set[surface_status_var_] == "completed") return true;
    return false;

}
