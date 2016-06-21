/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MdpManager.cpp
 * Author: mfiore
 * 
 * Created on June 16, 2016, 2:54 PM
 */

#include "MmdpManager.h"

MmdpManager::MmdpManager(vector<map<string, Hmdp*>> hmdp_map) : hmdp_map_(hmdp_map) {
}

MmdpManager::MmdpManager(const MmdpManager& orig) {
}

MmdpManager::~MmdpManager() {
}

Hmdp* MmdpManager::getMmdp(string name, string action_name, bool rewrite, bool first) {
    if (mmdp_map_.find(name) != mmdp_map_.end()) {
        return mmdp_map_[name];
    } else {
        Mmdp* sub_mmdp = new Mmdp(this);
        vector<string> single_actions = StringOperations::stringSplit(name, '-');
        int i = 0;
        for (string a : single_actions) {
            string agent_name="agent"+to_string(i);
            vector<string> action_parts = StringOperations::stringSplit(single_actions[i], '_');
            if (hmdp_map_[i].find(action_parts[1]) != hmdp_map_[i].end()) {
                sub_mmdp->agent_hmpd_[agent_name] =hmdp_map_[i][action_parts[1]];
            }
            else {
                cout<<"ERROR - CAN'T FIND ACTION\n";
            }
            i++;
        }
        mmdp_map_[name]=sub_mmdp;
        sub_mmdp->create(action_name,rewrite,first);
        return mmdp_map_[name];
    }

}


