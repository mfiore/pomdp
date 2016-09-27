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
 * -switching from flat to bolic
 * -better solving algorithms
 */

#ifndef MDP_H

#define MDP_H

#include <vector>
#include<map>

#include "VariableSet.h"
#include <fstream>
#include "PriorityQueue.h"

#include "NestedLoop.h"
#include "StringOperations.h"
#include <set>

//some useful typedefs
typedef pair<int, string> PairStateAction;
typedef std::map<int, double> StateProb;
typedef std::map<VariableSet, double> VarStateProb;

class Mdp {
    friend class MdpCreator;
public:
    Mdp(bool use_cost = true);
    Mdp(const Mdp& orig);
    virtual ~Mdp();


    double getQValue(VariableSet s, string action);
    string chooseAction(VariableSet s);

    //utility functions
    void printTransitionFunction();
    void printRewardFunction();
    void printStates();
    void printQValues(VariableSet s);
    void printActions();
    void printParameters();
    void printGoalStates();
    
    virtual void simulate(int n, VariableSet s);

    void assignParameters(std::map<string, string> instance);

    virtual string getDeparametrizedAction(string action_name);
    virtual string getParametrizedAction(string action_name);

    bool readMdp(string path);

    std::vector<std::string> getOriginalVars();

    string name_;
    string parametrized_name_;



    //mdp specification
    std::vector<string> variables_; //lists all variables;
    std::vector<string> actions_;
    std::map<string, std::vector<string> > var_values_; //values of the variables

    std::vector<string> parameters_; //list of parameters of the mdp (ex. object_name agent_name)
    std::map<string, string> parameter_instances_; //current set up of parameters (ex. object_name=grey_tape)
    std::map<string, std::vector<string> > parameter_variables_; //each parameter can be link to variables (ex. object_name is linked to object_isAt)
    std::map<string, string> variable_parameter_; //inverse link
    std::map<string, std::vector<string> > original_to_parametrized_; //std::map that converts a var to it's parametrized version (e.g. greyTape_isAt -> object_isAt)
    //it's a std::vector because theoretically more parameters could be set to the same value (e.g. greyTape_isAt -> object1_isAt, object2_isAt)
    std::map<string, string > parametrized_to_original_; //opposite of the one upper

    /*position of the parameters in an action. Ex. human_take_bottle: agent -> 1, object ->3
     */
    std::map<int, string> parameter_action_place_;


    //Variables
    //the system state enumeration is kept both way (from enumeration number to system state and other way)
    std::map<VariableSet, int> map_state_enum_;
    std::vector<VariableSet> vec_state_enum_;
    std::map<PairStateAction, StateProb> transition_;
    std::map<PairStateAction, std::vector<int>> predecessors_; //enumeration of the transition function
    std::map<PairStateAction, int> reward_; //reward function
    std::vector<int> starting_states_;
    std::vector<int> goal_states_;

    //abstract state. Each variable can have a map that links a real world state to a parameter world state
    //e.g. agent_isAt ->  { Greg -> other_agent } 
    std::map<string, std::map<string, string> > abstract_states_;


    bool use_cost_; //when true the mdp uses cost, otherwise it uses rewards.
    std::map<PairStateAction, double> qvalue_; //human action values





    double getTransitionProb(int s, string a, int s_new);


    //Q Value Functions
    double getQValue(int s, string action);
    int getBestQ(VariableSet state);
    virtual string chooseAction(int s); //virtualized for hierarchical mdps, which use a different mechanism

    //parameter functions
    virtual VariableSet convertToParametrizedState(VariableSet parameter_set); //converts a state space to it's parametrized version
    VariableSet convertToDeparametrizedState(VariableSet parameter_set, VariableSet full_state); //opposite

    //functions used in the creation of the MDP
    virtual void enumerateStates();





    //utility functions
    string findValue(string variable, std::vector<string> possible_values);
    VarStateProb getFutureStates(VariableSet state, string action);
    std::vector<string> getAbstractLinkedValues(string var, string abstract_value);
    int getReward(VariableSet state, string action);
    

    bool readModel(string file_name);
    bool readPolicy(string file_name);
    std::vector<string> getVariables() const;

};

#endif /* MDP_H */

