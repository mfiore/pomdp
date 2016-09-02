/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ConcreteMdp.h
 * Author: theworld
 *
 * Created on September 2, 2016, 4:48 PM
 */
#include "Mdp.h"

#ifndef CONCRETEMDP_H
#define CONCRETEMDP_H

class ConcreteMdp : public Mdp {
public:
    ConcreteMdp();
    ConcreteMdp(const ConcreteMdp& orig);
    virtual ~ConcreteMdp();

    void create(string name, bool rewrite);


    void writeModel(string file_name);
    void writePolicy(string file_name);

    //to override. Returns true if the state is starting
    virtual bool isStartingState(VariableSet state) = 0;
    virtual bool isGoalState(VariableSet state) = 0;
    virtual int rewardFunction(VariableSet state, string action) = 0;
    virtual VarStateProb transitionFunction(VariableSet state, string action)=0;


    //learning functions
    virtual int bellmanBackup(int i, std::vector<double> vhi);
    virtual void valueIteration();

    virtual void enumerateFunctions();
    virtual void enumerateGoalAndStartStates();
};

#endif /* CONCRETEMDP_H */

