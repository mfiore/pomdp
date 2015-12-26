/* 
 * File:   Pomdp.h
 * Author: mfiore
 *
 * Created on August 18, 2014, 11:53 AM
 */

#ifndef POMDP_H
#define	POMDP_H

#include <vector>
#include<map>

#include "VariableSet.h"
#include <fstream>
#include "PriorityQueue.h"

#include "NestedLoop.h"
using namespace std;

class Pomdp {
public:
    Pomdp();
    Pomdp(const Pomdp& orig);
    //    Pomdp(std::vector <PomdpVar> obsStates, std::vector<PomdpVar> hiddenStates, std::vector<string> actions, std::vector<PomdpVar> observations,
    //    std::map<string, std::vector< pair<PomdpVar, double > > > transition, std::map <pair<string,string>, double> observationProb, std::map<string, int> reward, 
    //    double discount, std::map<std::vector<PomdpVar> , double > initialBelief);
    virtual ~Pomdp();

    virtual void createPomdp(string name, bool rewrite=false);
    void setInitialState(std::vector<pair<VariableSet, double>> initialBelief);

    //three functions to override in the derived classes
    virtual std::map<VariableSet, double> transitionFunction(VariableSet state, string action) = 0;
    virtual int rewardFunction(VariableSet state, string action) = 0;
    virtual bool isGoalState(VariableSet state) = 0;

    void updateBelief(string action, std::map<string, string> obsStates, std::map<string, string> observations);
    void filterBelief(std::map<string, string> observedVariables);
    //not implemented yet
    std::map<VariableSet, double> applyObservationFunction(std::map<string, string> observations, string action, std::map<VariableSet, double> updatedBelief);

    std::map<string,double> getNormQValue();

    double getQValue(string action);
    
    //learning functions
    virtual int bellmanBackup(int i, std::vector<double> vhi);
    void valueIteration(bool rewrite=false);
    void prioritizedSweeping(); //for now it's slower on the tested examples. Maybe it's the queue overhead

    string chooseAction();
    string chooseAction(int s);
    string chooseAction(map<int,double> belief);

    
    double getTransitionProb(int s, string a, int s_new);

    double getActionValue(string action);

    //utility functions
    void printBelief();
    void printTransitionFunction();
    void printRewardFunction();
    void printStates();
    void printActualQValues();
    std::vector<double> getV_vector() const;



    //Variables
    std::map<VariableSet, int> mapStateEnum; //the system state enumeration is kept both way (from enumeration number to system state and other way)
    std::vector<VariableSet> vecStateEnum;
    std::map<pair<int, string>, std::map<int, double>> transition;
    std::map<pair<int, string>, std::vector<int>> predecessors; //enumeration of the transition function
    std::map<pair<int, string>, int> reward; //reward function
    std::vector<int> goalStates;


    //podmp specification
    //includes all the variables of the system (observed state variables, hidden state variables, observations and the action var)
    std::map<string, std::vector<string> > varValues;
    std::vector<string> variables; //lists all variables;
    std::vector<string> observedVariables; //lists which variables are observed

    std::map<string, std::vector<string>> observationValues;
    std::vector<string> observationVariables; //lists which variables are observations
    std::vector<string> hiddenVariables; //lists which variables are hidden
    std::vector<string> actions;


    std::map<pair<int, string>, double> qValue; //human action values
    std::vector<double> v_vector; //v_vector mantains the predicted reward in a state following the optimal policy

    std::map<int, double> belief; //std::maps a variable std::mapping into a belief

    string name;

private:


};

#endif	/* POMDP_H */

