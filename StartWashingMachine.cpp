/* 
 * File:   GetLaundry.cpp
 * Author: mfiore
 * 
 * Created on December 22, 2015, 12:18 PM
 */

#include <c++/4.6/bits/algorithmfwd.h>

#include "StartWashingMachine.h"

StartWashingMachine::StartWashingMachine() {
    std::vector<string> locations{"table", "counter", "shelf", "bed", "washing_machine"};
    std::vector<string> laundry_locations = {"box", "washing_machine"};
    std::vector<string> extendedLocations = locations;
    extendedLocations.push_back("human");
    std::vector<string> extended_plus_box = extendedLocations;
    extended_plus_box.push_back("box");
    std::map<string, std::vector < string>> variables;
    variables["human_isAt"] = locations;
    variables["laundry1_isAt"] = laundry_locations;
    variables["laundry2_isAt"] = laundry_locations;
    variables["laundry3_isAt"] = laundry_locations;
    variables["box_isAt"] = locations;
    variables["washing_machine_state"] = {"stopped", "running"};

    this->varValues = variables;
    std::vector<string> actions{"human_move_box_washing_machine", "human_fill_washing_machine", "human_use_washing_machine",
        "human_move_table", "human_move_counter", "human_move_shelf", "human_move_bed", "human_move_washing_machine"};

    this->actions = actions;
    this->variables = {"human_isAt", "laundry1_isAt", "laundry2_isAt", "laundry3_isAt", "box_isAt",
        "washing_machine_state"};


}

StartWashingMachine::StartWashingMachine(const StartWashingMachine& orig) {
}

StartWashingMachine::~StartWashingMachine() {
}

string StartWashingMachine::laundryHelper(string laundry_isAt, string human_isAt, string box_isAt, string action, string laundry_name) {
    if (laundry_isAt == "box" && human_isAt == box_isAt && box_isAt == "washing_machine" && action == "human_fill_washing_machine") {
        return "washing_machine";
    } else return laundry_isAt;
}

std::map<VariableSet, double> StartWashingMachine::transitionFunction(VariableSet state, string action) {
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

    string future_laundry1_isAt = laundryHelper(laundry1_isAt, human_isAt, box_isAt, action, "laundry1");
    string future_laundry2_isAt = laundryHelper(laundry2_isAt, human_isAt, box_isAt, action, "laundry2");
    string future_laundry3_isAt = laundryHelper(laundry3_isAt, human_isAt, box_isAt, action, "laundry3");


    string future_box_isAt;
    if (human_isAt == box_isAt && action == "human_move_box_washing_machine") {
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

int StartWashingMachine::rewardFunction(VariableSet state, string action) {
    string human_isAt = state.set["human_isAt"];
    string laundry1_isAt = state.set["laundry1_isAt"];
    string laundry2_isAt = state.set["laundry2_isAt"];
    string laundry3_isAt = state.set["laundry3_isAt"];
    string box_isAt = state.set["box_isAt"];
    string washing_machine_state = state.set["washing_machine_state"];


    if (human_isAt == box_isAt) {
        if (laundry1_isAt == "washing_machine" &&
                laundry2_isAt == "washing_machine" &&
                laundry3_isAt == "washing_machine" &&
                action == "human_use_washing_machine" &&
                human_isAt == "washing_machine" &&
                box_isAt == "washing_machine"
                && washing_machine_state == "stopped") {
            return 100;
        }
    }
    return 0;
}

bool StartWashingMachine::isGoalState(VariableSet state) {
    string washing_machine_state = state.set["washing_machine_state"];
    string human_isAt = state.set["human_isAt"];
    string laundry1_isAt = state.set["laundry1_isAt"];
    string laundry2_isAt = state.set["laundry2_isAt"];
    string laundry3_isAt = state.set["laundry3_isAt"];
    string box_isAt = state.set["box_isAt"];

        return washing_machine_state=="running" && human_isAt=="washing_machine" && laundry1_isAt=="washing_machine" &&
                laundry2_isAt=="washing_machine" && laundry3_isAt=="washing_machine" && box_isAt=="washing_machine";
//    return false;

}

bool StartWashingMachine::isStartingState(VariableSet state) {
    string human_isAt = state.set["human_isAt"];
    string laundry1_isAt = state.set["laundry1_isAt"];
    string laundry2_isAt = state.set["laundry2_isAt"];
    string laundry3_isAt = state.set["laundry3_isAt"];
    string box_isAt = state.set["box_isAt"];

        return laundry1_isAt=="box" && laundry2_isAt=="box" && laundry3_isAt=="box";
//    return true;
}
