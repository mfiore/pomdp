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
#include <set>

Mdp::Mdp() {
    use_cost_ = false;
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
    double maxValue = use_cost_ ? 10000 : 0;
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
        double havNew;
        if (use_cost_) {
            havNew = 1 + sum;
        } else {
            havNew = currentReward + 0.3 * sum; //0.3 weights the future rewards
        }
        qValue[qInput] = havNew; //update the human action value
        if (qValue[qInput] > maxValue && !use_cost_) {
            maxValue = qValue[qInput];
        } else if (qValue[qInput] < maxValue && use_cost_) {
            maxValue = qValue[qInput];
        }

    }
    return maxValue;
}

/*
 This procedure does value iteration and computes the Q-Values for the actions. At the moment we don't use the policy learnt through here,
 * preferring to use the one with sarsop. The algorithm works on enumerations of states
 */
void Mdp::valueIteration(string fileName, bool rewrite) {
    ifstream inputFile(fileName);

    if (fileName == "agent_glue_surface-agent_glue_surface.policy") {
        cout << "";
    }
    int starting_value = use_cost_ ? 100000 : 0;
    std::vector<double> vhi(vecStateEnum.size(), starting_value); //vhi mantains the predicted reward in a state following the optimal policy
    std::vector<double> vhiOld(vecStateEnum.size(), starting_value);

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

                if (use_cost_ && isGoalState(vecStateEnum[s])) {
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
    ofstream log("log.txt");
    cout << "Transition Function\n";
    for (int i = 0; i < vecStateEnum.size(); i++) {
        VariableSet vs = vecStateEnum[i];
        cout << "In state:\n";
        cout << vs.toString() << "\n";
        cout << "\n";
        log << "In state:\n";
        log << vs.toString() << "\n";
        log << "\n";
        for (string action : actions) {
            PairStateAction tInput{i, action};
            StateProb tOutput = transition[tInput];



            cout << "Executing " << action << "\n" << "Output is:\n";
            log << "Executing " << action << "\n" << "Output is:\n";
            for (auto out : tOutput) {
                if (out.first != 0) {
                    cout << "...\n";
                    log << "...\n";
                }
                VariableSet vo = vecStateEnum[out.first];
                cout << vo.toString() << "\n";
                cout << "\n";
                cout << out.first << "\n";
                log << vo.toString() << "\n";
                log << "\n";
                log << out.first << "\n";
            }
            cout << "\n";
            log << "\n";
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
            if (state.set["bracket2_isAt"] == "agentp0" && state.set["surface1_status"] == "completed"
                    && state.set["surface2_status"] == "completed" && state.set["surface3_status"] == "glued") {
                cout << "";
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
    cout << "\n\nPrint combinations\n";
    for (int i = 0; i < vecStateEnum.size(); i++) {
        cout << "State " << i << "\n";
        if (vecStateEnum[i].set.find("gluebottle_isAt") != vecStateEnum[i].set.end()) {
            if (vecStateEnum[i].set.at("gluebottle_isAt") != "surface3") {
                cout << "";
            }
        }
        cout << vecStateEnum[i].toString() << "\n";
    }
}

void Mdp::enumerateStates() {
    std::vector<std::vector<int>> enumInput;
    for (string variable : variables) {
        std::vector<int> valValues;
        for (int i = 0; i < varValues.at(variable).size(); i++) {
            valValues.push_back(i);
        }

        enumInput.push_back(valValues);
    }

    NestedLoop<int> loop(enumInput);
    std::vector<std::vector<int>> enumOutput = loop.buildMatrix();
    for (int i = 0; i < enumOutput.size(); i++) {
        VariableSet v;
        for (int j = 0; j < enumOutput[i].size(); j++) {
            string name = variables[j];
            std::vector<string> values = varValues.at(name);
            v.set[name] = values[enumOutput[i][j]];
        }

        mapStateEnum[v] = i;

        vecStateEnum.push_back(v);
    }

}

bool Mdp::readMdp(string fileName, bool rewrite) {

    ifstream inputFile(fileName);
    if (inputFile.good() && !rewrite) {
        for (int i = 0; i < vecStateEnum.size(); i++) {
            for (string action : actions) {
                PairStateAction transitionInput{i, action};
                StateProb transitionOutput;

                string line;
                getline(inputFile, line);
                vector<string> transition_v = StringOperations::stringSplit(line, ' ');
                int j = 0;

                PairStateAction bTransitionInput{i, action};

                while (j < transition_v.size()) {
                    transitionOutput[stoi(transition_v[j])] = stod(transition_v[j + 1]);
                    std::vector<int> previousBeliefs = predecessors[bTransitionInput];
                    previousBeliefs.push_back(stoi(transition_v[j]));
                    predecessors[bTransitionInput] = previousBeliefs;

                    j = j + 2;
                }

                transition[transitionInput] = transitionOutput;

                getline(inputFile, line);
                PairStateAction rewardInput = {j, action};
                reward[rewardInput] = stoi(line);
            }
        }
        inputFile.close();
        return true;
    }
    return false;
}

void Mdp::enumerateFunctions(string fileName) {
    ofstream file(fileName);
    cout << "Starting Enumeration\n";
    for (int i = 0; i < vecStateEnum.size(); i++) {
        for (string action : actions) {
            std::map<VariableSet, double>futureBeliefs = transitionFunction(vecStateEnum[i], action);
            StateProb transitionOutput;

            PairStateAction transitionInput{i, action};
            for (auto belief : futureBeliefs) {
                int s = mapStateEnum.at(belief.first);

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

void Mdp::create(string name, bool rewrite) {
    this->name = name;

    string fileName = name + ".mdp";


    //    //calculate inverse parameter variables 
    //    for (string p : parameters) {
    //        for (string v : parameter_variables[p]) {
    //            variable_parameter[v] = p;
    //        }
    //    }

    enumerateStates();
    bool read_mdp = readMdp(fileName, rewrite);
    if (!read_mdp) {
        enumerateFunctions(fileName);
    }

    valueIteration(name + ".policy", rewrite);
}

string Mdp::chooseAction(int s) {
    double max = use_cost_ ? 10000 : -1;
    string max_action;
    for (string a : actions) {
        double qv = getQValue(s, a);
        if (qv > max && !use_cost_) {
            max = qv;
            max_action = a;
        } else if (qv < max && use_cost_) {
            max = qv;
            max_action = a;
        }
    }
    return max_action;
}

string Mdp::chooseAction(VariableSet s) {
    VariableSet param_s = convertToParametrizedState(s);
    string action = chooseAction(mapStateEnum.at(param_s));
    return getDeparametrizedAction(action);
}

void Mdp::printQValues(VariableSet s) {
    VariableSet param_s = convertToParametrizedState(s);
    cout << param_s.toString() << "\n";
    for (string action : actions) {
        cout << action << " - " << getQValue(param_s, action) << "\n";
    }
}

double Mdp::getQValue(VariableSet s, string action) {
    VariableSet param_s = convertToParametrizedState(s);
    int i = mapStateEnum.at(param_s);
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

    VariableSet parametrized_vs = convertToParametrizedState(initial_state);
    int int_initial_state = mapStateEnum.at(parametrized_vs);
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

void Mdp::assignParameters(std::map<string, string> instance) {
    parameter_instances = instance;
    original_to_parametrized.clear();
    for (string p : parameters) {
        string this_instance = instance[p];
        for (string linked_var : parameter_variables[p]) {
            string new_var_name = linked_var;
            new_var_name.replace(new_var_name.find(p), p.length(), this_instance);
            original_to_parametrized[new_var_name].push_back(linked_var);
            parametrized_to_original[linked_var] = new_var_name;
        }
        original_to_parametrized[this_instance].push_back(p);
        parametrized_to_original[p] = this_instance;
    }
}

string Mdp::findValue(string variable, vector<string> possible_values) {
    for (string v : possible_values) {
        if (std::find(varValues.at(variable).begin(), varValues.at(variable).end(), v) != varValues.at(variable).end()) {
            return v;
        }
    }
    //we privilege real possible values over abstract variables. This is needed because in some cases we might have an abstract state
    //encompassing several values (ex. location1, location2, location3) and then another value set to a parameter
    if (abstract_states_.find(variable) != abstract_states_.end()) {

        for (string v : possible_values) {
            if (abstract_states_[variable].find(v) != abstract_states_[variable].end()) {
                return abstract_states_[variable][v];
            }
        }
    }
    return "";
}


//converts to parametrized set. Automatically removes variables that are not present

VariableSet Mdp::convertToParametrizedState(VariableSet s) {
    VariableSet vs_new;
    //for each variable in the set
    for (auto el : s.set) {
        vector<string> par_key;
        vector<string> possible_values;

        if (original_to_parametrized.find(el.first) != original_to_parametrized.end()) {
            par_key = original_to_parametrized[el.first];
        }
        if (original_to_parametrized.find(el.second) != original_to_parametrized.end()) {
            possible_values = original_to_parametrized[el.second];
        }
        possible_values.push_back(el.second);
        //if it's a parameter variable
        if (par_key.size() > 0) {
            for (string key : par_key) {
                if (vs_new.set.find(key) == vs_new.set.end()) {
                    string value = findValue(key, possible_values);
                    if (value=="") return VariableSet();
                    vs_new.set[key] = value;
                }
            }
        }
        if (std::find(variables.begin(), variables.end(), el.first) != variables.end()) {
            if (vs_new.set.find(el.first) == vs_new.set.end()) {
                string value = findValue(el.first, possible_values);
                if (value=="") return VariableSet();
                vs_new.set[el.first] = value;
            }
        }
    }
    return vs_new;
}

VariableSet Mdp::convertToDeparametrizedState(VariableSet parameter_set, VariableSet full_state) {
    VariableSet set;

    std::set<string> is_abstract_actual_key;
    for (auto s : parameter_set.set) {
        bool is_this_abstract = false;

        string actual_key = s.first;
        string actual_value = s.second;
        if (parametrized_to_original.find(s.first) != parametrized_to_original.end()) {
            actual_key = parametrized_to_original[s.first];
        }
        if (abstract_states_.find(s.first) != abstract_states_.end()) {
            for (auto abstract : abstract_states_[s.first]) {
                if (abstract.second == s.second) {
                    actual_value = abstract.first;
                    is_this_abstract = true;
                    is_abstract_actual_key.insert(actual_key);
                    break;
                }
            }
        }

        if (parametrized_to_original.find(actual_value) != parametrized_to_original.end()) {
            actual_value = parametrized_to_original[s.second];
        }

        if (set.set.find(actual_key) != set.set.end()) {
            if (is_abstract_actual_key.find(actual_key) != is_abstract_actual_key.end() && !is_this_abstract) {
                set.set[actual_key] = actual_value;
            }
        } else {
            set.set[actual_key] = actual_value;
        }
    }
    //we change "other" or abstract values with the full state versions and reintroduce missing variables
    for (auto s : full_state.set) {
        if (set.set.find(s.first) != set.set.end() && (is_abstract_actual_key.find(s.first) != is_abstract_actual_key.end() ||
                set.set.at(s.first) == "other")) {
            set.set[s.first] = s.second;
        } else if (set.set.find(s.first) == set.set.end()) {
            set.set[s.first] = s.second;
        }
    }

    return set;
}

string Mdp::getDeparametrizedAction(string action_name) {

    vector<string> action_parts = StringOperations::stringSplit(action_name, '_');
    stringstream depar_action_name;
    for (int i = 0; i < action_parts.size() - 1; i++) {
        string s = action_parts[i];
        if (parametrized_to_original.find(s) != parametrized_to_original.end()) {
            depar_action_name << parametrized_to_original[s] << "_";
        } else {
            depar_action_name << s << "_";
        }
    }
    if (action_parts.size() > 0) {
        string s = action_parts[action_parts.size() - 1];
        if (parametrized_to_original.find(s) != parametrized_to_original.end()) {
            depar_action_name << parametrized_to_original[s];
        } else {
            depar_action_name << s;
        }
    }
    return depar_action_name.str();
}

string Mdp::getParametrizedAction(string action_name) {
    vector<string> action_parts = StringOperations::stringSplit(action_name, '_');
    stringstream param_action_name;

    param_action_name << "agent_" << action_parts[1];
    if (action_parts.size() > 2) {
        param_action_name << "_object";
    }
    if (action_parts.size() > 3) {
        param_action_name << "_support";
    }
    return param_action_name.str();

}

void Mdp::printActions() {
    cout << "Actions are:" << endl;
    for (string a : actions) {
        cout << a << endl;
    }
    cout << endl;
}

void Mdp::printParameters() {
    cout << "List of Parameters:" << endl;
    for (string p : parameters) {
        cout << p << endl;
    }
    cout << endl;
    cout << "List of parameter variables" << endl;
    for (auto p : parameter_variables) {
        cout << "-" << p.first << ":" << endl;
        for (string pp : p.second) {
            cout << pp << endl;
        }
        cout << endl;
    }
    cout << endl;
    cout << "Parameter instances" << endl;
    for (auto s : parameter_instances) {
        cout << s.first << " " << s.second << endl;
    }
    cout << endl;
    cout << "Original to parametrized" << endl;
    for (auto s : original_to_parametrized) {
        cout << "-" << s.first << ":" << endl;
        for (string ss : s.second) {
            cout << ss << endl;
        }
    }
    cout << endl;
    cout << "Parametrized to original" << endl;
    for (auto s : parametrized_to_original) {
        cout << s.first << " " << s.second << endl;
    }
    cout << endl;
}
