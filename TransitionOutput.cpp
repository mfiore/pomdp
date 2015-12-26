/* 
 * File:   TransitionResult.cpp
 * Author: mfiore
 * 
 * Created on August 18, 2014, 11:53 AM
 */

#include "TransitionOutput.h"

TransitionOutput::TransitionOutput() {
}

TransitionOutput::TransitionOutput(const TransitionOutput& orig) {
    this->futureStates=orig.futureStates;
    this->probabilities=orig.probabilities;
}

TransitionOutput::~TransitionOutput() {
}


