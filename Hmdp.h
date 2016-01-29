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

using namespace std;

class Hmdp : public Mdp {
    typedef map<string, Hmdp*> HmdpMap;

public:
    Hmdp();
    Hmdp(const Hmdp& orig);
    virtual ~Hmdp();

    double getHierarcicReward(int i);
    double getHierarcicReward(VariableSet state);
    map<int, double> getHierarcicTransition(int s);
    map<VariableSet, double> getHierarcicTransition(VariableSet s);
    map<VariableSet, double> getHierarcicTransition(VariableSet set, string action);
    map<VariableSet,double> convertToHigherState(VariableSet original_set, map<VariableSet, double> temp_result);

    virtual void setParameters(string action_name)=0;

    string chooseHierarchicAction(VariableSet state);
    string chooseHierarchicAction(int s);

    void simulate(int n, VariableSet initial_state);

    HmdpMap hierarchy_map_;

    void create(string name, bool rewrite, bool first);

//private:
    virtual bool isStartingState(VariableSet state) = 0;
    virtual bool isGoalState(VariableSet state) = 0;


    //    int bellmanBackup(int i, std::vector<double> vhi);
    void calculateHierarcicReward();
    void calculateHierarcicTransition();

    int convertHierarchicState(VariableSet state);
    map<int, double> getMatchingStates(map<VariableSet, double> states);


    void readHierarchical();
    void writeHierarchical();

    std::map<pair<int, int>, double> hierarchic_transition_;
    std::map<int, double> hierarchic_reward_;
    string active_module;

//protected:
    vector<int> starting_states_;
    vector<int> goal_states_;

};

#endif	/* HMDP_H */

