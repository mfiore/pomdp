/* 
 * File:   Hmmdp.cpp
 * Author: mfiore
 * 
 * Created on December 14, 2015, 4:49 PM
 */

#include "Hmmdp.h"

Hmmdp::Hmmdp() {
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
        if (hierarchy_map_.find(action) != hierarchy_map_.end()) {
            Hmmdp* sub_mdp = hierarchy_map_[action];
            currentReward = sub_mdp->getHierarcicReward(i);
            futureBeliefs = sub_mdp->getHierarcicTransition(i);
        } else {
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
        }

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

void Hmmdp::solveHierarchical(bool first) {
    for (HmmdpMap::iterator i = hierarchy_map_.begin(); i != hierarchy_map_.end(); i++) {
        i->second->createPomdp(i->first, true,false);
    }

    valueIteration(true);
    if (!first) {
        calculateHierarcicReward();
        calculateHierarcicTransition();
    }
}

double Hmmdp::getHierarcicReward(int i) {
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

//void Hmmdp::calculateHierarcicRewardIterative() {
//    double epsilon=1;
//    double delta_difference=2;
//    
//    while (delta_difference>epsilon) {
//        for (int s: starting_states_) {
//            
//        }
//    }
//} 

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
    Pomdp::createPomdp(name, rewrite);
    for (auto state : mapStateEnum) {
        if (isGoalState(state.first)) {
            goal_states_.push_back(state.second);
        }
        if (isStartingState(state.first)) {
            starting_states_.push_back(state.second);
        }
    }
    cout << "Creating " << name << "\n";
    solveHierarchical(first);
}
