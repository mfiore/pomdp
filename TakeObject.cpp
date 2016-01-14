/* 
 * File:   TakeObject.cpp
 * Author: mfiore
 * 
 * Created on January 14, 2016, 12:40 PM
 */

#include "TakeObject.h"

TakeObject::TakeObject() {
    std::vector<string> locations{"table", "counter", "shelf", "bed", "washing_machine"};
    std::vector<string> extendedLocations = locations;
    extendedLocations.push_back("human");
    std::map<string, std::vector < string>> variables;

    variables["human_isAt"] = locations;
    variables["object_isAt"] = extendedLocations;

    this->varValues = variables;
    std::vector<string> actions{"human_move_bed", "human_move_washing_machine", "human_move_box_table", "human_move_box_counter", 
            "human_move_box_shelf", "human_take_object"};

    this->actions = actions;
    this->variables = {"human_isAt", "object_isAt"};
    parameters.push_back("object");
    vector<string> par_var;
    par_var.push_back(("object_isAt"));
    parameter_variables["object"] = par_var;
}

TakeObject::TakeObject(const TakeObject& orig) {
}

TakeObject::~TakeObject() {
}

std::map<VariableSet, double> TakeObject::transitionFunction(VariableSet state, string action) {
    string human_isAt = state.set["human_isAt"];
    string object_isAt = state.set["object_isAt"];

    string future_human_isAt;
    if (action == "human_move_table") {
        future_human_isAt = "table";
    } else if (action == "human_move_counter") {
        future_human_isAt = "counter";
    } else if (action == "human_move_shelf") {
        future_human_isAt = "shelf";
    } else if (action == "human_move_bed") {
        future_human_isAt = "bed";
    } else if (action == "human_move_washing_machine") {
        future_human_isAt = "washing_machine";
    } else {
        future_human_isAt = human_isAt;
    }

    string future_object_isAt = object_isAt;
    if (action == "human_take_object" && object_isAt == human_isAt) {
        object_isAt = "human";
    }

    VariableSet vs1;
    std::map<string, string> newState;
    newState["human_isAt"] = future_human_isAt;
    newState["object_isAt"] = future_object_isAt;

    vs1.set = newState;
    std::map<VariableSet, double> futureBeliefs;
    futureBeliefs[vs1] = 1;
    return futureBeliefs;
}

int TakeObject::rewardFunction(VariableSet state, string action) {
    string human_isAt = state.set["human_isAt"];
    string object_isAt = state.set["object_isAt"];

    if (human_isAt == object_isAt && action == "human_take_object") return 100;
    else return 0;
}

bool TakeObject::isGoalState(VariableSet state) {
    string human_isAt = state.set["human_isAt"];
    string object_isAt = state.set["object_isAt"];

    return object_isAt == "human";
}

bool TakeObject::isStartingState(VariableSet state) {
    string human_isAt = state.set["human_isAt"];
    string object_isAt = state.set["object_isAt"];

    return object_isAt != "human";
}
