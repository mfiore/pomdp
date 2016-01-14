/* 
 * File:   Hmdp.cpp
 * Author: mfiore
 * 
 * Created on December 14, 2015, 4:49 PM
 */

#include <map>
#include <vector>

#include "Hmdp.h"
#include "Mdp.h"

Hmdp::Hmdp() {
    active_module = "this";
}

Hmdp::Hmdp(const Hmdp& orig) {
}

Hmdp::~Hmdp() {
}

///*
// Executes a bellman backup on state i that is max a [R(i,a) + L * sum T(i,a,s) vhi(s)]
// * returns the new v values and updates the q values
// */
//int Hmdp::bellmanBackup(int i, std::vector<double> vhi) {
//    double maxValue = 0;
//    for (string action : actions) { //for every action
//
//        int currentReward;
//        std::map<int, double> futureBeliefs;
//        PairStateAction rInput{i, action}; //calculate the reward of this state with this action
//
//        //we have to do this because the std::map increments of one element if we just use [] with a non existing member and the reward function
//        //contains only transition with non zero rewards.
//        if (reward.find(rInput) != reward.end()) {
//            currentReward = reward[rInput];
//        } else {
//            currentReward = 0;
//        }
//
//        PairStateAction transitionInput{i, action};
//        futureBeliefs = transition[transitionInput];
//        //        }
//
//        double sum = 0;
//        for (auto aBelief : futureBeliefs) {
//            sum = sum + aBelief.second * vhi[aBelief.first]; //sum on the probabilities of the future states * the value of reaching that state
//        }
//        PairStateAction qInput{i, action};
//        double havNew = currentReward + 0.8 * sum; //0.3 weights the future rewards
//        qValue[qInput] = havNew; //update the human action value
//        if (qValue[qInput] > maxValue) {
//            maxValue = qValue[qInput];
//        }
//
//    }
//    return maxValue;
//}

double Hmdp::getHierarcicReward(int i) {
    return hierarchic_reward_[i];
}

double Hmdp::getHierarcicReward(VariableSet set) {
    int i = convertHierarchicState(set);
    double r=hierarchic_reward_[i];
    return hierarchic_reward_[i];
}

map<int, double> Hmdp::getHierarcicTransition(int i) {
    map<int, double> result;

    for (int g : goal_states_) {
        pair<int, int> hierachic_input{i, g};
        double prob = hierarchic_transition_[hierachic_input];
        if (prob > 0) {
            result[g] = prob;
        }
    }
    return result;

}

int Hmdp::convertHierarchicState(VariableSet set) {
    VariableSet this_set;
    for (string v : variables) {
        this_set.set[v] = set.set[v];
    }
    return mapStateEnum[this_set];

}

map<VariableSet,double> Hmdp::convertToHigherState(VariableSet original_set, map<VariableSet, double> temp_result) {
    map<VariableSet,double> result;
    
    vector<string> higher_variables;
    for (auto v : original_set.set) {
        higher_variables.push_back(v.first);
    }

    //we should:
    //- put parameters in their original form
    //- add missing variables back
    
    if (higher_variables != variables) {
        for (auto b : temp_result) {
            VariableSet new_vs = b.first;
            for (string s : higher_variables) {
                if (b.first.set.find(s) == b.first.set.end()) { //variable is not present. Either it's a parameter instance or it's a variable present only in the higher hierarchy
                    if (parametrized_to_original.find(s) != parametrized_to_original.end()) { //parameter
                        string actual_value=b.first.set.at(parametrized_to_original[s]); //this might still be a parameter!
                        if (parameter_instances.find(actual_value)!=parameter_instances.end()) {
                            actual_value=parameter_instances[actual_value];
                        }
                        new_vs.set[s] = actual_value;
                    } else {
                        new_vs.set[s] = original_set.set[s];
                    }
                } else {
                    new_vs.set[s] = b.first.set.at(s);
                }
            }
            result[new_vs] = b.second;

        }
    } else {
        result = temp_result;
    }
    return result;
}

map<VariableSet, double> Hmdp::getHierarcicTransition(VariableSet set) {
    map<VariableSet, double> temp_result;

    VariableSet v_param = assignParameters(set);
    //    int i = convertHierarchicState(set);
//    for (auto v:v_param.set) {
//        cout<<v.first<<" "<<v.second<<"\n";
//    }
    int i = mapStateEnum[v_param];

    for (int g : goal_states_) {
        pair<int, int> hierachic_input{i, g};
        double prob = hierarchic_transition_[hierachic_input];
        if (prob > 0) {
            temp_result[vecStateEnum[g]] = prob;
        }
    }
    
    return convertToHigherState(set,temp_result);

}

map<VariableSet, double> Hmdp::getHierarcicTransition(VariableSet set, string action) {
    map<VariableSet, double> temp_result;

    if (active_module != "this") {
        temp_result = hierarchy_map_[active_module]->getHierarcicTransition(set, action);
    } else {
        int i = convertHierarchicState(set);
        PairStateAction p{i, action};
        StateProb result_var = transition[p];
        for (auto r : result_var) {
            temp_result[vecStateEnum[r.first]] = r.second;
        }
    }
    return convertToHigherState(set,temp_result);

}

void Hmdp::calculateHierarcicReward() {
    cout << "Calculating hierarchic reward\n";
    if (hierarchic_reward_.size() == 0) {
        std::map<int, double> temp_hierarchic_reward;
        using Eigen::VectorXd;
        Eigen::SparseMatrix<double> a(vecStateEnum.size(), vecStateEnum.size());
        VectorXd b(vecStateEnum.size());
        double beta = 0.3;
        int i = -1;
        for (int s = 0; s < vecStateEnum.size(); s++) {
            a.insert(s, s) = 1;
            string action = chooseAction(s);
            PairStateAction transition_input{s, action};
            StateProb future_states = transition[transition_input];
            for (auto future_state : future_states) {
                int sp = future_state.first;
                if (!isGoalState(vecStateEnum[sp]) && sp != s) {
                    a.insert(s, sp) = -beta * future_state.second;
                }
            }
            PairStateAction reward_input{s, action};
            b(s) = reward[reward_input];
        }
        Eigen::BiCGSTAB<Eigen::SparseMatrix<double> > solver;
        a.makeCompressed();
        solver.compute(a);
        if (solver.info() != Eigen::Success) {
            // decomposition failed
            return;
        }
        VectorXd x = solver.solve(b);
        if (solver.info() != Eigen::Success) {
            // solving failed
            return;
        }
        //        VectorXd x = a.householderQr().solve(b);
        i = 0;
        for (int s : starting_states_) {
            temp_hierarchic_reward[s] = x(s);
            i++;
        }
        hierarchic_reward_ = temp_hierarchic_reward;

    }
}

void Hmdp::calculateHierarcicTransition() {
    cout << "Calculating hierarchic transition\n";

    if (hierarchic_transition_.size() == 0) {
        std::map<pair<int, int>, double> temp_hierarcic_transition;

        double beta = 0.3;

        for (int g : goal_states_) {
            using Eigen::VectorXd;
            Eigen::SparseMatrix<double> a(vecStateEnum.size(), vecStateEnum.size());
            VectorXd b(vecStateEnum.size());


            for (int s = 0; s < vecStateEnum.size(); s++) {
                string action = chooseAction(s);
                PairStateAction t_input{s, action};
                StateProb future_states = transition[t_input];
                a.insert(s, s) = 1;
                for (auto future_state : future_states) {
                    int sp = future_state.first;
                    if (sp != s && !isGoalState(vecStateEnum[sp])) {
                        a.insert(s, sp) = -beta * future_state.second;
                    }

                }
                double b_transition_prob = getTransitionProb(s, action, g);
                b[s] = b_transition_prob;
            }
            Eigen::SparseLU<Eigen::SparseMatrix<double> > solver;
            a.makeCompressed();
            solver.compute(a);
            if (solver.info() != Eigen::Success) {
                // decomposition failed
                return;
            }
            VectorXd x = solver.solve(b);
            if (solver.info() != Eigen::Success) {
                // solving failed
                return;
            }
            int i = 0;
            for (int s : starting_states_) {
                pair<int, int> hierarchic_input{s, g};
                temp_hierarcic_transition[hierarchic_input] = x(s);
                i++;
            }
        }

        hierarchic_transition_ = temp_hierarcic_transition;
    }
}

void Hmdp::create(string name, bool rewrite, bool first) {
    this->name = name;

    for (HmdpMap::iterator i = hierarchy_map_.begin(); i != hierarchy_map_.end(); i++) {
        i->second->create(i->first, rewrite, false);
    }

    cout << "Creating " << name << "\n";

    string fileName = name + ".pomdp";

    std::vector<std::vector<int>> enumInput;
    for (string variable : variables) {
        std::vector<int> valValues;
        for (int i = 0; i < varValues[variable].size(); i++) {
            valValues.push_back(i);
        }
        enumInput.push_back(valValues);
    }

    NestedLoop loop(enumInput);
    std::vector<std::vector<int>> enumOutput = loop.buildMatrix();
    for (int i = 0; i < enumOutput.size(); i++) {
        VariableSet v;
        for (int j = 0; j < enumOutput[i].size(); j++) {
            string name = variables[j];
            std::vector<string> values = varValues[name];
            v.set[name] = values[enumOutput[i][j]];
        }

        mapStateEnum[v] = i;

        vecStateEnum.push_back(v);
    }

    ifstream inputFile(fileName);
    if (inputFile.good() && !rewrite) {
        for (int i = 0; i < vecStateEnum.size(); i++) {
            for (string action : actions) {
                PairStateAction transitionInput{i, action};
                StateProb transitionOutput;

                string line;
                getline(inputFile, line);
                vector<string> transition_v = StringOperations::stringSplit(line, ' ');
                int i = 0;

                PairStateAction bTransitionInput{i, action};

                while (i < transition_v.size()) {
                    transitionOutput[stoi(transition_v[i])] = stod(transition_v[i + 1]);
                    std::vector<int> previousBeliefs = predecessors[bTransitionInput];
                    previousBeliefs.push_back(stoi(transition_v[i]));
                    predecessors[bTransitionInput] = previousBeliefs;

                    i = i + 2;
                }

                transition[transitionInput] = transitionOutput;

                getline(inputFile, line);
                PairStateAction rewardInput = {i, action};
                reward[rewardInput] = stoi(line);
            }
        }
        inputFile.close();
    } else {
        ofstream file(fileName);
        cout << "Starting Enumeration\n";
        for (int i = 0; i < vecStateEnum.size(); i++) {
            for (string action : actions) {
                double r;
                StateProb future_beliefs;
                if (hierarchy_map_.find(action) == hierarchy_map_.end()) {
                    VarStateProb future_beliefs_var = transitionFunction(vecStateEnum[i], action);
                    for (auto b : future_beliefs_var) {
                        future_beliefs[mapStateEnum[b.first]] = b.second;
                    }
                    r = rewardFunction(vecStateEnum[i], action);
                } else {
                    VarStateProb temp_future_beliefs = hierarchy_map_[action]->getHierarcicTransition(vecStateEnum[i]);
//                    future_beliefs = hierarchy_map_[action]->getHierarcicTransition(vecStateEnum[i]);
                   //                    future_beliefs = getMatchingStates(temp_future_beliefs);
                    for (auto temp_b:temp_future_beliefs) {
                        future_beliefs[mapStateEnum[temp_b.first]]=temp_b.second;
                    }
                    VariableSet vstry=vecStateEnum[i];
                    Hmdp* h=hierarchy_map_[action];
                   
                    r = h->getHierarcicReward(vstry);
                }

                PairStateAction transitionInput{i, action};
                for (auto belief : future_beliefs) {
                    int s = belief.first;

                    file << s << " " << belief.second << " ";
                    PairStateAction bTransitionInput{s, action};
                    std::vector<int> previousBeliefs = predecessors[bTransitionInput];
                    previousBeliefs.push_back(i);
                    predecessors[bTransitionInput] = previousBeliefs;
                }
                file << "\n";
                transition[transitionInput] = future_beliefs;

                PairStateAction rewardInput{i, action};
                reward[rewardInput] = r;
                file << r << "\n";
            }
        }
        file.close();
    }


    for (auto state : mapStateEnum) {
        if (isGoalState(state.first)) {
            goal_states_.push_back(state.second);
        }
        if (isStartingState(state.first)) {
            starting_states_.push_back(state.second);
        }
    }
    valueIteration(rewrite);


    if (!first) {
        ifstream i_file(name + ".hmdp");
        if (!rewrite && i_file.good()) {
            i_file.close();
            readHierarchical();
        } else {
            calculateHierarcicReward();
            calculateHierarcicTransition();
            writeHierarchical();
        }

    }

}

map<int, double> Hmdp::getMatchingStates(map<VariableSet, double> beliefs) {
    map<int, double> result;
    map<VariableSet, double> variable_set_result;
    for (auto b : beliefs) {
        map<string, string> this_set = b.first.set;
        vector<VariableSet> v_variable_sets;
        v_variable_sets.push_back(b.first);
        for (string v : variables) {
            if (this_set.find(v) == this_set.end()) {
                vector<VariableSet> temp_v;
                for (string value : varValues[v]) {
                    for (int i = 0; i < v_variable_sets.size(); i++) {
                        VariableSet vs = v_variable_sets[i];
                        vs.set[v] = value;
                        temp_v.push_back(vs);
                    }
                }
                v_variable_sets = temp_v;
            }
        }
        for (VariableSet vs : v_variable_sets) {
            variable_set_result[vs] = b.second / v_variable_sets.size();
        }
    }
    for (auto r : variable_set_result) {
        result[mapStateEnum[r.first]] = r.second;
    }
    return result;
}

void Hmdp::readHierarchical() {
    ifstream i_file(name + ".hmdp");
    if (i_file.good()) {
        string reward_line;
        getline(i_file, reward_line);
        vector<double> rewards = StringOperations::split(reward_line, ' ');
        int i = 0;
        for (int s : starting_states_) {
            hierarchic_reward_[s] = rewards[i];
            i++;
        }
        for (int g : goal_states_) {
            string goal_line;
            getline(i_file, goal_line);
            vector<double> goal_transitions = StringOperations::split(goal_line, ' ');
            int i = 0;
            for (int s : starting_states_) {
                pair<int, int> t_input{s, g};
                hierarchic_transition_[t_input] = goal_transitions[i];
                i++;
            }
        }
    }
    i_file.close();
}

void Hmdp::writeHierarchical() {
    ofstream o_file(name + ".hmdp");
    for (int s : starting_states_) {
        o_file << hierarchic_reward_[s] << " ";
    }
    o_file << "\n";
    for (int g : goal_states_) {
        for (int s : starting_states_) {
            pair<int, int> t_input{s, g};
            o_file << hierarchic_transition_[t_input] << " ";
        }
        o_file << "\n";
    }
    o_file.close();
}

string Hmdp::chooseHierarchicAction(VariableSet state) {
    VariableSet this_state = assignParameters(state);
    //    for (string var : variables) {
    //        this_state.set[var] = state.set[var];
    //    }
    cout << mapStateEnum[this_state] << "\n";
    if (isGoalState(this_state)) return "";
    if (active_module == "this") {

        string action = chooseAction(mapStateEnum[this_state]);
        if (hierarchy_map_.find(action) != hierarchy_map_.end()) {
            active_module = action;
            return hierarchy_map_[action]->chooseHierarchicAction(state);
        } else {
            return action;
        }
    } else {
        string action = hierarchy_map_[active_module]->chooseHierarchicAction(state);
        if (action != "") {
            return action;
        } else {
            active_module = "this";
            return chooseHierarchicAction(this_state);
        }
    }
}

string Hmdp::chooseHierarchicAction(int b) {
    return chooseHierarchicAction(vecStateEnum[b]);
}
//

void Hmdp::simulate(int n, VariableSet initial_state) {
    int int_initial_state = mapStateEnum[initial_state];
    StateProb b;
    b[int_initial_state] = 1.0;
    for (int i = 0; i < n; i++) {
        cout << "Step " << i << "\n";
        StateProb temp_b;
        for (auto s : b) {
            cout << "State: \n";
            cout << vecStateEnum[s.first].toString();
            string action = chooseHierarchicAction(s.first);
            cout << "Executing " << action << "\n";\
            
            StateProb output;
            if (active_module != "this") {
                Hmdp* sub_mdp = hierarchy_map_[active_module];
                VarStateProb var_output = sub_mdp->getHierarcicTransition(vecStateEnum[s.first], action);
                for (auto o : var_output) {
                    output[mapStateEnum[o.first]] = o.second;
                }
            } else {
                PairStateAction p{s.first, action};
                output = transition[p];
            }
            for (auto o : output) {
                if (temp_b.find(o.first) != temp_b.end()) {
                    temp_b[o.first] = temp_b.at(o.first) + o.second;
                } else {
                    temp_b[o.first] = o.second;
                }
            }
        }
        b = temp_b;
    }
    cout << "Final States\n";
    for (auto s : b) {
        cout << vecStateEnum[s.first].toString() << "\n";
    }
}
