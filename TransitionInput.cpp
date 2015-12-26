/* 
 * File:   TransitionElement.cpp
 * Author: mfiore
 * 
 * Created on August 18, 2014, 11:53 AM
 */

#include "TransitionInput.h"

TransitionInput::TransitionInput() {
}

TransitionInput::TransitionInput(const TransitionInput& orig) {
    this->state = orig.state;
    this->var = orig.var;

    
}

TransitionInput::~TransitionInput() {
}

