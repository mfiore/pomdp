/* 
 * File:   VariableSet.cpp
 * Author: mfiore
 * 
 * Created on August 19, 2014, 1:04 PM
 */

#include "VariableSet.h"

VariableSet::VariableSet(std::map<string, string> set) {
    set = set;

}

VariableSet::VariableSet() {

}

VariableSet::VariableSet(const VariableSet& orig) {
    set = orig.set;
}

VariableSet::~VariableSet() {
}

