/* 
 * File:   AlphaVector.h
 * Author: mfiore
 *
 * Created on October 15, 2014, 3:28 PM
 */

#ifndef ALPHAVECTOR_H
#define	ALPHAVECTOR_H
#include <string>
#include "VariableSet.h"

using namespace std;

class AlphaVector {
public:
    AlphaVector();
    AlphaVector(const AlphaVector& orig);
    virtual ~AlphaVector();
    string action;
    VariableSet observedVars;
    vector<double> coefficients;
private:


};

#endif	/* ALPHAVECTOR_H */

