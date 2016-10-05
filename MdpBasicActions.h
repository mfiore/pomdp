/* 
 * File:   MdpTakeAction.h
 * Author: mfiore
 *
 * Created on January 14, 2016, 4:55 PM
 */

#ifndef MDPBASICACTIONS_H
#define MDPBASICACTIONS_H

#include "Mdp.h"
#include <vector>
#include <string>
#include <map>
#include "StringOperations.h"

using namespace std;

class MdpBasicActions {
public:

    static vector<string> getActionParameters(string action) {
        return StringOperations::stringSplit(action, '_');
    }

    static VarStateProb applyTake(string agent_loc, string object_loc, string agent_name, string object_loc_var, VariableSet s) {
        VarStateProb result;
        if (agent_loc == object_loc) {
            s.set[object_loc_var] = agent_name;
        }
        result[s] = 1.0;
        return result;
    }

    static VarStateProb applyMove(string agent_loc_var, string location, VariableSet s) {
        VarStateProb result;
        s.set[agent_loc_var] = location;
        result[s] = 1.0;
        return result;
    }

    static VarStateProb applyMove(string agent_loc_var, string location, VariableSet s, map<string, vector<string> > connections, string agent_loc) {
        VarStateProb result;
        vector<string> conn_loc = connections.at(agent_loc);
        if (std::find(conn_loc.begin(), conn_loc.end(), location) != conn_loc.end()) {
            s.set[agent_loc_var] = location;
            result[s] = 1.0;
        }
        return result;
    }

    static VarStateProb applyPlace(string object_loc_var, string object_loc, string agent_loc, string support, string agent, VariableSet s) {
        VarStateProb result;
        if (object_loc == agent && agent_loc == support) {
            s.set[object_loc_var] = support;
        }
        result[s] = 1.0;
        return result;
    }

    static VarStateProb applyGive(string agent1_loc, string agent2_loc, string object_loc_var,
            string object_loc, string agent1, string agent2, VariableSet s) {
        VarStateProb result;
        if (agent1_loc == agent2_loc && object_loc == agent1) {
            s.set[object_loc_var] = agent2;
        }
        result[s] = 1.0;
        return result;
    }
};
#endif /* MDPBASICACTIONS_H */

