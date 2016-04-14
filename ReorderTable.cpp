/* 
 * File:   ReorderTable.cpp
 * Author: mfiore
 * 
 * Created on March 1, 2016, 5:10 PM
 */

#include "ReorderTable.h"

ReorderTable::ReorderTable() {
    agent_loc_var_ = "agent_isAt";
    n_dishes_ = 3;
    for (int i = 0; i<n_dishes_;i++) {
        dish_loc_var_.push_back("dish"+boost::lexical_cast<string>(i)+"_isAt");
    }

    agent_name_ = "agent";
    dish_name_ = "dish";
    goal_location_ = "counter";

    variables.push_back(agent_loc_var_);

    std::vector<string> locations{"table", "counter", "shelf", "bed"};



    std::map<string, std::vector < string>> var_values;
    var_values[agent_loc_var_] = locations;
    for (int i=0;i<n_dishes_;i++) {
        variables.push_back(dish_loc_var_[i]);;
        var_values[dish_loc_var_[i]] = locations;
        var_values[dish_loc_var_[i]].push_back(agent_name_);
    }
    this->varValues = var_values;

    std::vector<string> actions;
    for (int i = 0; i < n_dishes_; i++) {
        actions.push_back(agent_name_ + "_take_" + dish_name_  + boost::lexical_cast<string>(i));
        actions.push_back(agent_name_ + "_place_" + dish_name_ + boost::lexical_cast<string>(i) + "_" + goal_location_);
        hierarchy_map_[agent_name_ + "_take_" + dish_name_  + boost::lexical_cast<string>(i)]=new TakeObject();
        hierarchy_map_[agent_name_ + "_place_" + dish_name_  + boost::lexical_cast<string>(i)+ "_" + goal_location_]=new PlaceObject();
  
    }
//    actions.push_back(agent_name_+"_wait");

    this->actions = actions;

    parameters.push_back(agent_name_);
    vector<string> par_var;
    par_var.push_back(agent_loc_var_);
    parameter_variables[agent_name_] = par_var;
    variable_parameter[par_var[0]] = agent_name_;
}

ReorderTable::ReorderTable(const ReorderTable& orig) {

}

ReorderTable::~ReorderTable() {

}

std::map<VariableSet, double> ReorderTable::transitionFunction(VariableSet state, string action) {
    VarStateProb future_beliefs;

    if (action==agent_name_+"_wait") {
        future_beliefs[state]=1;
    }
    return future_beliefs;

}

bool ReorderTable::isStartingState(VariableSet state) {
    return true;

}

int ReorderTable::rewardFunction(VariableSet state, string action) {
    int n_not_placed=0;
    int i_not_placed=-1;
    for (int i=0;i<n_dishes_;i++) {
        if (state.set[dish_loc_var_[i]] != goal_location_) {
            n_not_placed++;
            i_not_placed=i;
        }
    }
//    if (n_not_placed==1) {
//        cout<<"asd";
//    }
    if (n_not_placed==1 && action==agent_name_+"_place_"+dish_name_+boost::lexical_cast<string>(i_not_placed)+"_"+goal_location_){
        return 1000;
    }
    else return 0;

}

void ReorderTable::assignParametersFromActionName(string action_name) {
    
}

string ReorderTable::getDeparametrizedAction(string action_name) {
    return action_name;
}


bool ReorderTable::isGoalState(VariableSet state) {
    for (int i = 0; i < n_dishes_; i++) {
        if (state.set[dish_loc_var_[i]] != goal_location_) {
            return false;
        }
    }
    return true;

}