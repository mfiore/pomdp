/* 
 * File:   ReorderTable.h
 * Author: mfiore
 *
 * Created on March 1, 2016, 5:10 PM
 */

#ifndef REORDERTABLE_H
#define	REORDERTABLE_H

#include "Hmdp.h"
#include <string>
#include "TakeObject.h"
#include "PlaceObject.h"
#include <map>
#include <boost/lexical_cast.hpp>

using namespace std;

class ReorderTable: public Hmdp {
public:
    ReorderTable();
    ReorderTable(const ReorderTable& orig);
    virtual ~ReorderTable();
    void setParameters(string action_name);

    string parametrizeAction(string action_name);

    std::map<VariableSet, double> transitionFunction(VariableSet state, string action);
    bool isStartingState(VariableSet state);
    int rewardFunction(VariableSet state, string action);
    bool isGoalState(VariableSet state);
    
private:
    string agent_loc_var_;
    vector<string> dish_loc_var_;
    string agent_name_;
    string goal_location_;
    int n_dishes_;
    string dish_name_;
};


#endif	/* REORDERTABLE_H */

