/*
 * File:   Mdp.h
 * Author: mfiore
 *
 * Created on January 6 2016, 11:53 AM
 * 
 * This class is the superclass of all MDPs. It includes just enough methods to be able to read an MDP from a file
 * and use it correctly.
 * 
 * 
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
public:
    /*creates an MDP: if use_cost=true than it will use goal states and a cost function (where the cost of each action is 1,
    outside of goal states). Else it will use a reward function */
    Mdp(bool use_cost = true);
    Mdp(const Mdp& orig);
    virtual ~Mdp();


    //return the value of executing an action in a state and then following the policy
    double getQValue(VariableSet s, string action);
    //return the best action in a state
    string chooseAction(VariableSet s);

    //utility functions
    void printTransitionFunction();
    void printRewardFunction();
    void printStates();
    void printQValues(VariableSet s);
    void printActions();
    void printParameters();
    void printGoalStates();

    //simulates n steps of the model starting from a certain state
    virtual void simulate(int n, VariableSet s);

    //assign parameters to the model
    void assignParameters(std::map<string, string> instance);


    bool readMdp(string path);

    //setters and getters
    std::vector<string> getVariables() const;
    std::vector<string> getValues(string var);
    std::vector<string> getParameters() const;
    bool isVariableParameter(string var);
    std::map<string, string> getAbstractStates(string var) const;
    std::map<string, string> getParameterInstance() const;
    std::vector<string> getActions() const;
    string getParametrizedName() const;
    string getName() const;
    std::vector<string> getParameterVariables(string par);
    string getParameterOfVariable(string var);
    bool isUseCost() const;
    void setName(string name_);
    string getParameterForActionPlace(int i);
    void setParametrizedName(string parametrized_name_);
    std::map<string, string> getParametrizedToOriginal() const;
    std::map<string, std::vector<string> > getOriginalToParametrized_() const;
    std::map<int, string> getParameterActionPlace() const;
    std::vector<int> getGoalStates() const;
    bool isGoalState(VariableSet state);

    //converts states from real world to parametrized representation and viceversa
    VariableSet convertToParametrizedState(VariableSet s);
    VariableSet convertToDeparametrizedState(VariableSet parameter_set, VariableSet full_state);

    string getDeparametrizedAction(string action_name);
    string getParametrizedAction(string action_name);

    std::vector<std::string> getOriginalVars();

protected:
    //mdp specification

    string name_; //name of the model
    string parametrized_name_; //name of the model after applying the parameters

    std::vector<string> variables_; //lists all variables;
    std::vector<string> actions_; //list of actions
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

    //returns the probability of a transition
    double getTransitionProb(int s, string a, int s_new);


    //Q Value Functions
    double getQValue(int s, string action);
    int getBestQ(VariableSet state);
    virtual string chooseAction(int s); //virtualized for hierarchical mdps, which use a different mechanism


    //functions used in the creation of the MDP
    virtual void enumerateStates();

    //utility functions
    //finds a suitable value of a variable among a list of possible values
    string findValue(string variable, std::vector<string> possible_values);
    //gets the future states after executing an action
    VarStateProb getFutureStates(VariableSet state, string action);
    //returns linked values to an abstract value
    std::vector<string> getAbstractLinkedValues(string var, string abstract_value);
    //returns the reward of executing an action in a state (or its cost if use_cost=true)
    int getReward(VariableSet state, string action);

    //read the model and the policy
    bool readModel(string file_name);
    bool readPolicy(string file_name);







};

#endif /* MDP_H */

