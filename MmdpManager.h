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
 */

#include "Hmdp.h"
#include "Mmdp.h"
#include <vector>
#include <map>

using namespace std;

#ifndef MDPMANAGER_H
#define MDPMANAGER_H

class MmdpManager {
public:
    MmdpManager(vector<map<string, Hmdp*> > hmdp_map);
    MmdpManager(const MmdpManager& orig);
    virtual ~MmdpManager();


    Hmdp* getMmdp(string name, string action_name, bool rewrite, bool first);
private:
    vector<map<string, Hmdp*> > hmdp_map_;
    map<string, Hmdp*> mmdp_map_;
    

};

#endif /* MDPMANAGER_H */

