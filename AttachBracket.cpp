/* 
 * File:   AttachBracket.cpp
 * Author: mfiore
 * 
 * Created on May 25, 2016, 2:06 PM
 */

#include "AttachBracket.h"

AttachBracket::AttachBracket() {
    agent_name_ = "agent";
    surface_name_ = "surface";
    bracket_name_ = "bracket";

    std::vector<string> locations{"other_location", "surface"};
    std::vector<string> statuses{"other_status", "glued", "completed"};

    agent_loc_var_ = agent_name_ + "_isAt";
    surface_status_var_ = surface_name_ + "_status";
    bracket_loc_var_ = bracket_name_ + "_isAt";

    variables_.push_back(agent_loc_var_);
    variables_.push_back(surface_status_var_);
    variables_.push_back(bracket_loc_var_);

    std::map<string, std::vector < string>> var_values;
    var_values[agent_loc_var_] = locations;
    var_values[surface_status_var_] = statuses;
    var_values[bracket_loc_var_] = locations;
    var_values[bracket_loc_var_].push_back(agent_name_);
    var_values[bracket_loc_var_].push_back("table");
    var_values[bracket_loc_var_].push_back("other_agent");

    this->var_values_ = var_values;

    abstract_states_[surface_status_var_]["none"] = "other_status";
    abstract_states_[surface_status_var_]["cleaned"] = "other_status";

    abstract_states_[agent_loc_var_]["surface1"] = "other_location";
    abstract_states_[agent_loc_var_]["surface2"] = "other_location";
    abstract_states_[agent_loc_var_]["surface3"] = "other_location";
    abstract_states_[agent_loc_var_]["table"] = "other_location";

    abstract_states_[bracket_loc_var_]["surface1"] = "other_location";
    abstract_states_[bracket_loc_var_]["surface2"] = "other_location";
    abstract_states_[bracket_loc_var_]["surface3"] = "other_location";
    abstract_states_[bracket_loc_var_]["agent1"] = "other_agent";
    abstract_states_[bracket_loc_var_]["agent2"] = "other_agent";


    actions_.push_back(agent_name_ + "_move_" + surface_name_);
    actions_.push_back(agent_name_ + "_apply_" + bracket_name_ + "_" + surface_name_);
    actions_.push_back(agent_name_ + "_get_" + bracket_name_);
    hierarchy_map_[agent_name_ + "_get_" + bracket_name_] = new GetObject();


    this->actions_ = actions_;

    parameters_.push_back(agent_name_);
    parameters_.push_back(surface_name_);
    parameters_.push_back(bracket_name_);

    parameter_action_place_[0] = agent_name_;
    parameter_action_place_[2] = bracket_name_;
    parameter_action_place_[3] = surface_name_;

    vector<string> par_var;
    par_var.push_back(agent_loc_var_);
    parameter_variables_[agent_name_] = par_var;
    variable_parameter_[par_var[0]] = agent_name_;


    par_var.clear();
    par_var.push_back(surface_status_var_);
    parameter_variables_[surface_name_] = par_var;
    variable_parameter_[par_var[0]] = surface_name_;

    par_var.clear();
    par_var.push_back(bracket_loc_var_);
    parameter_variables_[bracket_name_] = par_var;
    variable_parameter_[par_var[0]] = bracket_name_;

    name_ = "agent_apply_bracket_surface";


}

AttachBracket::AttachBracket(const AttachBracket& orig) {

}

AttachBracket::~AttachBracket() {
//    delete hierarchy_map_[agent_name_ + "_get_" + bracket_name_];

}

//void AttachBracket::assignParametersFromActionName(string action_name) {
//    vector<string> action_parts = StringOperations::stringSplit(action_name, '_');
//    std::map<string, string> instance;
//    if (action_parts.size() > 0) {
//        instance["agent"] = action_parts[0];
//    }
//    if (action_parts.size() > 2) {
//        instance["bracket"] = action_parts[2];
//    }
//    if (action_parts.size() > 3) {
//        instance["surface"] = action_parts[3];
//    }
//    assignParameters(instance);
//}

std::map<VariableSet, double> AttachBracket::transitionFunction(VariableSet state, string action) {
    VarStateProb future_beliefs;

    vector<string> action_parameters = MdpBasicActions::getActionParameters(action);
    string action_name = action_parameters[1];

    if (action_name == "wait") {
        future_beliefs[state] = 1;
    } else if (action_name == "apply"
            && state.set[surface_status_var_] == "glued"
            && state.set[agent_loc_var_] == surface_name_
            && state.set[bracket_loc_var_] == agent_name_) {
        state.set[surface_status_var_] = "completed";
        state.set[bracket_loc_var_] = surface_name_;
        future_beliefs[state] = 1;
    } else if (action_name == "move") {
        future_beliefs = MdpBasicActions::applyMove(agent_loc_var_, action_parameters[2], state);
    } else {
        future_beliefs[state] = 1;
    }
    return future_beliefs;
}

bool AttachBracket::isStartingState(VariableSet state) {
//    if (state.set[bracket_loc_var_] == "table") return true;
    return (state.set[surface_status_var_]!="completed");
//    return false;

}

int AttachBracket::rewardFunction(VariableSet state, string action) {
    if (state.set[surface_status_var_] == "glued"
            && state.set[agent_loc_var_ ] == surface_name_
            && action == agent_name_ + "_apply_" + surface_name_
            && state.set[bracket_loc_var_] == agent_name_) {
        return 100;
    } else return 0;

}

bool AttachBracket::isGoalState(VariableSet state) {
    if (state.set[surface_status_var_] == "completed") return true;
    return false;

}
