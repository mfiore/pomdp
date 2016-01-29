/* 
 * File:   PlaceObject.cpp
 * Author: mfiore
 * 
 * Created on January 14, 2016, 3:53 PM
 */

#include "PlaceObject.h"


PlaceObject::PlaceObject() {
    
}

PlaceObject::PlaceObject(const PlaceObject& orig) {
}

PlaceObject::~PlaceObject() {
}

std::map<VariableSet, double> PlaceObject::transitionFunction(VariableSet state, string action) {
  
}

int PlaceObject::rewardFunction(VariableSet state, string action) {
}

bool PlaceObject::isGoalState(VariableSet state) {
}
bool PlaceObject::isStartingState(VariableSet state) {
}
