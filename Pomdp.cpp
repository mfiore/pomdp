/* 
 * File:   Pomdp.cpp
 * Author: mfiore
 * 
 * Created on August 18, 2014, 11:53 AM
 */

#include "Pomdp.h"
#include "AlphaVector.h"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <stack>
#include <utility>
#include <algorithm>
#include <c++/4.6/bits/stl_queue.h>

Pomdp::Pomdp() {

    //    this->varValues=varValues;
    //    for (auto var:varValues) {
    //        variables.push_back(var.first);
    //    }
    //    this->hiddenVariables=hiddenVariables;
    //    this->observedVariables=observedVariables;
    //    for (auto var:observationValues) {
    //        observationVariables.push_back(var.first);
    //    }
    //    this->observationValues=observationValues;
    //    this->actions=actions;
}

Pomdp::Pomdp(const Pomdp& orig) {
}

Pomdp::~Pomdp() {
}

/* Applies the observation function to the belief-
 * 
 * observations is the set of observation with their values
 * action is the current action
 * updated belief is the starting belief of the function, which will be updated. (see transition function for the explanation of this).
 * 
 * Notes: was not working when we are dealing with an MDP. Did a change in the function but still didn't test. In general I don't remember this function
 * and I should redo it
 */
std::map<VariableSet, double> Pomdp::applyObservationFunction(std::map<string, string> observations, string action, std::map<VariableSet, double> updatedBelief) {
    std::map<VariableSet, double> beliefWithObservations;

    //    for (std::map<VariableSet, double>::iterator j = updatedBelief.begin(); j != updatedBelief.end(); j++) {
    //        //for each belief
    //        for (int k = 0; k<this->observationVariables.size(); k++) {
    //            //for each observation variable
    //            //prepare the transition
    //            TransitionInput input;
    //            TransitionOutput output;
    //
    //            input.state = j->first;
    //            input.state.set[actionVar] = action;
    //            input.var = this->observationVariables[k];
    //            output = observationProb[input]; //get the probability of the observation
    //
    //            for (int l = 0; l < output.futureStates.size(); l++) {
    //                if (output.futureStates[l] == observations[input.var]) { //don't remember why this check is here T_T
    //                    j->second = j->second * output.probabilities[l]; //edit the probability of the belief
    //                    if (j->second != 0) {
    //                        //if the resulting belief has probability zero, we insert it in a new variable which we will return
    //                        beliefWithObservations[j->first] = j->second;
    //                    }
    //                    break;
    //                }
    //            }
    //        }
    //    }
    return beliefWithObservations;
}

/* This function filters the current belief with the observed states, erasing the belief that don't respect them.
 
 * obsStates: the observed states and their values
 */
void Pomdp::filterBelief(std::map<string, string> obsStates) {
    std::map<int, double> newBelief;
    bool checkObservedVariables = true;

    for (auto currentBelief : belief) {
        checkObservedVariables = true;

        //we check if all the observed variables in the belief are equal the observed variables in input
        VariableSet v = vecStateEnum[currentBelief.first];
        for (auto j : obsStates) {
            if (v.set.at(j.first) != j.second) {
                checkObservedVariables = false;
                break;
            }
        }
        //if so we keep the belief
        if (checkObservedVariables) {
            newBelief[currentBelief.first] = currentBelief.second;

        }
    }
    //normalize
    double sum = 0;
    for (auto j : newBelief) {
        sum = sum + j.second;
    }
    for (auto j : newBelief) {
        j.second = j.second / sum;
    }

    belief = newBelief;
}

/*
 This function updates the current system belief
 * 
 * action: current action
 * obsStates: set of observed variables
 * observations: set of observations
 */
void Pomdp::updateBelief(string action, std::map<string, string> obsStates, std::map<string, string> observations) {
    std::map<int, double> finalBelief;

    //for each current belief
    for (auto currentBelief : belief) {
        int s = currentBelief.first;

        pair<int, string> transitionInput{s, action};
        std::map<int, double> futureBeliefs = transition[transitionInput];

        for (auto aBelief : futureBeliefs) {
            finalBelief[aBelief.first] = finalBelief[aBelief.first] + aBelief.second;
        }
        //normalize
        double sum = 0;
        for (auto aBelief : finalBelief) {
            sum = sum + aBelief.second;
        }
        for (auto aBelief : finalBelief) {
            aBelief.second = aBelief.second / sum;
        }
    }
    this->belief = finalBelief;

}

/*
 Executes a bellman backup on state i that is max a [R(i,a) + L * sum T(i,a,s) vhi(s)]
 * returns the new v values and updates the q values
 */
int Pomdp::bellmanBackup(int i, std::vector<double> vhi) {
    double maxValue = 0;
    for (string action : actions) { //for every action
        pair<int, string> rInput{i, action}; //calculate the reward of this state with this action

        int currentReward;
        //we have to do this because the std::map increments of one element if we just use [] with a non existing member and the reward function
        //contains only transition with non zero rewards.
        if (reward.find(rInput) != reward.end()) {
            currentReward = reward[rInput];
        } else {
            currentReward = 0;
        }
        pair<int, string> transitionInput{i, action};
        std::map<int, double> futureBeliefs = transition[transitionInput];

        double sum = 0;
        for (auto aBelief : futureBeliefs) {
            sum = sum + aBelief.second * vhi[aBelief.first]; //sum on the probabilities of the future states * the value of reaching that state
        }
        pair<int, string> qInput{i, action};
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
void Pomdp::valueIteration(bool rewrite) {
    string fileName = name + ".policy";
    ifstream inputFile(fileName);
    
    std::vector<double> vhi(vecStateEnum.size(), 0); //vhi mantains the predicted reward in a state following the optimal policy
    std::vector<double> vhiOld(vecStateEnum.size(), 0);

    if (inputFile.good() && !rewrite) {
        string line;
        for (int i = 0; i < vecStateEnum.size(); i++) {
            int i1 = 0;
            getline(inputFile, line);
            int i2 = line.find(" ", i1);
            string vs = line.substr(i1, i2 - i1);
            v_vector[i] = stod(vs);
            i1 = i2 + 1;        
            for (string action : actions) {
                int i2 = line.find(" ", i1);
                string qv = line.substr(i1, i2 - i1);
                pair<int, string> qInput{i, action};
                qValue[qInput] = stod(qv);
                i1 = i2 + 1;
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
            for (int i = 0; i < vhi.size(); i++) {  //calculate the maximum difference on the vhi (stopping parameter)
                double d = abs(vhi[i] - vhiOld[i]);
                if (d > maxDiff) {
                    maxDiff = d;
                }
            }

        } while (maxDiff > epsilon);
        v_vector=vhi;
        ofstream file(fileName);
        for (int i = 0; i < vecStateEnum.size(); i++) {
            file<<vhi[i]<<" ";
            for (string action : actions) {
                pair<int, string> qInput{i, action};
                file << qValue[qInput] << " ";
            }
            file << "\n";
        }
        file.close();
    }
}

void Pomdp::prioritizedSweeping() {
    std::vector<double> vhi(vecStateEnum.size(), 0);
    std::vector<double> vhiOld(vecStateEnum.size(), 0);
    double epsilon = 0.1;
    PriorityQueue pq;
    for (int i = 0; i < vecStateEnum.size(); i++) {
        pq.pushElement(pair<int, double>{i, 1});
    }
    //    for (int i = 0; i < goalStates.size(); i++) {
    //        pq.pushElement(pair<int, double>{i, 1});
    //    }

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
            pair<int, string> predInput{s, action};
            std::vector<int> vsp = predecessors[predInput];

            for (int sp : vsp) {
                if (isVisited[sp] == false) {
                    isVisited[sp] = true;

                    double maxValue = 0;
                    for (string action2 : actions) {
                        pair<int, string> transitionInput{sp, action2};
                        std::map<int, double> futureStates = transition[transitionInput];
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

void Pomdp::printTransitionFunction() {
    cout << "Transition Function\n";
    for (int i = 0; i < vecStateEnum.size(); i++) {
        for (string action : actions) {
            pair<int, string> tInput{i, action};
            std::map<int, double> tOutput = transition[tInput];

            VariableSet vs = vecStateEnum[i];
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

void Pomdp::printRewardFunction() {
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

void Pomdp::printStates() {
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

void Pomdp::printBelief() {
    int k = 0;
    for (auto b : belief) {
        VariableSet vs = vecStateEnum[b.first];
        k++;
        cout << "belief " << k << "\n";
        cout << "probability " << b.second << "\n";
        for (auto j : vs.set) {
            cout << j.first << " " << j.second << " ";
        }
        cout << "\n";
    }
}

void Pomdp::createPomdp(string name, bool rewrite) {
    this->name = name;

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
        //        if (v.set["cup_capacity"] == "0" && v.set["cup_containsWater"] == "0" && v.set["cup_isAt"] == "table" && v.set["cup_isHot"] == "0"
        //                && v.set["glass_capacity"] == "1" && v.set["glass_containsWater"] == "1" && v.set["glass_isAt"] == "table" && v.set["glass_isHot"] == "0"
        //                && v.set["human_isAt"] == "table" && v.set["waterBottle_capacity"] == "0" && v.set["waterBottle_isAt"] == "human") {
        //            cout << "ah" << "\n";
        //            int k = mapStateEnum[v];
        //            cout << k << "\n";
        //        }
        vecStateEnum.push_back(v);
    }

    ifstream inputFile(fileName);
    if (inputFile.good() && !rewrite) {
        for (int i = 0; i < vecStateEnum.size(); i++) {
            for (string action : actions) {
                pair<int, string> transitionInput{i, action};
                std::map<int, double> transitionOutput;


                string line;
                getline(inputFile, line);
                int i1, i2;
                i1 = 0;

                pair<int, string> bTransitionInput{i, action};

                i1 = i2 + 1;
                while ((i2 = line.find(" ", i1)) != string::npos) {
                    string s = line.substr(i1, i2 - i1);
                    i1 = i2 + 1;
                    i2 = line.find(" ", i1);
                    string b = line.substr(i1, i2 - 1);
                    i1 = i2 + 1;
                    transitionOutput[stoi(s)] = stod(b);

                    std::vector<int> previousBeliefs = predecessors[bTransitionInput];
                    previousBeliefs.push_back(stoi(s));
                    predecessors[bTransitionInput] = previousBeliefs;

                }
                //                string b = line.substr(i1);
                //                transitionOutput[stoi(s)] = stod(b);

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
            if (isGoalState(vecStateEnum[i])) {
                goalStates.push_back(i);
            }

            for (string action : actions) {
                std::map<VariableSet, double>futureBeliefs = transitionFunction(vecStateEnum[i], action);
                std::map<int, double> transitionOutput;

                pair<int, string> transitionInput{i, action};
                for (auto belief : futureBeliefs) {

                    //                    if (i == 528 && action == "human_fill_glass_waterBottle") {
                    //                        for (auto s : belief.first.set) {
                    //                            cout << s.first << " " << s.second << "\n";
                    //                        }
                    //                        
                    //                        VariableSet tv=vecStateEnum[578];
                    //                        for (auto s:tv.set) {
                    //                            cout<<s.first<<" "<<s.second<<"\n";
                    //                        }
                    //                    }


                    int s = mapStateEnum[belief.first];

                    transitionOutput[s] = belief.second;

                    file << s << " " << belief.second << " ";

                    pair<int, string> bTransitionInput{s, action};
                    std::vector<int> previousBeliefs = predecessors[bTransitionInput];
                    previousBeliefs.push_back(i);
                    predecessors[bTransitionInput] = previousBeliefs;
                }
                file << "\n";
                transition[transitionInput] = transitionOutput;



                pair<int, string> rewardInput{i, action};
                reward[rewardInput] = rewardFunction(vecStateEnum[i], action);
                if (reward[rewardInput]>1000) {
                    cout<<"wtf!\n";
                }
                file << reward[rewardInput] << "\n";
            }
        }
        file.close();
        //        cout << predecessors.size() << "\n";
    }
}

string Pomdp::chooseAction() {
    return chooseAction(belief);
}

string Pomdp::chooseAction(int s) {
    map<int,double> a_belief;
    a_belief[s]=1.0;
    return chooseAction(a_belief);
}


string Pomdp::chooseAction(map<int,double> belief) {
    string bestAction;
    double maxValue = -1;
    std::map<string, int> actionValues;
    for (auto aBelief : belief) {
        int s = aBelief.first;
        double prob = aBelief.second;
        for (string action : actions) {
            pair<int, string> qInput{s, action};
            actionValues[action] = actionValues[action] + qValue[qInput] * prob;
        }
    }
    for (auto av : actionValues) {
        if (av.second > maxValue) {
            maxValue = av.second;
            bestAction = av.first;
        }
    }

    return bestAction;
}


void Pomdp::setInitialState(std::vector<pair<VariableSet, double>> initialBelief) {
    for (auto aBelief : initialBelief) {
        int s = mapStateEnum[aBelief.first];
        belief[s] = aBelief.second;
    }
}

double Pomdp::getActionValue(string action) {
    int actionValue = 0;
    int i = 0;
    for (auto b : belief) {
        i++;
        actionValue = actionValue + qValue[pair<int, string>(b.first, action)] * b.second;
    }
    return actionValue / i;
}

void Pomdp::printActualQValues() {
    std::map<string, int> actionValues;
    for (auto aBelief : belief) {
        int s = aBelief.first;
        double prob = aBelief.second;
        for (string action : actions) {
            pair<int, string> qInput{s, action};
            actionValues[action] = actionValues[action] + qValue[qInput] * prob;
        }
    }
    cout << "Q-Values\n";
    for (auto av : actionValues) {
        cout << av.first << " " << av.second << "\n";
    }

}

std::vector<double> Pomdp::getV_vector() const {
    return v_vector;
}

std::map<string, double> Pomdp::getNormQValue() {
    std::map<string, double> result, result2;

    for (auto aBelief : belief) {
        int s = aBelief.first;
        double prob = aBelief.second;
        for (string action : actions) {
            pair<int, string> qInput{s, action};
            result[action] = result[action] + qValue[qInput] * prob;
        }
    }
    double sum = 0;
    for (auto r : result) {
        sum = sum + r.second;
    }
    for (auto r : result) {
        result2[r.first] = r.second / sum;
    }
    return result2;
}

double Pomdp::getQValue(string action) {
    double result=0;
    for (auto aBelief : belief) {
        int s = aBelief.first;
        double prob = aBelief.second;
        pair<int, string> qInput{s, action};
        result = result + qValue[qInput] * prob;
    }
    if (result<0) {
        result=0;
    }
    return result;
}

double Pomdp::getTransitionProb(int s, string a, int s_new) {
    pair<int,string> transition_input {s,a};
    std::map<VariableSet,double> output_states=transitionFunction(vecStateEnum[s],a);

    for (auto so:output_states) {
        if (mapStateEnum[so.first]==s_new) {
            return so.second;
        }
    }
    return 0;
}

