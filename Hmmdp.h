/* 
 * File:   Hmmdp.h
 * Author: mfiore
 *
 * Created on December 14, 2015, 4:49 PM
 */

#ifndef HMMDP_H
#define	HMMDP_H

#include "Pomdp.h"
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

class Hmmdp : public Pomdp {
    typedef map<string, Hmmdp*> HmmdpMap;

public:
    Hmmdp();
    Hmmdp(const Hmmdp& orig);
    virtual ~Hmmdp();
    int bellmanBackup(int i, std::vector<double> vhi);
    void solveHierarchical(bool first,bool rewrite);
    double getHierarcicReward(int i);
    double getHierarcicReward(VariableSet state);

    map<int, double> getHierarcicTransition(int i);
    map<VariableSet, double> getHierarcicTransition(VariableSet state);

    void createPomdp(string name, bool rewrite, bool first);
    virtual bool isStartingState(VariableSet state)=0;
    
    string chooseHierarchicAction(VariableSet state);
    string chooseHierarchicAction(map<int,double> a_belief);
    
    int convertHierarchicState(VariableSet state);

//    void simulate(int n);
    HmmdpMap hierarchy_map_;

    map<int,double> getMatchingStates(map<VariableSet,double> states);

private:
    void readHierarchical();
    void writeHierarchical();

    
    vector<double> gauss(vector< vector<double> > A);
    void calculateHierarcicReward();
    void calculateHierarcicTransition();

    std::map<pair<int, int>, double> hierarchic_transition_;
    std::map<int, double> hierarchic_reward_;
    
    string active_module;
protected:
    vector<int> starting_states_;
    vector<int> goal_states_;


};

#endif	/* HMMDP_H */

