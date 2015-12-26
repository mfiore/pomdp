/* 
 * File:   TransitionResult.h
 * Author: mfiore
 *
 * Created on August 19, 2014, 12:46 PM
 */

#ifndef TRANSITIONRESULT_H
#define	TRANSITIONRESULT_H

#include <string>
#include <vector>
#include "VariableSet.h"

using namespace std;

class TransitionOutput {
public:
    TransitionOutput();
    TransitionOutput(const TransitionOutput& orig);
    virtual ~TransitionOutput();

    inline bool operator<(const TransitionOutput& b) const {
        return futureStates < b.futureStates && probabilities < b.probabilities;
    }
    inline bool operator==(const TransitionOutput& b) const {
        return futureStates == b.futureStates && probabilities == b.probabilities;
    }
    vector<string> futureStates;
    vector<double> probabilities;
private:

};

#endif	/* TRANSITIONRESULT_H */

