/* 
 * File:   PlaceObject.cpp
 * Author: mfiore
 * 
 * Created on January 14, 2016, 3:53 PM
 */

#include "PlaceObject.h"

PlaceObject::PlaceObject() {
    agent_loc_var_ = "agent_isAt";
    object_loc_var_ = "object_isAt";
    variables.push_back(agent_loc_var_);
    variables.push_back(object_loc_var_);

    std::vector<string> locations{"table", "bed", "counter", "shelf"};
    agent_name_ = "agent";
    object_name_ = "object";
    goal_location_ = "support";

    std::map<string, std::vector<string> > var_values;
    var_values[agent_loc_var_] = locations;
    var_values[agent_loc_var_].push_back(goal_location_);
    var_values[object_loc_var_].push_back(agent_name_);
    var_values[object_loc_var_].push_back(goal_location_);

    this->varValues = var_values;

    std::vector<string> actions;
    for (string l : locations) {
        actions.push_back(agent_name_ + "_move_" + l);
    }
    actions.push_back(agent_name_ + "_move_" + goal_location_);
    actions.push_back(agent_name_ + "_place_" + object_name_ + "_" + goal_location_);

    this->actions = actions;
    parameters.push_back(object_name_);
    vector<string> par_var;
    par_var.push_back(object_loc_var_);
    parameter_variables[object_name_] = par_var;

    parameters.push_back(agent_name_);
    par_var.clear();
    par_var.push_back(agent_loc_var_);
    parameter_variables[agent_name_] = par_var;

    parameters.push_back(goal_location_);
}

void PlaceObject::assignParametersFromActionName(string action_name) {
    vector<string> action_parameters = StringOperations::stringSplit(action_name, '_');
    map<string, string> instance;
    instance[agent_name_] = action_parameters[0];
    instance[object_name_] = action_parameters[2];
    instance[goal_location_] = action_parameters[3];
    assignParameters(instance);
}

string PlaceObject::getDeparametrizedAction(string action_name) {
    vector<string> action_parameters = StringOperations::stringSplit(action_name, '_');
    string parametrized_action = parameter_instances[agent_name_] + "_" + action_parameters[1];
    if (action_parameters[1] == "place") {
        parametrized_action=parametrized_action+"_" + parameter_instances[object_name_] + "_" + parameter_instances[goal_location_];
    }
    else if (parameter_instances.find(action_parameters[2])!=parameter_instances.end()){
        parametrized_action=parametrized_action+"_"+parameter_instances[action_parameters[2]];
    }
    return parametrized_action;
}

PlaceObject::PlaceObject(const PlaceObject& orig) {
}

PlaceObject::~PlaceObject() {
}

std::map<VariableSet, double> PlaceObject::transitionFunction(VariableSet state, string action) {
    string human_isAt = state.set[agent_loc_var_];
    string object_isAt = state.set[object_loc_var_];

    vector<string> action_parameters = MdpBasicActions::getActionParameters(action);
    VarStateProb future_beliefs;
    string action_name = action_parameters[1];
    if (action_name == "place") {
        future_beliefs = MdpBasicActions::applyPlace(object_loc_var_, object_isAt, human_isAt, goal_location_,
                agent_name_, state);
    } else if (action_name == "move") {
        future_beliefs = MdpBasicActions::applyMove(agent_loc_var_, action_parameters[2], state);
    }

    return future_beliefs;
}

int PlaceObject::rewardFunction(VariableSet state, string action) {
    string human_isAt = state.set[agent_loc_var_];
    string object_isAt = state.set[object_loc_var_];

    if (human_isAt == goal_location_ && action == "agent_place_object_support") return 100;
    else return 0;
}

bool PlaceObject::isGoalState(VariableSet state) {
    string object_isAt = state.set[object_loc_var_];

    return object_isAt == goal_location_;
}

bool PlaceObject::isStartingState(VariableSet state) {
    string object_isAt = state.set[object_loc_var_];

    return object_isAt == agent_name_;
}
