/* 
 * File:   TakeObject.cpp
 * Author: mfiore
 * 
 * Created on January 14, 2016, 12:40 PM
 */

#include "TakeObject.h"

TakeObject::TakeObject() {
    agent_loc_var_="agent_isAt";
    object_loc_var_="object_isAt";
    variables.push_back(agent_loc_var_);
    variables.push_back(object_loc_var_);
    
    std::vector<string> locations {"table","bed","counter","shelf"};
    agent_name_ = "agent";
    object_name_="object";
  
    std::map<string,std::vector<string> > var_values;
    var_values[agent_loc_var_] = locations;
    var_values[object_loc_var_] = locations;
    var_values[object_loc_var_].push_back(agent_name_);

    this->varValues = var_values;
    
    std::vector<string> actions;
    for (string l:locations) {
        actions.push_back(agent_name_+"_move_"+l);
    }
    actions.push_back(agent_name_+"_take_"+object_name_);
//    actions.push_back(agent_name_+"_wait");
    
    this->actions = actions;
    parameters.push_back(object_name_);
    vector<string> par_var;
    par_var.push_back(object_loc_var_);
    parameter_variables[object_name_] = par_var;
    variable_parameter[par_var[0]] = object_name_;
    
    parameters.push_back(agent_name_);
    par_var.clear();
    par_var.push_back(agent_loc_var_);
    parameter_variables[agent_name_]=par_var;
    variable_parameter[par_var[0]]=agent_name_;
}

TakeObject::TakeObject(const TakeObject& orig) {
}

TakeObject::~TakeObject() {
}

void TakeObject::assignParametersFromActionName(string action_name) {
    vector<string> action_parameters=StringOperations::stringSplit(action_name,'_');
    std::map<string,string> instance;
 
    instance[agent_name_]=action_parameters[0];
    instance[object_name_]=action_parameters[2]; 
    assignParameters(instance);
}

string TakeObject::getDeparametrizedAction(string action_name) {
     vector<string> action_parameters=StringOperations::stringSplit(action_name,'_');
     string parametrized_action=parameter_instances[agent_name_]+"_"+action_parameters[1];
     if (action_parameters[1]=="take") {
        parametrized_action=parametrized_action+"_"+parameter_instances[object_name_];
     }
     return parametrized_action;
}

VarStateProb TakeObject::transitionFunction(VariableSet state, string action) {
    string human_isAt = state.set[agent_loc_var_];
    string object_isAt = state.set[object_loc_var_];
    

    vector<string> action_parameters=MdpBasicActions::getActionParameters(action);
    VarStateProb future_beliefs;
    string action_name=action_parameters[1];

    if (action_name=="wait") {
        future_beliefs[state]=1;
    }
    else if (action_name=="take") {
        future_beliefs=MdpBasicActions::applyTake(human_isAt,object_isAt,agent_name_,object_loc_var_,state);
    }
    else if (action_name=="move") {
        future_beliefs=MdpBasicActions::applyMove(agent_loc_var_,action_parameters[2],state);
    }

    return future_beliefs;
}

int TakeObject::rewardFunction(VariableSet state, string action) {
    string human_isAt = state.set[agent_loc_var_];
    string object_isAt = state.set[object_loc_var_];

    if (human_isAt == object_isAt && action == "agent_take_object") return 100;
    else return 0;
}

bool TakeObject::isGoalState(VariableSet state) {
    string human_isAt = state.set[agent_loc_var_];
    string object_isAt = state.set[object_loc_var_];

    return object_isAt == agent_name_;
}

bool TakeObject::isStartingState(VariableSet state) {
    string human_isAt = state.set[agent_loc_var_];
    string object_isAt = state.set[object_loc_var_];

    return object_isAt != agent_name_;
}
