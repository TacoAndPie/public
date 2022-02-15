#include "library2.h"
#include "system_manager.h"

static StatusType retValToStatType(ReturnValue result){
    switch (result) {
        case MY_ALLOCATION_ERROR:
            return ALLOCATION_ERROR;
        case MY_FAILURE:
            return FAILURE;
        case MY_SUCCESS:
            return SUCCESS;
        case MY_INVALID_INPUT:
            return INVALID_INPUT;
        default:
            return FAILURE;
    }
}

void* Init(int k, int scale){
    if(k<=0 || scale>200 || scale<=0){
        return nullptr;
    }
    SystemManager* new_game_system = new SystemManager(k, scale);
    if(!new_game_system){
        return nullptr;
    }
    return new_game_system;
}

StatusType MergeGroups(void *DS, int GroupID1, int GroupID2){
    if(DS == nullptr || GroupID1 <= 0 || GroupID2 <= 0){
        return INVALID_INPUT;
    }
    SystemManager* game_system = (SystemManager*)DS;
    if((GroupID1 > game_system->getNumOfGroups()) || (GroupID2 > game_system->getNumOfGroups())){
        return INVALID_INPUT;
    }
    ReturnValue result = game_system->mergeGroups(GroupID1, GroupID2);
    StatusType retRes = retValToStatType(result);
    return retRes;
}

StatusType AddPlayer(void *DS, int PlayerID, int GroupID, int score){
    if(DS == nullptr || GroupID <= 0 || PlayerID <= 0 || score <= 0){
        return INVALID_INPUT;
    }
    SystemManager* game_system = (SystemManager*)DS;
    if((score > game_system->getScale()) || (GroupID > game_system->getNumOfGroups())){
        return INVALID_INPUT;
    }
    ReturnValue result = game_system->addNewPlayer(PlayerID, GroupID, score);
    return retValToStatType(result);
}

StatusType RemovePlayer(void *DS, int PlayerID){
    if(DS == nullptr || PlayerID <= 0){
        return INVALID_INPUT;
    }
    SystemManager* game_system = (SystemManager*)DS;
    ReturnValue result = game_system->removePlayer(PlayerID);
    return retValToStatType(result);
}

StatusType IncreasePlayerIDLevel(void *DS, int PlayerID, int LevelIncrease){
    if(DS == nullptr || PlayerID <= 0 || LevelIncrease <= 0){
        return INVALID_INPUT;
    }
    SystemManager* game_system = (SystemManager*)DS;
    ReturnValue result = game_system->increasePlayerLevel(PlayerID, LevelIncrease);
    return retValToStatType(result);
}

StatusType ChangePlayerIDScore(void *DS, int PlayerID, int NewScore){
    if(DS == nullptr || PlayerID <= 0 || NewScore <= 0){
        return INVALID_INPUT;
    }
    SystemManager* game_system = (SystemManager*)DS;
    if(NewScore > game_system->getScale()){
        return INVALID_INPUT;
    }
    ReturnValue result = game_system->updatePlayerScore(PlayerID, NewScore);
    return retValToStatType(result);
}

StatusType GetPercentOfPlayersWithScoreInBounds(void *DS, int GroupID, int score, int lowerLevel, int higherLevel,
                                                double * players) {
    // check input
    if (DS == nullptr || players == nullptr || GroupID < 0) {
        return INVALID_INPUT;
    }

    SystemManager *game_system = (SystemManager *) DS;
    if (GroupID > game_system->getNumOfGroups()) {
        return INVALID_INPUT;
    }

    // get group_ptr from group_union in DS
    Group *group_ptr;
    game_system->getGroupPtr(GroupID, &group_ptr);
    if (!group_ptr) {
        return ALLOCATION_ERROR;
    }
    int players_with_score = 0;
    int players_count = 0;
    return retValToStatType(group_ptr->getPercentOfPlayersWithScoreInRange(lowerLevel,
                                                                           higherLevel, score, players,
                                                                           &players_with_score, &players_count));
}

StatusType AverageHighestPlayerLevelByGroup(void *DS, int GroupID, int m, double * level){
    // check input
    if(DS == nullptr || level == nullptr || GroupID < 0 || m <= 0){
        return INVALID_INPUT;
    }

    // check if groupID is in group range
    SystemManager* game_system = (SystemManager*)DS;
    if(GroupID > game_system->getNumOfGroups()){
        return INVALID_INPUT;
    }

    // if group is in range, calc the required average
    return retValToStatType(game_system->calcAverageLeadPlayersLevelByGroup(GroupID, m, level));
}

StatusType GetPlayersBound(void *DS, int GroupID, int score, int m,
                           int* LowerBoundPlayers, int* HigherBoundPlayers){
    // check input
    if(DS == nullptr || LowerBoundPlayers == nullptr || HigherBoundPlayers == nullptr || GroupID < 0 || m < 0 || score <= 0) {
        return INVALID_INPUT;
    }

    if (m == 0) {
        *LowerBoundPlayers = 0;
        *HigherBoundPlayers = 0;
        return SUCCESS;
    }

    SystemManager* game_system = (SystemManager*)DS;
    if((GroupID > game_system->getNumOfGroups()) || (score > game_system->getScale())){
        return INVALID_INPUT;
    }

    return retValToStatType(game_system->getPlayersBoundByGroup(GroupID, m, score, LowerBoundPlayers, HigherBoundPlayers));
}

void Quit(void** DS){
    if(!*DS){
        return;
    }
    *DS = nullptr;
}