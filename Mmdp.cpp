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
            delete el.second;
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
        for (auto var : mdp.second->variables) {
            //convert values to multiparameter if needed
            vector<string> actual_var_values;
            for (auto value : mdp.second->varValues.at(var)) {
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
                    if (std::find(varValues.at(actual_var).begin(), varValues.at(actual_var).end(), actual_value) == varValues.at(actual_var).end()) {
                        varValues.at(actual_var).push_back(actual_value);
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
                    if (std::find(mdp.second->parameters.begin(), mdp.second->parameters.end(), v.second) != mdp.second->parameters.end()) {
                        actual_value = actual_value + i_s;
                    }
                    new_states_values[v.first] = actual_value;
                }
                string actual_name = var;
                if (mdp.second->variable_parameter.find(var) != mdp.second->variable_parameter.end()) {
                    actual_name = convertToMultiParameter(mdp.second, var, i);
                }
                abstract_states_[actual_name] = new_states_values;
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
            actions.push_back(action_name);
        }
    }
    vector<string> new_actions;
    for (string a : actions) {
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
        actions.push_back(a);
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
        is_created_ = true;



        for (auto a : agent_hmpd_) {
            name = StringOperations::addToString(name, a.second->name, '-');
        }


        parametrized_name = action_name;

        cout << "Creating " << parametrized_name << "\n";

        std::set<string> changed_mdps;
        for (auto agent : agent_hmpd_) {
            changed_mdps.insert(agent.first);
        }
        createJointMdpVariables();
        assignParametersFromActionName(parametrized_name, changed_mdps, map<string, string>());




        //        createSubMmdps();

        enumerateStates();

        for (string a : actions) {
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
            fileName = parametrized_name;
        } else {
            fileName = name;
        }

        bool has_read = readMdp(fileName + ".pomdp", rewrite);

        enumerateGoalAndStartStates();

        assignParametersFromActionName(parametrized_name, changed_mdps, map<string, string>());

        if (!has_read) {
            enumerateFunctions(fileName + ".pomdp");
        }

        valueIteration(fileName + ".policy", rewrite);

        if (!first) {
            ifstream i_file(fileName + ".hmdp");
            if (!rewrite && i_file.good()) {
                i_file.close();
                readHierarchical(fileName + ".hmdp");
            } else {
                calculateHierarchicReward();
                calculateHierarchicTransition();
            }
            writeHierarchical(fileName + ".hmdp");

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

    parametrized_name = action_name;
    assignParameters(instance);
    i = 0;
    for (auto mdp : agent_hmpd_) {
        if (changed_mdps.find(mdp.first) == changed_mdps.end()) {
            i++;
            continue;
        }
        mdp.second->parametrized_name = single_actions[i];
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
        std::set<string> new_not_present_variables, VariableSet old_single_agent_state) {
    VarStateProb result;

    VariableSet old_single_agent_depar = mdp->convertToDeparametrizedState(old_single_agent_state, VariableSet());


    //if one of the two states is empty we return the other
    if (mdp_future_state.empty()) {
        result = cumulative_future_state;
        return result;
    } else if (cumulative_future_state.empty()) {
        //we return mdp_future_state but converted in the instance space
        for (auto state : mdp_future_state) {
            VariableSet instance_state = mdp->convertToDeparametrizedState(state.first, VariableSet());
            result[instance_state] = state.second;
        }
        return result;
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
                        //case 1: if one of the two values is different from original we take the other value
                        //we only consider the case where the new one is different. If not we just keep the old one
                        if (new_cumulative_state.set[var.first] == original_value &&
                                var.second != original_value && var.second != old_this_value) {
                            new_cumulative_state.set[var.first] = var.second;
                        }//case 2: the values are different from themselves and from original. It's an incongruency and we quit
                        else if (new_cumulative_state.set[var.first] != original_value &&
                                var.second != original_value && var.second != old_this_value) {
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

        if (i==178) {
            cout<<"";
        }
        if (!isMmdpStateCongruent(vecStateEnum[i])) continue;
        for (string action : actions) {
            string depar_action = getDeparametrizedAction(action);
            
            if (i == 178 && action == "agentp0_apply_bracketp0_surfacep0-agentp1_wait") {
                cout << "";
            }

            double r = 0;
            StateProb future_beliefs;

            vector<string> single_actions = StringOperations::stringSplit(action, '-');

            //            if (i == 112 && fileName == "agent_assemble_bracket_surface-agent_wait.pomdp" &&
            //                    action == "agentp0_apply_bracketp0_surfacep0-agentp1_wait") {
            //                cout << "";
            //            }
            


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

            VariableSet v_deparam = convertToDeparametrizedState(vecStateEnum[i], VariableSet());




            if (module_name == "this") {
                //if it's not a hierarchical action we have to check the state from each mdp, and look 
                //to see if the resulting state is not incongruent

                vector<string> single_actions = StringOperations::stringSplit(action, '-');
                int index = 0;
                VarStateProb cumulative_future_mdp_states; //this will hold the joint mdp future states in the instance space
                VariableSet mmdp_instance_state = convertToDeparametrizedState(vecStateEnum[i], VariableSet()); //will need it to check for inconsistencies

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
                        //                                                r = r + mdp.second->rewardFunction(mdp_state, mdp_action);
                    }
                    cumulative_future_mdp_states = joinMdpFutureStates(mdp_future_states, cumulative_future_mdp_states,
                            mmdp_instance_state, mdp.second, index, &no_incongruences, not_present_variables, mdp_state);

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
                        int i = mapStateEnum.at(converted_state);
                        future_beliefs[mapStateEnum.at(converted_state)] = state.second;
                    }
                } else {
                }
            } else {
                //if it's a hierarchical action it's the same as an hmdp
                if (std::find(joint_actions_.begin(), joint_actions_.end(), action) == joint_actions_.end()) {
                    h = hierarchy_map_[module_name];
                    Mmdp *mmdp_h = (Mmdp*) h;

                    mmdp_h->assignParametersFromActionName(depar_action, changed_mdps, parameter_instances);
                } else {
                    if (i == 18) {
                        cout << "";
                    }
                    h = hierarchy_map_[module_name];
                    h->assignParametersFromActionName(depar_action);
                }
                //                h->printParameters();


                //                VariableSet v_param = h->convertToParametrizedState(vecStateEnum[i]);

                //                v_param = fixAbstractStates(v_param, v_deparam, h);

                VarStateProb temp_future_beliefs = h->getHierarchicTransition(v_deparam, this);

                for (auto temp_b : temp_future_beliefs) {
                    VariableSet fb = temp_b.first;
                    VariableSet param_fb = convertToParametrizedState(fb);
                    for (auto v : vecStateEnum[i].set) {
                        if (param_fb.set.find(v.first) == param_fb.set.end()) {
                            param_fb.set[v.first] = v.second;
                        }
                    }

                    future_beliefs[mapStateEnum.at(param_fb)] = temp_b.second;
                }
                VariableSet vstry = vecStateEnum[i];

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

            if (module_name != "this") {
                //                r = use_cost_ ? h->getHierarchicReward(v_deparam, this) : rewardFunction(vecStateEnum[i], action);
                r = h->getHierarchicReward(v_deparam, this);
            } else if (no_incongruences) {
                r = use_cost_ ? 1 : rewardFunction(vecStateEnum[i], action);

            } else {
                r = use_cost_ ? 1000 : 0;

            }
            //            if (no_incongruences && use_cost_ && std::find(joint_actions_.begin(), joint_actions_.end(), action) == joint_actions_.end()
            //                    && !has_wait && isGoalState()) {
            //                int min_cost = 10000;
            //                string generic_name = module_name != "this" ? module_name : name;
            //                for (auto belief : future_beliefs) {
            //                    int c = estimateRemainingCost(vecStateEnum[belief.first], generic_name, action_name);
            //                    min_cost = c < min_cost ? c : min_cost;
            //                }
            //                r = r + min_cost;
            //            }

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

    PairStateAction p{mapStateEnum.at(state), action};
    new_states = transition[p];
    for (auto s : new_states) {
        for (auto a : agent_hmpd_) {
            pair<VariableSet, set<string> > mdp_state = convertToMdpState(a.second, index, vecStateEnum[s.first]);
            if (a.second->isGoalState(mdp_state.first)) {
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
    int i_state = mapStateEnum.at(state);
    return std::find(starting_states_.begin(), starting_states_.end(), i_state) != starting_states_.end();
}

bool Mmdp::isGoalState(VariableSet state) {
    int i_state = mapStateEnum.at(state);
    return std::find(goal_states_.begin(), goal_states_.end(), i_state) != goal_states_.end();
}

pair<VariableSet, set < string >> Mmdp::convertToMdpState(Hmdp* mdp, int index, VariableSet mmdp_state) {
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
                actual_var_value = mdp->varValues.at(mdp_var)[0];
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
                    //if it's an abstract state we could allow the values to be different in some instantiations.
                    if (abstract_states_.find(var.first)!=abstract_states_.end()) {
                        if (abstract_states_.at(var.first).find(actual_value)==abstract_states_.at(var.first).end()
                                && abstract_states_.at(var.first).find(new_value)==abstract_states_.at(var.first).end()
                                ) {
                            return false;
                        } 
                    }
                    else {
                        return false;
                    }
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
        string depar_action = getDeparametrizedAction(a);

        vector<string> single_actions_depar = StringOperations::stringSplit(depar_action, '-');
        vector<string> single_actions = StringOperations::stringSplit(a, '-');
        vector<string> parametrized_sub_names = StringOperations::stringSplit(parametrized_name, '-');
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
                    specific_name = StringOperations::addToString(specific_name, this_agent_hmdp->hierarchy_map_[sub_action]->parametrized_name, '-');
                    generic_name = StringOperations::addToString(generic_name, this_agent_hmdp->hierarchy_map_[sub_action]->name, '-');

                } else {
                    //                    this_agent_hmdp->assignParametersFromActionName(single_actions_depar[action_index]);

                    sub_mmdp->agent_hmpd_[mdp_agent.first] = this_agent_hmdp;
                    specific_name = StringOperations::addToString(specific_name, this_agent_hmdp->parametrized_name, '-');
                    generic_name = StringOperations::addToString(generic_name, this_agent_hmdp->name, '-');
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
                map<string, string> this_instance = agent1.second->parameter_instances;
                map<string, string> other_instance = agent2.second->parameter_instances;
                //if they have a parameter in common 
                for (auto s : this_instance) {
                    if (other_instance.find(s.first) != other_instance.end()) {
                        if (other_instance[s.first] == s.second) {
                            parameters_in_common.push_back(s.first);
                        }
                    }
                }
                //but also if a parameter is assigned to a variable of the other submmdps
                for (auto original : agent1.second->parametrized_to_original) {
                    if (std::find(agent2.second->variables.begin(), agent2.second->variables.end(), original.second) != agent2.second->variables.end()) {
                        if (agent1.second->variable_parameter.find(original.first) != agent1.second->variable_parameter.end()) {
                            parameters_in_common.push_back(agent1.second->variable_parameter.at(original.first));
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
    vector<string> parametrized_this_name = StringOperations::stringSplit(parametrized_name, '-');
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
                specific_name = StringOperations::addToString(specific_name, this_agent_hmdp->hierarchy_map_[sub_action]->parametrized_name, '-');
                generic_name = StringOperations::addToString(generic_name, this_agent_hmdp->hierarchy_map_[sub_action]->name, '-');
                agent_hmdps[mdp_agent.first] = this_agent_hmdp->hierarchy_map_[sub_action];
                changed_mdps.insert(mdp_agent.first);
            } else {
                this_agent_hmdp->assignParametersFromActionName(parametrized_this_name[action_index]);
                specific_name = StringOperations::addToString(specific_name, this_agent_hmdp->parametrized_name, '-');
                generic_name = StringOperations::addToString(generic_name, this_agent_hmdp->name, '-');
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
                    for (auto place : agent_hmdps[a.first]->parameter_action_place) {
                        if (place.second == p) {
                            agent_single_actions[a.first][place.first] = agent_hmdps[a.first]->parameter_instances.at(p);
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
//                printQValues(this_state);
        string action = chooseAction(mapStateEnum.at(this_state));
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
                mmdp_h->assignParametersFromActionName(dp, changed_mdps, parameter_instances);
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
    vector<string> module_agents = StringOperations::stringSplit(name, '-');
    vector<string> action_agents = StringOperations::stringSplit(parametrized_name, '-');

    for (int i = 0; i < module_agents.size(); i++) {
        if (module_agents[i] == "agent_wait") {
            return 0;
        }
    }
    for (auto agent : agent_hmpd_) {
        pair<VariableSet, set<string> > mdp_state = convertToMdpState(agent.second, index, state);
        if (!agent.second->isGoalState(mdp_state.first)) {
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
            c=c+bestq;
        }
        index++;
    }
    return c;

}

void Mmdp::valueIteration(string fileName, bool rewrite) {
    ifstream inputFile(fileName);

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
            if (fileName == "agent_glue_surface-agent_clean_surface.policy") {
                cout << " ";
            }
            for (int s = 0; s < vecStateEnum.size(); s++) { //for each enumeration
                vhiOld[s] = vhi[s];
                if (s == 12) {
                    VariableSet v = vecStateEnum[s];
                    cout << " ";
                }
                if (use_cost_ && isGoalState(vecStateEnum[s])) {
                    vhi[s] = estimateRemainingCost(vecStateEnum[s]);
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