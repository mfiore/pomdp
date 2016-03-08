/* 
 * File:   Hmdp.h
 * Author: mfiore
 *
 * Created on December 14, 2015, 4:49 PM
 */

#ifndef HMDP_H
#define	HMDP_H

#include "Mdp.h"
#include <map>
#include <cmath>
#include <vector>
#include <map>
#include <Eigen/Dense>
#include<Eigen/SparseLU>
#include <Eigen/SparseCore>
#include <Eigen/SparseCholesky>
#include <Eigen/IterativeLinearSolvers>
#include <string>
#include "StringOperations.h"
#include <unordered_set>

using namespace std;

class Hmdp : public Mdp {
    typedef map<string, Hmdp*> HmdpMap;

public:
    Hmdp();
    Hmdp(const Hmdp& orig);
    virtual ~Hmdp();

    //gets the hierarchic reward from a certain state. The "i" version is not actually needed
    double getHierarchicReward(int i);
    double getHierarchicReward(VariableSet state);
    
    //gets the hierarchic transition from a certain state. As before, the i version is not needed.
    //the version including an action is used for simulation purposes, when we want to know the resulting state of
    //applying an action in a hierarchical mdp setting.
    map<int, double> getHierarchicTransition(int s);
    map<VariableSet, double> getHierarchicTransition(VariableSet s);
    map<VariableSet, double> getHierarchicTransition(VariableSet set, string action);
    
    //converts the current state space to the one of the caller (meaning the mdp at the higher level of hierarchy which called
    //the current mdp). original_set is the higher level original state space (needed because some variables might not exist
    //the current mdp, and we will fill them with the originalvalues). temp_results is the current state space of this mdp
    map<VariableSet,double> convertToHigherState(VariableSet original_set, map<VariableSet, double> temp_result);

    
    //asigns the parameters of an action from it's name (e.g. we call the TakeObject MDP with action human_take_take
    //parameters will be: agent -> human, object -> tape). We provide a basic one which split an action in a vector 
    //separting it's parts with an '_' and considers action as agent_action_object_support (where object and support are optional)
    virtual void assignParametersFromActionName(string action_name);


    //returns the action chosen from a hierarchy. The action will be returned as the parametrized version. To get the original one
    //we need to call getDeparametrizedAction
    string chooseHierarchicAction(VariableSet state);
    string chooseHierarchicAction(int s);
    
    //simulate n steps on the initial state using the policy
    void simulate(int n, VariableSet initial_state);

    //links action names to mdps. Must be set for all parameter instances
    //etcs. human_take_tape -> "TakeObject", human_take_glass -> "TakeObject"
    HmdpMap hierarchy_map_;

    //creates the mdp. name is the assigned name on the mdp. If rewrite is true the mdp will generate a new policy and hmdp file
    //if first=true it won't calculate hierarchical reward and transition, saving some time (it's usable only on the first node
    //of a hierarchical mdp architecture)
    void create(string name, bool rewrite, bool first);

//private:
    //to override. Returns true if the state is starting
    virtual bool isStartingState(VariableSet state) = 0;
    virtual bool isGoalState(VariableSet state) = 0;

    //calculates hierarchic reward and transition
    void calculateHierarchicReward();
    void calculateHierarchicTransition();

    //converts a state (already parametrized! so we need to call the convertToParametrizedState before) to the current state
    //in practice it just eliminates variables that are not needed in the current mdp
    int convertHierarchicState(VariableSet state);

    //loads and write hierarchical mdp files
    void readHierarchical();
    void writeHierarchical();

    //contains the hierarchic transition and reward
    std::map<pair<int, int>, double> hierarchic_transition_;
    std::map<int, double> hierarchic_reward_;
    
    //active module tracks the current operating MDP in the hierachy ("this" if the actual object is active)
    string active_module;

//protected:
    vector<int> starting_states_;
    vector<int> goal_states_;

};

#endif	/* HMDP_H */
