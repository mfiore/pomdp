/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   MdpManager.h
 * Author: mfiore
 *
 * Created on June 16, 2016, 2:54 PM
 * 
 * This class is used by the MMDP to create needed models in the hierarchy,
 */

//#include "Hmdp.h"
#include "Mmdp.h"
#include <vector>
#include <map>
#include <string>
#include "StringOperations.h"

using namespace std;

#ifndef MDPMANAGER_H
#define MDPMANAGER_H

class MmdpManager {
public:
    MmdpManager();
    MmdpManager(const MmdpManager& orig);
    virtual ~MmdpManager();

    //returns an MMDP requested (creating it if needed from the mdp_map)
    Hmdp* getMmdp(string name, string action_name, bool rewrite, bool first);
    //returns a single agent MDP
    Hmdp* getMdp(string name, string action_name, bool rewrite, bool first);

    vector<map<string, Hmdp*> > hmdp_map_; //list of the known hmdps

private:
    map<string, Hmdp*> mmdp_map_;
    

};

#endif /* MDPMANAGER_H */