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

using namespace std;

class Hmmdp : public Pomdp {
    typedef map<string, Hmmdp*> HmmdpMap;

public:
    Hmmdp();
    Hmmdp(const Hmmdp& orig);
    virtual ~Hmmdp();
    int bellmanBackup(int i, std::vector<double> vhi);
    void solveHierarchical(bool first);
    double getHierarcicReward(int i);
    map<int, double> getHierarcicTransition(int i);
    void createPomdp(string name, bool rewrite, bool first);
    virtual bool isStartingState(VariableSet state)=0;
    HmmdpMap hierarchy_map_;


private:
   
    vector<double> gauss(vector< vector<double> > A);
    void calculateHierarcicReward();
    void calculateHierarcicTransition();

    std::map<pair<int, int>, double> hierarchic_transition_;
    std::map<int, double> hierarchic_reward_;
protected:
    vector<int> starting_states_;
    vector<int> goal_states_;
    vector<string> macro_actions_;


};

#endif	/* HMMDP_H */

