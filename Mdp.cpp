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
#include <bits/stl_map.h>

Mdp::Mdp(bool use_cost) : use_cost_(use_cost) {
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

/*
 This procedure does value iteration and computes the Q-Values for the actions. At the moment we don't use the policy learnt through here,
 * preferring to use the one with sarsop. The algorithm works on enumerations of states
 */
void Mdp::valueIteration() {
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

void Mdp::create(string name, bool rewrite) {
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




//UTILITY FUNCTIONS

void Mdp::printTransitionFunction() {
    ofstream log("log.txt");
    cout << "Transition Function\n";
    for (int i = 0; i < vec_state_enum_.size(); i++) {
        VariableSet vs = vec_state_enum_[i];
        cout << "In state:\n";
        cout << vs.toString() << "\n";
        cout << "\n";
        log << "In state:\n";
        log << vs.toString() << "\n";
        log << "\n";
        for (string action : actions_) {
            PairStateAction tInput{i, action};
            StateProb tOutput = transition_[tInput];



            cout << "Executing " << action << "\n" << "Output is:\n";
            log << "Executing " << action << "\n" << "Output is:\n";
            for (auto out : tOutput) {
                if (out.first != 0) {
                    cout << "...\n";
                    log << "...\n";
                }
                VariableSet vo = vec_state_enum_[out.first];
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
    for (auto el : reward_) {
        if (el.second > 0) {
            VariableSet state = vec_state_enum_[el.first.first];
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
    for (std::map<string, std::vector<string> >::iterator i = var_values_.begin(); i != var_values_.end(); i++) {
        cout << i->first << "\n";
        cout << "Values= ";
        for (int j = 0; j < i->second.size(); j++) {
            cout << i->second[j] << " ";
        }
        cout << "\n";
    }
    cout << "\n\nPrint combinations\n";
    for (int i = 0; i < vec_state_enum_.size(); i++) {
        cout << "State " << i << "\n";
        if (vec_state_enum_[i].set.find("gluebottle_isAt") != vec_state_enum_[i].set.end()) {
            if (vec_state_enum_[i].set.at("gluebottle_isAt") != "surface3") {
                cout << "";
            }
        }
        cout << vec_state_enum_[i].toString() << "\n";
    }
}

void Mdp::enumerateStates() {
    std::vector<std::vector<int>> enumInput;
    for (string variable : variables_) {
        std::vector<int> valValues;
        for (int i = 0; i < var_values_.at(variable).size(); i++) {
            valValues.push_back(i);
        }

        enumInput.push_back(valValues);
    }

    NestedLoop<int> loop(enumInput);
    std::vector<std::vector<int>> enumOutput = loop.buildMatrix();
    for (int i = 0; i < enumOutput.size(); i++) {
        VariableSet v;
        for (int j = 0; j < enumOutput[i].size(); j++) {
            string name = variables_[j];
            std::vector<string> values = var_values_.at(name);
            v.set[name] = values[enumOutput[i][j]];
        }

        map_state_enum_[v] = i;

        vec_state_enum_.push_back(v);
    }

}

void Mdp::enumerateFunctions() {
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

void Mdp::enumerateGoalAndStartStates() {
    for (auto state : map_state_enum_) {
        if (isGoalState(state.first)) {
            goal_states_.push_back(state.second);
        }
        if (isStartingState(state.first)) {

            starting_states_.push_back(state.second);
        }
    }
}

string Mdp::chooseAction(int s) {
    double max = use_cost_ ? 10000 : -1;
    string max_action;
    for (string a : actions_) {
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
    string action = chooseAction(map_state_enum_.at(param_s));
    return getDeparametrizedAction(action);
}

void Mdp::printQValues(VariableSet s) {
    VariableSet param_s = convertToParametrizedState(s);
    cout << param_s.toString() << "\n";
    for (string action : actions_) {
        cout << action << " - " << getQValue(param_s, action) << "\n";
    }
}

double Mdp::getQValue(VariableSet s, string action) {
    VariableSet param_s = convertToParametrizedState(s);
    int i = map_state_enum_.at(param_s);
    PairStateAction p{i, action};
    return qvalue_.at(p);
}

double Mdp::getQValue(int s, string action) {
    PairStateAction p{s, action};
    return qvalue_.at(p);
}

double Mdp::getTransitionProb(int s, string a, int s_new) {
    PairStateAction transition_input{s, a};
    StateProb output_states = transition_[transition_input];
    for (auto so : output_states) {
        if (so.first == s_new) {
            return so.second;
        }
    }
    return 0;
}

void Mdp::simulate(int n, VariableSet initial_state) {

    VariableSet parametrized_vs = convertToParametrizedState(initial_state);
    int int_initial_state = map_state_enum_.at(parametrized_vs);
    StateProb b;
    b[int_initial_state] = 1.0;
    for (int i = 0; i < n; i++) {
        cout << "Step " << i << "\n";
        StateProb temp_b;
        for (auto s : b) {
            cout << "State: \n";
            cout << vec_state_enum_[s.first].toString();
            string action = chooseAction(s.first);
            cout << "Executing " << action << "\n";
            PairStateAction p{s.first, action};
            StateProb output = transition_[p];
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
    parameter_instances_ = instance;
    original_to_parametrized_.clear();
    for (string p : parameters_) {
        string this_instance = instance[p];
        for (string linked_var : parameter_variables_[p]) {
            string new_var_name = linked_var;
            new_var_name.replace(new_var_name.find(p), p.length(), this_instance);
            original_to_parametrized_[new_var_name].push_back(linked_var);
            parametrized_to_original_[linked_var] = new_var_name;
        }
        original_to_parametrized_[this_instance].push_back(p);
        parametrized_to_original_[p] = this_instance;
    }
}

string Mdp::findValue(string variable, std::vector<string> possible_values) {
    for (string v : possible_values) {
        if (std::find(var_values_.at(variable).begin(), var_values_.at(variable).end(), v) != var_values_.at(variable).end()) {
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
        std::vector<string> par_key;
        std::vector<string> possible_values;

        //check if the value is a parameter.
        if (original_to_parametrized_.find(el.second) != original_to_parametrized_.end()) {
            possible_values = original_to_parametrized_[el.second];
        }
        possible_values.push_back(el.second); //the original value is always a possibility, but it has a minor priority

        //check if it's a parameter variable
        if (original_to_parametrized_.find(el.first) != original_to_parametrized_.end()) {
            par_key = original_to_parametrized_[el.first];
        }

        //if it's a parameter variable
        if (par_key.size() > 0) {
            for (string key : par_key) {
                //                if (vs_new.set.find(key) == vs_new.set.end()) {
                string value = findValue(key, possible_values);
                if (value == "") {
                    cout << "WARNING! NO VALUE FOUND\n";
                    return VariableSet();
                }
                vs_new.set[key] = value;
                //                }
            }
        }
        //check if it's a variable
        if (std::find(variables_.begin(), variables_.end(), el.first) != variables_.end()) {
            if (vs_new.set.find(el.first) == vs_new.set.end()) {
                string value = findValue(el.first, possible_values);
                if (value == "") {
                    cout << "WARNING! NO VALUE FOUND\n";
                    return VariableSet();
                }
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
        bool was_this_abstract = false;
        string actual_key = s.first;
        string actual_value = s.second;
        if (parametrized_to_original_.find(s.first) != parametrized_to_original_.end()) {
            actual_key = parametrized_to_original_[s.first];
        }
        was_this_abstract = is_abstract_actual_key.find(actual_key) != is_abstract_actual_key.end();
        if (abstract_states_.find(s.first) != abstract_states_.end()) {

            std::vector<string> possible_abstract_values;
            for (auto abstract : abstract_states_[s.first]) {
                if (abstract.second == s.second) { //if it's an abstract value
                    possible_abstract_values.push_back(abstract.first);
                }
            }
            bool found_value = false;
            //if there is a parameter value with the same value of this abstract state it takes precedence normally. So if we have an abstract
            //state which can assume different value and one of these is a parameter, we want to look for an abstract value which is 
            //not that parameter. For example other_location => (surface1, surface2, surface3) and surface1 is also a parameter, assigned
            //to surface. If the real deparametrized value was "surface1" we would expect to have "surface" has value of this var in the mdp.
            //if it's other location it means it's another one (surface1, surface2, surface3)
            for (string av : possible_abstract_values) {
                if (original_to_parametrized_.find(av) == original_to_parametrized_.end()) { //if the abstract value is not a parameter
                    found_value = true;
                    actual_value = av;
                    break;
                } else {
                    string par = original_to_parametrized_.at(av)[0]; //if the abstract value is also a parameter
                    //if it's a parameter we take it only if it's not a value of this variable 
                    //ex surface => surface1 but the current variable can't assume the value surface
                    if (std::find(var_values_.at(s.first).begin(), var_values_.at(s.first).end(), par) == var_values_.at(s.first).end()) {
                        found_value = true;
                        actual_value = av;
                        break;
                    }
                }
            }
            if (found_value) {
                is_this_abstract = true; //we sign this because we will substitute them with real values if we have them
                is_abstract_actual_key.insert(actual_key);

            } else if (!found_value && possible_abstract_values.size() > 0) { //if not found value we take just one random?
                is_this_abstract = true;
                is_abstract_actual_key.insert(actual_key);
                actual_value = possible_abstract_values[0];
            }

        }

        if (parametrized_to_original_.find(actual_value) != parametrized_to_original_.end()) {
            actual_value = parametrized_to_original_[actual_value];
        }
        bool assigned_value = false;

        if (set.set.find(actual_key) != set.set.end()) {
            //this is the case where we had 2 variables in the mdp assigned to the same real value (for ex. two parameter variables)
            //and only one of those had an abstract value. In this case we take the non abstract
            if (is_abstract_actual_key.find(actual_key) != is_abstract_actual_key.end() && !is_this_abstract) {
                set.set[actual_key] = actual_value;
                is_abstract_actual_key.erase(is_abstract_actual_key.find(actual_key));
            }
        } else {
            set.set[actual_key] = actual_value;
            assigned_value = true;
        }
        if (!assigned_value && is_abstract_actual_key.find(actual_key) != is_abstract_actual_key.end()
                && !was_this_abstract) {
            is_abstract_actual_key.erase(is_abstract_actual_key.find(actual_key));
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

    std::vector<string> action_parts = StringOperations::stringSplit(action_name, '_');
    stringstream depar_action_name;
    for (int i = 0; i < action_parts.size() - 1; i++) {
        string s = action_parts[i];
        if (parametrized_to_original_.find(s) != parametrized_to_original_.end()) {
            depar_action_name << parametrized_to_original_[s] << "_";
        } else {
            depar_action_name << s << "_";
        }
    }
    if (action_parts.size() > 0) {
        string s = action_parts[action_parts.size() - 1];
        if (parametrized_to_original_.find(s) != parametrized_to_original_.end()) {
            depar_action_name << parametrized_to_original_[s];
        } else {
            depar_action_name << s;
        }
    }
    return depar_action_name.str();
}

string Mdp::getParametrizedAction(string action_name) {
    std::vector<string> action_parts = StringOperations::stringSplit(action_name, '_');
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
    for (string a : actions_) {
        cout << a << endl;
    }
    cout << endl;
}

void Mdp::printParameters() {
    cout << "List of Parameters:" << endl;
    for (string p : parameters_) {
        cout << p << endl;
    }
    cout << endl;
    cout << "List of parameter variables" << endl;
    for (auto p : parameter_variables_) {
        cout << "-" << p.first << ":" << endl;
        for (string pp : p.second) {
            cout << pp << endl;
        }
        cout << endl;
    }
    cout << endl;
    cout << "Parameter instances" << endl;
    for (auto s : parameter_instances_) {
        cout << s.first << " " << s.second << endl;
    }
    cout << endl;
    cout << "Original to parametrized" << endl;
    for (auto s : original_to_parametrized_) {
        cout << "-" << s.first << ":" << endl;
        for (string ss : s.second) {
            cout << ss << endl;
        }
    }
    cout << endl;
    cout << "Parametrized to original" << endl;
    for (auto s : parametrized_to_original_) {
        cout << s.first << " " << s.second << endl;
    }
    cout << endl;
}

int Mdp::getBestQ(VariableSet state) {
    int best = use_cost_ ? 1000 : 0;

    for (string a : actions_) {
        int q = getQValue(state, a);
        if (use_cost_ && q < best) {
            best = q;
        } else if (!use_cost_ && q > best) {
            best = q;
        }
    }
    return best;
}

VarStateProb Mdp::getFutureStates(VariableSet state, string action) {
    VariableSet v_par = convertToParametrizedState(state);
    VarStateProb result;
    PairStateAction p;
    p.first = map_state_enum_.at(v_par);
    p.second = action;
    StateProb fs = transition_[p];
    for (auto s : fs) {
        result[vec_state_enum_[s.first]] = s.second;
    }
    return result;

}

std::vector<string> Mdp::getAbstractLinkedValues(string var, string abstract_value) {
    std::vector<string> result;
    if (abstract_states_.find(var) != abstract_states_.end()) {
        for (auto s : abstract_states_[var]) {
            if (s.second == abstract_value) {
                result.push_back(s.first);
            }
        }
    }
    return result;

}

int Mdp::getReward(VariableSet state, string action) {
    VariableSet v_par = convertToParametrizedState(state);
    PairStateAction p;
    p.first = map_state_enum_.at(v_par);
    p.second = action;
    return reward_.at(p);
}

void Mdp::writeModel(string file_name) {
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
            of << variable_parameter_.at(v);
        } else {
            of << "\n";
        }
    }
    of << "parameter\n";
    for (std::string p : parameters_) {
        of << p << " ";
        for (std::string v : parameter_variables_.at(p)) {
            of << v << " ";
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
            of << abs_map.first << " " << abs_map.second;
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

void Mdp::writePolicy(string file_name) {
    ofstream of(file_name);
    for (auto q : qvalue_) {
        of << q.first.first << " " << q.first.second << " " << q.second << "\n";
    }
    of.close();
}

void Mdp::readModel(string file_name) {
    ifstream f(file_name);

    string line;

    getline(f, name_);
    getline(f, line); //skip "variables line"
    getline(f, line);
    while (line != "parameter") {
        string var_name = line; //first line is the var name
        variables_.push_back(var_name);
        getline(f, line); //second line is the var values
        var_values_[var_name] = StringOperations::stringSplit(line, ' ');
        getline(f, line); //third line is the parameter
        if (line != "notpar") {
            variable_parameter_[var_name] = line;
        }
        getline(f, line); //either other var or start of parameters
    }
    while (line != "parameter action place") {
        vector<string> par_stuff = StringOperations::stringSplit(line, ' ');
        string par_name = par_stuff[0];
        parameters_.push_back(par_name);
        for (int i = 1; i < par_stuff.size(); i++) {
            parameter_variables_[par_name].push_back(par_stuff[i]);
        }
        getline(f, line); //new parameter or parameter action place
    }
    vector<string> v_par_action_place = StringOperations::stringSplit(line, ' ');
    for (int i = 0; i < v_par_action_place.size() - 1; i = i + 2) {
        int pos = stoi(v_par_action_place[i]);
        parameter_action_place_[pos] = v_par_action_place[i + 1];
    }
    getline(f, line); //starting states
    getline(f, line);
    vector<string> v_starting_s = StringOperations::stringSplit(line, ' ');
    for (string ss : v_starting_s) {
        starting_states_.push_back(stoi(ss));
    }
    getline(f, line); //goal states
    getline(f, line);
    vector<string> v_goal_s = StringOperations::stringSplit(line, ' ');
    for (string gs : v_goal_s) {
        starting_states_.push_back(stoi(gs));
    }
    getline(f, line); //abstract states
    getline(f, line);
    while (line != "actions") {
        string abstract_name = line;
        map<string, string> map_abstract;
        getline(f, line); //values
        vector<string> v_map = StringOperations::stringSplit(line, ' ');
        for (int i = 0; i < v_map.size() - 1; i = i + 2) {
            map_abstract[v_map[i]] = v_map[i + 1];
        }
               
        abstract_states_[abstract_name] = map_abstract;
        getline(f, line);
    }
    actions_ = StringOperations::stringSplit(line, ' ');
    getline(f, line); //transition
    getline(f, line);
    while (line != "reward") {
        PairStateAction p;
        vector<string> p_v = StringOperations::stringSplit(line, ' ');
        p.first = stoi(p_v[0]);
        p.second = p_v[1];

        getline(f, line);
        StateProb state_prob;

        vector<string> state_prob_v = StringOperations::stringSplit(line, ' ');
        for (int i = 0; i < state_prob_v.size() - 1; i = i + 2) {
            state_prob[stoi(state_prob_v[i])] = stod(state_prob_v[i + 1]);
        }
        getline(f, line);
    }
    while (getline(f, line)) {
        PairStateAction p;
        vector<string> v_r = StringOperations::stringSplit(line, ' ');
        p.first = stoi(v_r[0]);
        p.second = v_r[1];
        reward_[p] = stoi(v_r[2]);
    }
    f.close();
}

void Mdp::readPolicy(string file_name) {
    ifstream f(file_name);
    string line;
    while (getline(f, line)) {
        vector<string> v_q = StringOperations::stringSplit(line, ' ');
        PairStateAction p;
        p.first = stoi(v_q[0]);
        p.second = v_q[1];
        qvalue_[p] = stod(v_q[2]);
    }
    f.close();
}