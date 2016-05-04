/*
 * File:   Mdp.h
 * Author: mfiore
 *
 * Created on January 6 2016, 11:53 AM
 * 
 * This abstract class represents an MDP, including just the standard bellman backup and a prioritized sweeping
 * implementation (that seems to be usually slower). Actual MDP models need to extend this class and implement functions
 * for the rewardFunction and transitionFunction.
 * 
 * The class uses a flat representation for the state, with integers. Users can define state with partioned variable sets.
 * 
 * Future developments might be:
 * -switching from flat to symbolic
 * -better solving algorithms
 */

#ifndef MDP_H

#define	MDP_H

#include <vector>
#include<map>

#include "VariableSet.h"
#include <fstream>
#include "PriorityQueue.h"

#include "NestedLoop.h"
#include "StringOperations.h"

using namespace std;

typedef pair<int, string> PairStateAction;
typedef std::map<int, double> StateProb;
typedef std::map<VariableSet, double> VarStateProb;

class Mdp {
public:
    Mdp();
    Mdp(const Mdp& orig);
    virtual ~Mdp();

    virtual void create(string name, bool rewrite = false);

    double getQValue(VariableSet s, string action);
    string chooseAction(VariableSet s);

    //utility functions
    void printTransitionFunction();
    void printRewardFunction();
    void printStates();
    void printQValues(VariableSet s);

    virtual void simulate(int n, VariableSet s);

    void assignParameters(std::map<string, string> instance);

    virtual string getDeparametrizedAction(string action_name);
    virtual string getParametrizedAction(string action_name);
    
    //private:

    //protected:
    //mdp specification
    std::vector<string> variables; //lists all variables;
    std::vector<string> actions;
    std::map<string, std::vector<string> > varValues;

    std::vector<string> parameters; //list of parameters of the mdp (ex. object_name agent_name)
    std::map<string, string> parameter_instances; //current set up of parameters (ex. object_name=grey_tape)
    std::map<string, std::vector<string> > parameter_variables; //each parameter can be link to variables (ex. object_name is linked to object_isAt)
    std::map<string, string> variable_parameter; //inverse link
    std::map<string, std::vector<string> > original_to_parametrized; //map that converts a var to it's parametrized version (e.g. greyTape_isAt -> object_isAt)
    //it's a vector because theoretically more parameters could be set to the same value (e.g. greyTape_isAt -> object1_isAt, object2_isAt)
    std::map<string, string > parametrized_to_original; //opposite of the one upper


    //Variables

    //the system state enumeration is kept both way (from enumeration number to system state and other way)
    std::map<VariableSet, int> mapStateEnum;
    std::vector<VariableSet> vecStateEnum;
    std::map<PairStateAction, StateProb> transition;
    std::map<PairStateAction, std::vector<int>> predecessors; //enumeration of the transition function
    std::map<PairStateAction, int> reward; //reward function

    std::map<PairStateAction, double> qValue; //human action values

    string name;


    double getTransitionProb(int s, string a, int s_new);


    //two functions to override in the derived classes
    virtual VarStateProb transitionFunction(VariableSet state, string action) = 0;
    virtual int rewardFunction(VariableSet state, string action) = 0;

    //learning functions
    virtual int bellmanBackup(int i, std::vector<double> vhi);
    void valueIteration(bool rewrite = false);
    void prioritizedSweeping(); //for now it's slower on the tested examples. Maybe it's the queue overhead


    double getQValue(int s, string action);

    string chooseAction(int s);


    virtual VariableSet convertToParametrizedState(VariableSet parameter_set); //converts a state space to it's parametrized version
    VariableSet convertToDeparametrizedState(VariableSet parameter_set); //opposite


    virtual void enumerateStates();
    virtual void enumerateFunctions(string fileName);
    virtual bool readMdp(string fileName, bool rewrite);

    void printActions();

    void printParameters();


};

#endif	/* MDP_H */

