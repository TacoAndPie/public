#include "system_manager.h"

SystemManager::SystemManager(int groups_num, int scale) {
    // update all params with given values
    num_of_groups = groups_num+1;
    this->scale = scale;

    // create new empty union for groups
    groups_union = Union<Group*>(num_of_groups);

    // create empty node for inserting groups to union
    UpTreeNode<Group*>* temp_node;
    if (!temp_node){
        throw std::bad_alloc();
    }

    // for each group, create new Group object and insert it to the up_tree node in union array
    ReturnValue res;
    for (int i = 0; i < num_of_groups; i++){
        Group* new_group = new Group(i, scale);
        if (!new_group){
            throw std::bad_alloc();
        }

        res = groups_union.getNodeByID(i, &temp_node);
        if (res != MY_SUCCESS){
            throw std::exception();
        }
        temp_node->setData(new_group);
    }
}

ReturnValue SystemManager::getGroupPtr(int groupID, Group** group_ptr) {
    if (groupID < 0 || groupID > num_of_groups){
        return MY_INVALID_INPUT;
    }

    ReturnValue res = groups_union.findDataPtrByIndex(groupID, group_ptr);
    if (res != MY_SUCCESS){
        return res;
    }
    return MY_SUCCESS;
}

ReturnValue SystemManager::addNewPlayer(int playerID, int groupID, int score){
    // check input
    if (playerID <= 0 || score <= 0 || score > scale || groupID > num_of_groups || groupID < 0 ){
        return MY_INVALID_INPUT;
    }

    // check if player is already in game (in groupID=0, "all_players_group")
    // get the group pointer of "all_players_group" (group 0)
    Group* all_players_group;
    ReturnValue res = getGroupPtr(0, &all_players_group);
    if (res != MY_SUCCESS){
        return res;
    }

    // create new player and try to insert it to "all_players_group" (group 0)
    Player* new_player = new Player(playerID, groupID, score);
    res = all_players_group->addPlayer(new_player);
    if(res != MY_SUCCESS) { // if the player exists, we return FAILURE
        return res;
    }

    // finished with "all_players_group" (group 0), now insert to given group
    // get the group pointer of given groupID
    Group* group;
    res = getGroupPtr(groupID, &group);
    if (res != MY_SUCCESS){
        return res;
    }

    // try to insert new player to group matching groupID
    res = group->addPlayer(new_player);
    if(res != MY_SUCCESS) {
        return res;
    }
    return MY_SUCCESS;
}

ReturnValue SystemManager::addExistingPlayer(Player* player){
    // check input
    if (player == nullptr ){
        return MY_INVALID_INPUT;
    }

    // check if player is already in game (in groupID=0, "all_players_group")
    // get the group pointer of "all_players_group" (group 0)
    Group* all_players_group;
    ReturnValue res = getGroupPtr(0, &all_players_group);
    if (res != MY_SUCCESS){
        return res;
    }

    // add player to all_players_group
    res = all_players_group->addPlayer(player);
    if(res != MY_SUCCESS) { // if the player exists, we return FAILURE
        return res;
    }

    // finished with "all_players_group" (group 0), now insert to given group
    // get the group pointer of given groupID
    Group* group;
    res = getGroupPtr(player->getGroupID(), &group);
    if (res != MY_SUCCESS){
        return res;
    }

    // insert player to group matching groupID
    res = group->addPlayer(player);
    if(res != MY_SUCCESS) {
        return res;
    }
    return MY_SUCCESS;
}

ReturnValue SystemManager::removePlayer(int playerID) {
    // check input
    if (playerID <= 0){
        return MY_INVALID_INPUT;
    }

    // get the group pointer of "all_players_group" (group 0) from union
    Group* all_players_group;
    getGroupPtr(0, &all_players_group);

    // check if "all_players_group" (group 0) contains player with same playerID
    Player* temp_player;
    ReturnValue res = all_players_group->getPlayerPtr(playerID, &temp_player);
    if (res == ELEMENT_DOES_NOT_EXIST || res == MY_FAILURE){
        return MY_FAILURE;
    }

    // if player is in game, we can remove it. get the groupID for removing it later from the relevant group.
    int group_id = temp_player->getGroupID();

    // remove player from "all_players_group" (group 0)
    all_players_group->removePlayer(temp_player);

    // get the group pointer of groupID from group_union
    Group* group;
    getGroupPtr(group_id, &group);

    // find the player in group and remove it
    group->getPlayerPtr(playerID, &temp_player);
    group->removePlayer(temp_player);

    return MY_SUCCESS;
}

ReturnValue SystemManager::increasePlayerLevel(int playerID, int level_increase){
    // check input
    if (playerID <= 0 || level_increase <= 0) {
        return MY_INVALID_INPUT;
    }

    // get player ptr from "all_players_group" (group 0)
    Group* all_players_group;
    ReturnValue res = getGroupPtr(0, &all_players_group);
    if (res != MY_SUCCESS){
        return res;
    }

    Player* temp_player;
    res = all_players_group->getPlayerPtr(playerID, &temp_player);
    if (res == ELEMENT_DOES_NOT_EXIST || res == MY_FAILURE){
        return MY_FAILURE;
    }

    // remove player with playerID from "all_players_group" (group 0) and from group with groupID
    // removePlayer function will check if player is in game, and if so it will remove it from both groups
    res = removePlayer(playerID);
    if (res != MY_SUCCESS){
        return res;
    }

    // increase player level (of real player ptr)
    temp_player->increaseLevel(level_increase);

    // insert player with increased level to both groups
    return addExistingPlayer(temp_player);
}

ReturnValue SystemManager::updatePlayerScore(int playerID, int new_score){
    // check input
    if (playerID <= 0 || new_score <= 0 || new_score > scale) {
        return MY_INVALID_INPUT;
    }

    // get the group pointer of "all_players_group" (group 0) from union
    Group* all_players_group;
    ReturnValue res = getGroupPtr(0, &all_players_group);
    if (res != MY_SUCCESS){
        return res;
    }

    // get the player ptr
    Player* temp_player;
    res = all_players_group->getPlayerPtr(playerID, &temp_player);
    if (res == ELEMENT_DOES_NOT_EXIST || res == MY_FAILURE){
        return MY_FAILURE;
    }
    int old_score = temp_player->getScore();
    // update the score to the new_score for the actual player object
    temp_player->setScore(new_score);

    // update the score of player registered in all_players_group
    res = all_players_group->updatePlayerScore(temp_player, new_score, old_score);
    if (res != MY_SUCCESS){
        return res;
    }

    // get the group pointer of group with groupID from union
    Group* group;
    res = getGroupPtr(temp_player->getGroupID(), &group);
    if (res != MY_SUCCESS){
        return res;
    }

    // update the score of player registered in group
    res = group->updatePlayerScore(temp_player, new_score, old_score);
    if (res != MY_SUCCESS){
        return res;
    }


    return MY_SUCCESS;
}

ReturnValue SystemManager::mergeGroups(int group1, int group2) {
    // check input
    if (group1 <= 0 || group2 <= 0 || group1 > this->num_of_groups || group2 > this->num_of_groups) {
        return MY_INVALID_INPUT;
    }

    // check if group1ID=group2ID, if so, no need to merge anything and is considered success.
    if (group1 == group2) {
        return MY_SUCCESS;
    }

    // both groups are valid groups in range (1 to scale-1), merge them
    return groups_union.unify(group1, group2);
}

ReturnValue SystemManager::calcAverageLeadPlayersLevelByGroup(int groupID, int m, double* calc_avg) {
    // groupID given is valid (in range of groups)
    // get the group pointer
    Group* group_ptr;
    ReturnValue res = getGroupPtr(groupID, &group_ptr);
    if (res != MY_SUCCESS) {
        return res;
    }

    // get average level of m lead players in group
    double avg = group_ptr->calcAverageLeadPlayersLevel(m);

    if (avg == -1) {
        return MY_FAILURE;
    }

    *calc_avg = avg;
    return MY_SUCCESS;
}

ReturnValue SystemManager::getPlayersBoundByGroup(int groupID, int m, int score, int* lower_bound_players, int* higher_bound_players) {
    // groupID given is valid (in range of groups)
    // get the group pointer
    Group* group_ptr;
    ReturnValue res = getGroupPtr(groupID, &group_ptr);
    if (res != MY_SUCCESS) {
        return res;
    }

    // find required bounds
    res = group_ptr->calcPlayerBounds(m, score, lower_bound_players, higher_bound_players);
    return res;
}






