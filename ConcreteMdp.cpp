/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ConcreteMdp.cpp
 * Author: theworld
 * 
 * Created on September 2, 2016, 4:48 PM
 */

#include "ConcreteMdp.h"

ConcreteMdp::ConcreteMdp() {
}

ConcreteMdp::ConcreteMdp(const ConcreteMdp& orig) {
}

ConcreteMdp::~ConcreteMdp() {
}

void ConcreteMdp::writeModel(string file_name) {
    ofstream of(file_name);

    of << name_ << "\n";
    of << "variables\n";
    for (std::string v : variables_) {
        of << v << "\n";
        for (std::string value : var_values_.at(v)) {
            of << value << " ";
        }
        of << "\n";
        if (variable_parameter_.find(v) != variable_parameter_.end()) {
            of << variable_parameter_.at(v) << "\n";
        } else {
            of << "notpar\n";
        }
    }
    of << "parameter\n";
    for (std::string p : parameters_) {
        of << p << " ";
        if (parameter_variables_.find(p) != parameter_variables_.end()) {
            for (std::string v : parameter_variables_.at(p)) {
                of << v << " ";
            }
        }
        of << "\n";
    }
    of << "parameter action place\n";
    for (auto el : parameter_action_place_) {
        of << el.first << " " << el.second << " ";
    }
    of << "\n";
    of << "starting states\n";
    for (int i : starting_states_) {
        of << i << " ";
    }
    of << "\n";
    of << "goal states\n";
    for (int i : goal_states_) {
        of << i << " ";
    }
    of << "\n";
    of << "abstract states\n";
    for (auto abs : abstract_states_) {
        of << abs.first << "\n";
        for (auto abs_map : abs.second) {
            of << abs_map.first << " " << abs_map.second << " ";
        }
        of << "\n";
    }
    of << "actions\n";
    for (string a : actions_) {
        of << a << " ";
    }
    of << "\n";
    of << "transition\n";
    for (auto t : transition_) {
        of << t.first.first << " " << t.first.second << "\n";
        for (auto sp : t.second) {
            of << sp.first << " " << sp.second << " ";
        }
        of << "\n";
    }
    of << "reward\n";
    for (auto r : reward_) {
        of << r.first.first << " " << r.first.second << " " << r.second << "\n";
    }
    of.close();
}

void ConcreteMdp::writePolicy(string file_name) {
    ofstream of(file_name);
    for (auto q : qvalue_) {
        of << q.first.first << " " << q.first.second << " " << q.second << "\n";
    }
    of.close();
}

void ConcreteMdp::create(string name, bool rewrite) {
    //    this->name = name;

    string model_file_name = name + ".mdp";
    string policy_file_name = name + ".policy";

    bool write_policy = rewrite;
    bool write_model = rewrite;

    if (!write_model) {
        std::ifstream model_file(model_file_name);
        if (model_file) {
            readModel(model_file_name);
            enumerateStates();
        } else {
            write_model = true;
        }
    }
    if (write_model) {
        enumerateStates();
        enumerateFunctions();
        enumerateGoalAndStartStates();
        writeModel(model_file_name);
    }
    if (!write_policy) {
        std::ifstream policy_file(policy_file_name);
        if (policy_file) {
            readPolicy(policy_file_name);
        } else {
            write_policy = true;
        }
    }
    if (write_policy) {
        valueIteration();
        writePolicy(policy_file_name);
    }
}

/*This procedure does value iteration and computes the Q-Values for the actions. At the moment we don't use the policy learnt through here,
 * preferring to use the one with sarsop. The algorithm works on enumerations of states
 */
void ConcreteMdp::valueIteration() {
    std::map<PairStateAction, double> qValue;

    int starting_value = use_cost_ ? 100000 : 0;
    std::vector<double> vhi(vec_state_enum_.size(), starting_value); //vhi mantains the predicted reward in a state following the optimal policy
    std::vector<double> vhiOld(vec_state_enum_.size(), starting_value);

    double epsilon = 0.1; //stopping value of the value iteration
    double maxDiff = 0;
    cout << "Starting Value Iteration\n";
    do { //we loop until vhiOld-hvi<epsilon (more or less)

        for (int s = 0; s < vec_state_enum_.size(); s++) { //for each enumeration
            vhiOld[s] = vhi[s];

            if (use_cost_ && isGoalState(vec_state_enum_[s])) {
                vhi[s] = 0;
            } else {
                vhi[s] = bellmanBackup(s, vhi);
            }
        }
        maxDiff = 0;
        for (int i = 0; i < vhi.size(); i++) { //calculate the maximum difference on the vhi (stopping parameter)
            double d = abs(vhi[i] - vhiOld[i]);
            if (d > maxDiff) {
                maxDiff = d;
            }
        }

    } while (maxDiff > epsilon);
}

/*
 Executes a bellman backup on state i that is max a [R(i,a) + L * sum T(i,a,s) vhi(s)]
 * returns the new v values and updates the q values
 */
int ConcreteMdp::bellmanBackup(int i, std::vector<double> vhi) {
    double maxValue = use_cost_ ? 10000 : 0;
    for (string action : actions_) { //for every action
        PairStateAction rInput{i, action}; //calculate the reward of this state with this action

        int currentReward;
        //we have to do this because the std::map increments of one element if we just use [] with a non existing member and the reward function
        //contains only transition with non zero rewards.
        if (reward_.find(rInput) != reward_.end()) {
            currentReward = reward_[rInput];
        } else {
            currentReward = use_cost_ ? 1000 : 0;
        }
        PairStateAction transitionInput{i, action};
        StateProb futureBeliefs = transition_[transitionInput];
        double sum = 0;

        if (futureBeliefs.size() == 0) {
            sum = use_cost_ ? 1000 : 0;
        } else {
            for (auto aBelief : futureBeliefs) {
                sum = sum + aBelief.second * vhi[aBelief.first]; //sum on the probabilities of the future states * the value of reaching that state
            }
        }
        PairStateAction qInput{i, action};
        double havNew;
        if (use_cost_) {
            havNew = currentReward + sum;
        } else {
            havNew = currentReward + 0.3 * sum; //0.3 weights the future rewards
        }
        (qvalue_)[qInput] = havNew; //update the human action value
        if (qvalue_[qInput] > maxValue && !use_cost_) {
            maxValue = qvalue_[qInput];
        } else if (qvalue_[qInput] < maxValue && use_cost_) {
            maxValue = qvalue_[qInput];
        }

    }
    return maxValue;
}

void ConcreteMdp::enumerateFunctions() {
    cout << "Starting Enumeration\n";
    for (int i = 0; i < vec_state_enum_.size(); i++) {
        for (string action : actions_) {
            std::map<VariableSet, double>futureBeliefs = transitionFunction(vec_state_enum_[i], action);
            StateProb transitionOutput;

            PairStateAction transitionInput{i, action};
            for (auto belief : futureBeliefs) {
                int s = map_state_enum_.at(belief.first);

                transitionOutput[s] = belief.second;

                //                file << s << " " << belief.second << " ";

                PairStateAction bTransitionInput{s, action};
                std::vector<int> previousBeliefs = predecessors_[bTransitionInput];
                previousBeliefs.push_back(i);
                predecessors_[bTransitionInput] = previousBeliefs;
            }
            transition_[transitionInput] = transitionOutput;

            PairStateAction rewardInput{i, action};
            reward_[rewardInput] = use_cost_ ? 1 : rewardFunction(vec_state_enum_[i], action);
        }
    }
}

void ConcreteMdp::enumerateGoalAndStartStates() {
    for (auto state : map_state_enum_) {
        if (isGoalState(state.first)) {
            goal_states_.push_back(state.second);
        }
        if (isStartingState(state.first)) {

            starting_states_.push_back(state.second);
        }
    }
}

