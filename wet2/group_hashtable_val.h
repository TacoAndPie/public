#ifndef WET2_GROUP_HASHTABLE_VAL_H
#define WET2_GROUP_HASHTABLE_VAL_H

#include "player.h"
#include "player_rank.h"
#include "rank_tree_node.h"
#include "doubly_linked_list_node.h"

class GroupHashTableVal {
    Player* player_ptr;
    int level;
    int score;
    RankTreeNode<Player, PlayerRank>* tree_node;
    DoublyLinkedListNode<Player>* list_node;

public:
    GroupHashTableVal(Player* player_ptr, RankTreeNode<Player, PlayerRank>* tree_node, DoublyLinkedListNode<Player>* list_node) :
    player_ptr(player_ptr), level(player_ptr->getLevel()), score(player_ptr->getScore()), tree_node(tree_node), list_node(list_node) {}
    explicit GroupHashTableVal(Player* player_ptr) : player_ptr(player_ptr), level(player_ptr->getLevel()), score(player_ptr->getScore()),
                                                     tree_node(nullptr), list_node(nullptr) {}
    GroupHashTableVal() : player_ptr(nullptr), level(0), score(0), tree_node(nullptr), list_node(nullptr) {}
    ~GroupHashTableVal();
    Player* getPlayerPtr() { return player_ptr; }
    int getKey() { return player_ptr->getPlayerID(); }
    int getScore() { return score; }
    int getLevel() { return level; }
    DoublyLinkedListNode<Player>* getListNode() { return list_node; }
    RankTreeNode<Player, PlayerRank>* getTreeNode() { return tree_node; }
    void setNullListNode();
    void setListNode(DoublyLinkedListNode<Player>* new_list_node);
    void setTreeNode(RankTreeNode<Player, PlayerRank>* new_tree_node);
    void increaseLevel(int level_increase) { level += level_increase; }
    void updateScore(int new_score) { score = new_score; }

    bool operator==(GroupHashTableVal* other_node);
    GroupHashTableVal& operator=(GroupHashTableVal* other_node);

    GroupHashTableVal* getGroupHashTableNode() { return this; }

};


#endif //WET2_GROUP_HASHTABLE_VAL_H

