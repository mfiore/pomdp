/* 
 * File:   Mmdp.h
 * Author: mfiore
 *
 * Created on March 8, 2016, 3:10 PM
 * 
 * This class represent an MMDP, which creates from 2 single angent mdps a model to plan for both together
 */

#ifndef MMDP_H
#define MMDP_H

#include "Hmdp.h"
#include <string>
#include <vector>
#include <boost/lexical_cast.hpp>
#include "Wait.h"
#include <set>
#include "MmdpManager.h"
#include <tuple>


using namespace std;

class Mmdp : public Hmdp {
public:
    Mmdp(MmdpManager* mmdp_manager);
    Mmdp(const Mmdp& orig);
    virtual ~Mmdp();

    virtual void create(string name, bool rewrite, bool first);

    std::map<string, Hmdp*> agent_hmpd_;

    void createJointMdpVariables();
    virtual void enumerateGoalAndStartStates();
    virtual void enumerateFunctions(string fileName);
    virtual void assignParametersFromActionName(string action_name, set<string> changed_mdps, map<string, string> super_instance);


    string chooseHierarchicAction(VariableSet state);



    //two functions to override in the derived classes
    virtual VarStateProb transitionFunction(VariableSet state, string action);
    virtual int rewardFunction(VariableSet state, string action);
    virtual bool isStartingState(VariableSet state);
    virtual bool isGoalState(VariableSet state);


    string convertToSingleParameter(string var_name, int index);
    string convertToMultiParameter(Mdp* mdp, string var_name, int i);


    VariableSet convertToMmdpState(VariableSet mdp_state, Hmdp* mdp, int index);

    tuple<VariableSet, set<string>, set<string> > convertToMdpState(Hmdp* mdp, int index, VariableSet mmdp_state);
    void assignParametersToMdp(Hmdp* mdp, int index);

    pair<VarStateProb, set<string> > joinMdpFutureStates(VarStateProb mdp_future_state, VarStateProb cumulative_future_state,
            VariableSet mmdp_state, Hmdp *mdp, int index, bool *no_incongruences, std::set<string> new_not_present_variables,
            VariableSet old_single_agent_state, set<string> abstract_variables, set<string> old_abstract_variables);

    bool isMmdpStateCongruent(VariableSet state);

    string getDeparametrizedAction(string action_name);

    string convertToSingleMdpAction(Mdp *mdp, int index, string mmdp_action);


    void createSubMmdps();

    bool readMdp(string fileName, bool rewrite);


    void createSubMdpNames(string name);
    //    void printMmdpHierarchy();

    pair<vector<string>, set<string> > getSubMdpName(string action);



    //transitions can make a state set not congruent when there is a resource conflict between two mdps. This procedure
    //checks the values and make them congruent. 
    //    VariableSet makeMmdpStateCongruent(VariableSet new_vs, VariableSet original_vs);  


    string original_action_;

    vector<string> hasParametersInCommon();

    vector<string> hasParametersInCommon(map<string, Hmdp*> agents);

    VariableSet fixAbstractStates(VariableSet sub_set, VariableSet super_set, Hmdp* sub_mmdp);

    MmdpManager* mmdp_manager_;

    vector<string> forbidden_actions_;
    vector<string> joint_actions_;
    vector<string> joint_modules_;
    map<string, string> joint_actions_module_name_;

    int estimateRemainingCost(VariableSet state);

    void valueIteration(string fileName, bool rewrite);

    void testEnumerate(int i, string action);

    string chooseAction(int s);



};

#endif /* MMDP_H */

