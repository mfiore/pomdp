/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Handover.cpp
 * Author: theworld
 * 
 * Created on June 17, 2016, 12:01 PM
 */

#include "Handover.h"

Handover::Handover() {
    agent1_loc_var_ = "agent1_isAt";
    agent2_loc_var_ = "agent2_isAt";
    object_loc_var_ = "object_isAt";
    variables_.push_back(agent1_loc_var_);
    variables_.push_back(agent2_loc_var_);
    variables_.push_back(object_loc_var_);

    std::vector<string> locations{"table", "surface1", "surface2", "surface3"};
    agent1_name_ = "agent1";
    agent2_name_ = "agent2";
    object_name_ = "object";

    std::map<string, std::vector<string> > var_values;
    var_values[agent1_loc_var_] = locations;
    var_values[agent2_loc_var_] = locations;
    var_values[object_loc_var_].push_back(agent1_name_);
    var_values[object_loc_var_].push_back(agent2_name_);

    this->var_values_ = var_values;

    std::vector<string> actions;
    for (string l1 : locations) {
        for (string l2 : locations) {
            actions.push_back(agent1_name_ + "_move_" + l1 + "-" + agent2_name_ + "_move_" + l2);
            actions.push_back(agent1_name_ + "_move_" + l1 + "-" + agent2_name_ + "_wait");
            actions.push_back(agent1_name_ + "_wait" + "-" + agent2_name_ + "_move_" + l2);
        }
    }
    actions.push_back(agent1_name_ + "_give_" + object_name_ + "_" + agent2_name_ + "-" + agent2_name_ + "_receive_" + object_name_ + "_" + agent1_name_);

    this->actions_ = actions;
    parameters_.push_back(object_name_);
    vector<string> par_var;
    par_var.push_back(object_loc_var_);
    parameter_variables_[object_name_] = par_var;
    variable_parameter_[par_var[0]] = object_name_;

    parameters_.push_back(agent1_name_);
    par_var.clear();
    par_var.push_back(agent1_loc_var_);
    parameter_variables_[agent1_name_] = par_var;
    variable_parameter_[par_var[0]] = agent1_name_;

    parameters_.push_back(agent2_name_);
    par_var.clear();
    par_var.push_back(agent2_loc_var_);
    parameter_variables_[agent2_name_] = par_var;
    variable_parameter_[par_var[0]] = agent2_name_;

    parameter_action_place_[0] = agent1_name_;
    parameter_action_place_[2] = object_name_;
    parameter_action_place_[3] = agent2_name_;
    name_ = "agent_handover_object_agent";

}

Handover::Handover(const Handover & orig) {
}

Handover::~Handover() {
}

//void Handover::assignParametersFromActionName(string action_name) {
//    vector<string> action_parameters=StringOperations::stringSplit(action_name,'_');
//    std::map<string,string> instance;
// 
//    instance[agent_name_]=action_parameters[0];
//    instance[object_name_]=action_parameters[2]; 
//    assignParameters(instance);
//}

string Handover::getDeparametrizedAction(string action_name) {
    vector<string> single_actions = StringOperations::stringSplit(action_name, '-');
    stringstream depar_action_name;

    int index=0;
    for (string action : single_actions) {
        if (index>0) depar_action_name<<"-";
        vector<string> action_parts = StringOperations::stringSplit(action, '_');
        for (int i = 0; i < action_parts.size() - 1; i++) {
            string s = action_parts[i];
            if (parametrized_to_original_.find(s) != parametrized_to_original_.end()) {
                depar_action_name << parametrized_to_original_[s] << "_";
            } else {
                depar_action_name << s << "_";
            }
        }
        if (action_parts.size() > 0) {
            string s = action_parts[action_parts.size() - 1];
            if (parametrized_to_original_.find(s) != parametrized_to_original_.end()) {
                depar_action_name << parametrized_to_original_[s];
            } else {

                depar_action_name << s;
            }
        }
        index++;
    }
    return depar_action_name.str();

}

VarStateProb Handover::transitionFunction(VariableSet state, string action) {
    string human1_isAt = state.set[agent1_loc_var_];
    string human2_isAt = state.set[agent2_loc_var_];
    string object_isAt = state.set[object_loc_var_];

    string future_human1_isAt = human1_isAt;
    string future_human2_isAt = human2_isAt;
    string future_object_isAt = object_isAt;

    vector<string> single_actions = StringOperations::stringSplit(action, '-');
    for (string a : single_actions) {
        vector<string> action_parameters = StringOperations::stringSplit(a, '_');
        string action_name = action_parameters[1];
        string agent = action_parameters[0];
        if (action_name == "move") {
            if (agent == agent1_name_) {
                future_human1_isAt = action_parameters[2];
            } else if (agent == agent2_name_) {
                future_human2_isAt = action_parameters[2];
            }
        } else if (action_name == "give"
                && object_isAt == agent1_name_ && human1_isAt == human2_isAt) {
            future_object_isAt = agent2_name_;
        }
    }
    VarStateProb future_beliefs;

    VariableSet v;
    v.set[agent1_loc_var_] = future_human1_isAt;
    v.set[agent2_loc_var_] = future_human2_isAt;
    v.set[object_loc_var_] = future_object_isAt;

    future_beliefs[v] = 1;

    return future_beliefs;
}

int Handover::rewardFunction(VariableSet state, string action) {
    string human1_isAt = state.set[agent1_loc_var_];
    string human2_isAt = state.set[agent2_loc_var_];
    string object_isAt = state.set[object_loc_var_];

    if (object_isAt == agent1_name_ &&
            action == agent1_name_ + "_give_" + object_name_ + "_" + agent2_name_ + "-" + agent2_name_ + "_receive_" + object_name_ + "_" + agent1_name_
            && human1_isAt == human2_isAt
            ) {

        return 1000;
    } else return 0;
}

bool Handover::isGoalState(VariableSet state) {
    string object_isAt = state.set[object_loc_var_];

    return object_isAt == agent2_name_;
}

bool Handover::isStartingState(VariableSet state) {
    string object_isAt = state.set[object_loc_var_];

    return object_isAt == agent1_name_;
}
