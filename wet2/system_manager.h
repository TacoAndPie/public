#ifndef WET2_SYSTEM_MANAGER_H
#define WET2_SYSTEM_MANAGER_H

#include "union.h"
#include "up_tree_node.h"
#include "group.h"

class SystemManager {
    int scale;
    int num_of_groups;
    Union<Group*> groups_union;

    ReturnValue addExistingPlayer(Player* player);

public:
    SystemManager(int groups_num, int scale);
    ~SystemManager() = default;

    int getNumOfGroups() const { return num_of_groups; }
    int getScale() const { return scale; }
    ReturnValue getGroupPtr(int groupID, Group** group_ptr);
    ReturnValue addNewPlayer(int playerID, int groupID, int score);
    ReturnValue removePlayer(int playerID);
    ReturnValue increasePlayerLevel(int playerID, int level_increase);
    ReturnValue updatePlayerScore(int playerID, int new_score);
    ReturnValue mergeGroups(int group1, int group2);
    ReturnValue calcAverageLeadPlayersLevelByGroup( int groupID, int m, double* calc_avg);
    ReturnValue getPlayersBoundByGroup(int groupID, int m, int score, int* lower_bound_players, int* higher_bound_players);

};

#endif //WET2_SYSTEM_MANAGER_H
