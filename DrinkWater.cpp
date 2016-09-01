/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DrinkWater.cpp
 * Author: mfiore
 * 
 * Created on August 12, 2016, 11:22 AM
 */

#include "DrinkWater.h"

DrinkWater::DrinkWater() {
    agent_loc_var_ = "agent_isAt";
    bottle_loc_var_ = "bottle_isAt";
    glass_loc_var_ = "glass_isAt";
    glass_capacity_var_ = "glass_capacity";
    bottle_capacity_var_ = "bottle_capacity";
    has_drunk_var_ = "has_drunk";

    variables_.push_back(agent_loc_var_);
    variables_.push_back(bottle_loc_var_);
    variables_.push_back(glass_loc_var_);
    variables_.push_back(glass_capacity_var_);
    variables_.push_back(bottle_capacity_var_);
    variables_.push_back(has_drunk_var_);

    agent_name_ = "agent";
    glass_name_ = "glass";
    bottle_name_ = "bottle";

    std::vector<string> locations{"table", "counter", "kitchen"};

    std::map<string, std::vector<string> > var_values;
    var_values[agent_loc_var_] = locations;
    var_values[bottle_loc_var_] = locations;
    var_values[bottle_loc_var_].push_back(agent_name_);
    var_values[glass_loc_var_] = locations;
    var_values[glass_loc_var_].push_back(agent_name_);
    var_values[bottle_capacity_var_] = {"0", "1"};
    var_values[glass_capacity_var_] = {"0", "1"};
    var_values[has_drunk_var_] = {"0", "1"};

    this->var_values_ = var_values;

    std::vector<string> actions;
    for (string l : locations) {
        actions.push_back(agent_name_ + "_move_" + l);
    }
    actions.push_back(agent_name_ + "_take_" + glass_name_);
    actions.push_back(agent_name_ + "_take_" + bottle_name_);
    actions.push_back(agent_name_ + "_fill_" + glass_name_ + "_" + bottle_name_);
    actions.push_back(agent_name_ + "_drink_" + glass_name_);

    this->actions_ = actions;

    name_ = "agent_drink_water";

}

DrinkWater::DrinkWater(const DrinkWater& orig) {
}

DrinkWater::~DrinkWater() {
}

VarStateProb DrinkWater::transitionFunction(VariableSet state, string action) {
    string agent_isAt = state.set[agent_loc_var_];
    string glass_isAt = state.set[glass_loc_var_];
    string bottle_isAt = state.set[bottle_loc_var_];
    string glass_capacity = state.set[glass_capacity_var_];
    string bottle_capacity = state.set[bottle_capacity_var_];
    string has_drunk = state.set[has_drunk_var_];

    vector<string> action_parameters = MdpBasicActions::getActionParameters(action);
    VarStateProb future_beliefs;
    string action_name = action_parameters[1];

    if (action_name == "take" && action_parameters[2] == glass_name_) {
        future_beliefs = MdpBasicActions::applyTake(agent_isAt, glass_isAt, agent_name_, glass_loc_var_, state);
    } else if (action_name == "take" && action_parameters[2] == bottle_name_) {
        future_beliefs = MdpBasicActions::applyTake(agent_isAt, bottle_isAt, agent_name_, bottle_loc_var_, state);
    } else if (action_name == "move") {
        future_beliefs = MdpBasicActions::applyMove(agent_loc_var_, action_parameters[2], state);
    } else if (action_name == "fill" && bottle_isAt == agent_name_ && glass_isAt == agent_name_ && bottle_capacity == "1") {
        VariableSet new_s = state;
        new_s.set[glass_capacity_var_] = "1";
        future_beliefs[new_s] = 1;
    } else if (action_name == "drink" && glass_isAt == agent_name_ && glass_capacity == "1") {
        VariableSet new_s = state;
        new_s.set[glass_capacity_var_] = "0";
        new_s.set[has_drunk_var_] = "1";
        future_beliefs[new_s] = 1;
    }

    return future_beliefs;
}

int DrinkWater::rewardFunction(VariableSet state, string action) {
    string agent_isAt = state.set[agent_loc_var_];
    string glass_isAt = state.set[glass_loc_var_];
    string bottle_isAt = state.set[bottle_loc_var_];
    string glass_capacity = state.set[glass_capacity_var_];
    string bottle_capacity = state.set[bottle_capacity_var_];

    if (glass_isAt == agent_name_ && glass_capacity == "1" && action == agent_name_ + "_drink_" + glass_name_) {
        return 100;
    } else return 0;
}

bool DrinkWater::isGoalState(VariableSet state) {
    string has_drunk = state.set[has_drunk_var_];

    return has_drunk=="1";
}

bool DrinkWater::isStartingState(VariableSet state) {
    string has_drunk = state.set[has_drunk_var_];

    return has_drunk=="0";
}
