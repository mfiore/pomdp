/* 
 * File:   WashLaundry.cpp
 * Author: mfiore
 * 
 * Created on December 22, 2015, 9:45 PM
 */

#include "WashLaundry.h"

WashLaundry::WashLaundry() {
    std::vector<string> locations{"table", "counter", "shelf", "bed", "washing_machine", "box"};
    std::vector<string> extendedLocations = locations;
    extendedLocations.push_back("human");
    std::map<string, std::vector < string>> variables;
    variables["human_isAt"] = locations;
    variables["laundry1_isAt"] = extendedLocations;
    variables["laundry2_isAt"] = extendedLocations;
    variables["laundry3_isAt"] = extendedLocations;
    variables["box_isAt"] = locations;
    variables["washing_machine_state"] = {"stopped", "running"};


    this->varValues = variables;
    std::vector<string> actions{"get_laundry", "start_washing_machine"};

    this->actions = actions;
    this->variables = {"human_isAt", "laundry1_isAt", "laundry2_isAt", "laundry3_isAt", "box_isAt",
        "washing_machine_state"};
}

WashLaundry::WashLaundry(const WashLaundry& orig) {
}

WashLaundry::~WashLaundry() {
}

bool WashLaundry::isStartingState(VariableSet state) {
    return true;
}

std::map<VariableSet, double> WashLaundry::transitionFunction(VariableSet state, string action) {
    return std::map<VariableSet, double>();
}

int WashLaundry::rewardFunction(VariableSet state, string action) {
    string human_isAt = state.set["human_isAt"];
    string laundry1_isAt = state.set["laundry1_isAt"];
    string laundry2_isAt = state.set["laundry2_isAt"];
    string laundry3_isAt = state.set["laundry3_isAt"];
    string box_isAt = state.set["box_isAt"];
    string washing_machine_state = state.set["washing_machine_state"];

    if (human_isAt == box_isAt) {
        if (action=="start_washing_machine" && laundry1_isAt == "washing_machine" && laundry2_isAt == "washing_machine" && laundry3_isAt == "washing_machine" &&
                washing_machine_state == "stopped") {
            return 100;
        }
    }
    return 0;
}

bool WashLaundry::isGoalState(VariableSet state) {
    string washing_machine_state = state.set["washing_machine_state"];

    return washing_machine_state == "running";

}
