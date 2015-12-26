/* 
 * File:   WashLaundryFull.cpp
 * Author: mfiore
 * 
 * Created on December 25, 2015, 10:01 PM
 */

#include "WashLaundryFull.h"

WashLaundryFull::WashLaundryFull() {
    std::vector<string> locations{"table", "counter", "shelf", "bed", "washing_machine"};
    std::vector<string> extendedLocations = locations;
    extendedLocations.push_back("human");
    std::vector<string> extended_plus_box = extendedLocations;
    extended_plus_box.push_back("box");
    std::map<string, std::vector < string>> variables;
    variables["human_isAt"] = locations;
    variables["laundry1_isAt"] = extended_plus_box;
    variables["laundry2_isAt"] = extended_plus_box;
    variables["laundry3_isAt"] = extended_plus_box;
    variables["box_isAt"] = locations;
    variables["washing_machine_state"] = {"stopped", "running"};


    this->varValues = variables;
    std::vector<string> actions{"human_take_laundry1", "human_take_laundry2", "human_take_laundry3", "human_place_laundry1_box",
        "human_place_laundry2_box", "human_place_laundry3_box", "human_move_table", "human_move_counter", "human_move_shelf",
        "human_move_bed", "human_move_washing_machine", "human_move_box_table", "human_move_box_counter", "human_move_box_shelf",
        "human_move_box_bed", "human_move_box_washing_machine", "human_fill_washing_machine", "human_use_washing_machine"};

    this->actions = actions;
    this->observedVariables = {"human_isAt", "laundry1_isAt", "laundry2_isAt", "laundry3_isAt", "box_isAt", "washing_machine_state"};
    this->variables = this->observedVariables;


}

WashLaundryFull::WashLaundryFull(const WashLaundryFull& orig) {
}

WashLaundryFull::~WashLaundryFull() {
}

string laundryHelper(string human_isAt, string laundry_isAt, string box_isAt, string action, string laundry_name) {
    if (laundry_isAt == "box") {
        return laundry_isAt;
    } else if (human_isAt == laundry_isAt && action == "human_take_" + laundry_name) {
        return "human";
    } else if (human_isAt == box_isAt && laundry_isAt == "human" && action == "human_place_" + laundry_name + "_box") {
        return "box";
    } else if (laundry_isAt == "box" && human_isAt == box_isAt && box_isAt == "washing_machine" && action == "human_fill_washing_machine") {
        return "washing_machine";
    } else return laundry_isAt;
}

std::map<VariableSet, double> WashLaundryFull::transitionFunction(VariableSet state, string action) {
    string human_isAt = state.set["human_isAt"];
    string laundry1_isAt = state.set["laundry1_isAt"];
    string laundry2_isAt = state.set["laundry2_isAt"];
    string laundry3_isAt = state.set["laundry3_isAt"];
    string box_isAt = state.set["box_isAt"];
    string washing_machine_state = state.set["washing_machine_state"];


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

    string future_laundry1_isAt = laundryHelper(human_isAt, laundry1_isAt, box_isAt, action, "laundry1");
    string future_laundry2_isAt = laundryHelper(human_isAt, laundry2_isAt, box_isAt, action, "laundry2");
    string future_laundry3_isAt = laundryHelper(human_isAt, laundry3_isAt, box_isAt, action, "laundry3");

    string future_box_isAt;
    if (human_isAt == box_isAt && action == "human_move_box_table") {
        future_box_isAt = "table";
        future_human_isAt = "table";
    } else if (human_isAt == box_isAt && action == "human_move_box_counter") {
        future_box_isAt = "counter";
        future_human_isAt = "counter";
    } else if (human_isAt == box_isAt && action == "human_move_box_shelf") {
        future_box_isAt = "shelf";
        future_human_isAt = "shelf";
    } else if (human_isAt == box_isAt && action == "human_move_box_bed") {
        future_box_isAt = "bed";
        future_human_isAt = "bed";
    } else if (human_isAt == box_isAt && action == "human_move_box_washing_machine") {
        future_box_isAt = "washing_machine";
        future_human_isAt = "washing_machine";
    } else {
        future_box_isAt = box_isAt;
    }

    string future_washing_machine_state;
    if (human_isAt == "washing_machine" && action == "human_use_washing_machine") {
        future_washing_machine_state = "running";
    } else {
        future_washing_machine_state = washing_machine_state;
    }


    VariableSet vs1;
    std::map<string, string> newState;
    newState["human_isAt"] = future_human_isAt;
    newState["laundry1_isAt"] = future_laundry1_isAt;
    newState["laundry2_isAt"] = future_laundry2_isAt;
    newState["laundry3_isAt"] = future_laundry3_isAt;
    newState["box_isAt"] = future_box_isAt;
    newState["washing_machine_state"] = future_washing_machine_state;

    vs1.set = newState;
    std::map<VariableSet, double> futureBeliefs;
    futureBeliefs[vs1] = 1;
    return futureBeliefs;
}

int WashLaundryFull::rewardFunction(VariableSet state, string action) {
    string human_isAt = state.set["human_isAt"];
    string laundry1_isAt = state.set["laundry1_isAt"];
    string laundry2_isAt = state.set["laundry2_isAt"];
    string laundry3_isAt = state.set["laundry3_isAt"];
    string box_isAt = state.set["box_isAt"];

    if (human_isAt == box_isAt) {
        if (laundry1_isAt == "washing_machine" && laundry2_isAt == "washing_machine" && laundry3_isAt == "washing_machine" &&
                action == "human_use_washing_machine") return 100;
    }
    return 0;
}

bool WashLaundryFull::isGoalState(VariableSet state) {
    string human_isAt = state.set["human_isAt"];
    string laundry1_isAt = state.set["laundry1_isAt"];
    string laundry2_isAt = state.set["laundry2_isAt"];
    string laundry3_isAt = state.set["laundry3_isAt"];
    string box_isAt = state.set["box_isAt"];

    return laundry1_isAt == "box" && laundry2_isAt == "box" && laundry3_isAt == "box";
}

bool WashLaundryFull::isStartingState(VariableSet state) {
    string human_isAt = state.set["human_isAt"];
    string laundry1_isAt = state.set["laundry1_isAt"];
    string laundry2_isAt = state.set["laundry2_isAt"];
    string laundry3_isAt = state.set["laundry3_isAt"];
    string box_isAt = state.set["box_isAt"];

    return laundry1_isAt != "box" || laundry2_isAt != "box" || laundry3_isAt != "box";
}
