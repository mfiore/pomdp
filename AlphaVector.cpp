/* 
 * File:   AlphaVector.cpp
 * Author: mfiore
 * 
 * Created on October 15, 2014, 3:28 PM
 */

#include "AlphaVector.h"

AlphaVector::AlphaVector() {
}

AlphaVector::AlphaVector(const AlphaVector& orig) {
    this->action=orig.action;
    this->coefficients=orig.coefficients;
    this->observedVars=orig.observedVars;
}

AlphaVector::~AlphaVector() {
}

