/* 
 * 
 * File:   Mmdp.cpp
 * Author: mfiore
 * 
 * Created on March 8, 2016, 3:10 PM
 */

#include <vector>

#include "Mmdp.h"

Mmdp::Mmdp() {
    is_created_ = false;
}

Mmdp::Mmdp(const Mmdp& orig) {
}

Mmdp::~Mmdp() {

}

void Mmdp::createJointMdpVariables() {

    vector<vector<string> > all_actions;
    //get variables from single agents mdps
    int i = 0;
    for (auto mdp : agent_hmpd_) {
        cout << mdp.first << "\n";
        string i_s = "p" + boost::lexical_cast<string>(i);
        for (auto var : mdp.second->variables) {
            //convert values to multiparameter if needed
            vector<string> actual_var_values;
            for (auto value : mdp.second->varValues[var]) {
                string actual_value = value;
                if (std::find(mdp.second->parameters.begin(), mdp.second->parameters.end(), value) != mdp.second->parameters.end()) {
                    actual_value = actual_value + i_s;
                }
                actual_var_values.push_back(actual_value);
            }

            string actual_var = var;
            if (mdp.second->variable_parameter.find(var) != mdp.second->variable_parameter.end()) {
                actual_var = convertToMultiParameter(mdp.second, var, i);
                variables.push_back(actual_var);
                varValues[actual_var] = actual_var_values;
            } else if (std::find(variables.begin(), variables.end(), actual_var) == variables.end()) {
                variables.push_back(actual_var);
                varValues[actual_var] = actual_var_values;
            } else {
                for (string actual_value : actual_var_values) {
                    if (std::find(varValues[actual_var].begin(), varValues[actual_var].end(), actual_value) == varValues[actual_var].end()) {
                        varValues[actual_var].push_back(actual_value);
                    }
                }
                //push back only values that aren't there
                //TODO
            }
        }
        for (auto par : mdp.second->parameters) {
            string new_par_name = par + i_s;
            parameters.push_back(new_par_name);
            for (auto par_var : mdp.second->parameter_variables[par]) {
                string new_var_name = convertToMultiParameter(mdp.second, par_var, i);
                parameter_variables[new_par_name].push_back(new_var_name);
                //              }
            }
        }
        vector<string> mdp_param_actions;
        for (string action : mdp.second->actions) {
            vector<string> action_parts = StringOperations::stringSplit(action, '_');
            string new_action = "";
            for (int i = 0; i < action_parts.size() - 1; i++) {
                string part = action_parts[i];
                if (std::find(mdp.second->parameters.begin(), mdp.second->parameters.end(), part) != mdp.second->parameters.end()) {
                    new_action = new_action + part + i_s + "_";
                } else {
                    new_action = new_action + part + "_";
                }
            }
            string part = action_parts[action_parts.size() - 1];
            if (std::find(mdp.second->parameters.begin(), mdp.second->parameters.end(), part) != mdp.second->parameters.end()) {
                new_action = new_action + part + i_s;
            } else {
                new_action = new_action + part;
            }
            mdp_param_actions.push_back(new_action);
        }
        mdp_param_actions.push_back("agent" + i_s + "_wait");
        all_actions.push_back(mdp_param_actions);
        i++;
    }

    NestedLoop<string> loop(all_actions);
    vector<vector<string> > action_matrix = loop.buildMatrix();
    for (int i = 0; i < action_matrix.size(); i++) {
        string action_name = "";
        for (int j = 0; j < action_matrix[i].size(); j++) {
            if (action_name != "") {

                action_name = action_name + "-" + action_matrix[i][j];
            } else {
                action_name = action_matrix[i][j];
            }
        }
        actions.push_back(action_name);
    }

}

void Mmdp::createSubMdpNames(string name) {
    vector<string> single_names = StringOperations::stringSplit(name, '-');
    int i = 0;
    for (auto mdp : agent_hmpd_) {
        mdp.second->name = single_names[i];
        i++;
    }
}

void Mmdp::create(string action_name, bool rewrite, bool first) {
    if (!is_created_) {

        //       
        for (auto a : agent_hmpd_) {
            name = StringOperations::addToString(name, a.second->name, '-');
        }
        //        this->name = name;
        //        createSubMdpNames(name);
        cout << "Creating " << name << "\n";


        is_created_ = true;
        createJointMdpVariables();
        createSubMmdps();

        enumerateStates();

        //        for (auto h : hierarchy_map_) {
        //            h.second->create(h.first, rewrite, false);
        //        }
        for (string a : actions) {
            pair<string, set<string> > sub_mdp_result = getSubMdpName(a);
            string module_name = sub_mdp_result.first;
            if (module_name != "this") {
                if (hierarchy_map_[module_name]->is_created_ == false) {
                    hierarchy_map_[module_name]->create(a, rewrite, false);
                }
            }
        }



        std::set<string> changed_mdps;
        for (auto agent : agent_hmpd_) {
            changed_mdps.insert(agent.first);
        }
        assignParametersFromActionName(action_name, changed_mdps, map<string, string>());



        string fileName = name + ".pomdp";

        bool has_read = readMdp(fileName, rewrite);
        if (name == "reorder_table") {
            cout << "here wew are";
        }
        enumerateGoalAndStartStates();

        if (!has_read) {
            enumerateFunctions(fileName);
        }

        valueIteration(rewrite);

        if (!first) {
            ifstream i_file(name + ".hmdp");
            if (!rewrite && i_file.good()) {
                i_file.close();
                readHierarchical();
            } else {
                calculateHierarchicReward();
                calculateHierarchicTransition();
            }
            writeHierarchical();

        }
    }
}

void Mmdp::assignParametersFromActionName(string action_name, set<string> changed_mdps, map<string, string> super_instance) {
    std::map<string, string> instance;

    vector<string> single_actions = StringOperations::stringSplit(action_name, '-');
    int i = 0;
    for (auto agent : agent_hmpd_) {
        if (changed_mdps.find(agent.first) == changed_mdps.end()) {
            for (auto el : super_instance) {
                if (el.first.substr(el.first.size() - 1) == to_string(i)) {
                    instance[el.first] = el.second;
                }
            }
        } else {
            string i_s = "p" + boost::lexical_cast<string>(i);
            vector<string> action_parts = StringOperations::stringSplit(single_actions[i], '_');
            for (int j = 0; j < action_parts.size(); j++) {
                if (agent.second->parameter_action_place.find(j) != agent.second->parameter_action_place.end()) {
                    instance[agent.second->parameter_action_place[j] + i_s] = action_parts[j];
                }
            }
        }
        i++;
    }

    assignParameters(instance);
    i = 0;
    for (auto mdp : agent_hmpd_) {
        if (changed_mdps.find(mdp.first) == changed_mdps.end()) {
            i++;
            continue;
        }
        assignParametersToMdp(mdp.second, i);
        i++;
    }
}

string Mmdp::convertToSingleParameter(string var_name, int index) {
    string i_s = "p" + boost::lexical_cast<string>(index);
    string actual_var_name = var_name;
    if (variable_parameter.find(var_name) != variable_parameter.end()) {
        string par = variable_parameter[var_name];
        actual_var_name.replace(actual_var_name.find(par, 0), par.length(), par.substr(0, par.length() - i_s.length()));
    } else if (std::find(parameters.begin(), parameters.end(), var_name) != parameters.end()) {
        string par = var_name;
        actual_var_name.replace(actual_var_name.find(par, 0), par.length(), par.substr(0, par.length() - i_s.length()));
    }
    return actual_var_name;

}

string Mmdp::convertToMultiParameter(Mdp* mdp, string var_name, int i) {
    string i_s = "p" + boost::lexical_cast<string>(i);
    string actual_var_name = var_name;
    if (mdp->variable_parameter.find(var_name) != mdp->variable_parameter.end()) {
        string par = mdp->variable_parameter[var_name];
        actual_var_name.replace(actual_var_name.find(par, 0), par.length(), par + i_s);
        return actual_var_name;
    } else if (std::find(mdp->parameters.begin(), mdp->parameters.end(), var_name) != mdp->parameters.end()) {
        return actual_var_name + i_s;
    } else return actual_var_name;
}

VarStateProb Mmdp::joinMdpFutureStates(VarStateProb mdp_future_state, VarStateProb cumulative_future_state,
        VariableSet mmdp_instance_state, Hmdp *mdp, int index, bool *no_incongruences,
        std::set<string> new_not_present_variables) {
    VarStateProb result;

    //if one of the two states is empty we return the other
    if (mdp_future_state.empty()) {
        result = cumulative_future_state;
        return result;
    } else if (cumulative_future_state.empty()) {
        //we return mdp_future_state but converted in the instance space
        for (auto state : mdp_future_state) {
            VariableSet instance_state = mdp->convertToDeparametrizedState(state.first);
            result[instance_state] = state.second;
        }
        return result;
    }
    //if both states aren't empty we mix them
    for (auto state : mdp_future_state) {
        VariableSet instance_state = mdp->convertToDeparametrizedState(state.first);
        for (auto cumulative_state : cumulative_future_state) {
            VariableSet new_cumulative_state = cumulative_state.first;

            for (auto var : instance_state.set) {
                //if we should consider this variable (it doesn't have a value not present in the current sub-mdp)
                if (new_not_present_variables.find(var.first) == new_not_present_variables.end()) {
                    //value of the variable before the transition (original mmdp space)
                    string original_value = mmdp_instance_state.set[var.first];
                    //if the variable is not present in the cumulative state we just add it
                    if (new_cumulative_state.set.find(var.first) == new_cumulative_state.set.end()) {
                        new_cumulative_state.set[var.first] = var.second;
                    }//if it's there it's more complex
                    else if (new_cumulative_state.set[var.first] != var.second) { //if they have different values 
                        //case 1: if one of the two values is different from original we take the other value
                        //we only consider the case where the new one is different. If not we just keep the old one
                        if (new_cumulative_state.set[var.first] == original_value &&
                                var.second != original_value) {
                            new_cumulative_state.set[var.first] = var.second;
                        }//case 2: the values are different from themselves and from original. It's an incongruency and we quit
                        else if (new_cumulative_state.set[var.first] != original_value &&
                                var.second != original_value) {
                            *no_incongruences = false;
                            return result;
                        }
                    }
                }
                //                else {
                //                    new_cumulative_state.set[var.first] = var.second;
                //                }
            }
            result[new_cumulative_state] = cumulative_state.second * state.second;
        }
    }
    return result;
}

void Mmdp::enumerateFunctions(string fileName) {
    ofstream file(fileName);
    cout << "Starting Enumeration\n";

    int nline = 0;

    for (int i = 0; i < vecStateEnum.size(); i++) {

        if (!isMmdpStateCongruent(vecStateEnum[i])) continue;
        for (string action : actions) {
            if (i == 0 && action == "agentp0_take_objectp0-agentp1_move_table") {
                cout << "";
            }

            double r = 0;
            StateProb future_beliefs;

            vector<string> single_actions = StringOperations::stringSplit(action, '-');

            pair<string, set<string> > sub_mdp_details = getSubMdpName(action);
            string module_name = sub_mdp_details.first;
            set<string> changed_mdps = sub_mdp_details.second;
            if (module_name == "this") {
                //if it's not a hierarchical action we have to check the state from each mdp, and look 
                //to see if the resulting state is not incongruent

                vector<string> single_actions = StringOperations::stringSplit(action, '-');
                int index = 0;
                VarStateProb cumulative_future_mdp_states; //this will hold the joint mdp future states in the instance space
                bool no_incongruences = true; //this will become false if 2 mdp states in the instance space have a different value (and different from the original too)
                VariableSet mmdp_instance_state = convertToDeparametrizedState(vecStateEnum[i]); //will need it to check for inconsistencies

                for (auto mdp : agent_hmpd_) {

                    vector<string> action_parts = StringOperations::stringSplit(single_actions[index], '_');
                    VarStateProb mdp_future_states;
                    pair<VariableSet, set<string> > mdp_state_result = convertToMdpState(mdp.second, index, vecStateEnum[i]);
                    VariableSet mdp_state = mdp_state_result.first;
                    set<string> not_present_variables = mdp_state_result.second;
                    if (action_parts[1] == "wait") {
                        mdp_future_states[mdp_state] = 1;
                    } else {
                        string mdp_action = convertToSingleMdpAction(mdp.second, index, single_actions[index]);
                        mdp_future_states = mdp.second->transitionFunction(mdp_state, mdp_action);
                        r = r + mdp.second->rewardFunction(mdp_state, mdp_action);
                    }
                    cumulative_future_mdp_states = joinMdpFutureStates(mdp_future_states, cumulative_future_mdp_states,
                            mmdp_instance_state, mdp.second, index, &no_incongruences, not_present_variables);

                    if (!no_incongruences) {
                        break;
                    }
                    index++;
                }

                if (no_incongruences) {
                    //convert the cumulative_future_state to parameter state
                    for (auto state : cumulative_future_mdp_states) {

                        VariableSet converted_state = convertToParametrizedState(state.first);

                        //                        cout << converted_state.toString() << endl;
                        future_beliefs[mapStateEnum.at(converted_state)] = state.second;
                    }
                } else {
                    r = 0;
                }
            } else {
                //if it's a hierarchical action it's the same as an hmdp
                Mmdp* h = (Mmdp*) hierarchy_map_[module_name];
                if (action == "agentp0_get_bracketp0-agentp1_wait" &&
                        i == 0) {
                    cout << "h";
                }
                string depar_action = getDeparametrizedAction(action);
                h->assignParametersFromActionName(getDeparametrizedAction(action), changed_mdps, parameter_instances);
                h->printParameters();



                //                VariableSet v_param = h->convertToParametrizedState(vecStateEnum[i]);
                VariableSet v_deparam = convertToDeparametrizedState(vecStateEnum[i]);
                VariableSet v_param = h->convertToParametrizedState(v_deparam);

                if (h->mapStateEnum.find(v_param) != h->mapStateEnum.end()) {


                    //                    VarStateProb temp_future_beliefs = h->getHierarchicTransition(vecStateEnum[i]);
                    VarStateProb temp_future_beliefs = h->getHierarchicTransition(v_deparam);

                    for (auto temp_b : temp_future_beliefs) {
                        VariableSet fb = temp_b.first;
                        VariableSet param_fb = convertToParametrizedState(fb);
                        for (auto v : vecStateEnum[i].set) {
                            if (param_fb.set.find(v.first) == param_fb.set.end()) {
                                param_fb.set[v.first] = v.second;
                            }
                        }

                        //                        for (string v : variables) {
                        //                            if (std::find(varValues[v].begin(), varValues[v].end(), fb.set[v]) == varValues[v].end()) {
                        //                                string original_var = v;
                        //                                if (parametrized_to_original.find(v) != parametrized_to_original.end()) {
                        //                                    original_var = parametrized_to_original[v];
                        //                                }
                        //                                if (original_to_parametrized.find(original_var) != original_to_parametrized.end()) {
                        //                                    vector<string> parameter_vars = original_to_parametrized[original_var];
                        //                                    for (string p : parameter_vars) {
                        //                                        if (p != v && std::find(varValues[p].begin(), varValues[p].end(), fb.set[v]) != varValues[p].end()) {
                        //                                            fb.set[p] = fb.set[v];
                        //                                        }
                        //                                    }
                        //                                }
                        //                                fb.set[v] = "other";
                        //                            }
                        //                        }
                        future_beliefs[mapStateEnum.at(param_fb)] = temp_b.second;
                    }
                    VariableSet vstry = vecStateEnum[i];

                    r = rewardFunction(vecStateEnum[i], action);
                }
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
            nline++;
            transition[transitionInput] = future_beliefs;

            PairStateAction rewardInput{i, action};
            reward[rewardInput] = r;
            file << r << "\n";
            nline++;
        }
    }
    file.close();
}

void Mmdp::enumerateGoalAndStartStates() {
    for (auto state : mapStateEnum) {
        int i = 0;
        bool is_starting_state = true;
        bool is_goal_state = false;
        for (auto mdp : agent_hmpd_) {
            pair<VariableSet, set<string> > mdp_state = convertToMdpState(mdp.second, i, state.first);
            is_starting_state = is_starting_state && mdp.second->isStartingState(mdp_state.first);
            is_goal_state = is_goal_state || mdp.second->isGoalState(mdp_state.first);
            i++;
        }

        if (is_starting_state) {
            starting_states_.push_back(state.second);
        }
        if (is_goal_state) {
            goal_states_.push_back(state.second);
        }
    }
}

VarStateProb Mmdp::transitionFunction(VariableSet state, string action) {
    return VarStateProb();
}

int Mmdp::rewardFunction(VariableSet state, string action) {
    if (isGoalState(state)) return 1000;
    return 0;
    //        int index=0;
    //    int r=0;
    //    vector<string> single_actions=StringOperations::stringSplit(action,'-');
    //    for (auto mdp:agent_hmpd_) {
    //        string mdp_action=convertToSingleMdpAction(mdp.second,index,single_actions[index]);
    //        VariableSet mdp_state=convertToMdpState(mdp.second,index,state);
    //        r=r+mdp.second->rewardFunction(mdp_state,mdp_action);
    //        index++;
    //    }
    ////    if (r>0) {
    ////        cout<<"ah";
    ////    }
    //    return r;
}

bool Mmdp::isStartingState(VariableSet state) {
    int i_state = mapStateEnum.at(state);
    return std::find(starting_states_.begin(), starting_states_.end(), i_state) != starting_states_.end();
}

bool Mmdp::isGoalState(VariableSet state) {
    int i_state = mapStateEnum.at(state);
    return std::find(goal_states_.begin(), goal_states_.end(), i_state) != goal_states_.end();
}

pair<VariableSet, set<string >> Mmdp::convertToMdpState(Hmdp* mdp, int index, VariableSet mmdp_state) {
    pair<VariableSet, set<string> > result;
    set<string> not_present_variables;
    VariableSet mdp_state;
    //    cout<<mmdp_state.toString()<<"\n";
    for (auto mdp_var : mdp->variables) {
        string actual_var_name = mdp_var;
        if (mdp->variable_parameter.find(mdp_var) != mdp->variable_parameter.end()) {
            actual_var_name = convertToMultiParameter(mdp, mdp_var, index);
        }
        string actual_var_value = mmdp_state.set[actual_var_name];
        if (std::find(parameters.begin(), parameters.end(), actual_var_value) != parameters.end()) {
            string agent_parameter = actual_var_value.substr(actual_var_value.length() - 1);
            if (agent_parameter == to_string(index)) { //it's actually a parameter of this mmdp
                actual_var_value = convertToSingleParameter(actual_var_value, index);
            } else {
                not_present_variables.insert(actual_var_name);
                actual_var_value = mdp->varValues[mdp_var][0];
            }
        }
        mdp_state.set[mdp_var] = actual_var_value;
    }
    result.first = mdp_state;
    result.second = not_present_variables;
    return result;
}

VariableSet Mmdp::convertToMmdpState(VariableSet mdp_state, Hmdp* mdp, int index) {
    VariableSet mmdp_state;
    for (auto var : mdp_state.set) {
        string actual_var_name = var.first;
        string actual_var_value = var.second;
        if (mdp->variable_parameter.find(actual_var_name) != mdp->variable_parameter.end()) {
            actual_var_name = convertToMultiParameter(mdp, actual_var_name, index);
        }
        if (std::find(mdp->parameters.begin(), mdp->parameters.end(), actual_var_value) != mdp->parameters.end()) {
            actual_var_value = convertToMultiParameter(mdp, actual_var_value, index);
        }
        mmdp_state.set[actual_var_name] = actual_var_value;
    }
    return mmdp_state;
}

void Mmdp::assignParametersToMdp(Hmdp* mdp, int index) {
    std::map<string, string> instance;
    for (auto parameter : mdp->parameters) {
        string actual_par = convertToMultiParameter(mdp, parameter, index);
        instance[parameter] = parameter_instances[actual_par];
    }
    //    for (auto i : instance) {
    //        cout << i.first << " " << i.second << "\n";
    //    }
    mdp->assignParameters(instance);
}

bool Mmdp::isMmdpStateCongruent(VariableSet state) {
    std::map<string, string> var_values;

    for (auto var : state.set) {
        string actual_value = var.second;
        if (parametrized_to_original.find(actual_value) != parametrized_to_original.end()) {
            actual_value = parametrized_to_original[var.second];
        }
        if (parametrized_to_original.find(var.first) != parametrized_to_original.end()) {
            if (var_values.find(parametrized_to_original[var.first]) != var_values.end()) {
                string new_value = var_values[parametrized_to_original[var.first]];
                if (actual_value != new_value) {
                    return false;
                }
            } else {
                string par_var = parametrized_to_original[var.first];
                var_values[par_var] = actual_value;
            }
        }
    }

    return true;
}

string Mmdp::getDeparametrizedAction(string action_name) {
    std::map<string, string> instance;
    vector<string> single_actions = StringOperations::stringSplit(action_name, '-');

    vector<string> action_parts = StringOperations::stringSplit(action_name, '_');
    stringstream depar_action_name;

    vector<Hmdp*> mdp;

    for (auto amdp : agent_hmpd_) {
        mdp.push_back(amdp.second);
    }
    for (int i = 0; i < single_actions.size(); i++) {

        string i_s = "p" + boost::lexical_cast<string>(i);
        vector<string> action_parts = StringOperations::stringSplit(single_actions[i], '_');
        for (int j = 0; j < action_parts.size() - 1; j++) {
            string s = action_parts[j];
            s = convertToMultiParameter(mdp[i], s, i);
            if (parametrized_to_original.find(s) != parametrized_to_original.end()) {
                depar_action_name << parametrized_to_original[s] << "_";
            } else {
                depar_action_name << s << "_";
            }

        }
        string s = action_parts[action_parts.size() - 1];
        s = convertToMultiParameter(mdp[i], s, i);

        if (parametrized_to_original.find(s) != parametrized_to_original.end()) {
            depar_action_name << parametrized_to_original[s];
        } else {
            depar_action_name << s;
        }
        if (i < single_actions.size() - 1) {
            depar_action_name << "-";
        }
    }
    return depar_action_name.str();

}

string Mmdp::findValue(string variable, vector<string> possible_values) {
    for (string v : possible_values) {
        if (std::find(varValues[variable].begin(), varValues[variable].end(), v) != varValues[variable].end()) {
            return v;
        }
    }
    return "other";
}

VariableSet Mmdp::convertToParametrizedState(VariableSet s) {
    VariableSet vs_new;
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
                    vs_new.set[key] = value;
                }
            }
        }
        if (std::find(variables.begin(), variables.end(), el.first) != variables.end()) {
            if (vs_new.set.find(el.first) == vs_new.set.end()) {
                string value = findValue(el.first, possible_values);
                vs_new.set[el.first] = value;
            }
        }
    }
    return vs_new;

}

string Mmdp::convertToSingleMdpAction(Mdp *mdp, int index, string mmdp_action) {
    vector<string> action_parts = StringOperations::stringSplit(mmdp_action, '_');
    stringstream ss;
    for (int i = 0; i < action_parts.size() - 1; i++) {
        if (std::find(parameters.begin(), parameters.end(), action_parts[i]) != parameters.end()) {
            string actual_parameter = convertToSingleParameter(action_parts[i], index);
            ss << actual_parameter << "_";
        } else {
            ss << action_parts[i] << "_";
        }
    }
    int i = action_parts.size() - 1;
    if (std::find(parameters.begin(), parameters.end(), action_parts[i]) != parameters.end()) {
        string actual_parameter = convertToSingleParameter(action_parts[i], index);
        ss << actual_parameter;
    } else {
        ss << action_parts[i];
    }
    return ss.str();
}

void Mmdp::createSubMmdps() {

    //for each hierarchical action we create a submdp:
    //if the action is hierarchical for the agent_mdp will be the linked sub_hmdp 
    //else the action for the agent_mdp will be the same hmdp
    for (string a : actions) {

        string generic_name = ""; //the generic name is the name of the modules.
        string specific_name = ""; //the specific name is the name of actions

        vector<string> wait_indexs;

        vector<string> single_actions = StringOperations::stringSplit(a, '-');
        int action_index = 0;
        bool is_hierarchical = false;
        Mmdp* sub_mmdp = new Mmdp();

        bool parameters_in_common = false; //when the actions have parameters in common it's necessary to create a new submmdp

        for (auto mdp_agent : agent_hmpd_) {
            vector<string> action_parts = StringOperations::stringSplit(single_actions[action_index], '_');
            if (action_parts[1] == "wait") {
                //
                wait_indexs.push_back(mdp_agent.first);
                specific_name = StringOperations::addToString(specific_name, single_actions[action_index], '-');
                generic_name = StringOperations::addToString(generic_name, single_actions[action_index], '-');
            } else {
                string sub_action = convertToSingleMdpAction(mdp_agent.second, action_index, single_actions[action_index]);

                Hmdp *this_agent_hmdp = mdp_agent.second;
                this_agent_hmdp->assignParametersFromActionName(single_actions[action_index]);
                if (this_agent_hmdp->hierarchy_map_.find(sub_action) != this_agent_hmdp->hierarchy_map_.end()) {
                    sub_mmdp->agent_hmpd_[mdp_agent.first] = this_agent_hmdp->hierarchy_map_[sub_action];
                    is_hierarchical = true;
                    specific_name = StringOperations::addToString(specific_name, single_actions[action_index], '-');
                    generic_name = StringOperations::addToString(generic_name, this_agent_hmdp->hierarchy_map_[sub_action]->name, '-');

                } else {
                    sub_mmdp->agent_hmpd_[mdp_agent.first] = this_agent_hmdp;
                    specific_name = StringOperations::addToString(specific_name, single_actions[action_index], '-');
                    generic_name = StringOperations::addToString(generic_name, this_agent_hmdp->name, '-');
                }
            }
            //            }
            action_index++;
        }

        if (is_hierarchical) {
            //add wait mdps
            for (int i = 0; i < wait_indexs.size(); i++) {
                sub_mmdp->agent_hmpd_[wait_indexs[i]] = new Wait();
            }

            //check if mdp already exists
            bool already_exists = false;
            if (hierarchy_map_.find(generic_name) != hierarchy_map_.end()) {
                already_exists = true;
            }
            //check if the subactions have parameters in common
            std::map<string, Hmdp*>::iterator it = agent_hmpd_.begin();
            std::map<string, Hmdp*>::iterator next = it;
            next++;
            while (next != agent_hmpd_.end()) {
                map<string, string> this_instance = it->second->parameter_instances;
                map<string, string> other_instance = next->second->parameter_instances;
                for (auto s : this_instance) {
                    if (other_instance.find(s.first) != other_instance.end()) {
                        if (other_instance[s.first] == s.second) {
                            parameters_in_common = true;
                            break;
                        }
                    }
                }
                it++;
                next++;
            }
            if (parameters_in_common) {
                hierarchy_map_[specific_name] = sub_mmdp;
            } else if (already_exists) {
                delete sub_mmdp;
            } else {
                hierarchy_map_[generic_name] = sub_mmdp;
            }

        } else {
            delete sub_mmdp;
        }
    }
    //    printHierarchy();
}

bool Mmdp::readMdp(string fileName, bool rewrite) {
    int nline = 0;
    ifstream inputFile(fileName);
    if (inputFile.good() && !rewrite) {
        for (int i = 0; i < vecStateEnum.size(); i++) {
            if (!isMmdpStateCongruent(vecStateEnum[i])) {
                continue;
            }
            for (string action : actions) {
                vector<string> single_actions = StringOperations::stringSplit(action, '-');

                PairStateAction transitionInput{i, action};
                StateProb transitionOutput;

                string line;
                getline(inputFile, line);
                nline++;
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
                nline++;
                PairStateAction rewardInput = {i, action};
                reward[rewardInput] = stoi(line);
            }
        }
        inputFile.close();
        return true;
    }
    return false;
}

pair<string, set<string> > Mmdp::getSubMdpName(string action) {
    pair<string, std::set<string> > result;
    if (hierarchy_map_.find(action) != hierarchy_map_.end()) {
        result.first = action;
        for (auto agent : agent_hmpd_) {
            result.second.insert(agent.first);
        }
        return result;
        //        return hierarchy_map_[action]; //if there is a specific mdp use that one
    }
    //else find the generic one
    string module_name = "";
    vector<string> single_actions = StringOperations::stringSplit(action, '-');
    int i = 0;
    bool is_hierarchical = false;
    for (auto agent : agent_hmpd_) {
        string sub_action = convertToSingleMdpAction(agent.second, i, single_actions[i]);
        if (sub_action == "agent_wait") {
            module_name = StringOperations::addToString(module_name, single_actions[i], '-');
            result.second.insert(agent.first);
        } else if (agent.second->hierarchy_map_.find(sub_action) != agent.second->hierarchy_map_.end()) {
            module_name = StringOperations::addToString(module_name, agent.second->hierarchy_map_[sub_action]->name, '-');
            is_hierarchical = true;
            result.second.insert(agent.first);
        } else {
            module_name = StringOperations::addToString(module_name, agent.second->name, '-');
        }
        i++;
    }
    if (is_hierarchical) {
        result.first = module_name;
        //        return module_name;
        //        return hierarchy_map_[module_name];
    } else {
        result.first = "this";
        //        return "this";
    }
    return result;
}

string Mmdp::chooseHierarchicAction(VariableSet state) {
    VariableSet this_state = convertToParametrizedState(state);
    if (isGoalState(this_state)) return "";
    if (active_module == "this") {
        string action = chooseAction(mapStateEnum.at(this_state));
        pair<string, set<string> > sub_mdp_details = getSubMdpName(action);
        string module_name = sub_mdp_details.first;
        set<string> changed_mdps = sub_mdp_details.second;
        if (module_name != "this") {
            active_module = module_name;
            Mmdp *h = (Mmdp*) hierarchy_map_[active_module];
            string dp = getDeparametrizedAction(action);
            h->assignParametersFromActionName(dp, changed_mdps, parameter_instances);
            //            h->printParameters();
            return h->chooseHierarchicAction(state);
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

