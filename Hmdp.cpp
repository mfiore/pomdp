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
#include "Mmdp.h"

Hmdp::Hmdp() {
    active_module = "this";
    is_hierarchy_cached_ = false;
}

Hmdp::Hmdp(const Hmdp& orig) {
}

Hmdp::~Hmdp() {
}

double Hmdp::getHierarchicRewardFromFile(int i) {
    double r = use_cost_ ? 1000 : 0;
    bool found = false;
    ifstream i_file(hierarchic_file_);
    if (i_file.good()) {
        string reward_line;
        getline(i_file, reward_line);
        vector<double> rewards = StringOperations::split(reward_line, ' ');
        int pos = 0;
        for (int s : starting_states_) {
            if (i == s) {
                r = rewards[pos];
                break;
            }
            pos++;
        }
    }
    i_file.close();
    return r;

}

double Hmdp::getHierarchicReward(int i) {
    return hierarchic_reward_[i];
}

double Hmdp::getHierarchicReward(VariableSet set, Hmdp * super_hmdp) {

    VariableSet parametrized_set = convertToParametrizedState(set, super_hmdp);
    if (parametrized_set.set.size() == 0) {
        int c = use_cost_ ? 1000 : 0;
        return c;
    }
    int i = map_state_enum_.at(parametrized_set);

    if (is_hierarchy_cached_) {
        double r;
        if (hierarchic_reward_.find(i) == hierarchic_reward_.end()) {
            r = use_cost_ ? 1000 : 0;
        } else {
            r = hierarchic_reward_.at(i);
        }
        return r;
    } else {
        return getHierarchicRewardFromFile(i);
    }

}

std::map<int, double> Hmdp::getHierarchicTransition(int i) {
    std::map<int, double> result;

    for (int g : goal_states_) {
        pair<int, int> hierachic_input{i, g};
        double prob = hierarchic_transition_[hierachic_input];
        if (prob > 0) {
            result[g] = prob;
        }
    }
    return result;

}

std::map<VariableSet, double> Hmdp::getHierarchicTransitionFromFile(int i, VariableSet original_set) {
    std::map<VariableSet, double> temp_result;

    ifstream i_file(hierarchic_file_);
    if (i_file.good()) {
        string reward_line;
        getline(i_file, reward_line); //reward 
        for (int g : goal_states_) {
            string goal_line;
            getline(i_file, goal_line);
            vector<double> goal_transitions = StringOperations::split(goal_line, ' ');
            int pos = 0;
            for (int s : starting_states_) {
                if (s == i) break;
                pos++;
            }
            double prob = goal_transitions[pos];
            if (prob > 0) {
                VariableSet depar_set = convertToDeparametrizedState(vec_state_enum_[g], original_set);
                temp_result[depar_set] = prob;
            }
        }

    }
    i_file.close();
    return temp_result;
}

std::map<VariableSet, double> Hmdp::getHierarchicTransition(VariableSet original_set, Hmdp * super_hmdp) {
    std::map<VariableSet, double> temp_result;


    VariableSet v_param = convertToParametrizedState(original_set, super_hmdp);
    if (v_param.set.size() == 0) return temp_result;
    int i = map_state_enum_.at(v_param);

    if (is_hierarchy_cached_) {
        for (int g : goal_states_) {
            pair<int, int> hierachic_input{i, g};
            double prob = hierarchic_transition_[hierachic_input];
            if (prob != 0 && abs(prob) <= 1) {
                VariableSet depar_set = convertToDeparametrizedState(vec_state_enum_[g], original_set);
                temp_result[depar_set] = abs(prob);
            }
        }
        return temp_result;

    } else {
        return getHierarchicTransitionFromFile(i, original_set);
    }
}

std::map<VariableSet, double> Hmdp::getHierarchicTransition(VariableSet original_set, string action, Hmdp * super_hmdp) {
    std::map<VariableSet, double> temp_result;

    if (active_module != "this") {
        //        hierarchy_map_[active_module]->assignParametersFromActionName(action);
        return hierarchy_map_[active_module]->getHierarchicTransition(original_set, action, this);
    } else {
        //                printParameters();
        VariableSet v_param = convertToParametrizedState(original_set);

        int i = map_state_enum_.at(v_param);
        PairStateAction p{i, action};
        StateProb result_var = transition_[p];
        for (auto r : result_var) {
            //we put the state back in a deparametrized form, since it needs to be converted to the higher state mdp properly.
            VariableSet depar_set = convertToDeparametrizedState(vec_state_enum_[r.first], original_set);
            temp_result[depar_set] = r.second;
        }
    }
    return temp_result;
}

void Hmdp::calculateHierarchicReward() {
    cout << "Calculating hierarchic reward\n";
    if (hierarchic_reward_.size() == 0) {
        std::map<int, double> temp_hierarchic_reward;
        using Eigen::VectorXd;
        Eigen::SparseMatrix<double> a(vec_state_enum_.size(), vec_state_enum_.size());
        VectorXd b(vec_state_enum_.size());
        double beta = 0.3;
        int i = -1;
        for (int s = 0; s < vec_state_enum_.size(); s++) {
            a.insert(s, s) = 1;
            string action = chooseAction(s);
            PairStateAction transition_input{s, action};
            StateProb future_states = transition_[transition_input];
            for (auto future_state : future_states) {
                int sp = future_state.first;
                if (!isGoalState(vec_state_enum_[sp]) && sp != s) {
                    if (!use_cost_) {
                        a.insert(s, sp) = -beta * future_state.second;
                    } else {
                        a.insert(s, sp) = -future_state.second;
                    }
                }
            }
            PairStateAction reward_input{s, action};
            if (!use_cost_) {
                b(s) = reward_[reward_input];
            } else {
                b(s) = reward_[reward_input];
            }
        }
        //        cout<<a<<"\n";
        //        cout<<b<<"\n";
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
        i = 0;
        for (int s : starting_states_) {
            temp_hierarchic_reward[s] = x(s);
            i++;
        }
        hierarchic_reward_ = temp_hierarchic_reward;

    }
}
//I actually think this is the right one but it doesn't work with this ^^
//
//
//for (int s = 0; s < vecStateEnum.size(); s++) {
//    if (isGoalState(vecStateEnum[s])) {
//        a.insert(s, g) = 1;
//        b[s] = 0;
//    } else {
//        string action = chooseAction(s);
//        PairStateAction t_input{s, action};
//        StateProb future_states = transition[t_input];
//        a.insert(s, g) = 1;
//        for (auto future_state : future_states) {
//
//            int sp = future_state.first;
//            if (sp != g) {
//                //                    if (sp != s && !isGoalState(vecStateEnum[sp])) {
//                a.insert(s, sp) = -beta * future_state.second;
//            }
//        }
//
//        double b_transition_prob = getTransitionProb(s, action, g);
//        b[s] = b_transition_prob;
//    }
//}

void Hmdp::calculateHierarchicTransition() {
    cout << "Calculating hierarchic transition\n";

    if (hierarchic_transition_.size() == 0) {
        std::map<pair<int, int>, double> temp_hierarcic_transition;

        double beta = 0.3;

        for (int g : goal_states_) {
            using Eigen::VectorXd;
            Eigen::SparseMatrix<double> a(vec_state_enum_.size(), vec_state_enum_.size());
            VectorXd b(vec_state_enum_.size());


            for (int s = 0; s < vec_state_enum_.size(); s++) {
                string action = chooseAction(s);
                PairStateAction t_input{s, action};
                StateProb future_states = transition_[t_input];
                a.insert(s, s) = 1;
                for (auto future_state : future_states) {
                    int sp = future_state.first;
                    if (sp != s && !isGoalState(vec_state_enum_[sp])) {
                        if (use_cost_) {
                            a.insert(s, sp) = -future_state.second;
                        } else {
                            a.insert(s, sp) = -beta * future_state.second;
                        }
                    }

                }
                double b_transition_prob = getTransitionProb(s, action, g);
                b[s] = b_transition_prob;
            }

            //            cout << a << "\n\n";
            //            cout << b << "\n\n";

            Eigen::SparseLU<Eigen::SparseMatrix<double> > solver;
            a.makeCompressed();
            solver.compute(a);
            if (solver.info() != Eigen::Success) {
                cout << "DECOMPOSITON FAILED\n";
                // decomposition failed
                return;
            }
            VectorXd x = solver.solve(b);
            if (solver.info() != Eigen::Success) {
                cout << "SOLVING FAILED\n";
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

void Hmdp::testEnumerate(int i, string action) {

    double r;
    StateProb future_beliefs;
    VariableSet v = vec_state_enum_[i];
    if (hierarchy_map_.find(action) == hierarchy_map_.end()) {
        VarStateProb future_beliefs_var = transitionFunction(vec_state_enum_[i], action);
        for (auto b : future_beliefs_var) {
            future_beliefs[map_state_enum_.at(b.first)] = b.second;
        }
        r = use_cost_ ? 1 : rewardFunction(vec_state_enum_[i], action);
    } else {
        Hmdp* h = hierarchy_map_[action];

        h->assignParametersFromActionName(action);

        VariableSet depar_set = convertToDeparametrizedState(vec_state_enum_[i], VariableSet());
        //                VarStateProb temp_future_beliefs = h->getHierarchicTransition(vecStateEnum[i]);

        VarStateProb temp_future_beliefs = h->getHierarchicTransition(depar_set, this);
        for (auto temp_b : temp_future_beliefs) {
            VariableSet par_set = convertToParametrizedState(temp_b.first);
            future_beliefs[map_state_enum_.at(par_set)] = temp_b.second;
        }
        VariableSet vstry = vec_state_enum_[i];

        r = use_cost_ ? h->getHierarchicReward(vstry, this) : h->getHierarchicReward(vstry, this) * rewardFunction(vec_state_enum_[i], action);
    }
}

void Hmdp::enumerateFunctions() {
    cout << "Starting Enumeration\n";
    for (int i = 0; i < vec_state_enum_.size(); i++) {
        for (string action : actions_) {


            double r;
            StateProb future_beliefs;
            if (hierarchy_map_.find(action) == hierarchy_map_.end()) {
                VarStateProb future_beliefs_var = transitionFunction(vec_state_enum_[i], action);
                for (auto b : future_beliefs_var) {
                    future_beliefs[map_state_enum_.at(b.first)] = b.second;
                }
                r = use_cost_ ? 1 : rewardFunction(vec_state_enum_[i], action);
            } else {
                Hmdp* h = hierarchy_map_[action];

                h->assignParametersFromActionName(action);

                VariableSet depar_set = convertToDeparametrizedState(vec_state_enum_[i], VariableSet());
                //                VarStateProb temp_future_beliefs = h->getHierarchicTransition(vecStateEnum[i]);

                VarStateProb temp_future_beliefs = h->getHierarchicTransition(depar_set, this);
                for (auto temp_b : temp_future_beliefs) {
                    VariableSet par_set = convertToParametrizedState(temp_b.first);
                    future_beliefs[map_state_enum_.at(par_set)] = temp_b.second;
                }
                VariableSet vstry = vec_state_enum_[i];

                r = use_cost_ ? h->getHierarchicReward(vstry, this) : h->getHierarchicReward(vstry, this) * rewardFunction(vec_state_enum_[i], action);
            }

            PairStateAction transitionInput{i, action};
            for (auto belief : future_beliefs) {

                int s = belief.first;

                PairStateAction bTransitionInput{s, action};
                std::vector<int> previousBeliefs = predecessors_[bTransitionInput];
                previousBeliefs.push_back(i);
                predecessors_[bTransitionInput] = previousBeliefs;
            }
            transition_[transitionInput] = future_beliefs;

            PairStateAction rewardInput{i, action};
            reward_[rewardInput] = r;
        }
    }
}

void Hmdp::create(string name, bool rewrite, bool first) {

    if (!is_created_) {
        is_created_ = true;
        //        name = getParametrizedAction(name); //transform something like human_take_dish in agent_take_object 
        //in order to be generic;
        //        this->name_ = name;



        for (HmdpMap::iterator i = hierarchy_map_.begin(); i != hierarchy_map_.end(); i++) {
            i->second->create(i->first, rewrite, false);
        }

        cout << "Creating " << name << "\n";

        Mdp::create(name, rewrite);

        hierarchic_file_ = name + ".hmdp";
        if (!first) {
            ifstream i_file(name + ".hmdp");
            if (!rewrite && i_file.good()) {
                i_file.close();
                readHierarchical(name + ".hmdp");
            } else {

                calculateHierarchicReward();
                calculateHierarchicTransition();
                writeHierarchical(name + ".hmdp");
            }
        }
    }
}

void Hmdp::readHierarchical(string fileName) {
    ifstream i_file(fileName);
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

void Hmdp::writeHierarchical(string fileName) {
    ofstream o_file(fileName);
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
    hierarchic_transition_.clear();
    hierarchic_reward_.clear();
}

string Hmdp::chooseHierarchicAction(VariableSet state) {
    VariableSet this_state = convertToParametrizedState(state);
    if (isGoalState(this_state)) return "";
    if (active_module == "this") {
        //        printQValues(this_state);
        string action = chooseAction(map_state_enum_.at(this_state));
        if (hierarchy_map_.find(action) != hierarchy_map_.end()) {
            Hmdp * h = hierarchy_map_[action];
            active_module = action;
            string dp = getDeparametrizedAction(action);
            h->assignParametersFromActionName(dp);
            //            h->printParameters();
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

    return chooseHierarchicAction(vec_state_enum_[b]);
}

void Hmdp::simulate(int n, VariableSet initial_state) {
    printParameters();
    VariableSet parametrized_vs = convertToParametrizedState(initial_state);

    int int_initial_state = map_state_enum_.at(parametrized_vs);
    StateProb b;
    b[int_initial_state] = 1.0;

    VariableSet previous_orig_state = initial_state;
    for (int i = 0; i < n; i++) {
        cout << "Step " << i << "\n";
        StateProb temp_b;
        for (auto s : b) {
            VariableSet depar_s = convertToDeparametrizedState(vec_state_enum_[s.first], previous_orig_state);
            previous_orig_state = depar_s;


            //            cout<<"Q- Values:\n";
            cout << "State: \n";
            cout << depar_s.toString() << "\n";

            if (i == 6) {
                cout << "";
            }
            //            printQValues(vecStateEnum[s.first]);


            string action = chooseHierarchicAction(depar_s);

            StateProb output;
            if (active_module != "this") {
                Hmdp* lowest_active = getLowestActiveModule();
                cout << "Executing " << lowest_active->getDeparametrizedAction(action) << "\n";


                Hmdp* sub_mdp = hierarchy_map_[active_module];
                VarStateProb var_output = sub_mdp->getHierarchicTransition(depar_s, action, this);

                for (auto o : var_output) {
                    previous_orig_state = o.first;
                    VariableSet par_o = convertToParametrizedState(o.first);
                    output[map_state_enum_.at(par_o)] = o.second;
                }
            } else {
                cout << "Executing " << getDeparametrizedAction(action) << "\n";\

                PairStateAction p{s.first, action};
                output = transition_[p];
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

        cout << vec_state_enum_[s.first].toString() << "\n";
    }
}

void Hmdp::assignParametersFromActionName(string action_name) {
    vector<string> action_parts = StringOperations::stringSplit(action_name, '_');
    vector<string> name_parts = StringOperations::stringSplit(name_, '_');
    std::map<string, string> instance;
    for (int i = 0; i < action_parts.size(); i++) {
        if (parameter_action_place_.find(i) != parameter_action_place_.end()) {
            instance[parameter_action_place_[i]] = action_parts[i];
        }
    }
    assignParameters(instance);
    parametrized_name_ = action_parts[0];
    parametrized_name_ += '_' + name_parts[1];
    for (int i = 2; i < action_parts.size(); i++) {

        parametrized_name_ += "_" + action_parts[i];
    }
}

void Hmdp::printGoalStates() {
    cout << "Goal states:\n";
    for (int i = 0; i < goal_states_.size(); i++) {

        cout << vec_state_enum_[goal_states_[i]].toString() << "\n";
    }
}

void Hmdp::printHierarchy() {
    cout << "name: " << name_ << endl;
    cout << "actions: " << endl;
    for (string a : actions_) {
        cout << a << endl;
    }

    for (auto h : hierarchy_map_) {

        cout << "Son of " << name_ << endl;
        h.second->printHierarchy();
    }

}

string Hmdp::getDeparametrizedAction(string action_name) {
    //    if (hierarchy_map_.find(action_name) != hierarchy_map_.end()) {
    //        return hierarchy_map_[action_name]->getDeparametrizedAction(action_name);
    //    } else {

    vector<string> action_parts = StringOperations::stringSplit(action_name, '_');
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
    //    }
}

Hmdp * Hmdp::getLowestActiveModule() {
    if (active_module != "this") return hierarchy_map_[active_module]->getLowestActiveModule();
    else return this;
}

VariableSet Hmdp::fixAbstractStates(VariableSet sub_set, Hmdp * super_mmdp) {
    VariableSet result;
    for (auto v : sub_set.set) {
        if (v.second == "other") {
            vector<string> sub_values = var_values_.at(v.first);

            string actual_key = v.first;
            vector<string> super_keys;
            super_keys.push_back(v.first);
            if (parametrized_to_original_.find(actual_key) != parametrized_to_original_.end()) {
                actual_key = parametrized_to_original_[actual_key];
                super_keys = super_mmdp->original_to_parametrized_[actual_key];
            }

            vector<string> super_values = super_mmdp->var_values_.at(super_keys[0]);

            for (string value : sub_values) {
                if (std::find(super_values.begin(), super_values.end(), value) == super_values.end()) {
                    result.set[v.first] = value;
                    break;
                }
            }
        } else {
            result.set[v.first] = v.second;
        }
    }
    return result;
}

VariableSet Hmdp::convertToParametrizedState(VariableSet s, Hmdp * super_mdp) {
    VariableSet vs_new;
    //for each variable in the set
    for (auto el : s.set) {

        vector<string> par_key;
        vector<string> possible_values;

        if (original_to_parametrized_.find(el.first) != original_to_parametrized_.end()) {
            par_key = original_to_parametrized_[el.first];
        }

        if (original_to_parametrized_.find(el.second) != original_to_parametrized_.end()) {
            possible_values = original_to_parametrized_[el.second];
        }
        possible_values.push_back(el.second);
        //if it's a parameter variable
        if (par_key.size() > 0) {
            for (string key : par_key) {
                if (vs_new.set.find(key) == vs_new.set.end()) {
                    string value = findValue(key, possible_values);
                    if (value == "") {
                        return VariableSet();
                    }
                    vs_new.set[key] = value;
                }
            }
        }
        //possible incongruence in var-parameter situation
        if (std::find(variables_.begin(), variables_.end(), el.first) != variables_.end()) {
            if (vs_new.set.find(el.first) == vs_new.set.end()) {
                string value = findValue(el.first, possible_values);
                if (value == "") {
                    return VariableSet();
                }
                vs_new.set[el.first] = value;
            }
        }
    }
    for (string var : variables_) {
        if (vs_new.set.find(var) == vs_new.set.end()) {
            vs_new.set[var] = var_values_.at(var)[0];
        }
    }
    return vs_new;
}

VariableSet Hmdp::convertToParametrizedState(VariableSet parameter_set) {
    return Mdp::convertToParametrizedState(parameter_set);
} //converts a state space to it's parametrized version

void Hmdp::loadHierarchicInCache() {
    readHierarchical(hierarchic_file_);
    is_hierarchy_cached_ = true;
}

void Hmdp::emptyHierarchicCache() {
    hierarchic_transition_.clear();
    hierarchic_reward_.clear();
    is_hierarchy_cached_ = false;
}

