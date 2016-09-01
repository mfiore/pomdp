/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ReorderTable.cpp
 * Author: mfiore
 * 
 * Created on August 12, 2016, 2:25 PM
 */

#include "ReorderTable.h"

ReorderTable::ReorderTable() {
    agent_loc_var_ = "agent_isAt";
    bottle_loc_var_ = "bottle_isAt";
    glass_loc_var_ = "glass_isAt";

    variables_.push_back(agent_loc_var_);
    variables_.push_back(bottle_loc_var_);
    variables_.push_back(glass_loc_var_);

    agent_name_ = "agent";
    glass_name_ = "glass";
    bottle_name_ = "bottle";

    place_location_ = "kitchen";

    std::vector<string> locations{"table", "counter", "kitchen"};

    std::map<string, std::vector<string> > var_values;
    var_values[agent_loc_var_] = locations;
    var_values[bottle_loc_var_] = locations;
    var_values[bottle_loc_var_].push_back(agent_name_);
    var_values[glass_loc_var_] = locations;
    var_values[glass_loc_var_].push_back(agent_name_);
    this->var_values_ = var_values;

    std::vector<string> actions;
    for (string l : locations) {
        actions.push_back(agent_name_ + "_move_" + l);
        actions.push_back(agent_name_ + "_place_" + glass_name_ + "_" + l);
        actions.push_back(agent_name_ + "_place_" + bottle_name_ + "_" + l);
    }
    actions.push_back(agent_name_ + "_take_" + glass_name_);
    actions.push_back(agent_name_ + "_take_" + bottle_name_);


    this->actions_ = actions;

    name_ = "agent_reorder_table";

}

ReorderTable::ReorderTable(const ReorderTable& orig) {
}

ReorderTable::~ReorderTable() {
}

VarStateProb ReorderTable::transitionFunction(VariableSet state, string action) {
    string agent_isAt = state.set[agent_loc_var_];
    string glass_isAt = state.set[glass_loc_var_];
    string bottle_isAt = state.set[bottle_loc_var_];


    vector<string> action_parameters = MdpBasicActions::getActionParameters(action);
    VarStateProb future_beliefs;
    string action_name = action_parameters[1];

    if (action_name == "take" && action_parameters[2] == glass_name_) {
        future_beliefs = MdpBasicActions::applyTake(agent_isAt, glass_isAt, agent_name_, glass_loc_var_, state);
    } else if (action_name == "take" && action_parameters[2] == bottle_name_) {
        future_beliefs = MdpBasicActions::applyTake(agent_isAt, bottle_isAt, agent_name_, bottle_loc_var_, state);
    } else if (action_name == "move") {
        future_beliefs = MdpBasicActions::applyMove(agent_loc_var_, action_parameters[2], state);
    } else if (action_name == "place" && action_parameters[2] == bottle_name_) {
        future_beliefs = MdpBasicActions::applyPlace(bottle_loc_var_, bottle_isAt, agent_isAt, action_parameters[3],
                agent_name_, state);
    } else if (action_name == "place" && action_parameters[2] == glass_name_) {
        future_beliefs = MdpBasicActions::applyPlace(glass_loc_var_, glass_isAt, agent_isAt, action_parameters[3],
                agent_name_, state);
    }
    

    
    return future_beliefs;
}

int ReorderTable::rewardFunction(VariableSet state, string action) {
    string agent_isAt = state.set[agent_loc_var_];
    string glass_isAt = state.set[glass_loc_var_];
    string bottle_isAt = state.set[bottle_loc_var_];

    if (glass_isAt == place_location_ && bottle_isAt == place_location_) {
        return 1000;
    } else return 0;
}

bool ReorderTable::isGoalState(VariableSet state) {
    string glass_isAt = state.set[glass_loc_var_];
    string bottle_isAt = state.set[bottle_loc_var_];

    return glass_isAt == place_location_ && bottle_isAt == place_location_;
}

bool ReorderTable::isStartingState(VariableSet state) {
    string glass_isAt = state.set[glass_loc_var_];
    string bottle_isAt = state.set[bottle_loc_var_];
    return glass_isAt != place_location_ || bottle_isAt != place_location_;
}
