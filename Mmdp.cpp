/* 
 * 
 * File:   Mmdp.cpp
 * Author: mfiore
 * 
 * Created on March 8, 2016, 3:10 PM
 */

#include <vector>

#include "Mmdp.h"

Mmdp::Mmdp(MmdpManager *mmdp_manager) : mmdp_manager_(mmdp_manager) {
    is_created_ = false;
}

Mmdp::Mmdp(const Mmdp& orig) {
}

Mmdp::~Mmdp() {
    for (auto el : hierarchy_map_) {
        if (el.second != NULL) {
            //            delete el.second;
        }
    }

}

void Mmdp::createJointMdpVariables() {

    vector<vector<string> > all_actions;
    //get variables from single agents mdps
    int i = 0;
    for (auto mdp : agent_hmpd_) {
        cout << mdp.first << "\n";
        string i_s = "p" + boost::lexical_cast<string>(i);
        for (auto var : mdp.second->variables_) {
            //convert values to multiparameter if needed
            vector<string> actual_var_values;
            for (auto value : mdp.second->var_values_.at(var)) {
                string actual_value = value;
                if (std::find(mdp.second->parameters_.begin(), mdp.second->parameters_.end(), value) != mdp.second->parameters_.end()) {
                    actual_value = actual_value + i_s;
                }
                actual_var_values.push_back(actual_value);
            }

            string actual_var = var;
            if (mdp.second->variable_parameter_.find(var) != mdp.second->variable_parameter_.end()) {
                actual_var = convertToMultiParameter(mdp.second, var, i);
                variables_.push_back(actual_var);
                var_values_[actual_var] = actual_var_values;
            } else if (std::find(variables_.begin(), variables_.end(), actual_var) == variables_.end()) {
                variables_.push_back(actual_var);
                var_values_[actual_var] = actual_var_values;
            } else {
                for (string actual_value : actual_var_values) {
                    if (std::find(var_values_.at(actual_var).begin(), var_values_.at(actual_var).end(), actual_value) == var_values_.at(actual_var).end()) {
                        var_values_.at(actual_var).push_back(actual_value);
                    }
                }
                //push back only values that aren't there
                //TODO
            }
            if (mdp.second->abstract_states_.find(var) != mdp.second->abstract_states_.end()) {
                map<string, string> states_values = mdp.second->abstract_states_[var];
                map<string, string> new_states_values = mdp.second->abstract_states_[var];
                for (auto v : states_values) {
                    string actual_value = v.second;
                    if (std::find(mdp.second->parameters_.begin(), mdp.second->parameters_.end(), v.second) != mdp.second->parameters_.end()) {
                        actual_value = actual_value + i_s;
                    }
                    new_states_values[v.first] = actual_value;
                }
                string actual_name = var;
                if (mdp.second->variable_parameter_.find(var) != mdp.second->variable_parameter_.end()) {
                    actual_name = convertToMultiParameter(mdp.second, var, i);
                }
                abstract_states_[actual_name] = new_states_values;
            }
        }
        for (auto par : mdp.second->parameters_) {
            string new_par_name = par + i_s;
            parameters_.push_back(new_par_name);
            for (auto par_var : mdp.second->parameter_variables_[par]) {
                string new_var_name = convertToMultiParameter(mdp.second, par_var, i);
                parameter_variables_[new_par_name].push_back(new_var_name);
                //              }
            }
        }
        vector<string> mdp_param_actions;
        for (string action : mdp.second->actions_) {
            vector<string> action_parts = StringOperations::stringSplit(action, '_');
            string new_action = "";
            for (int i = 0; i < action_parts.size() - 1; i++) {
                string part = action_parts[i];
                if (std::find(mdp.second->parameters_.begin(), mdp.second->parameters_.end(), part) != mdp.second->parameters_.end()) {
                    new_action = new_action + part + i_s + "_";
                } else {
                    new_action = new_action + part + "_";
                }
            }
            string part = action_parts[action_parts.size() - 1];
            if (std::find(mdp.second->parameters_.begin(), mdp.second->parameters_.end(), part) != mdp.second->parameters_.end()) {
                new_action = new_action + part + i_s;
            } else {
                new_action = new_action + part;
            }
            mdp_param_actions.push_back(new_action);
        }
        if (std::find(mdp_param_actions.begin(), mdp_param_actions.end(), "agent" + i_s + "_wait") == mdp_param_actions.end()) {
            mdp_param_actions.push_back("agent" + i_s + "_wait");
        }
        all_actions.push_back(mdp_param_actions);
        i++;
    }
    //    for (string v : variables) {
    //        vector<string> values = varValues.at(v);
    //        if (std::find(values.begin(), values.end(), "agentp0") != values.end() ||
    //                std::find(values.begin(), values.end(), "agentp1") != values.end()) {
    //            if (std::find(values.begin(), values.end(), "agentp0") == values.end()) {
    //                values.push_back("agentp0");
    //            }
    //            if (std::find(values.begin(), values.end(), "agentp1") == values.end()) {
    //                values.push_back("agentp1");
    //            }
    //        }
    //        varValues[v] = values;
    //    }

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
        if (std::find(forbidden_actions_.begin(), forbidden_actions_.end(), action_name) == forbidden_actions_.end()) {
            actions_.push_back(action_name);
        }
    }
    vector<string> new_actions;
    for (string a : actions_) {
        vector<string> agent_parts = StringOperations::stringSplit(a, '-');
        for (string agent_action : agent_parts) {
            vector<string> action_parts = StringOperations::stringSplit(agent_action, '_');
            if (action_parts[1] == "get") {
                string handover_action;

                if (action_parts[0] == "agentp0") {
                    handover_action = "agentp1_handover_" + action_parts[2] + "_agentp0";
                } else {
                    handover_action = "agentp0_handover_" + action_parts[2] + "_agentp1";
                }
                if (std::find(new_actions.begin(), new_actions.end(), handover_action) == new_actions.end()) {
                    new_actions.push_back(handover_action);
                    joint_actions_.push_back(handover_action);

                }
                if (std::find(joint_modules_.begin(), joint_modules_.end(), "handover") == joint_modules_.end()) {
                    joint_modules_.push_back("handover");
                }
            }
        }
    }
    for (string a : new_actions) {
        actions_.push_back(a);
    }

}

void Mmdp::createSubMdpNames(string name) {
    vector<string> single_names = StringOperations::stringSplit(name, '-');
    int i = 0;
    for (auto mdp : agent_hmpd_) {
        mdp.second->name_ = single_names[i];
        i++;
    }
}

void Mmdp::create(string action_name, bool rewrite, bool first) {
    if (!is_created_) {
        is_created_ = true;




        for (auto a : agent_hmpd_) {
            name_ = StringOperations::addToString(name_, a.second->name_, '-');
        }


        parametrized_name_ = action_name;

        cout << "Creating " << parametrized_name_ << "\n";

        std::set<string> changed_mdps;
        for (auto agent : agent_hmpd_) {
            changed_mdps.insert(agent.first);
        }
        createJointMdpVariables();
        assignParametersFromActionName(parametrized_name_, changed_mdps, map<string, string>());




        //        createSubMmdps();

        enumerateStates();

        for (string a : actions_) {
            if (name_ == "agent_apply_bracket_surface-agent_get_object" && a == "agentp0_get_bracketp0-agentp1_move_table") {
                cout << "";
            }


            pair<vector<string>, set<string> > sub_mdp_result = getSubMdpName(a);
            string module_name = sub_mdp_result.first[0];
            if (module_name != "this") {
                hierarchy_map_[module_name] = mmdp_manager_->getMmdp(module_name, sub_mdp_result.first[2], rewrite, false);
                //                if (hierarchy_map_[module_name]->is_created_ == false) {
                //                    hierarchy_map_[module_name]->create(sub_mdp_result.first[2], rewrite, false);
                //                }
            }
        }



        string fileName;
        if (hasParametersInCommon().size() > 0) {
            fileName = parametrized_name_;
        } else {
            fileName = name_;
        }

        bool has_read = readMdp(fileName + ".pomdp", rewrite);

        enumerateGoalAndStartStates();

        assignParametersFromActionName(parametrized_name_, changed_mdps, map<string, string>());

        if (!has_read) {
            enumerateFunctions(fileName + ".pomdp");
        }

        valueIteration(fileName + ".policy", rewrite);

        hierarchic_file_ = fileName + ".hmdp";

        if (!first) {
            ifstream i_file(fileName + ".hmdp");
            if (!rewrite && i_file.good()) {
                i_file.close();
                //                readHierarchical(fileName + ".hmdp");
            } else {
                calculateHierarchicReward();
                calculateHierarchicTransition();
                writeHierarchical(fileName + ".hmdp");

            }

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
                if (agent.second->parameter_action_place_.find(j) != agent.second->parameter_action_place_.end()) {
                    instance[agent.second->parameter_action_place_[j] + i_s] = action_parts[j];
                }
            }
        }
        i++;
    }

    parametrized_name_ = action_name;
    assignParameters(instance);
    i = 0;
    for (auto mdp : agent_hmpd_) {
        if (changed_mdps.find(mdp.first) == changed_mdps.end()) {
            i++;
            continue;
        }
        mdp.second->parametrized_name_ = single_actions[i];
        assignParametersToMdp(mdp.second, i);
        i++;
    }
}

string Mmdp::convertToSingleParameter(string var_name, int index) {
    string i_s = "p" + boost::lexical_cast<string>(index);
    string actual_var_name = var_name;
    if (variable_parameter_.find(var_name) != variable_parameter_.end()) {
        string par = variable_parameter_[var_name];
        actual_var_name.replace(actual_var_name.find(par, 0), par.length(), par.substr(0, par.length() - i_s.length()));
    } else if (std::find(parameters_.begin(), parameters_.end(), var_name) != parameters_.end()) {
        string par = var_name;
        actual_var_name.replace(actual_var_name.find(par, 0), par.length(), par.substr(0, par.length() - i_s.length()));
    }
    return actual_var_name;

}

string Mmdp::convertToMultiParameter(Mdp* mdp, string var_name, int i) {
    string i_s = "p" + boost::lexical_cast<string>(i);
    string actual_var_name = var_name;
    if (mdp->variable_parameter_.find(var_name) != mdp->variable_parameter_.end()) {
        string par = mdp->variable_parameter_[var_name];
        actual_var_name.replace(actual_var_name.find(par, 0), par.length(), par + i_s);
        return actual_var_name;
    } else if (std::find(mdp->parameters_.begin(), mdp->parameters_.end(), var_name) != mdp->parameters_.end()) {
        return actual_var_name + i_s;
    } else return actual_var_name;
}

pair<VarStateProb, set<string> > Mmdp::joinMdpFutureStates(VarStateProb mdp_future_state, VarStateProb cumulative_future_state,
        VariableSet mmdp_instance_state, Hmdp *mdp, int index, bool *no_incongruences,
        std::set<string> new_not_present_variables, VariableSet old_single_agent_state, set<string> abstract_variables,
        set<string> old_abstract_variables) {
    VarStateProb result;

    pair<VarStateProb, set<string> > complete_result;
    VariableSet old_single_agent_depar = mdp->convertToDeparametrizedState(old_single_agent_state, VariableSet());


    //if one of the two states is empty we return the other
    if (mdp_future_state.empty()) {
        complete_result.first = result;
        complete_result.second = old_abstract_variables;
        return complete_result;
    } else if (cumulative_future_state.empty()) {
        //we return mdp_future_state but converted in the instance space
        for (auto state : mdp_future_state) {
            VariableSet instance_state = mdp->convertToDeparametrizedState(state.first, VariableSet());
            VariableSet instance_state_only_present;
            for (auto s : instance_state.set) {
                if (new_not_present_variables.find(s.first) == new_not_present_variables.end()) {
                    instance_state_only_present.set[s.first] = s.second;
                }
            }
            result[instance_state_only_present] = state.second;
        }
        complete_result.first = result;
        complete_result.second = abstract_variables;
        return complete_result;
    }
    //if both states aren't empty we mix them
    for (auto state : mdp_future_state) {
        VariableSet instance_state = mdp->convertToDeparametrizedState(state.first, VariableSet());

        for (auto cumulative_state : cumulative_future_state) {
            VariableSet new_cumulative_state = cumulative_state.first;

            for (auto var : instance_state.set) {
                //if we should consider this variable (it doesn't have a value not present in the current sub-mdp)
                if (new_not_present_variables.find(var.first) == new_not_present_variables.end()) {
                    //value of the variable before the transition (original mmdp space)
                    string original_value = mmdp_instance_state.set[var.first];
                    string old_this_value = old_single_agent_depar.set[var.first];
                    //if the variable is not present in the cumulative state we just add it
                    if (new_cumulative_state.set.find(var.first) == new_cumulative_state.set.end()) {
                        new_cumulative_state.set[var.first] = var.second;
                    }//if it's there it's more complex
                    else if (new_cumulative_state.set[var.first] != var.second) { //if they have different values 
                        if (old_abstract_variables.find(var.first) != old_abstract_variables.end() &&
                                abstract_variables.find(var.first) == abstract_variables.end()) {
                            //if one is abstract take the other
                            new_cumulative_state.set[var.first] = var.second;
                        } else if (new_cumulative_state.set[var.first] == original_value &&
                                var.second != original_value && var.second != old_this_value) {
                            //case 1: if one of the two values is different from original we take the other value
                            //we only consider the case where the new one is different. If not we just keep the old one
                            new_cumulative_state.set[var.first] = var.second;
                        } else if (new_cumulative_state.set[var.first] != original_value &&
                                var.second != original_value && var.second != old_this_value
                                && old_abstract_variables.find(var.first) == old_abstract_variables.end()
                                && abstract_variables.find(var.first) == abstract_variables.end()) {
                            //case 3: the values are different from themselves and from original and 
                            //both are not abstract. It's an incongruency and we quit
                            *no_incongruences = false;
                            return complete_result;
                        }
                    }
                }
            }
            //                else {
            //                    new_cumulative_state.set[var.first] = var.second;
            //                }

            result[new_cumulative_state] = cumulative_state.second * state.second;
        }

    }
    for (string abstract : abstract_variables) {
        if (old_abstract_variables.find(abstract) == old_abstract_variables.end()) {
            old_abstract_variables.insert(abstract);
        }
    }
    complete_result.first = result;
    complete_result.second = old_abstract_variables;
    return complete_result;
}

void Mmdp::testEnumerate(int i, string action) {
    if (!isMmdpStateCongruent(vec_state_enum_[i])) return;
    string depar_action = getDeparametrizedAction(action);
    double r = 0;
    StateProb future_beliefs;

    vector<string> single_actions = StringOperations::stringSplit(action, '-');

    pair<vector<string>, set<string> > sub_mdp_details = getSubMdpName(action);
    string module_name = sub_mdp_details.first[0];
    string action_name = sub_mdp_details.first[2];
    set<string> changed_mdps = sub_mdp_details.second;
    bool no_incongruences = true; //this will become false if 2 mdp states in the instance space have a different value (and different from the original too)
    Hmdp * h;

    bool has_wait = false;

    for (int i = 0; i < single_actions.size(); i++) {
        if (single_actions[i] == "agentp" + to_string(i) + "_wait") {
            has_wait = true;
            break;
        }
    }

    VariableSet v_deparam = convertToDeparametrizedState(vec_state_enum_[i], VariableSet());

    if (module_name == "this") {
        //if it's not a hierarchical action we have to check the state from each mdp, and look 
        //to see if the resulting state is not incongruent

        vector<string> single_actions = StringOperations::stringSplit(action, '-');
        int index = 0;
        VarStateProb cumulative_future_mdp_states; //this will hold the joint mdp future states in the instance space
        VariableSet mmdp_instance_state = convertToDeparametrizedState(vec_state_enum_[i], VariableSet()); //will need it to check for inconsistencies

        set<string> old_abstract_variables;
        for (auto mdp : agent_hmpd_) {

            vector<string> action_parts = StringOperations::stringSplit(single_actions[index], '_');
            VarStateProb mdp_future_states;
            tuple<VariableSet, set<string>, set<string > > mdp_state_result = convertToMdpState(mdp.second, index, vec_state_enum_[i]);
            VariableSet mdp_state = std::get<0>(mdp_state_result);
            set<string> not_present_variables = std::get<1>(mdp_state_result);
            set<string> abstract_variables = std::get<2>(mdp_state_result);
            if (action_parts[1] == "wait") {
                mdp_future_states[mdp_state] = 1;
            } else {
                string mdp_action = convertToSingleMdpAction(mdp.second, index, single_actions[index]);
                mdp_future_states = mdp.second->transitionFunction(mdp_state, mdp_action);
                //                                                r = r + mdp.second->rewardFunction(mdp_state, mdp_action);
            }
            pair<VarStateProb, set<string> > join_result = joinMdpFutureStates(mdp_future_states, cumulative_future_mdp_states,
                    mmdp_instance_state, mdp.second, index, &no_incongruences, not_present_variables, mdp_state,
                    abstract_variables, old_abstract_variables);
            old_abstract_variables = std::get<1>(join_result);
            cumulative_future_mdp_states = std::get<0>(join_result);

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
                int i = map_state_enum_.at(converted_state);
                future_beliefs[map_state_enum_.at(converted_state)] = state.second;
            }
        } else {
        }
    } else {
        //if it's a hierarchical action it's the same as an hmdp
        if (std::find(joint_actions_.begin(), joint_actions_.end(), action) == joint_actions_.end()) {
            h = hierarchy_map_[module_name];
            Mmdp *mmdp_h = (Mmdp*) h;

            mmdp_h->assignParametersFromActionName(depar_action, changed_mdps, parameter_instances_);
        } else {
            h = hierarchy_map_[module_name];
            h->assignParametersFromActionName(depar_action);
        }

        VarStateProb temp_future_beliefs = h->getHierarchicTransition(v_deparam, this);

        for (auto temp_b : temp_future_beliefs) {
            VariableSet fb = temp_b.first;
            VariableSet param_fb = convertToParametrizedState(fb);
            for (auto v : vec_state_enum_[i].set) {
                if (param_fb.set.find(v.first) == param_fb.set.end()) {
                    param_fb.set[v.first] = v.second;
                }
            }

            future_beliefs[map_state_enum_.at(param_fb)] = temp_b.second;
        }
        VariableSet vstry = vec_state_enum_[i];

    }

    for (auto belief : future_beliefs) {
        VariableSet v = vec_state_enum_[belief.first];
        cout << v.toString() << "\n";
    }

    if (module_name != "this") {
        r = h->getHierarchicReward(v_deparam, this);
    } else if (no_incongruences) {
        r = use_cost_ ? 1 : rewardFunction(vec_state_enum_[i], action);

    } else {
        r = use_cost_ ? 1000 : 0;
    }
    cout << r << "\n";
}

void Mmdp::enumerateFunctions(string fileName) {
    ofstream file(fileName);
    cout << "Starting Enumeration\n";

    int nline = 0;

    for (string action : actions_) {
        string depar_action = getDeparametrizedAction(action);


        vector<string> single_actions = StringOperations::stringSplit(action, '-');

        pair<vector<string>, set<string> > sub_mdp_details = getSubMdpName(action);
        string module_name = sub_mdp_details.first[0];
        //        string action_name = sub_mdp_details.first[2];
        set<string> changed_mdps = sub_mdp_details.second;

        Hmdp * h;
        if (module_name != "this") {
            //if it's a hierarchical action it's the same as an hmdp
            if (std::find(joint_actions_.begin(), joint_actions_.end(), action) == joint_actions_.end()) {
                h = hierarchy_map_[module_name];
                Mmdp *mmdp_h = (Mmdp*) h;
                mmdp_h->assignParametersFromActionName(depar_action, changed_mdps, parameter_instances_);
            } else {
                h = hierarchy_map_[module_name];
                h->assignParametersFromActionName(depar_action);
            }
            h->loadHierarchicInCache();
        }

        for (int i = 0; i < vec_state_enum_.size(); i++) {

            if (!isMmdpStateCongruent(vec_state_enum_[i])) continue;
            VariableSet v_deparam = convertToDeparametrizedState(vec_state_enum_[i], VariableSet());


            double r = 0;
            StateProb future_beliefs;


            bool no_incongruences = true; //this will become false if 2 mdp states in the instance space have a different value (and different from the original too)

            if (module_name == "this") {
                //if it's not a hierarchical action we have to check the state from each mdp, and look 
                //to see if the resulting state is not incongruent

                int index = 0;
                VarStateProb cumulative_future_mdp_states; //this will hold the joint mdp future states in the instance space

                set<string> old_abstract_variables;
                for (auto mdp : agent_hmpd_) {

                    vector<string> action_parts = StringOperations::stringSplit(single_actions[index], '_');
                    VarStateProb mdp_future_states;
                    tuple<VariableSet, set<string>, set<string> > mdp_state_result = convertToMdpState(mdp.second, index, vec_state_enum_[i]);
                    VariableSet mdp_state = get<0>(mdp_state_result);
                    set<string> not_present_variables = get<1>(mdp_state_result);
                    set<string> abstract_variables = get<2>(mdp_state_result);
                    if (action_parts[1] == "wait") {
                        mdp_future_states[mdp_state] = 1;
                    } else {
                        string mdp_action = convertToSingleMdpAction(mdp.second, index, single_actions[index]);
                        mdp_future_states = mdp.second->transitionFunction(mdp_state, mdp_action);
                        //                                                r = r + mdp.second->rewardFunction(mdp_state, mdp_action);
                    }
                    pair<VarStateProb, set<string> > join_result = joinMdpFutureStates(mdp_future_states, cumulative_future_mdp_states,
                            v_deparam, mdp.second, index, &no_incongruences, not_present_variables, mdp_state,
                            abstract_variables, old_abstract_variables);

                    cumulative_future_mdp_states = std::get<0>(join_result);
                    old_abstract_variables = std::get<1>(join_result);
                    if (!no_incongruences) {
                        break;
                    }
                    index++;
                }

                if (no_incongruences) {
                    //convert the cumulative_future_state to parameter state
                    for (auto state : cumulative_future_mdp_states) {
                        VariableSet converted_state = convertToParametrizedState(state.first);
                        int i = map_state_enum_.at(converted_state);
                        future_beliefs[map_state_enum_.at(converted_state)] = state.second;
                    }
                } else {
                }
            } else {

                VarStateProb temp_future_beliefs = h->getHierarchicTransition(v_deparam, this);

                for (auto temp_b : temp_future_beliefs) {
                    VariableSet fb = temp_b.first;
                    VariableSet param_fb = convertToParametrizedState(fb);
                    for (auto v : vec_state_enum_[i].set) {
                        if (param_fb.set.find(v.first) == param_fb.set.end()) {
                            param_fb.set[v.first] = v.second;
                        }
                    }

                    future_beliefs[map_state_enum_.at(param_fb)] = temp_b.second;
                }
            }


            PairStateAction transitionInput{i, action};
            for (auto belief : future_beliefs) {
                int s = belief.first;

                file << s << " " << belief.second << " ";
                PairStateAction bTransitionInput{s, action};
                std::vector<int> previousBeliefs = predecessors_[bTransitionInput];
                previousBeliefs.push_back(i);
                predecessors_[bTransitionInput] = previousBeliefs;
            }
            file << "\n";
            nline++;
            transition_[transitionInput] = future_beliefs;

            if (module_name != "this") {
                //                r = use_cost_ ? h->getHierarchicReward(v_deparam, this) : rewardFunction(vecStateEnum[i], action);
                r = h->getHierarchicReward(v_deparam, this);
            } else if (no_incongruences) {
                r = use_cost_ ? 1 : rewardFunction(vec_state_enum_[i], action);

            } else {
                r = use_cost_ ? 1000 : 0;

            }

            PairStateAction rewardInput{i, action};
            reward_[rewardInput] = r;
            file << r << "\n";
            nline++;
        }
        if (module_name != "this") {
            h->emptyHierarchicCache();
        }
    }
    file.close();
}

void Mmdp::enumerateGoalAndStartStates() {
    for (auto state : map_state_enum_) {
        int i = 0;
        bool is_starting_state = true;
        bool is_goal_state = false;
        for (auto mdp : agent_hmpd_) {
            tuple<VariableSet, set<string>, set<string> > mdp_state_result = convertToMdpState(mdp.second, i, state.first);
            VariableSet mdp_state = std::get<0> (mdp_state_result);
            is_starting_state = is_starting_state && mdp.second->isStartingState(mdp_state);
            is_goal_state = is_goal_state || mdp.second->isGoalState(mdp_state);
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
    vector<bool> is_goal_state_for_agent;

    int index = 0;
    double reward = 0;
    //        for (auto a : agent_hmpd_) {
    //            pair<VariableSet, set<string> > mdp_state = convertToMdpState(a.second, index, state);
    //            if (a.second->isGoalState(mdp_state.first)) {
    //                reward = reward + 500;
    //            }
    //        }
    //        return reward;

    //    

    //    for (auto a: agent_hmpd_) {
    //        VariableSet mdp_state=convertToMdpState(a.second,index,state);
    //        r=r+a.second->rewardFunction(mdp_state,)
    //        i++;
    //    }
    //    
    //    pair<vector<string>, set<string> > sub_mdp_details = getSubMdpName(action);
    //    if (sub_mdp_details.first[0] == "this") C{
    StateProb new_states;

    PairStateAction p{map_state_enum_.at(state), action};
    new_states = transition_[p];
    for (auto s : new_states) {
        for (auto a : agent_hmpd_) {
            tuple<VariableSet, set<string>, set<string> > mdp_state_result = convertToMdpState(a.second, index, vec_state_enum_[s.first]);
            VariableSet mdp_state = std::get<0>(mdp_state_result);
            if (a.second->isGoalState(mdp_state)) {
                reward = reward + 500;
            }
            index++;
        }
        //            if (isGoalState(vecStateEnum[s.first])) {
        //                return 100000000;
        //            }
    }
    //    } else {
    //        map<VariableSet, double> new_states = hierarchy_map_[sub_mdp_details.first[0]]->getHierarchicTransition(convertToDeparametrizedState(state, VariableSet()), this);
    //        for (auto s : new_states) {
    //            VariableSet param_vs = convertToParametrizedState(s.first);
    //            for (auto a : agent_hmpd_) {
    //                pair<VariableSet, set<string> > mdp_state = convertToMdpState(a.second, index, param_vs);
    //                if (a.second->isGoalState(mdp_state.first)) {
    //                    reward = reward + 500;
    //                }
    //
    //                //            if (isGoalState(convertToParametrizedState(s.first))) {
    //                //                return 100000000;
    //                //            }
    //            }
    //        }
    //    }

    return reward;
    //    if (isGoalState(state)) return 1000;
}

bool Mmdp::isStartingState(VariableSet state) {
    int i_state = map_state_enum_.at(state);
    return std::find(starting_states_.begin(), starting_states_.end(), i_state) != starting_states_.end();
}

bool Mmdp::isGoalState(VariableSet state) {
    int i_state = map_state_enum_.at(state);
    return std::find(goal_states_.begin(), goal_states_.end(), i_state) != goal_states_.end();
}

std::tuple<VariableSet, set < string >, set<string> > Mmdp::convertToMdpState(Hmdp* mdp, int index, VariableSet mmdp_state) {
    tuple<VariableSet, set<string>, set<string> > result;
    set<string> not_present_variables;
    set<string> abstract_variables;
    VariableSet mdp_state;
    //    cout<<mmdp_state.toString()<<"\n";
    for (auto mdp_var : mdp->variables_) {
        string actual_var_name = mdp_var;
        if (mdp->variable_parameter_.find(mdp_var) != mdp->variable_parameter_.end()) {
            actual_var_name = convertToMultiParameter(mdp, mdp_var, index);
        }
        string original_var_name = actual_var_name;
        if (parametrized_to_original_.find(actual_var_name) != parametrized_to_original_.end()) {
            original_var_name = parametrized_to_original_[actual_var_name];
        }
        string actual_var_value = mmdp_state.set[actual_var_name];
        bool my_parameter = false;
        if (std::find(parameters_.begin(), parameters_.end(), actual_var_value) != parameters_.end()) {
            string agent_parameter = actual_var_value.substr(actual_var_value.length() - 1);
            if (agent_parameter == to_string(index)) { //it's actually a parameter of this mmdp
                actual_var_value = convertToSingleParameter(actual_var_value, index);
                my_parameter = true;
            } else {
                actual_var_value = parametrized_to_original_.at(actual_var_value);
            }
        }
        if (mdp->abstract_states_.find(mdp_var) != mdp->abstract_states_.end() && !my_parameter) {
            //            if (std::find(mdp->varValues.at(mdp_var).begin(), mdp->varValues.at(mdp_var).end(), actual_var_value) == mdp->varValues.at(mdp_var).end()) {
            if (mdp->abstract_states_.at(mdp_var).find(actual_var_value) != mdp->abstract_states_.at(mdp_var).end()) {
                actual_var_value = mdp->abstract_states_.at(mdp_var)[actual_var_value];
                abstract_variables.insert(original_var_name);
            } else {
                for (auto s : mdp->abstract_states_.at(mdp_var)) {
                    if (s.second == actual_var_value) {
                        abstract_variables.insert(original_var_name);
                        break;
                    }
                }
            }
        }
        //            }

        if (std::find(mdp->var_values_.at(mdp_var).begin(), mdp->var_values_.at(mdp_var).end(), actual_var_value) == mdp->var_values_.at(mdp_var).end()) {
            cout << "WARNING! ABSTRACT VALUE NOT FOUND IN CONVERTTOMDPSTATE\n";

        }

        //        if (std::find(parameters.begin(), parameters.end(), actual_var_value) != parameters.end()) {
        //            string agent_parameter = actual_var_value.substr(actual_var_value.length() - 1);
        //            if (agent_parameter == to_string(index)) { //it's actually a parameter of this mmdp
        //                actual_var_value = convertToSingleParameter(actual_var_value, index);
        //            } else {
        //                not_present = true;
        //                not_present_variables.insert(actual_var_name);
        //                actual_var_value = mdp->varValues.at(mdp_var)[0]; //metti un valore a caso per la transizione
        //            }
        //        }
        //        if (!not_present) { //if the value is abstract we consider it as not present, because we might have more accurate info from the
        //            //other mdp
        //            if (mdp->abstract_states_.find(mdp_var) != mdp->abstract_states_.end()) {
        //                for (auto ass : mdp->abstract_states_.at(mdp_var)) {
        //                    if (ass.second == actual_var_value) {
        //                        abstract_variables.insert(actual_var_name);
        //                    }
        //                }
        //            }
        //        }
        mdp_state.set[mdp_var] = actual_var_value;
    }
    result = std::make_tuple(mdp_state, not_present_variables, abstract_variables);
    return result;
}

VariableSet Mmdp::convertToMmdpState(VariableSet mdp_state, Hmdp* mdp, int index) {
    VariableSet mmdp_state;
    for (auto var : mdp_state.set) {
        string actual_var_name = var.first;
        string actual_var_value = var.second;
        if (mdp->variable_parameter_.find(actual_var_name) != mdp->variable_parameter_.end()) {
            actual_var_name = convertToMultiParameter(mdp, actual_var_name, index);
        }
        if (std::find(mdp->parameters_.begin(), mdp->parameters_.end(), actual_var_value) != mdp->parameters_.end()) {
            actual_var_value = convertToMultiParameter(mdp, actual_var_value, index);
        }
        mmdp_state.set[actual_var_name] = actual_var_value;
    }
    return mmdp_state;
}

void Mmdp::assignParametersToMdp(Hmdp* mdp, int index) {
    std::map<string, string> instance;
    for (auto parameter : mdp->parameters_) {
        string actual_par = convertToMultiParameter(mdp, parameter, index);
        instance[parameter] = parameter_instances_[actual_par];
    }
    //    for (auto i : instance) {
    //        cout << i.first << " " << i.second << "\n";
    //    }
    mdp->assignParameters(instance);
}

bool Mmdp::isMmdpStateCongruent(VariableSet state) {
    std::map<string, string> depar_state;
    map<string, vector<string> > var_abstract_values;


    for (auto var : state.set) {
        string par_var = var.first;
        string par_value = var.second;
        string original_value = var.second;
        if (parametrized_to_original_.find(par_value) != parametrized_to_original_.end()) {
            original_value = parametrized_to_original_[par_value];
        }
        //if it's not a parameter than it has to be congruent
        if (parametrized_to_original_.find(par_var) != parametrized_to_original_.end()) {
            string original_var = parametrized_to_original_[par_var];


            if (depar_state.find(original_var) != depar_state.end()) {
                string depar_value = depar_state.at(original_var);
                if (original_value != depar_value) {
                    //case 1: one of the two values is abstract, while the other is  not.
                    //in this case if the abstract var range includes the real value it's ok, if not it's incongruent
                    if (abstract_states_.find(par_var) != abstract_states_.end()) {
                        vector<string> abstract_values = getAbstractLinkedValues(par_var, par_value);
                        if (abstract_values.size() == 0) {
                            abstract_values.push_back(par_value); //variable is abstract but not value
                        }
                        //this is abstract but the original is not
                        if (var_abstract_values.find(original_var) == var_abstract_values.end()) {
                            if (std::find(abstract_values.begin(), abstract_values.end(), depar_value) == abstract_values.end()) {
                                return false;
                            }
                        } else {
                            //both are abstract. In this case the state is congruent if the intersection between the two abstract_values
                            //is not null. This intersection will also become the next abstract_values for this var.
                            vector<string> other_abstract_values = var_abstract_values.at(original_var);
                            vector<string> intersection(10);
                            std::set_intersection(abstract_values.begin(), abstract_values.end(),
                                    other_abstract_values.begin(), other_abstract_values.end(),
                                    intersection.begin());
                            if (intersection.size() == 0) {
                                return false;
                            } else {
                                var_abstract_values[original_var] = intersection;
                            }

                        }
                    }

                }
            } else {
                depar_state[original_var] = original_value;
                //saves states linked to this abstract var
                if (abstract_states_.find(par_var) != abstract_states_.end()) {
                    vector<string> abstract_values;
                    for (auto s : abstract_states_[par_var]) {
                        if (s.second == par_value) {
                            abstract_values.push_back(s.first);
                        }
                    }
                    var_abstract_values[original_var] = abstract_values;
                }

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
            if (parametrized_to_original_.find(s) != parametrized_to_original_.end()) {
                depar_action_name << parametrized_to_original_[s] << "_";
            } else {
                depar_action_name << s << "_";
            }

        }
        string s = action_parts[action_parts.size() - 1];
        s = convertToMultiParameter(mdp[i], s, i);

        if (parametrized_to_original_.find(s) != parametrized_to_original_.end()) {
            depar_action_name << parametrized_to_original_[s];
        } else {
            depar_action_name << s;
        }
        if (i < single_actions.size() - 1) {
            depar_action_name << "-";
        }
    }
    return depar_action_name.str();

}

string Mmdp::convertToSingleMdpAction(Mdp *mdp, int index, string mmdp_action) {
    vector<string> action_parts = StringOperations::stringSplit(mmdp_action, '_');
    stringstream ss;
    for (int i = 0; i < action_parts.size() - 1; i++) {
        if (std::find(parameters_.begin(), parameters_.end(), action_parts[i]) != parameters_.end()) {
            string actual_parameter = convertToSingleParameter(action_parts[i], index);
            ss << actual_parameter << "_";
        } else {
            ss << action_parts[i] << "_";
        }
    }
    int i = action_parts.size() - 1;
    if (std::find(parameters_.begin(), parameters_.end(), action_parts[i]) != parameters_.end()) {
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
    for (string a : actions_) {

        string generic_name = ""; //the generic name is the name of the modules.
        string specific_name = ""; //the specific name is the name of actions

        vector<string> wait_indexs;
        string depar_action = getDeparametrizedAction(a);

        vector<string> single_actions_depar = StringOperations::stringSplit(depar_action, '-');
        vector<string> single_actions = StringOperations::stringSplit(a, '-');
        vector<string> parametrized_sub_names = StringOperations::stringSplit(parametrized_name_, '-');
        int action_index = 0;
        bool is_hierarchical = false;
        Mmdp* sub_mmdp = new Mmdp(mmdp_manager_);

        for (auto mdp_agent : agent_hmpd_) {
            vector<string> action_parts = StringOperations::stringSplit(single_actions[action_index], '_');


            if (action_parts[1] == "wait") {
                //
                wait_indexs.push_back(mdp_agent.first);
                specific_name = StringOperations::addToString(specific_name, single_actions_depar[action_index], '-');
                generic_name = StringOperations::addToString(generic_name, "agent_wait", '-');
            } else {
                string sub_action = convertToSingleMdpAction(mdp_agent.second, action_index, single_actions[action_index]);

                Hmdp *this_agent_hmdp = mdp_agent.second;
                //                this_agent_hmdp->assignParametersFromActionName(single_actions[action_index]);
                if (this_agent_hmdp->hierarchy_map_.find(sub_action) != this_agent_hmdp->hierarchy_map_.end()) {

                    this_agent_hmdp->hierarchy_map_[sub_action]->assignParametersFromActionName(single_actions_depar[action_index]);

                    sub_mmdp->agent_hmpd_[mdp_agent.first] = this_agent_hmdp->hierarchy_map_[sub_action];

                    is_hierarchical = true;
                    specific_name = StringOperations::addToString(specific_name, this_agent_hmdp->hierarchy_map_[sub_action]->parametrized_name_, '-');
                    generic_name = StringOperations::addToString(generic_name, this_agent_hmdp->hierarchy_map_[sub_action]->name_, '-');

                } else {
                    //                    this_agent_hmdp->assignParametersFromActionName(single_actions_depar[action_index]);

                    sub_mmdp->agent_hmpd_[mdp_agent.first] = this_agent_hmdp;
                    specific_name = StringOperations::addToString(specific_name, this_agent_hmdp->parametrized_name_, '-');
                    generic_name = StringOperations::addToString(generic_name, this_agent_hmdp->name_, '-');
                }
                //                sub_mmdp->assignParametersFromActionName()

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
            //when the actions have parameters in common it's necessary to create a new submmdp
            //            bool parameters_in_common = sub_mmdp->hasParametersInCommon();

            //            if (parameters_in_common) {
            //                hierarchy_map_[specific_name] = sub_mmdp;
            //            } else if (already_exists) {
            //                delete sub_mmdp;
            //            } else {
            //                hierarchy_map_[generic_name] = sub_mmdp;
            //            }

        } else {
            delete sub_mmdp;
        }
    }
    //    printHierarchy();
}

vector<string> Mmdp::hasParametersInCommon(map<string, Hmdp*> agents) {
    vector<string> parameters_in_common;
    for (auto agent1 : agents) {
        for (auto agent2 : agents) {
            if (agent1.first != agent2.first) {
                map<string, string> this_instance = agent1.second->parameter_instances_;
                map<string, string> other_instance = agent2.second->parameter_instances_;
                //if they have a parameter in common //EDIT when two parameters had the same value but different names
                // (ex. brakcet= bracket1 and object=bracket1)
                //it didn't work
                for (auto s : this_instance) {
                    for (auto s2 : other_instance) {
                        if (s.second == s2.second) {
                            parameters_in_common.push_back(s.first);
                            continue;
                        }

                    }
                    //                    if (other_instance.find(s.first) != other_instance.end()) {
                    //                        if (other_instance[s.first] == s.second) {
                    //                            parameters_in_common.push_back(s.first);
                    //                        }
                    //                    }
                }
                //but also if a parameter is assigned to a variable of the other submmdps
                for (auto original : agent1.second->parametrized_to_original_) {
                    if (std::find(agent2.second->variables_.begin(), agent2.second->variables_.end(), original.second) != agent2.second->variables_.end()) {
                        if (agent1.second->variable_parameter_.find(original.first) != agent1.second->variable_parameter_.end()) {
                            parameters_in_common.push_back(agent1.second->variable_parameter_.at(original.first));
                        } else {

                            parameters_in_common.push_back(original.first);
                        }
                    }
                }
            }
        }
    }
    return parameters_in_common;
}

vector<string> Mmdp::hasParametersInCommon() {

    return hasParametersInCommon(agent_hmpd_);
}

bool Mmdp::readMdp(string fileName, bool rewrite) {
    int nline = 0;
    ifstream inputFile(fileName);
    if (inputFile.good() && !rewrite) {
        for (string action : actions_) {

            for (int i = 0; i < vec_state_enum_.size(); i++) {
                if (!isMmdpStateCongruent(vec_state_enum_[i])) {
                    continue;
                }
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
                    std::vector<int> previousBeliefs = predecessors_[bTransitionInput];
                    previousBeliefs.push_back(stoi(transition_v[j]));
                    predecessors_[bTransitionInput] = previousBeliefs;

                    j = j + 2;
                }

                transition_[transitionInput] = transitionOutput;

                getline(inputFile, line);
                nline++;
                PairStateAction rewardInput = {i, action};
                reward_[rewardInput] = stoi(line);
            }
        }
        inputFile.close();

        return true;
    }
    return false;
}

pair<vector<string>, set<string> > Mmdp::getSubMdpName(string action) {

    string module_name = "";

    string specific_name;
    string generic_name;

    set<string> changed_mdps;

    pair<vector<string>, set<string> > result;

    if (std::find(joint_actions_.begin(), joint_actions_.end(), action) != joint_actions_.end()) {
        module_name = StringOperations::stringSplit(action, '_')[1];
        specific_name = action;
        generic_name = module_name;
        result.first.push_back(module_name);
        result.first.push_back(generic_name);
        result.first.push_back(specific_name);
        return result;
    }


    int action_index = 0;
    vector<string> single_actions = StringOperations::stringSplit(action, '-');

    string depar_action = getDeparametrizedAction(action);
    vector<string> depar_single_actions = StringOperations::stringSplit(depar_action, '-');
    vector<string> parametrized_this_name = StringOperations::stringSplit(parametrized_name_, '-');
    map<string, Hmdp*> agent_hmdps;

    map<string, vector<string> > agent_single_actions;

    bool is_hierarchical = false;
    for (auto mdp_agent : agent_hmpd_) {
        vector<string> action_parts = StringOperations::stringSplit(single_actions[action_index], '_');
        vector<string> depar_action_parts = StringOperations::stringSplit(depar_single_actions[action_index], '_');

        if (action_parts[1] == "wait") {
            //
            specific_name = StringOperations::addToString(specific_name, depar_action_parts[0] + "_wait", '-');
            generic_name = StringOperations::addToString(generic_name, "agent_wait", '-');
        } else {
            string sub_action = convertToSingleMdpAction(mdp_agent.second, action_index, single_actions[action_index]);

            Hmdp *this_agent_hmdp = mdp_agent.second;
            if (this_agent_hmdp->hierarchy_map_.find(sub_action) != this_agent_hmdp->hierarchy_map_.end()) {
                this_agent_hmdp->hierarchy_map_[sub_action]->assignParametersFromActionName(depar_single_actions[action_index]);
                is_hierarchical = true;
                specific_name = StringOperations::addToString(specific_name, this_agent_hmdp->hierarchy_map_[sub_action]->parametrized_name_, '-');
                generic_name = StringOperations::addToString(generic_name, this_agent_hmdp->hierarchy_map_[sub_action]->name_, '-');
                agent_hmdps[mdp_agent.first] = this_agent_hmdp->hierarchy_map_[sub_action];
                changed_mdps.insert(mdp_agent.first);
            } else {
                this_agent_hmdp->assignParametersFromActionName(parametrized_this_name[action_index]);
                specific_name = StringOperations::addToString(specific_name, this_agent_hmdp->parametrized_name_, '-');
                generic_name = StringOperations::addToString(generic_name, this_agent_hmdp->name_, '-');
                agent_hmdps[mdp_agent.first] = this_agent_hmdp;
            }
        }
        action_index++;
    }
    if (!is_hierarchical) {
        module_name = "this";
    } else {

        vector<string> parameters_in_common = hasParametersInCommon(agent_hmdps);
        if (parameters_in_common.size() > 0) {
            vector<string> agents_generic_action = StringOperations::stringSplit(generic_name, '-');
            int index = 0;
            for (auto a : agent_hmpd_) {
                vector<string> action_parts = StringOperations::stringSplit(agents_generic_action[index], '_');
                agent_single_actions[a.first] = action_parts;
                index++;
            }

            module_name = "";
            for (string p : parameters_in_common) {
                for (auto a : agent_hmpd_) {
                    for (auto place : agent_hmdps[a.first]->parameter_action_place_) {
                        if (place.second == p) {
                            //                            agent_single_actions[a.first][place.first] = agent_hmdps[a.first]->parameter_instances.at(p);
                            agent_single_actions[a.first][place.first] = agent_single_actions[a.first][place.first] + "c";
                        }
                    }
                }
            }
            index = 0;
            for (auto a : agent_hmpd_) {
                if (index != 0) {
                    module_name = module_name + "-";
                }
                for (int i = 0; i < agent_single_actions[a.first].size() - 1; i++) {
                    module_name = module_name + agent_single_actions[a.first][i] + "_";
                }
                module_name = module_name + agent_single_actions[a.first][agent_single_actions[a.first].size() - 1];
                index++;
            }
        } else {
            module_name = generic_name;
        }

    }
    result.first.push_back(module_name);
    result.first.push_back(generic_name);
    result.first.push_back(specific_name);
    result.second = changed_mdps;

    return result;

}

string Mmdp::chooseHierarchicAction(VariableSet state) {
    VariableSet this_state = convertToParametrizedState(state);
    if (isGoalState(this_state)) return "";
    if (active_module == "this") {
        //                        printQValues(this_state);
        string action = chooseAction(map_state_enum_.at(this_state));
        pair<vector<string>, set<string> > sub_mdp_details = getSubMdpName(action);
        string module_name = sub_mdp_details.first[0];
        set<string> changed_mdps = sub_mdp_details.second;
        if (module_name != "this") {
            string dp = getDeparametrizedAction(action);

            active_module = module_name;
            Hmdp *h = hierarchy_map_[active_module];
            if (std::find(joint_modules_.begin(), joint_modules_.end(), module_name) != joint_modules_.end()) {
                h->assignParametersFromActionName(dp);
                cout << h->getHierarchicReward(state, this) << "\n";
            } else {
                Mmdp *mmdp_h = (Mmdp*) h;
                mmdp_h->assignParametersFromActionName(sub_mdp_details.first[2], changed_mdps, parameter_instances_);
            }
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
            return chooseHierarchicAction(state);
        }
    }
}

//VariableSet Mmdp::makeMmdpStateCongruent(VariableSet new_vs, VariableSet original_vs) {
//    VariableSet result_vs;
//    for (auto s : new_vs.set) {
//        string actual_value=s.second;
//        //check if its a parameter
//        if (parametrized_to_original.find(s.first) != parametrized_to_original.end()) {
//            //we need to check if there are other parameters linked to the original variable and if their values conflict
//            string original_var = parametrized_to_original(s.first);
//            vector<string> parameter_vars = original_to_parametrized[original_var];
//            for (string par : parameter_vars) {
//                if (par != s.first) { //if it's another par and it's different from the current variable
//                    if (result_vs.set.find(par) != result_vs.set.end()) {
//                        //if it's already in the set we need to choose which value we take
//                        if (result_vs[par]!=s.second) {
//                            
//                        }
//                    }
//                }
//            }
//        }
//    }
//}

int Mmdp::estimateRemainingCost(VariableSet state) {
    int index = 0;
    int c = 0;

    VariableSet depar_state = convertToDeparametrizedState(state, VariableSet());
    vector<string> module_agents = StringOperations::stringSplit(name_, '-');
    vector<string> action_agents = StringOperations::stringSplit(parametrized_name_, '-');

    for (int i = 0; i < module_agents.size(); i++) {
        if (module_agents[i] == "agent_wait") {
            return 0;
        }
    }
    for (auto agent : agent_hmpd_) {
        tuple<VariableSet, set<string>, set<string> > mdp_state_result = convertToMdpState(agent.second, index, state);
        VariableSet mdp_state = std::get<0>(mdp_state_result);
        if (!agent.second->isGoalState(mdp_state)) {
            //not a goal estate. Estimate the cost to reach the goal state
            vector<string> new_action_name_v;
            vector<string> new_module_name_v;

            for (int i = 0; i < agent_hmpd_.size(); i++) {
                vector<string> module_parts = StringOperations::stringSplit(module_agents[i], '_');
                vector<string> action_parts = StringOperations::stringSplit(action_agents[i], '_');
                if (i == index) {
                    new_action_name_v.push_back(action_agents[index]);
                    new_module_name_v.push_back(module_agents[index]);
                } else {
                    new_action_name_v.push_back(action_parts[0] + "_wait");
                    new_module_name_v.push_back(module_parts[0] + "_wait");
                }
            }
            string new_action_name_s = new_action_name_v[0];
            string new_module_name_s = new_module_name_v[0];
            for (int i = 1; i < new_action_name_v.size(); i++) {
                new_action_name_s = new_action_name_s + "-" + new_action_name_v[i];
            }
            for (int i = 1; i < new_action_name_v.size(); i++) {
                new_module_name_s = new_module_name_s + "-" + new_module_name_v[i];
            }


            Mmdp *single_mmdp = (Mmdp*) mmdp_manager_->getMmdp(new_module_name_s, new_action_name_s, false, false);
            //            VariableSet mmdp_state = single_mmdp->convertToMmdpState(mdp_state.first, agent.second, index);
            int bestq = single_mmdp->getBestQ(depar_state);
            //            c = c + 3;
            c = c + bestq;
        }
        index++;
    }
    return c;

}

void Mmdp::valueIteration(string fileName, bool rewrite) {
    ifstream inputFile(fileName);

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
                vhi[s] = estimateRemainingCost(vec_state_enum_[s]);
                //                                        vhi[s] = 0;
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

string Mmdp::chooseAction(int s) {
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
        } else if (qv == max && a.find("wait") != a.npos) {
            max_action = a;
        }
    }
    return max_action;
}