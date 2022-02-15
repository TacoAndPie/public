#include "group.h"

Group::Group(int new_groupID, int scale) {
    groupID = new_groupID;
    num_of_players = 0;
    this->scale = scale;
    highest_level_player = nullptr;
    lowest_level_player = nullptr;
    players_hash_table = new DynamicHashTable<GroupHashTableVal>();
    level_0_players_list = new DoublyLinkedList<Player>();
    level_0_score_hist = new Histogram(scale);
    non_0_level_players_tree = new RankTree<Player, PlayerRank>();
    if(!players_hash_table || !level_0_players_list || !level_0_score_hist || !non_0_level_players_tree){
        throw std::bad_alloc();
    }
}

Group::~Group() {
    delete level_0_score_hist;
    delete level_0_players_list;
    delete non_0_level_players_tree;
    delete players_hash_table;
    delete highest_level_player;
    delete lowest_level_player;
    num_of_players = 0;
}

//this function doesn't delete the group, but clears all data structures and sets num_of_players=0
void Group::resetGroup() {
    non_0_level_players_tree->clearTree();
    level_0_score_hist->clearHistogram();
    players_hash_table->clearTable();
    num_of_players = 0;
    highest_level_player = nullptr;
    lowest_level_player = nullptr;
    // if we use resetGroup, this means that the group was merged to another group, so the list nodes were moved to the
    // list of the other group, head&tail were set to nullptr.
}

ReturnValue Group::addPlayer(Player *player){
    // check input
    if (player == nullptr){
        return MY_INVALID_INPUT;
    }

    // create empty hash_table val containing player, and check if player is already in group (exists in hash_table)
    GroupHashTableVal* temp_hash_val = new GroupHashTableVal(player);
    ReturnValue res;
    res = getPlayerPtr(player->getPlayerID(), &player);
    if (res == ELEMENT_EXISTS){
        return res;
    }

    // player doesn't exist.
    // check if player added is new (level==0) or after levelIncrease (level > 0)
    if(player->getLevel() == 0){ // player is new (level==0)
        // insert player to level_0_linked_list
        res = level_0_players_list->insert(player);
        if (res != MY_SUCCESS){
            return res;
        }

        // player was added to head of list. get the list_node_ptr and update it in temp_hash_val
        temp_hash_val->setListNode(level_0_players_list->getHead());

        // set tree_node ptr in temp_hash_val to nullptr
        temp_hash_val->setTreeNode(nullptr);

        // insert the updated temp_hash_val to hash_table
        res = players_hash_table->insertData(*temp_hash_val);
        if (res != MY_SUCCESS){
            return res;
        }

        // update the level_0_histogram
        level_0_score_hist->increaseElement(player->getScore()-1);
    }
    else { // player is after level increase (level > 0)
        // insert player to tree
        res = non_0_level_players_tree->insert(player, scale);
        if (res != MY_SUCCESS){
            return res;
        }

        // player was added to tree.
        // create new PlayerRank for find function, and find the tree node with player
        RankTreeNode<Player, PlayerRank>* temp_tree_node;
        res = non_0_level_players_tree->find(*player, &temp_tree_node);
        if (res != ELEMENT_EXISTS){
            return MY_FAILURE;
        }

        // set tree_node ptr in temp_hash_val to found tree_node ptr
        temp_hash_val->setTreeNode(temp_tree_node);

        // set the list_node_ptr in temp_hash_val to nullptr
        temp_hash_val->setListNode(nullptr);

        // insert the updated temp_hash_val to hash_table
        res = players_hash_table->insertData(*temp_hash_val);
        if (res != MY_SUCCESS){
            return res;
        }
    }

    // update highest and lowest players ptr
    if (highest_level_player == nullptr) {
        highest_level_player = player;
    }
    else if(highest_level_player->getLevel() < player->getLevel()){
        highest_level_player = player;
    }
    if (lowest_level_player == nullptr) {
        lowest_level_player = player;
    }
    else if ( player->getLevel() < lowest_level_player->getLevel()) {
        lowest_level_player = player;
    }

    // +1 to num of players in group
    num_of_players++;

    return MY_SUCCESS;
}

ReturnValue Group::removePlayer(Player *player){
    // check input
    if (player == nullptr){
        return MY_INVALID_INPUT;
    }

    // create hash_table val for given player, and check if it exists in hash_table
    GroupHashTableVal* temp_val;
    ReturnValue res = getPlayerArrayNode(player->getPlayerID(), &temp_val);
    if (res == ELEMENT_DOES_NOT_EXIST){
        return res;
    }

    if(player->getLevel() == 0){ //player is in linked list.
        // update histogram (-1 in the index of the player score (player_score-1))
        level_0_score_hist->decreaseElement(player->getScore()-1);

        // remove list_node from list.
        res = level_0_players_list->remove(temp_val->getListNode());
        if (res != MY_SUCCESS){
            return res;
        }
    }
    else{ //player is in rank tree.
        // remove player from tree.
        res = non_0_level_players_tree->remove(*player);
        if (res != MY_SUCCESS){
            return res;
        }
    }

    // update hash_table after removal (mark as freed in graveyard)
    players_hash_table->removeData(*temp_val);

    // decrease num of players in group
    num_of_players--;

    // update the highest_level_player and lowest_level_player in group
    updateHighestLowestPlayers();
    return MY_SUCCESS;
}

ReturnValue Group::getPlayerArrayNode(int player_id, GroupHashTableVal** hash_table_node) {
    // check input
    if (player_id <= 0 ){
        return MY_INVALID_INPUT;
    }

    // check if the group has players (if no players, player defiantly not found)
    if (num_of_players == 0){
        return ELEMENT_DOES_NOT_EXIST;
    }

    // create dummy player with the correct playerID, and dummy hashtable node to search in the hashtable
    Player dummy_player = Player(player_id, groupID, 0);

    // create dummy hash_table_val for searching the player in the hash_table
    GroupHashTableVal dummy_val = GroupHashTableVal(&dummy_player);

    // find the node that holds the player in hash_table
    *hash_table_node = players_hash_table->getDataPtr(dummy_val);
    if (*hash_table_node == nullptr) {
        return MY_FAILURE;
    }

    return MY_SUCCESS;
}

ReturnValue Group::getPlayerPtr(int player_id, Player** player){
    // create empty hash table node and find player node in array
    GroupHashTableVal* temp_val;
    ReturnValue res = getPlayerArrayNode(player_id, &temp_val);

    if (res != MY_SUCCESS){
        return res;
    }

    // found node of player, get the player ptr from node.
    *player = temp_val->getPlayerPtr();
    return ELEMENT_EXISTS;
}

ReturnValue Group::increasePlayerLevel(Player *player, int level_increase) {
    // check input
    if (player == nullptr || level_increase <= 0){
        return MY_INVALID_INPUT;
    }

    // create empty hash_table_val, and find the player_val in the hash_table
    GroupHashTableVal* temp_val;
    ReturnValue res = getPlayerArrayNode(player->getPlayerID(), &temp_val);
    if (res != MY_SUCCESS){
        return res;
    }


    // if player had level=0, it was in the linked list.
    // need to:
    // increase it's level
    // remove from linked list
    // update histogram (-1 in the index of (player_score-1))
    // insert to tree
    if (temp_val->getLevel() == 0){
        temp_val->increaseLevel(level_increase);
        DoublyLinkedListNode<Player>* list_node_to_remove = temp_val->getListNode();
        level_0_players_list->remove(list_node_to_remove);
        temp_val->setNullListNode();
        level_0_score_hist->decreaseElement(temp_val->getScore()-1); // score is between 1 and scale, but hist is between 0 and scale-1
        res = non_0_level_players_tree->insert(player, scale);
        if (res != MY_SUCCESS){
            return res;
        }
        RankTreeNode<Player, PlayerRank>* temp_tree_node;
        non_0_level_players_tree->find(*player, &temp_tree_node);
        temp_val->setTreeNode(temp_tree_node);
        updateHighestLowestPlayers();
        return MY_SUCCESS;
    }

    // if player had level>0, it was in the tree.
    // need to:
    // increase it's level
    // remove the tree_node with this player
    // insert new node to tree with new level (tree is sorted by level, so will be inserted in new place)
    temp_val->increaseLevel(level_increase);
    Player dummy_player = Player(player->getPlayerID(), player->getGroupID(), player->getScore());
    dummy_player.increaseLevel(player->getLevel()-level_increase);
    res = non_0_level_players_tree->remove(dummy_player);
    if (res != MY_SUCCESS){
        return res;
    }

    res = non_0_level_players_tree->insert(player, scale);
    if (res != MY_SUCCESS){
        return res;
    }
    updateHighestLowestPlayers();
    return MY_SUCCESS;
}

ReturnValue Group::updatePlayerScore(Player *player, int new_score, int old_score) {
    // check input
    if (player == nullptr || new_score <= 0 || new_score > level_0_score_hist->size) {
        return MY_INVALID_INPUT;
    }

    // create empty hash_table val, and find the player_val in the hash_table
    GroupHashTableVal* temp_val;
    ReturnValue res = getPlayerArrayNode(player->getPlayerID(), &temp_val);
    if (res != MY_SUCCESS){
        return res;
    }

    // if level=0, player is in linked list.
    // need to:
    // update histogram (-1 in the index of the old player score (player_score-1))
    // update player to the new score
    // update histogram (+1 in the index of the new player score (player_score-1))
    if (temp_val->getLevel() == 0){
        level_0_score_hist->decreaseElement(old_score-1);
        temp_val->updateScore(new_score);
        level_0_score_hist->increaseElement(new_score-1);
        return MY_SUCCESS;
    }

    // if level>0, player is in rank tree.
    // need to:
    // update player to the new score
    // update the rank (which includes score_hist) in tree, starting with the players' tree_node, and up to the root
    if (temp_val->getLevel() > 0){
        temp_val->updateScore(new_score);
        RankTreeNode<Player, PlayerRank>* tree_node = (*temp_val).getTreeNode();
        non_0_level_players_tree->updateRankAlongPath(tree_node);
        return MY_SUCCESS;
    }

    return MY_FAILURE;
}

ReturnValue Group::getPercentOfPlayersWithScoreInRange(int lowerLevel, int higherLevel, int score, double* percent,
                                                       int* players_with_score,  int* players_count ) {

    if (higherLevel < lowerLevel || num_of_players == 0) {
        *percent = -1;
        return MY_FAILURE;
    }

    // if lowerLevel is higher than the highest level player in group, there are 0 players in range.
    // if higherLevel is lower than the lowest level player in group, there are 0 players in range.
    if (higherLevel < lowest_level_player->getLevel() || lowerLevel > highest_level_player->getLevel()) {
        *percent = -1;
        return MY_FAILURE;
    }

    // calculate how many players are in the range using the rank
    *players_count = 0;
    *players_with_score = 0;
    bool list_included = (lowerLevel <= 0 && higherLevel >= 0);
    bool tree_included = (lowerLevel > 0 || higherLevel > 0);

    // if 0 is included in range (list_included) get num of players with level 0
    if (list_included) {
        *players_count += level_0_players_list->getSize();
        *players_with_score += level_0_score_hist->getVal(score-1);
    }

    // if tree is included (levels 1 and up are in range)
    if (tree_included) {
        RankTreeNode<Player, PlayerRank> *highest_node = getHighestLevelNode(higherLevel,
                                                                             non_0_level_players_tree->begin().getPtr());
        RankTreeNode<Player, PlayerRank> *lowest_node = getLowestLevelNode(lowerLevel,
                                                                           non_0_level_players_tree->begin().getPtr());
        if (highest_node == nullptr || lowest_node == nullptr) {
            *players_count += 0;
            *players_with_score += 0;
        }
        else {
            // in Rank = score_hist of right_node - score_hist of left node + score of left node itself
            // in sum of nodes = sum of levels of right node - sum of levels of left node + 1
            PlayerRank highest_rank = calcRankUptoNodeWrapper(highest_node);
            PlayerRank lowest_rank = calcRankUptoNodeWrapper(lowest_node);
            highest_rank -= lowest_rank;
            *players_count += highest_rank.getNodeCount() + 1;
            *players_with_score += highest_rank.getScoreHist().getVal(score - 1);

            if (lowest_node->getData()->getScore() == score) {
                *players_with_score += 1;
            }
        }
    }
    if(*players_count == 0){
        *percent = -1;
        return MY_FAILURE;
    }
    *percent = 100*((double)*players_with_score/(double)*players_count);
    return MY_SUCCESS;
}

void Group::updateHighestLowestPlayers() {
    // update the highest_level_player and lowest_level_player in group
    // if there are no players in the group, there are no longer highest and lowest players to hold.
    if ( num_of_players == 0) {
        highest_level_player = nullptr;
        lowest_level_player= nullptr;
    }
    // if there are players in the group, and tree size is 0, then all players are in the list.
    // get highest and lowest from the list
    else if ( non_0_level_players_tree->getSize() == 0) {
        // get the tail - the 1st player inserted to list (players are inserted to head of list)
        // if there is a player, this will be the new lowest_level_player
        highest_level_player = level_0_players_list->getTail()->getData();
        lowest_level_player = highest_level_player;
    }
    // if there are players in the group, and list size is 0, then all players are in the tree.
    else if (level_0_players_list->getSize() == 0) {
        lowest_level_player = non_0_level_players_tree->getLeftMostNode()->getData();
        highest_level_player = non_0_level_players_tree->getRightMostNode()->getData();
    }
    // if there are players in the group, and both list size and tree size are not 0, then highest will be
    // from tree and lowest will be from list
    else {
        lowest_level_player = level_0_players_list->getTail()->getData();
        highest_level_player = non_0_level_players_tree->getRightMostNode()->getData();
    }
}

double Group::calcAverageLeadPlayersLevel(int m) {
    // check amount of players in group
    if (num_of_players < m){
        // not enough players in group
        return -1;
    }

    // m is bigger/equal to amount of players in group
    // check amount of players in tree. if m is bigger, level_0_list is included
    bool list_included = (non_0_level_players_tree->getSize() <= m);
    double tot_level_sum = 0;

    // if list is included, we need to get ALL the players from the tree, and the extra from the list
    if (list_included) {
        if (non_0_level_players_tree->getSize() != 0){
            RankTreeIterator<Player, PlayerRank> iter = non_0_level_players_tree->begin();
            tot_level_sum += iter.getPtr()->getRank().getSumOfLevels();
        }

        // total level count is (0 + sum_of_levels of root in tree) divided by m
        return (tot_level_sum/(double)m);
    }

    // if list is not included, all m lead players are from tree.
    // get the tree_node of the mth lead player (m from top)
    RankTreeNode<Player, PlayerRank>* mth_node = findMthPlayerTreeNodeWrapper(m);
    RankTreeNode<Player, PlayerRank>* right_most_node = non_0_level_players_tree->getRightMostNode();

    // calc the sum_of_levels of both nodes
    PlayerRank right_rank = calcRankUptoNodeWrapper(right_most_node);
    PlayerRank left_rank = calcRankUptoNodeWrapper(mth_node);

    right_rank -= left_rank;
    tot_level_sum = right_rank.getSumOfLevels() + mth_node->getData()->getLevel();
    return (tot_level_sum/(double)m);
}

RankTreeNode<Player, PlayerRank> *Group::getHighestLevelNode(int higher_bound, RankTreeNode<Player, PlayerRank> *node) {
    if (node == nullptr) {
        return node;
    }

    if (node->getData()->getLevel() <= higher_bound) {
        RankTreeNode<Player, PlayerRank>* temp_node = node;
        temp_node = getHighestLevelNode(higher_bound, temp_node->getRight());
        if (temp_node == nullptr) {
            return node;
        }
        return temp_node;
    }
    else {
        return getHighestLevelNode(higher_bound, node->getLeft());
    }
}

RankTreeNode<Player, PlayerRank> *Group::getLowestLevelNode(int lower_bound, RankTreeNode<Player, PlayerRank> *node) {
    if (node == nullptr) {
        return node;
    }

    if (node->getData()->getLevel() >= lower_bound) {
        RankTreeNode<Player, PlayerRank>* temp_node = node;
        temp_node = getLowestLevelNode(lower_bound, temp_node->getLeft());
        if (temp_node == nullptr) {
            return node;
        }
        return temp_node;
    }
    else {
        return getLowestLevelNode(lower_bound, node->getRight());
    }
}

PlayerRank Group::calcRankUptoNodeWrapper(RankTreeNode<Player, PlayerRank> *target_node) {
    PlayerRank rank_tot = PlayerRank(level_0_score_hist->size);
    RankTreeIterator<Player, PlayerRank> iter = non_0_level_players_tree->begin();
    if (*(iter.getPtr()->getData()) == *(target_node->getData())) {
        PlayerRank other_rank = iter.getPtr()->getRank();
        rank_tot += other_rank;
        if (!iter.checkNullRight()){
            iter.goRight();
            other_rank = iter.getPtr()->getRank();
            rank_tot -= other_rank;
        }
    }
    else if (*(iter.getPtr()->getData()) < *(target_node->getData())) {
        calcRankUptoNode(LEFT, target_node, &iter, &rank_tot);
    }
    else{
        calcRankUptoNode(NO, target_node, &iter, &rank_tot);
    }
    return rank_tot;
}

void Group::calcRankUptoNode(CROSSED crossed, RankTreeNode<Player, PlayerRank> *target_node,
                                   RankTreeIterator<Player, PlayerRank> *iter, PlayerRank* rank_tot) {
    if(iter->getPtr() == nullptr){
        return;
    }
    if(crossed != NO){
        if(crossed == RIGHT){
            PlayerRank other_rank = iter->getPtr()->getRank();
            rank_tot->operator-=(other_rank);
        }
        else{
            PlayerRank other_rank = iter->getPtr()->getRank();
            rank_tot->operator+=(other_rank);
        }
    }

    if(iter->getPtr() == target_node) {
        if(!iter->checkNullRight()){
            iter->goRight();
            PlayerRank other_rank = iter->getPtr()->getRank();
            rank_tot->operator-=(other_rank);
        }
        return;
    }

    crossed = NO;
    if (*(iter->getPtr()->getData()) < *(target_node->getData())) {
        iter->goRight();
        if(iter->getPtr() == nullptr){
            return;
        }
        if (*(iter->getPtr()->getData()) > *(target_node->getData())) {
            crossed = RIGHT;
        }
    }
    else {
        iter->goLeft();
        if(iter->getPtr() == nullptr){
            return;
        }
        if (*(iter->getPtr()->getData()) < *(target_node->getData()) || *(iter->getPtr()->getData()) == *(target_node->getData()) ) {
            crossed = LEFT;
        }
    }
    calcRankUptoNode(crossed, target_node, iter, rank_tot);
}

RankTreeNode<Player, PlayerRank>* Group::findMthPlayerTreeNodeWrapper(int m) {
    RankTreeIterator<Player, PlayerRank> iter = non_0_level_players_tree->begin();
    if (iter.getPtr() == nullptr) {
        return nullptr;
    }

    return findMthPlayerTreeNode(m, &iter);
}

RankTreeNode<Player, PlayerRank>* Group::findMthPlayerTreeNode(int m, RankTreeIterator<Player, PlayerRank> *iter) {
    RankTreeIterator<Player, PlayerRank>* temp_iter = iter;
    if (iter->getPtr() == nullptr) {
        return nullptr;
    }

    int right_node_count = 0;
    int left_node_count = 0;

    if (!temp_iter->checkNullRight()){
        temp_iter->goRight();
        right_node_count += temp_iter->getPtr()->getRank().getNodeCount();
        temp_iter->goFather();
    }

    if ( right_node_count == m-1){
        return iter->getPtr();
    }

    if (!temp_iter->checkNullLeft()){
        temp_iter->goLeft();
        left_node_count += temp_iter->getPtr()->getRank().getNodeCount();
        temp_iter->goFather();
    }

    if (right_node_count >= m) {
        iter->goRight();
        return findMthPlayerTreeNode(m, iter);
    }
    else {
        iter->goLeft();
        return findMthPlayerTreeNode(m - right_node_count - 1, iter);
    }
}

ReturnValue Group::calcPlayerBounds(int m, int score, int *Lower_bound_players, int *higher_bound_players) {
    // check amount of players in group
    if (num_of_players < m) {
        // not enough players in group
        Lower_bound_players = nullptr;
        higher_bound_players = nullptr;
        return MY_FAILURE;
    }

    // m is bigger/equal to amount of players in group
    // check amount of players in tree. if m is bigger, level_0_list is included
    bool list_included = (non_0_level_players_tree->getSize() < m);
    int mth_player_level = 0; // level_m
    int more_than_mth_level_players = 0; // t
    int more_than_mth_with_score = 0; // k
    int players_with_mth_player_level = 0; // x
    int mth_level_with_score = 0; // y
    ReturnValue res;

    // if list is included, we need to get ALL the players from the tree, and the extra from the list
    if (list_included) {
        mth_player_level = 0;
        players_with_mth_player_level = level_0_players_list->getSize();
        mth_level_with_score = level_0_score_hist->getVal(score - 1);

        more_than_mth_level_players = non_0_level_players_tree->getSize();
        if (more_than_mth_level_players != 0) {
            RankTreeIterator<Player, PlayerRank> iter = non_0_level_players_tree->begin();
            RankTreeNode<Player, PlayerRank> *root_ptr = iter.getPtr();
            more_than_mth_with_score = root_ptr->getRank().getScoreHist().getVal(score - 1);
        }
    }
    else {
        //find the level of the mth player
        mth_player_level = findMthPlayerTreeNodeWrapper(m)->getData()->getLevel();

        //find the amount of players with said level, and the amount out of them with the correct score
        double dummy = 0;
        res = getPercentOfPlayersWithScoreInRange(mth_player_level, mth_player_level, score, &dummy,
                                                  &mth_level_with_score, &players_with_mth_player_level);
        if(res != MY_SUCCESS) {
            return res;
        }

        //find the level of the top player
        int top_level = non_0_level_players_tree->getRightMostNode()->getData()->getLevel();

        //find the amount of players with levels from mth +1 to top level, and the amount out of them with the correct score
        if(top_level == mth_player_level){
            more_than_mth_level_players = 0;
            more_than_mth_with_score = 0;
        }
        else {
            res = getPercentOfPlayersWithScoreInRange(mth_player_level + 1, top_level, score, &dummy,
                                                      &more_than_mth_with_score, &more_than_mth_level_players);
            if(res != MY_SUCCESS) {
                return res;
            }
        }
    }

    //calculate higher bound
    if(m - more_than_mth_level_players <= mth_level_with_score){
        *higher_bound_players = more_than_mth_with_score + (m - more_than_mth_level_players);
    }
    else{
        *higher_bound_players = more_than_mth_with_score + mth_level_with_score;
    }

    //calculate lower bound
    if(m - more_than_mth_level_players <= players_with_mth_player_level - mth_level_with_score){
        *Lower_bound_players = more_than_mth_with_score;
    }
    else{
        *Lower_bound_players = more_than_mth_with_score + ((m - more_than_mth_level_players) - (players_with_mth_player_level - mth_level_with_score));
    }
    return MY_SUCCESS;
}

Group& Group::operator+=(Group& other_group) {
//    if (&other_group == nullptr){
//        throw std::exception();
//    }

    if (other_group.num_of_players == 0) {
        return *this;
    }

    // add other group's num_of_players to this group's num_of_players
    this->num_of_players += other_group.num_of_players;

    // merge other_node list into this list, and merge other_node level_0_histogram into this level_0_histogram
    this->level_0_players_list->merge_to_me(*other_group.level_0_players_list);
    *(this->level_0_score_hist) += *(other_group.level_0_score_hist);

    // merge other_node tree into this tree
    this->non_0_level_players_tree->mergeTreeToMe(*other_group.non_0_level_players_tree, scale);

    // insert all players of other_group to this group
    this->players_hash_table->mergeToMe(other_group.players_hash_table);

    // after merging trees, update the tree_ptr in hash_table_val to the new tree_ptr after the merge
    this->updateHashTableTreePtrs();

    // update this group's highest level players ptr
    if (this->highest_level_player == nullptr){
        if (other_group.highest_level_player != nullptr){
            this->highest_level_player = other_group.highest_level_player;
        }
    }
    else {
        if (other_group.highest_level_player != nullptr){
            if (this->highest_level_player->getLevel() < other_group.highest_level_player->getLevel()) {
                this->highest_level_player = other_group.highest_level_player;
            }
        }
    }

    // update this group's lowest level players ptr
    if (this->lowest_level_player == nullptr){
        if (other_group.lowest_level_player != nullptr){
            this->lowest_level_player = other_group.lowest_level_player;
        }
    }
    else {
        if (other_group.lowest_level_player != nullptr){
            if (this->lowest_level_player->getLevel() > other_group.lowest_level_player->getLevel()) {
                this->lowest_level_player = other_group.lowest_level_player;
            }
        }
    }

    // other group was merged to this group, reset the other group
    other_group.resetGroup();

    return *this;
}

void Group::updateHashTableTreePtrs() {
    RankTreeIterator<Player, PlayerRank> iter = non_0_level_players_tree->begin();

    recursiveUpdateHashTableTreePtrs(iter);
}

void Group::recursiveUpdateHashTableTreePtrs(RankTreeIterator<Player, PlayerRank> iter) {
    if (iter.getPtr() == nullptr){
        return;
    }

    if (!iter.checkNullLeft()){
        iter.goLeft();
        recursiveUpdateHashTableTreePtrs(iter);
        iter.goFather();
    }

    if (!iter.checkNullRight()){
        iter.goRight();
        recursiveUpdateHashTableTreePtrs(iter);
        iter.goFather();
    }

    // find hash_table_val of curr player
    int curr_player_id = iter.getPtr()->getData()->getPlayerID();
    GroupHashTableVal* temp_val;
    getPlayerArrayNode(curr_player_id, &temp_val);

    // update the tree_node_ptr in the hash_table_val
    (*temp_val).setTreeNode(iter.getPtr());
}







