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

double Hmdp::getHierarchicReward(int i) {
    return hierarchic_reward_[i];
}

double Hmdp::getHierarchicReward(VariableSet set) {

    VariableSet parametrized_set = convertToParametrizedState(set);
    int i = convertHierarchicState(parametrized_set);
    double r = hierarchic_reward_[i];
    return hierarchic_reward_[i];
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

int Hmdp::convertHierarchicState(VariableSet set) {
    VariableSet this_set;
    for (string v : variables) {
        this_set.set[v] = set.set[v];
    }
    return mapStateEnum.at(this_set);

}

std::map<VariableSet, double> Hmdp::convertToHigherState(VariableSet original_set, std::map<VariableSet, double> temp_result) {
    std::map<VariableSet, double> result;

    vector<string> higher_variables;
    for (auto v : original_set.set) {
        higher_variables.push_back(v.first);
    }

    //we should:
    //- put parameters in their original form
    //- add missing variables back



    for (auto b : temp_result) {
        VariableSet param_vs = b.first;
        VariableSet deparametrized_vs = convertToDeparametrizedState(param_vs);
        VariableSet result_vs;
        for (string s : higher_variables) {
            if (deparametrized_vs.set.find(s) != deparametrized_vs.set.end()) {
                result_vs.set[s] = deparametrized_vs.set[s];
            } else {
                result_vs.set[s] = original_set.set[s];
            }
        }
        result[result_vs] = b.second;

    }
    return result;

}

std::map<VariableSet, double> Hmdp::getHierarchicTransition(VariableSet set) {
    std::map<VariableSet, double> temp_result;


    VariableSet v_param = convertToParametrizedState(set);
//        cout<<"Param in hierarchic transition\n";
//        cout<<v_param.toString()<<"\n";
//        printParameters();
    int i = convertHierarchicState(v_param);


    for (int g : goal_states_) {
        pair<int, int> hierachic_input{i, g};
        double prob = hierarchic_transition_[hierachic_input];
        if (prob > 0) {
            VariableSet depar_set = convertToDeparametrizedState(vecStateEnum[g]);
            temp_result[depar_set] = prob;
        }
    }


    return convertToHigherState(set, temp_result);

}

std::map<VariableSet, double> Hmdp::getHierarchicTransition(VariableSet set, string action) {
    std::map<VariableSet, double> temp_result;

    if (active_module != "this") {
//        hierarchy_map_[active_module]->assignParametersFromActionName(action);
        temp_result = hierarchy_map_[active_module]->getHierarchicTransition(set, action);
    } else {
//                printParameters();
        VariableSet v_param = convertToParametrizedState(set);

        int i = convertHierarchicState(v_param);
        PairStateAction p{i, action};
        StateProb result_var = transition[p];
        for (auto r : result_var) {
            //we put the state back in a deparametrized form, since it needs to be converted to the higher state mdp properly.
            VariableSet depar_set = convertToDeparametrizedState(vecStateEnum[r.first]);
            temp_result[depar_set] = r.second;
        }
    }
    return convertToHigherState(set, temp_result);

}

void Hmdp::calculateHierarchicReward() {
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
        i = 0;
        for (int s : starting_states_) {
            temp_hierarchic_reward[s] = x(s);
            i++;
        }
        hierarchic_reward_ = temp_hierarchic_reward;

    }
}

void Hmdp::calculateHierarchicTransition() {
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

void Hmdp::enumerateFunctions(string fileName) {
    ofstream file(fileName);
    cout << "Starting Enumeration\n";
    for (int i = 0; i < vecStateEnum.size(); i++) {
        for (string action : actions) {
         
            double r;
            StateProb future_beliefs;
            if (hierarchy_map_.find(action) == hierarchy_map_.end()) {
                VarStateProb future_beliefs_var = transitionFunction(vecStateEnum[i], action);
                for (auto b : future_beliefs_var) {
                    future_beliefs[mapStateEnum.at(b.first)] = b.second;
                }
                r = rewardFunction(vecStateEnum[i], action);
            } else {
                Hmdp* h = hierarchy_map_[action];

                h->assignParametersFromActionName(action);

                //                if (action=="agent_assemble_bracket1_surface1" &&
                //                        fileName=="agent_saphari.pomdp") {
                //                    cout<<"";
                //                    VariableSet s;
                //                    s.set["agent1_isAt"]="surface1";
                //                    s.set["bracket1_isAt"]="agent";
                //                    s.set["bracket2_isAt"]="table";
                //                    s.set["bracket3_isAt"]="table";
                //                    s.set["gluebottle_isAt"]="agent";
                //                    s.set["surface1_status"]="completed";
                //                    s.set["surface2_status"]="none";
                //                    s.set["surface3_status"]="none";
                //                    mapStateEnum.at(s);
                //                    printStates();
                //
                //                }
                VarStateProb temp_future_beliefs = h->getHierarchicTransition(vecStateEnum[i]);
                for (auto temp_b : temp_future_beliefs) {
                    future_beliefs[mapStateEnum.at(temp_b.first)] = temp_b.second;
                }
                VariableSet vstry = vecStateEnum[i];

                r = h->getHierarchicReward(vstry) * rewardFunction(vecStateEnum[i], action);
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

void Hmdp::enumerateGoalAndStartStates() {
    for (auto state : mapStateEnum) {
        if (isGoalState(state.first)) {
            goal_states_.push_back(state.second);
        }
        if (isStartingState(state.first)) {
            starting_states_.push_back(state.second);
        }
    }
}

void Hmdp::create(string name, bool rewrite, bool first) {
    if (!is_created_) {
        is_created_ = true;
        name = getParametrizedAction(name); //transform something like human_take_dish in agent_take_object 
        //in order to be generic;
        this->name = name;


        for (HmdpMap::iterator i = hierarchy_map_.begin(); i != hierarchy_map_.end(); i++) {
            i->second->create(i->first, rewrite, false);
        }

        cout << "Creating " << name << "\n";
        string fileName = name + ".pomdp";


        enumerateStates();

        bool has_read = readMdp(fileName, rewrite);
        if (!has_read) {
            enumerateFunctions(fileName);
        }

        enumerateGoalAndStartStates();

        valueIteration(rewrite);

        if (!first) {
            ifstream i_file(name + ".hmdp");
            if (!rewrite && i_file.good()) {
                i_file.close();
                readHierarchical();
            } else {
                calculateHierarchicReward();
                calculateHierarchicTransition();
                writeHierarchical();
            }
        }
    }
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
    VariableSet this_state = convertToParametrizedState(state);
    if (isGoalState(this_state)) return "";
    if (active_module == "this") {
        string action = chooseAction(mapStateEnum.at(this_state));
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
    return chooseHierarchicAction(vecStateEnum[b]);
}

void Hmdp::simulate(int n, VariableSet initial_state) {
    printParameters();
    VariableSet parametrized_vs = convertToParametrizedState(initial_state);

    int int_initial_state = mapStateEnum.at(parametrized_vs);
    StateProb b;
    b[int_initial_state] = 1.0;
    for (int i = 0; i < n; i++) {
        cout << "Step " << i << "\n";
        StateProb temp_b;
        for (auto s : b) {
            VariableSet depar_s = convertToDeparametrizedState(vecStateEnum[s.first]);
            
            cout << "State: \n";
//            cout << vecStateEnum[s.first].toString();
            cout<<depar_s.toString();
            string action = chooseHierarchicAction(depar_s);

            StateProb output;
            if (active_module != "this") {
                Hmdp* lowest_active=getLowestActiveModule();
                cout << "Executing " << lowest_active->getDeparametrizedAction(action) << "\n";


                Hmdp* sub_mdp = hierarchy_map_[active_module];
                VarStateProb var_output = sub_mdp->getHierarchicTransition(depar_s, action);

                for (auto o : var_output) {
                    VariableSet par_o = convertToParametrizedState(o.first);
                    output[mapStateEnum.at(par_o)] = o.second;
                }
            } else {
                cout << "Executing " << getDeparametrizedAction(action) << "\n";\

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

void Hmdp::assignParametersFromActionName(string action_name) {
    vector<string> action_parts = StringOperations::stringSplit(action_name, '_');
    std::map<string, string> instance;
    for (int i=0; i<action_parts.size();i++) {
        if(parameter_action_place.find(i)!=parameter_action_place.end()) {
            instance[parameter_action_place[i]]=action_parts[i];
        }
    }
    assignParameters(instance);
}

void Hmdp::printGoalStates() {
    cout << "Goal states:\n";
    for (int i = 0; i < goal_states_.size(); i++) {
        cout << vecStateEnum[goal_states_[i]].toString() << "\n";
    }
}

void Hmdp::printHierarchy() {
    cout << "name: " << name << endl;
    cout << "actions: " << endl;
    for (string a : actions) {
        cout << a << endl;
    }

    for (auto h : hierarchy_map_) {
        cout << "Son of " << name << endl;
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
//    }
}


Hmdp* Hmdp::getLowestActiveModule() {
  if (active_module!="this") return hierarchy_map_[active_module]->getLowestActiveModule();
  else return this;  
}
