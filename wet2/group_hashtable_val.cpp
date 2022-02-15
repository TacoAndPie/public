#include "group_hashtable_val.h"


GroupHashTableVal::~GroupHashTableVal() {
    player_ptr = nullptr;
    tree_node = nullptr;
    list_node = nullptr;
}

void GroupHashTableVal::setNullListNode() {
    list_node = nullptr;
}


void GroupHashTableVal::setListNode(DoublyLinkedListNode<Player>* new_list_node) {
    list_node = new_list_node;
}

void GroupHashTableVal::setTreeNode(RankTreeNode<Player, PlayerRank> *new_tree_node) {
    tree_node = new_tree_node;
}

bool GroupHashTableVal::operator==(GroupHashTableVal* other_node) {
    if (other_node == nullptr) {
        return (*this == nullptr);
    }
    if (other_node->player_ptr == nullptr) {
        return (this->player_ptr == nullptr);
    }
    return (player_ptr->getPlayerID() == other_node->player_ptr->getPlayerID());
}

GroupHashTableVal& GroupHashTableVal::operator=(GroupHashTableVal *other_node) {
    if (other_node == nullptr) {
        player_ptr = nullptr;
        level = 0;
        score = 0;
        tree_node = nullptr;
        list_node = nullptr;
    }
    else {
        player_ptr = other_node->player_ptr;
        level = other_node->level;
        score = other_node->score;
        tree_node = other_node->tree_node;
        list_node = other_node->list_node;
    }
    return *this;
}


