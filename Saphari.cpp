/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Saphari.cpp
 * Author: mfiore
 * 
 * Created on May 4, 2016, 4:31 PM
 */

#include "Saphari.h"

Saphari::Saphari() {
    agent_name_ = "agent";
    bracket_name_ = "bracket";
    glue_name_ = "gluebottle";
    surface_name_ = "surface";


    std::vector<string> locations{"table", "surface1", "surface2", "surface3"};
    std::vector<string> statuses{"other_status", "completed"};

    agent_loc_var_ = agent_name_ + "_isAt";
    for (int i = 1; i < 4; i++) {
        bracket_loc_var_.push_back(bracket_name_ + boost::lexical_cast<string>(i) + "_isAt");
        surface_status_var_.push_back(surface_name_ + boost::lexical_cast<string>(i) + "_status");
    }

    glue_loc_var_ = glue_name_ + "_isAt";

    //    variables.push_back(glue_loc_var_);
    //    variables.push_back(agent_loc_var_);

    std::map<string, std::vector < string>> var_values;
    //    var_values[agent_loc_var_] = locations;
    //    var_values[glue_loc_var_] = locations;
    //    var_values[glue_loc_var_].push_back(agent_name_);
    //    var_values[glue_loc_var_].push_back("other");
    for (int i = 0; i < 3; i++) {
        variables_.push_back(bracket_loc_var_[i]);
        var_values[bracket_loc_var_[i]] = locations;
        var_values[bracket_loc_var_[i]].push_back(agent_name_);
        var_values[bracket_loc_var_[i]].push_back("other_agent");

        abstract_states_[bracket_loc_var_[i]]["agent1"] = "other_agent";
        abstract_states_[bracket_loc_var_[i]]["agent2"] = "other_agent";


        variables_.push_back(surface_status_var_[i]);
        var_values[surface_status_var_[i]] = statuses;

        abstract_states_[surface_status_var_[i]]["none"] = "other_status";
        abstract_states_[surface_status_var_[i]]["cleaned"] = "other_status";
        abstract_states_[surface_status_var_[i]]["glued"] = "other_status";

    }
    this->var_values_ = var_values;

    std::vector<string> actions;
    for (int i = 1; i < 4; i++) {
        for (int j = 1; j < 4; j++) {
            actions.push_back(agent_name_ + "_assemble_" + bracket_name_ + to_string(i) + "_" + surface_name_ + to_string(j));
            hierarchy_map_[agent_name_ + "_assemble_" + bracket_name_ + to_string(i) + "_" + surface_name_ + to_string(j)] = new AssembleBracket();

        }

    }
    this->actions_ = actions;

    parameters_.push_back(agent_name_);
    vector<string> par_var;
    //    par_var.push_back(agent_loc_var_);
    //    parameter_variables[agent_name_] = par_var;
    //    variable_parameter[par_var[0]] = agent_name_;

    parameter_action_place_[0] = agent_name_;

    name_ = "agent_saphari";

}

Saphari::Saphari(const Saphari& orig) {

}

Saphari::~Saphari() {

}

std::map<VariableSet, double> Saphari::transitionFunction(VariableSet state, string action) {
    VarStateProb future_beliefs;

    if (action == agent_name_ + "_wait") {
        future_beliefs[state] = 1;
    }
    return future_beliefs;

}

bool Saphari::isStartingState(VariableSet state) {
    return true;

}

int Saphari::rewardFunction(VariableSet state, string action) {
    int n_not_completed = 0;
    int i_not_completed = -1;
    int i_not_placed = -1;
    vector<bool> is_placed = {false, false, false};
    for (int i = 0; i < 3; i++) {
        if (state.set[surface_status_var_[i]] != "completed") {
            n_not_completed++;
            i_not_completed = i;
        } else {
            if (state.set[bracket_loc_var_[0]] == surface_name_ + to_string(i + 1)) {
                is_placed[0] = true;
            } else if (state.set[bracket_loc_var_[1]] == surface_name_ + to_string(i + 1)) {
                is_placed[1] = true;
            } else if (state.set[bracket_loc_var_[2]] == surface_name_ + to_string(i + 1)) {
                is_placed[2] = true;
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        if (is_placed[i] == false) {
            i_not_placed = i;
            break;
        }
    }
    if (n_not_completed == 1 &&
            action == agent_name_ + "_assemble_" + bracket_name_ + to_string(i_not_placed + 1) + "_" + surface_name_ + to_string(i_not_completed + 1)) {
        return 1000;
    } else return 0;

}

bool Saphari::isGoalState(VariableSet state) {
    for (int i = 0; i < 3; i++) {
        if (state.set[surface_status_var_[i]] != "completed") {
            return false;
        }
    }
    for (int i = 0; i < 3; i++) {
        if (state.set[bracket_loc_var_[i]] == "table" || state.set[bracket_loc_var_[i]]==agent_name_ ||
                state.set[bracket_loc_var_[i]]=="other_agent") {
            return false;
        }
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i != j && state.set[bracket_loc_var_[i]] == state.set[bracket_loc_var_[j]]) {
                return false;
            }
        }
    }
    return true;

}