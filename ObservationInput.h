/* 
 * File:   ObservationElement.h
 * Author: mfiore
 *
 * Created on August 19, 2014, 12:19 PM
 */

#ifndef OBSERVATIONELEMENT_H
#define	OBSERVATIONELEMENT_H

#include <vector>
#include <string>
#include <map>
#include "VariableSet.h"

using namespace std;

class ObservationInput {
public:
    ObservationInput();
    ObservationInput(const ObservationInput& orig);
    virtual ~ObservationInput();

    map<string,string> observations;
    VariableSet state;
    string action;
    
     
    inline bool operator<(const ObservationInput& b) const {
        return observations < b.observations && state < b.state && action.size()<=b.action.size();
    }
    inline bool operator==(const ObservationInput& b) const {
        return observations == b.observations && state == b.state && action==b.action;
    }
private:
   

};

#endif	/* OBSERVATIONELEMENT_H */

