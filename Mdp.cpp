/* 
 * File:   Mdp.cpp
 * Author: mfiore
 * 
 * Created on August 18, 2014, 11:53 AM
 */

#include "Mdp.h"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <stack>
#include <utility>
#include <algorithm>
#include <c++/4.6/bits/stl_queue.h>

Mdp::Mdp() {
}

Mdp::Mdp(const Mdp& orig) {
}

Mdp::~Mdp() {
}

/*
 Executes a bellman backup on state i that is max a [R(i,a) + L * sum T(i,a,s) vhi(s)]
 * returns the new v values and updates the q values
 */
int Mdp::bellmanBackup(int i, std::vector<double> vhi) {
    double maxValue = 0;
    for (string action : actions) { //for every action
        PairStateAction rInput{i, action}; //calculate the reward of this state with this action

        int currentReward;
        //we have to do this because the std::map increments of one element if we just use [] with a non existing member and the reward function
        //contains only transition with non zero rewards.
        if (reward.find(rInput) != reward.end()) {
            currentReward = reward[rInput];
        } else {
            currentReward = 0;
        }
        PairStateAction transitionInput{i, action};
        StateProb futureBeliefs = transition[transitionInput];

        double sum = 0;
        for (auto aBelief : futureBeliefs) {
            sum = sum + aBelief.second * vhi[aBelief.first]; //sum on the probabilities of the future states * the value of reaching that state
        }
        PairStateAction qInput{i, action};
        double havNew = currentReward + 0.3 * sum; //0.3 weights the future rewards
        qValue[qInput] = havNew; //update the human action value
        if (qValue[qInput] > maxValue) {
            maxValue = qValue[qInput];
        }

    }
    return maxValue;
}

/*
 This procedure does value iteration and computes the Q-Values for the actions. At the moment we don't use the policy learnt through here,
 * preferring to use the one with sarsop. The algorithm works on enumerations of states
 */
void Mdp::valueIteration(bool rewrite) {
    string fileName = name + ".policy";
    ifstream inputFile(fileName);

    std::vector<double> vhi(vecStateEnum.size(), 0); //vhi mantains the predicted reward in a state following the optimal policy
    std::vector<double> vhiOld(vecStateEnum.size(), 0);

    if (inputFile.good() && !rewrite) {
        string line;
        for (int i = 0; i < vecStateEnum.size(); i++) {
            getline(inputFile, line);
            vector<double> q_line = StringOperations::split(line, ' ');
            int j = 0;
            for (string action : actions) {
                PairStateAction qInput{i, action};
                qValue[qInput] = q_line[j];
                j++;
            }
        }
        inputFile.close();

    } else {
        double epsilon = 0.1; //stopping value of the value iteration
        double maxDiff = 0;
        cout << "Starting Value Iteration\n";
        do { //we loop until vhiOld-hvi<epsilon (more or less)

            for (int s = 0; s < vecStateEnum.size(); s++) { //for each enumeration
                vhiOld[s] = vhi[s];

                vhi[s] = bellmanBackup(s, vhi);
            }
            maxDiff = 0;
            for (int i = 0; i < vhi.size(); i++) { //calculate the maximum difference on the vhi (stopping parameter)
                double d = abs(vhi[i] - vhiOld[i]);
                if (d > maxDiff) {
                    maxDiff = d;
                }
            }

        } while (maxDiff > epsilon);
        ofstream file(fileName);
        for (int i = 0; i < vecStateEnum.size(); i++) {
            for (string action : actions) {
                PairStateAction qInput{i, action};
                file << qValue[qInput] << " ";
            }
            file << "\n";
        }
        file.close();
    }
}

void Mdp::prioritizedSweeping() {
    std::vector<double> vhi(vecStateEnum.size(), 0);
    std::vector<double> vhiOld(vecStateEnum.size(), 0);
    double epsilon = 0.1;
    PriorityQueue pq;
    for (int i = 0; i < vecStateEnum.size(); i++) {
        pq.pushElement(pair<int, double>{i, 1});
    }

    while (!pq.isEmpty()) {
        pair<int, double> actualElement = pq.pop();
        int s = actualElement.first;
        //        cout << pq.size() << "\n";
        //
        //        cout << s << "\n";
        vhiOld[s] = vhi[s];
        vhi[s] = bellmanBackup(s, vhi);

        std::map<int, bool> isVisited;
        for (string action : actions) {
            PairStateAction predInput{s, action};
            std::vector<int> vsp = predecessors[predInput];

            for (int sp : vsp) {
                if (isVisited[sp] == false) {
                    isVisited[sp] = true;

                    double maxValue = 0;
                    for (string action2 : actions) {
                        PairStateAction transitionInput{sp, action2};
                        StateProb futureStates = transition[transitionInput];
                        double value = futureStates[s] * (vhi[s] - vhiOld[s]);
                        if (value > maxValue) {
                            maxValue = value;
                        }
                    }
                    if (sp != s) {
                        double sppriority = pq.getPriority(sp);
                        if (sppriority > maxValue) {
                            maxValue = sppriority;
                        }
                    }
                    if (maxValue > epsilon) {
                        //                        cout << maxValue << "\n";

                        pair<int, double> newElement{sp, maxValue};
                        pq.pushElement(newElement);
                        //                        pq.print();
                    }
                }

            }
        }
    }
}





//UTILITY FUNCTIONS

void Mdp::printTransitionFunction() {
    cout << "Transition Function\n";
    for (int i = 0; i < vecStateEnum.size(); i++) {
        for (string action : actions) {
            PairStateAction tInput{i, action};
            StateProb tOutput = transition[tInput];


            VariableSet vs = vecStateEnum[i];
            if (vs.set["human_isAt"] == vs.set["box_isAt"]) {
                for (auto s : vs.set) {
                    cout << s.first << " " << s.second << " ";
                }
                cout << "\n";
                cout << action << "\n";
                for (auto out : tOutput) {
                    VariableSet vo = vecStateEnum[out.first];
                    for (auto s : vo.set) {
                        cout << s.first << " " << s.second << " ";
                    }
                    cout << "\n";
                    cout << out.first << "\n";
                }
                cout << "\n";
            }
        }
    }
}

void Mdp::printRewardFunction() {
    cout << "Reward Function\n";
    for (auto el : reward) {
        if (el.second > 0) {
            VariableSet state = vecStateEnum[el.first.first];
            for (auto s : state.set) {

                cout << s.first << " " << s.second << "\n";
            }
            cout << el.first.second << " " << el.second << "\n";
        }
    }
}

void Mdp::printStates() {
    cout << "\n\nPrint Variables\n";
    for (std::map<string, std::vector<string> >::iterator i = varValues.begin(); i != varValues.end(); i++) {
        cout << i->first << "\n";
        cout << "Values= ";
        for (int j = 0; j < i->second.size(); j++) {
            cout << i->second[j] << " ";
        }
        cout << "\n";
    }
}

void Mdp::create(string name, bool rewrite) {
    this->name = name;

    string fileName = name + ".mdp";

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
                std::map<VariableSet, double>futureBeliefs = transitionFunction(vecStateEnum[i], action);
                StateProb transitionOutput;

                PairStateAction transitionInput{i, action};
                for (auto belief : futureBeliefs) {
                    int s = mapStateEnum[belief.first];

                    transitionOutput[s] = belief.second;

                    file << s << " " << belief.second << " ";

                    PairStateAction bTransitionInput{s, action};
                    std::vector<int> previousBeliefs = predecessors[bTransitionInput];
                    previousBeliefs.push_back(i);
                    predecessors[bTransitionInput] = previousBeliefs;
                }
                file << "\n";
                transition[transitionInput] = transitionOutput;

                PairStateAction rewardInput{i, action};
                reward[rewardInput] = rewardFunction(vecStateEnum[i], action);
                file << reward[rewardInput] << "\n";
            }
        }
        file.close();
    }
}

string Mdp::chooseAction(int s) {
    double max = -1;
    string max_action;
    for (string a : actions) {
        double qv = getQValue(s, a);
        if (qv > max) {
            max = qv;
            max_action = a;
        }
    }
    return max_action;
}

string Mdp::chooseAction(VariableSet s) {
    return chooseAction(mapStateEnum[s]);
}

void Mdp::printQValues(VariableSet s) {
    for (string action : actions) {
        cout << action << " - " << getQValue(s, action) << "\n";
    }
}

double Mdp::getQValue(VariableSet s, string action) {
    int i = mapStateEnum[s];
    PairStateAction p{i, action};
    return qValue[p];
}

double Mdp::getQValue(int s, string action) {
    PairStateAction p{s, action};
    return qValue[p];
}


double Mdp::getTransitionProb(int s, string a, int s_new) {
    PairStateAction transition_input{s, a};
    StateProb output_states = transition[transition_input];
    for (auto so : output_states) {
        if (so.first == s_new) {
            return so.second;
        }
    }
    return 0;
}

void Mdp::simulate(int n, VariableSet initial_state) {


    int int_initial_state = mapStateEnum[initial_state];
    StateProb b;
    b[int_initial_state] = 1.0;
    for (int i = 0; i < n; i++) {
        cout << "Step " << i << "\n";
        StateProb temp_b;
        for (auto s : b) {
            cout << "State: \n";
            cout << vecStateEnum[s.first].toString();
            string action = chooseAction(s.first);
            cout << "Executing " << action << "\n";
            PairStateAction p{s.first, action};
            StateProb output = transition[p];
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
}


