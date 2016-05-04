/* 
 * File:   Mmdp.h
 * Author: mfiore
 *
 * Created on March 8, 2016, 3:10 PM
 */

#ifndef MMDP_H
#define	MMDP_H

#include "Hmdp.h"
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include "Wait.h"

using namespace std;

class Mmdp : public Hmdp {
public:
    Mmdp();
    Mmdp(const Mmdp& orig);
    virtual ~Mmdp();

    virtual void create(string name, bool rewrite, bool first);

    std::map<string, Hmdp*> agent_hmpd_;
    VariableSet convertToParametrizedState(VariableSet s);

private:
    void createJointMdpVariables();
    virtual void enumerateGoalAndStartStates();
    virtual void enumerateFunctions(string fileName);
    virtual void assignParametersFromActionName(string action_name);

    

    //two functions to override in the derived classes
    virtual VarStateProb transitionFunction(VariableSet state, string action);
    virtual int rewardFunction(VariableSet state, string action);
    virtual bool isStartingState(VariableSet state);
    virtual bool isGoalState(VariableSet state) ;
    

    string convertToSingleParameter(string var_name, int index);
    string convertToMultiParameter(Mdp* mdp, string var_name, int i);


    VariableSet convertToMmdpState(VariableSet mdp_state, Hmdp* mdp, int index);

    VariableSet convertToMdpState(Hmdp* mdp, int index, VariableSet mmdp_state);
    void assignParametersToMdp(Hmdp* mdp, int index);

    VarStateProb joinMdpFutureStates(VarStateProb mdp_future_state, VarStateProb cumulative_future_state,
    VariableSet mmdp_state, Hmdp *mdp, int index, bool *no_incongruences);
    
    bool isMmdpStateCongruent(VariableSet state);

    string getDeparametrizedAction(string action_name);
    
    string convertToSingleMdpAction(Mdp *mdp,int index,string mmdp_action);
    
    
    void createSubMmdps();

    bool readMdp(string fileName, bool rewrite);

    
    void createSubMdpNames(string name);
//    void printMmdpHierarchy();
};

#endif	/* MMDP_H */

