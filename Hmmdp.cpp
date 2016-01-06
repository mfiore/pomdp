/* 
 * File:   Hmmdp.cpp
 * Author: mfiore
 * 
 * Created on December 14, 2015, 4:49 PM
 */

#include <map>
#include <vector>

#include "Hmmdp.h"

Hmmdp::Hmmdp() {
    active_module = "this";
}

Hmmdp::Hmmdp(const Hmmdp& orig) {
}

Hmmdp::~Hmmdp() {
}

//gaussian elimination, used to compute T and R for macro actions

vector<double> Hmmdp::gauss(vector< vector<double> > A) {
    int n = A.size();

    for (int i = 0; i < n; i++) {
        // Search for maximum in this column
        double maxEl = abs(A[i][i]);
        int maxRow = i;
        for (int k = i + 1; k < n; k++) {
            if (abs(A[k][i]) > maxEl) {
                maxEl = abs(A[k][i]);
                maxRow = k;
            }
        }

        // Swap maximum row with current row (column by column)
        for (int k = i; k < n + 1; k++) {
            double tmp = A[maxRow][k];
            A[maxRow][k] = A[i][k];
            A[i][k] = tmp;
        }

        // Make all rows below this one 0 in current column
        for (int k = i + 1; k < n; k++) {
            double c = -A[k][i] / A[i][i];
            for (int j = i; j < n + 1; j++) {
                if (i == j) {
                    A[k][j] = 0;
                } else {
                    A[k][j] += c * A[i][j];
                }
            }
        }
    }

    // Solve equation Ax=b for an upper triangular matrix A
    vector<double> x(n);
    for (int i = n - 1; i >= 0; i--) {
        x[i] = A[i][n] / A[i][i];
        for (int k = i - 1; k >= 0; k--) {
            A[k][n] -= A[k][i] * x[i];
        }
    }
    return x;
}

/*
 Executes a bellman backup on state i that is max a [R(i,a) + L * sum T(i,a,s) vhi(s)]
 * returns the new v values and updates the q values
 */
int Hmmdp::bellmanBackup(int i, std::vector<double> vhi) {
    double maxValue = 0;
    for (string action : actions) { //for every action

        int currentReward;
        std::map<int, double> futureBeliefs;
        //        if (hierarchy_map_.find(action) != hierarchy_map_.end()) {
        //            Hmmdp* sub_mdp = hierarchy_map_[action];
        //            currentReward = sub_mdp->getHierarcicReward(i);
        //            futureBeliefs = sub_mdp->getHierarcicTransition(i);
        //        } else {
        pair<int, string> rInput{i, action}; //calculate the reward of this state with this action

        //we have to do this because the std::map increments of one element if we just use [] with a non existing member and the reward function
        //contains only transition with non zero rewards.
        if (reward.find(rInput) != reward.end()) {
            currentReward = reward[rInput];
        } else {
            currentReward = 0;
        }

        pair<int, string> transitionInput{i, action};
        futureBeliefs = transition[transitionInput];
        //        }

        double sum = 0;
        for (auto aBelief : futureBeliefs) {
            sum = sum + aBelief.second * vhi[aBelief.first]; //sum on the probabilities of the future states * the value of reaching that state
        }
        pair<int, string> qInput{i, action};
        double havNew = currentReward + 0.8 * sum; //0.3 weights the future rewards
        qValue[qInput] = havNew; //update the human action value
        if (qValue[qInput] > maxValue) {
            maxValue = qValue[qInput];
        }

    }
    return maxValue;
}

void Hmmdp::solveHierarchical(bool first, bool rewrite) {

}

double Hmmdp::getHierarcicReward(int i) {
    return hierarchic_reward_[i];
}

double Hmmdp::getHierarcicReward(VariableSet set) {
    int i = convertHierarchicState(set);
    return hierarchic_reward_[i];
}

map<int, double> Hmmdp::getHierarcicTransition(int i) {
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

int Hmmdp::convertHierarchicState(VariableSet set) {
    VariableSet this_set;
    for (string v : variables) {
        this_set.set[v] = set.set[v];
    }
    return mapStateEnum[this_set];

}

map<VariableSet, double> Hmmdp::getHierarcicTransition(VariableSet set) {
    map<VariableSet, double> temp_result;
    map<VariableSet, double> result;

    int i = convertHierarchicState(set);

    for (int g : goal_states_) {
        pair<int, int> hierachic_input{i, g};
        double prob = hierarchic_transition_[hierachic_input];
        if (prob > 0) {
            temp_result[vecStateEnum[g]] = prob;
        }
    }
    vector<string> higher_variables;
    for (auto v : set.set) {
        higher_variables.push_back(v.first);
    }
    if (higher_variables != variables) {
        for (auto b : temp_result) {
            VariableSet new_vs=b.first;
            for (string s : higher_variables) {
                if (b.first.set.find(s) == b.first.set.end()) {
                   new_vs.set[s] = set.set[s];
                }
                else {
                    string a=s;
                   new_vs.set[s] = b.first.set.at(s);
                }
            }
            result[new_vs]=b.second;
        }
    }
    else {
        result=temp_result;
    }
    return result;

}

void Hmmdp::calculateHierarcicReward() {
    cout << "Calculating hierarchic reward\n";
    if (hierarchic_reward_.size() == 0) {
        std::map<int, double> temp_hierarchic_reward;
        using Eigen::VectorXd;
        Eigen::SparseMatrix<double> a(vecStateEnum.size(), vecStateEnum.size());
        VectorXd b(vecStateEnum.size());
        //        vector<vector<double> > gaussian_input;
        double beta = 0.3;
        int i = -1;
        for (int s = 0; s < vecStateEnum.size(); s++) {
            //            i++;
            //            vector<double> gaussian_row(vecStateEnum.size());
            //            gaussian_row[s] = 1;
            a.insert(s, s) = 1;
            string action = chooseAction(s);
            map<VariableSet, double> future_states = transitionFunction(vecStateEnum[s], action);
            for (auto future_state : future_states) {
                int sp = mapStateEnum[future_state.first];
                if (!isGoalState(future_state.first) && sp != s) {
                    //                    gaussian_row[sp] = future_state.second;
                    a.insert(s, sp) = -beta * future_state.second;
                }
            }
            //            for (int sp = 0; sp < vecStateEnum.size(); sp++) {
            //                double transition_prob = getTransitionProb(s, action, sp);
            //                gaussian_row.push_back(-beta * transition_prob);
            //            }
            b(s) = rewardFunction(vecStateEnum[s], action);
            //            gaussian_row.push_back(rewardFunction(vecStateEnum[s], action));
            //            gaussian_input.push_back(gaussian_row);
        }
        //        vector<double> gaussian_result = gauss(gaussian_input);
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

void Hmmdp::calculateHierarcicTransition() {
    cout << "Calculating hierarchic transition\n";

    if (hierarchic_transition_.size() == 0) {
        std::map<pair<int, int>, double> temp_hierarcic_transition;

        double beta = 0.3;

        for (int g : goal_states_) {
            //            vector<vector<double> > gaussian_input;

            using Eigen::VectorXd;
            Eigen::SparseMatrix<double> a(vecStateEnum.size(), vecStateEnum.size());
            VectorXd b(vecStateEnum.size());


            for (int s = 0; s < vecStateEnum.size(); s++) {
                string action = chooseAction(s);
                map<VariableSet, double> future_states = transitionFunction(vecStateEnum[s], action);
                //                vector<double> gaussian_row(0, vecStateEnum.size());
                //                gaussian_row[0]=1;
                //                gaussian_row[s] = 1;
                a.insert(s, s) = 1;
                for (auto future_state : future_states) {
                    int sp = mapStateEnum[future_state.first];
                    if (sp != s && !isGoalState(future_state.first)) {
                        //                        gaussian_row[sp] = future_state.second;
                        a.insert(s, sp) = -beta * future_state.second;
                    }

                }
                //                for (int sp = 0; s < vecStateEnum.size(); sp++) {
                //                    if (!isGoalState(vecStateEnum[sp])) {
                //                        double transition_prob = getTransitionProb(s, action, sp);
                //                        gaussian_row.push_back(-beta * transition_prob);
                //                    }
                //                }
                double b_transition_prob = getTransitionProb(s, action, g);
                b[s] = b_transition_prob;
                //                gaussian_row.push_back(b_transition_prob);
                //                gaussian_input.push_back(gaussian_row);
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
            //            vector<double> gaussian_result = gauss(gaussian_input);
            int i = 0;
            for (int s : starting_states_) {
                pair<int, int> hierarchic_input{s, g};
                //                temp_hierarcic_transition[hierarchic_input] = gaussian_result[i];
                temp_hierarcic_transition[hierarchic_input] = x(s);
                i++;
            }
        }

        hierarchic_transition_ = temp_hierarcic_transition;
    }
}

void Hmmdp::createPomdp(string name, bool rewrite, bool first) {
    this->name = name;

    for (HmmdpMap::iterator i = hierarchy_map_.begin(); i != hierarchy_map_.end(); i++) {
        i->second->createPomdp(i->first, rewrite, false);
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

    cout<<vecStateEnum.size()<<"\n";
    ifstream inputFile(fileName);
    if (inputFile.good() && !rewrite) {
        for (int i = 0; i < vecStateEnum.size(); i++) {
            for (string action : actions) {
                pair<int, string> transitionInput{i, action};
                std::map<int, double> transitionOutput;

                string line;
                getline(inputFile, line);
                vector<string> transition_v = StringOperations::stringSplit(line, ' ');
                int i = 0;

                pair<int, string> bTransitionInput{i, action};

                while (i < transition_v.size()) {
                    transitionOutput[stoi(transition_v[i])] = stod(transition_v[i + 1]);
                    std::vector<int> previousBeliefs = predecessors[bTransitionInput];
                    previousBeliefs.push_back(stoi(transition_v[i]));
                    predecessors[bTransitionInput] = previousBeliefs;

                    i = i + 2;
                }

                transition[transitionInput] = transitionOutput;

                getline(inputFile, line);
                pair<int, string> rewardInput = {i, action};
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
                map<int, double> future_beliefs;
                if (hierarchy_map_.find(action) == hierarchy_map_.end()) {
                    std::map<VariableSet, double> future_beliefs_var = transitionFunction(vecStateEnum[i], action);
                    for (auto b : future_beliefs_var) {
                        future_beliefs[mapStateEnum[b.first]] = b.second;
                    }
                    r = rewardFunction(vecStateEnum[i], action);
                } else {
                    map<VariableSet, double> temp_future_beliefs = hierarchy_map_[action]->getHierarcicTransition(vecStateEnum[i]);
                    future_beliefs = getMatchingStates(temp_future_beliefs);
                    r = hierarchy_map_[action]->getHierarcicReward(vecStateEnum[i]);
                }

                pair<int, string> transitionInput{i, action};
                for (auto belief : future_beliefs) {
                    int s = belief.first;
                    pair<int, string> bTransitionInput{s, action};
                    std::vector<int> previousBeliefs = predecessors[bTransitionInput];
                    previousBeliefs.push_back(i);
                    predecessors[bTransitionInput] = previousBeliefs;
                }
                file << "\n";
                transition[transitionInput] = future_beliefs;

                pair<int, string> rewardInput{i, action};
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

map<int, double> Hmmdp::getMatchingStates(map<VariableSet, double> beliefs) {
    map<int, double> result;
    map<VariableSet, double> variable_set_result;
    for (auto b : beliefs) {
        map<string, string> this_set = b.first.set;
        vector<VariableSet> v_variable_sets;
        v_variable_sets.push_back(b.first);
        for (string v : variables) {
            if (this_set.find(v) == this_set.end()) {
                for (string value : varValues[v]) {
                    for (int i = 0; i < v_variable_sets.size(); i++) {
                        v_variable_sets[i].set[v] = value;
                    }

                }
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

void Hmmdp::readHierarchical() {
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

void Hmmdp::writeHierarchical() {
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

string Hmmdp::chooseHierarchicAction(VariableSet state) {
    VariableSet this_state;
    for (string var : variables) {
        this_state.set[var] = state.set[var];
    }
    cout << mapStateEnum[this_state] << "\n";
    if (isGoalState(this_state)) return "";
    if (active_module == "this") {

        string action = chooseAction(mapStateEnum[this_state]);
        if (hierarchy_map_.find(action) != hierarchy_map_.end()) {
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

string Hmmdp::chooseHierarchicAction(map<int, double> a_belief) {
    for (auto b : a_belief) {
        return chooseHierarchicAction(vecStateEnum[b.first]);
    }
}
//
//void Hmmdp::simulate(int n) {
//    cout << "initial belief\n";
//    printBelief();
//    for (int i = 0; i < n; i++) {
////        string action = chooseHierarchicAction(this->belief);
//        
//        cout << "Executing " << action << "\n";
//        updateBelief(action, map<string, string>(), map<string, string>());
//        printBelief();
//    }
//}
