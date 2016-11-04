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

    //create the mdp. If rewrite is false it won't overwrite an existing file, but read the model
    void create(string name, bool rewrite);

  //to override in subclasses. 
    virtual bool isStartingState(VariableSet state) = 0;
    virtual bool isGoalState(VariableSet state) = 0;
    virtual int rewardFunction(VariableSet state, string action) = 0;
    virtual VarStateProb transitionFunction(VariableSet state, string action)=0;

    protected:

  
    //learning functions
    virtual int bellmanBackup(int i, std::vector<double> vhi);
    virtual void valueIteration();

    //will call the transition reward function etc. and save the results in the Mdp variables
    virtual void enumerateFunctions();
    virtual void enumerateGoalAndStartStates();
    
    //functions to actually write the model and policy file in the same format read by the Mdp class
    void writeModel(string file_name);
    void writePolicy(string file_name);
};


#endif /* CONCRETEMDP_H */

