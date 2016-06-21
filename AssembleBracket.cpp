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

//    std::vector<string> locations{"table", "surface1", "surface2", "surface3","surface"};
    std::vector<string> statuses{"none","cleaned","glued","completed"};
    
//    agent_loc_var_ = agent_name_+"_isAt";
    bracket_loc_var_=bracket_name_+"_isAt";
    glue_loc_var_=glue_name_+"_isAt";
    surface_status_var_=surface_name_+"_status";

//    variables.push_back(agent_loc_var_);
    variables.push_back(bracket_loc_var_);
    variables.push_back(surface_status_var_);
    variables.push_back(glue_loc_var_);
    
    std::map<string, std::vector < string>> var_values;
//    var_values[agent_loc_var_] = locations;
//    var_values[glue_loc_var_] = locations;
    var_values[glue_loc_var_].push_back(agent_name_);
    var_values[glue_loc_var_].push_back("other");
//    var_values[bracket_loc_var_]=locations;
    var_values[bracket_loc_var_].push_back(agent_name_);
    var_values[bracket_loc_var_].push_back("other");
    var_values[bracket_loc_var_].push_back(surface_name_);
    var_values[surface_status_var_]=statuses;

    
    this->varValues = var_values;

    actions.push_back(agent_name_ +"_clean_"+surface_name_);
    actions.push_back(agent_name_ +"_glue_"+surface_name_);
    actions.push_back(agent_name_ +"_apply_"+bracket_name_+"_"+surface_name_);
    hierarchy_map_[agent_name_ +"_clean_"+surface_name_] = new CleanSurface();
    hierarchy_map_[agent_name_ +"_glue_"+surface_name_] = new GlueSurface();
    hierarchy_map_[agent_name_ +"_apply_"+bracket_name_+"_"+surface_name_] = new AttachBracket();

    this->actions = actions;

    parameters.push_back(agent_name_);
    parameters.push_back(bracket_name_);
    parameters.push_back(surface_name_);
    
    parameter_action_place[0]=agent_name_;
    parameter_action_place[2]=bracket_name_;
    parameter_action_place[3]=surface_name_;
    
    vector<string> par_var;
//    par_var.push_back(agent_loc_var_);
//    parameter_variables[agent_name_] = par_var;
//    variable_parameter[par_var[0]] = agent_name_;
    
    par_var.clear();
    par_var.push_back(bracket_loc_var_);
    parameter_variables[bracket_name_]=par_var;
    variable_parameter[par_var[0]]=bracket_name_;
    
    par_var.clear();
    par_var.push_back(surface_status_var_);
    parameter_variables[surface_name_] = par_var;
    variable_parameter[par_var[0]]=surface_name_;
    
    name="agent_assemble_bracket_surface";
}

AssembleBracket::AssembleBracket(const AssembleBracket& orig) {

}

AssembleBracket::~AssembleBracket() {
    delete hierarchy_map_[agent_name_ + "_clean_" + surface_name_];
    delete hierarchy_map_[agent_name_ + "_glue_" + surface_name_];
    delete hierarchy_map_[agent_name_ + "_apply_" + bracket_name_ + "_" + surface_name_];

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

    if (action==agent_name_+"_wait") {
        future_beliefs[state]=1;
    }
    return future_beliefs;

}

bool AssembleBracket::isStartingState(VariableSet state) {
    return state.set[surface_status_var_]!="completed";

}

int AssembleBracket::rewardFunction(VariableSet state, string action) {
    if (state.set[surface_status_var_]=="glued" && action==agent_name_+"_apply_"+bracket_name_+"_"+surface_name_) {
        return 100;
    }
    else return 0;
    
}


bool AssembleBracket::isGoalState(VariableSet state) {
    if (state.set[surface_status_var_]=="completed") return true;
    return false;

}
