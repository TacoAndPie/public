#ifndef WET2_RANK_TREE_H
#define WET2_RANK_TREE_H

#include "rank_tree_iterator.h"
#include <cmath>
#include <iostream>

typedef enum {MY_ALLOCATION_ERROR, MY_INVALID_INPUT, MY_FAILURE, MY_SUCCESS, ELEMENT_EXISTS,
    ELEMENT_DOES_NOT_EXIST, NO_ELEMENT_INSERT_LEFT, NO_ELEMENT_INSERT_RIGHT, NO_ROOT} ReturnValue;

using std::ostream;

template<typename data_t, typename rank_t>
class RankTree {
    RankTreeNode<data_t, rank_t>* root;
    int size;

    //Tree Rolls
    ReturnValue fixTree(RankTreeNode<data_t, rank_t>* node);
    ReturnValue RollTree(RankTreeNode<data_t, rank_t>* node);
    ReturnValue LLRoll(RankTreeNode<data_t, rank_t>* node);
    ReturnValue RRRoll(RankTreeNode<data_t, rank_t>* node);
    ReturnValue LRRoll(RankTreeNode<data_t, rank_t>* node);
    ReturnValue RLRoll(RankTreeNode<data_t, rank_t>* node);

    //Node Removal Helper Functions
    ReturnValue removeRoot(RankTreeNode<data_t, rank_t>* node);
    ReturnValue removeNonRoot(RankTreeNode<data_t, rank_t>* node);

    //Node Swaps
    void swapNodes(RankTreeNode<data_t, rank_t>* node1, RankTreeNode<data_t, rank_t>* node2);
    void swapRoot(RankTreeNode<data_t, rank_t>* received_root, RankTreeNode<data_t, rank_t>* node);
    void swapNonRoot(RankTreeNode<data_t, rank_t>* node1, RankTreeNode<data_t, rank_t>* node2);

    //Tree Merging Helper Function and Calculations
    static int findRequiredEmptyTreeHeight(int tree_size);
    static int findNumOfNodesToDelete(int tree_size);
    static RankTreeNode<data_t, rank_t>* createEmptyTree(int* height, int scale);
    static bool removeExtraTreeNodes(RankTreeNode<data_t, rank_t>* node, int* num_of_nodes_to_delete, int* tree_height);

    //Array Casting
    static void putTreeToArray(RankTreeNode<data_t, rank_t>* node, RankTreeNode<data_t, rank_t>* array[], int* i);
    static void putArrayIntoTree(RankTreeNode<data_t, rank_t>** node, RankTreeNode<data_t, rank_t>*** array, int* i);

    //Array Merging
    static void mergeArrays(RankTreeNode<data_t, rank_t>* arr1[], RankTreeNode<data_t, rank_t>* arr2[],
                            RankTreeNode<data_t, rank_t>* merged_arr[], int arr1_size, int arr2_size);

    // Tree rank update functions
    void fullTreeRankUpdate();
    void recursiveRankUpdate(RankTreeIterator<data_t, rank_t> *iter);

public:


    RankTree() : size(0), root(nullptr) {}
    ~RankTree();
    void clearTree();

    int getSize() const { return size; }
    ReturnValue find(data_t data, RankTreeNode<data_t, rank_t>** node_find);
    ReturnValue insert(data_t* data, int scale);
    ReturnValue remove(data_t data);
    void mergeTreeToMe(RankTree<data_t, rank_t>& other_tree, int scale);
    RankTreeIterator<data_t, rank_t> begin();

    //Rank functions
    void updateRankAlongPath(RankTreeNode<data_t, rank_t>* node);

    // functions for getting highest/lowest level players from tree
    RankTreeNode<data_t, rank_t>* getLeftMostNode();
    RankTreeNode<data_t, rank_t>* getRightMostNode();
};

// public class functions
template<typename data_t, typename rank_t>
RankTree<data_t, rank_t>::~RankTree(){
    RankTreeNode<data_t, rank_t>::recursiveNodeDeletion(root);
    root = nullptr;
    size = 0;
}

// this doesn't delete the tree, but it deletes all tree nodes ane resets the root to nullptr.
template<typename data_t, typename rank_t>
void RankTree<data_t, rank_t>::clearTree(){
    RankTreeNode<data_t, rank_t>::recursiveNodeDeletion(root);
    root = nullptr;
    size = 0;
}

template<typename data_t, typename rank_t>
ReturnValue RankTree<data_t, rank_t>::find(data_t data, RankTreeNode<data_t, rank_t>** node_find){
    if(root == nullptr) {
        return NO_ROOT;
    }

    RankTreeIterator<data_t, rank_t> iter = begin();

    while(iter.node_ptr) {
        *node_find = iter.node_ptr;
        if (iter.getData() == data) {
            return ELEMENT_EXISTS;
        } else if (iter.getData() > data) {
            iter.goLeft();
        } else {
            iter.goRight();
        }
    }

    if(*((*node_find)->getData()) > data){
        return NO_ELEMENT_INSERT_LEFT;
    }
    return NO_ELEMENT_INSERT_RIGHT;
}

template<typename data_t, typename rank_t>
ReturnValue RankTree<data_t, rank_t>::insert(data_t* data, int scale){
    RankTreeNode<data_t, rank_t>* node_to_insert = new RankTreeNode<data_t, rank_t>(data, scale);
    if(!node_to_insert){
        return MY_ALLOCATION_ERROR;
    }

    if(!root){
        size++;
        root = node_to_insert;
        node_to_insert->updateRank();
        return MY_SUCCESS;
    }

    RankTreeNode<data_t, rank_t>* node_find = root;

    // find where to insert the new node. node_find will hold the would-be-father of new node.
    ReturnValue res = find(*data, &node_find);
    switch (res) {
        case MY_ALLOCATION_ERROR :
            return MY_ALLOCATION_ERROR;
        case NO_ROOT :
            size++;
            root = node_to_insert;
            return MY_SUCCESS;
        case ELEMENT_EXISTS :
            return MY_FAILURE;
        case NO_ELEMENT_INSERT_LEFT :
            node_find->left = node_to_insert;
            node_to_insert->father = node_find;
            break;
        case NO_ELEMENT_INSERT_RIGHT :
            node_find->right = node_to_insert;
            node_to_insert->father = node_find;
        default :
            break;
    }

    // increase num of nodes in tree
    size++;

    // fix tree after insertion of new_node, starting at the father of new_node.
    RankTree<data_t, rank_t>::updateRankAlongPath(node_to_insert);
    return fixTree(node_find);
}

template<typename data_t, typename rank_t>
ReturnValue RankTree<data_t, rank_t>::remove(data_t data) {
    RankTreeNode<data_t, rank_t>* node_find = root;

    ReturnValue res = find(data, &node_find);
    switch (res) {
        case MY_ALLOCATION_ERROR :
            return MY_ALLOCATION_ERROR;
        case ELEMENT_EXISTS:
            break;
        case ELEMENT_DOES_NOT_EXIST:
            return ELEMENT_DOES_NOT_EXIST;
        default:
            return MY_FAILURE;
    }
    if(node_find == root){
        return removeRoot(node_find);
    }
    else{
        return removeNonRoot(node_find);
    }
}

template<typename data_t, typename rank_t>
void RankTree<data_t, rank_t>::mergeTreeToMe(RankTree<data_t, rank_t>& other_tree, int scale){

    if (other_tree.size == 0){
        return;
    }

    // create 2 empty arrays with matching size of trees
    RankTreeNode<data_t, rank_t>** array1 = new RankTreeNode<data_t, rank_t>*[size];
    RankTreeNode<data_t, rank_t>** array2 = new RankTreeNode<data_t, rank_t>*[other_tree.size];

    // create array for merged tree
    int merged_size = (size + other_tree.size);
    RankTreeNode<data_t, rank_t>** merged_array = new RankTreeNode<data_t, rank_t>*[merged_size];

    // insert tree nodes to arrays
    int i=0 , j = 0;
    RankTree<data_t, rank_t>::putTreeToArray(root, array1, &i);
    RankTree<data_t, rank_t>::putTreeToArray(other_tree.root, array2, &j);

    // merge arrays
    RankTree<data_t, rank_t>::mergeArrays(array1, array2, merged_array, size, other_tree.size);

    // create new empty tree with correct amount of nodes
    int merged_tree_height = RankTree<data_t, rank_t>::findRequiredEmptyTreeHeight(merged_size);
    RankTreeNode<data_t, rank_t>* future_root = RankTree<data_t, rank_t>::createEmptyTree( &merged_tree_height, scale);

    int num_of_nodes_to_delete = RankTree<data_t, rank_t>::findNumOfNodesToDelete(merged_size);
    RankTree<data_t, rank_t>::removeExtraTreeNodes(future_root, &num_of_nodes_to_delete, &merged_tree_height);

    // insert nodes from merged array to empty tree
    int m=0;
    RankTree<data_t, rank_t>::putArrayIntoTree(&future_root, &merged_array, &m);
    this->size = merged_size;
    RankTreeNode<data_t, rank_t>* old_root = this->root;
    RankTreeNode<data_t,rank_t>::recursiveNodeDeletion(old_root);
    this->root = future_root;
    this->fullTreeRankUpdate();

    delete[] array1;
    delete[] array2;
    delete[] merged_array;
}

template<typename data_t, typename rank_t>
void RankTree<data_t, rank_t>::fullTreeRankUpdate() {
    RankTreeIterator<data_t, rank_t> iter = begin();
    recursiveRankUpdate(&iter);
}

template<typename data_t, typename rank_t>
void RankTree<data_t, rank_t>::recursiveRankUpdate(RankTreeIterator<data_t, rank_t>* iter) {
    if (iter->node_ptr == nullptr){
        return;
    }

    if (!iter->checkNullLeft()) {
        iter->goLeft();
        recursiveRankUpdate(iter);
        iter->goFather();
    }

    if (!iter->checkNullRight()) {
        iter->goRight();
        recursiveRankUpdate(iter);
        iter->goFather();
    }

    iter->node_ptr->updateRank();
}

template<typename data_t, typename rank_t>
RankTreeIterator<data_t, rank_t> RankTree<data_t, rank_t>::begin() {
    return RankTreeIterator<data_t, rank_t>(root);
}

// private class functions
template<typename data_t, typename rank_t>
ReturnValue RankTree<data_t, rank_t>::fixTree(RankTreeNode<data_t, rank_t>* node) {
    int old_height = 0;
    RankTreeIterator<data_t, rank_t> iter = RankTreeIterator<data_t, rank_t>(node);
    while(iter.node_ptr != nullptr){
        old_height = iter.node_ptr->height;
        iter.node_ptr->updateHeight();
        if(abs(iter.node_ptr->getBF()) == 2){
            if(RollTree(iter.node_ptr) == MY_ALLOCATION_ERROR) {
                return MY_ALLOCATION_ERROR;
            }
            iter.goFather();
        }

        if(iter.node_ptr == nullptr){
            break;
        }
        if(old_height == iter.node_ptr->height){
            break;
        }
        iter.goFather();
    }
    return MY_SUCCESS;
}

template<typename data_t, typename rank_t>
ReturnValue RankTree<data_t, rank_t>::RollTree(RankTreeNode<data_t, rank_t>* node){
    if(node->getBF() == 2){
        if(node->left->getBF() >= 0){
            return LLRoll(node);
        }
        else if(node->left->getBF() == -1)
            return LRRoll(node);
    }
    if(node->getBF() == -2){
        if(node->right->getBF() <= 0){
            return RRRoll(node);
        }
        else if(node->right->getBF() == 1){
            return RLRoll(node);
        }
    }
    return MY_SUCCESS;
}

template<typename data_t, typename rank_t>
ReturnValue RankTree<data_t, rank_t>::LLRoll(RankTreeNode<data_t, rank_t>* node) {
    // save temp node (the one we detach)
    RankTreeNode<data_t, rank_t>* saved_left_ptr = node->left;
    if(!saved_left_ptr){
        return MY_ALLOCATION_ERROR;
    }

    // connect my left son to my father
    saved_left_ptr->father = node->father;
    if(node->isARightSon()){
        saved_left_ptr->father->right = saved_left_ptr;
    }
    else if(node->isALeftSon()){
        saved_left_ptr->father->left = saved_left_ptr;
    }

    // change my left ptr to hold the right son of my (previous) left son
    node->left = saved_left_ptr->right;
    if(saved_left_ptr->right){
        saved_left_ptr->right->father = node;
    }

    // connect me as the right son of my previous left son
    saved_left_ptr->right = node;
    node->father = saved_left_ptr;

    // if previously I was the root, change me previous left son to be the new root
    if(saved_left_ptr->father == nullptr){
        root = saved_left_ptr;
    }

    node->updateHeight();
    saved_left_ptr->updateHeight();
    updateRankAlongPath(node);

    return MY_SUCCESS;
}

template<typename data_t, typename rank_t>
ReturnValue RankTree<data_t, rank_t>::RRRoll(RankTreeNode<data_t, rank_t>* node) {
    // save temp node (the one we detach)
    RankTreeNode<data_t, rank_t>* saved_right_ptr = node->right;
    if(!saved_right_ptr){
        return MY_ALLOCATION_ERROR;
    }

    // connect my right son to my father
    saved_right_ptr->father = node->father;
    if(node->isARightSon()){
        saved_right_ptr->father->right = saved_right_ptr;
    }
    else if(node->isALeftSon()){
        saved_right_ptr->father->left = saved_right_ptr;
    }

    // change my right ptr to hold the left son of my (previous) right son
    node->right = saved_right_ptr->left;
    if(saved_right_ptr->left){
        saved_right_ptr->left->father = node;
    }

    // connect me as the left son of my previous right son
    saved_right_ptr->left = node;
    node->father = saved_right_ptr;

    // if previously I was the root, change me previous right son to be the new root
    if(saved_right_ptr->father == nullptr){
        root = saved_right_ptr;
    }
    node->updateHeight();
    saved_right_ptr->updateHeight();
   updateRankAlongPath(node);
    return MY_SUCCESS;
}

template<typename data_t, typename rank_t>
ReturnValue RankTree<data_t, rank_t>::LRRoll(RankTreeNode<data_t, rank_t>* node) {
    if(RRRoll(node->left) == MY_SUCCESS){
        if (LLRoll(node) == MY_SUCCESS){
            return MY_SUCCESS;
        }
    }
    return MY_ALLOCATION_ERROR;
}

template<typename data_t, typename rank_t>
ReturnValue RankTree<data_t, rank_t>::RLRoll(RankTreeNode<data_t, rank_t>* node) {
    if(LLRoll(node->right) == MY_SUCCESS){
        if(RRRoll(node) == MY_SUCCESS){
            return MY_SUCCESS;
        }
    }
    return MY_ALLOCATION_ERROR;
}

template<typename data_t, typename rank_t>
ReturnValue RankTree<data_t, rank_t>::removeRoot(RankTreeNode<data_t, rank_t>* node) {
    if(node->isLeaf()){
        root = nullptr;
        delete node;
        size--;
        return MY_SUCCESS;
    }
    else if(node->onlyHaveRightSon()) {
        node->right->father = nullptr;
        root = node->right;
        delete node;
        size--;
        return MY_SUCCESS;
    }
    else if(node->onlyHaveLeftSon()){
        node->left->father = nullptr;
        root = node->left;
        delete node;
        size--;
        return MY_SUCCESS;
    }
    else{
        RankTreeIterator<data_t, rank_t> iter = RankTreeIterator<data_t, rank_t>(node);
        iter.goRight();
        while(!iter.checkNullLeft()){
            iter.goLeft();
        }
        swapNodes(node, iter.node_ptr);
        return removeNonRoot(node);
    }

}

template<typename data_t, typename rank_t>
ReturnValue RankTree<data_t, rank_t>::removeNonRoot(RankTreeNode<data_t, rank_t>* node) {
    if(node->haveTwoSons()){
        RankTreeIterator<data_t, rank_t> iter = RankTreeIterator<data_t, rank_t>(node);
        iter.goRight();
        while(!iter.checkNullLeft()){
            iter.goLeft();
        }
        swapNodes(node, iter.node_ptr);
    }

    RankTreeNode<data_t, rank_t>* future_father;
    if(node->isLeaf()){
        if(node->isALeftSon()){
            node->father->left = nullptr;
        }
        else if(node->isARightSon()){
            node->father->right = nullptr;
        }
        future_father = node->father;
        delete node;
        size--;
        RankTree<data_t, rank_t>::updateRankAlongPath(future_father);
        return fixTree(future_father);
    }
    if(node->onlyHaveLeftSon()){
        if(node->isALeftSon()){
            node->father->left = node->left;
            node->left->father = node->father;
            future_father = node->father;
            delete node;
            size--;
            RankTree<data_t, rank_t>::updateRankAlongPath(future_father);
            return fixTree(future_father);
        } else if(node->isARightSon()){
            node->father->right = node->left;
            node->left->father = node->father;
            future_father = node->father;
            delete node;
            size--;
            RankTree<data_t, rank_t>::updateRankAlongPath(future_father);
            return fixTree(future_father);
        }
    }
    else if(node->onlyHaveRightSon()){
        if(node->isALeftSon()){
            node->father->left = node->right;
            node->right->father = node->father;
            future_father = node->father;
            delete node;
            size--;
            RankTree<data_t, rank_t>::updateRankAlongPath(future_father);
            return fixTree(future_father);
        } else if(node->isARightSon()){
            node->father->right = node->right;
            node->right->father = node->father;
            future_father = node->father;
            delete node;
            size--;
            RankTree<data_t, rank_t>::updateRankAlongPath(future_father);
            return fixTree(future_father);
        }
    }
    return MY_SUCCESS;
}


template<typename data_t, typename rank_t>
void RankTree<data_t, rank_t>::swapNodes(RankTreeNode<data_t, rank_t>* node1, RankTreeNode<data_t, rank_t>* node2){
    if(root == node1){
        swapRoot(node1, node2);
    }
    else{
        swapNonRoot(node1, node2);
    }
}

template<typename data_t, typename rank_t>
void RankTree<data_t, rank_t>::swapRoot(RankTreeNode<data_t, rank_t>* received_root, RankTreeNode<data_t, rank_t>* node){
    /*
     this function assumes that node is the smallest node that is bigger than received_root.
     this means that node does not have a left son.
     (if it had a left son, it would be smaller than node, we would choose it instead.)
    */
    RankTreeNode<data_t, rank_t>* orig_father_of_node = node->father;
    RankTreeNode<data_t, rank_t>* orig_right_son_of_node = node->right;

    // node is in right sub_tree of received_root
    if(received_root->right != node){ // case: node isn't right son of received_root
        // move node to be root
        node->right = received_root->right;
        if(received_root->right){
            received_root->right->father = node;
        }
        node->left = received_root->left;
        if(received_root->left){
            received_root->left->father = node;
        }
        node->father = nullptr;
        root = node;

        // move received_root to node's previous location
        received_root->father = orig_father_of_node;
        orig_father_of_node->left = received_root;
        received_root->right = orig_right_son_of_node;
        received_root->left = nullptr;
        if(orig_right_son_of_node != nullptr){
            orig_right_son_of_node->father = received_root;
        }
    }
    else if(received_root->right == node){ // case: node is right son of received_root
        // move node to be root
        node->right = received_root;
        node->left = received_root->left;
        if(received_root->left){
            received_root->left->father = node;
        }
        node->father = nullptr;
        root = node;

        // move received_root to be son of node
        received_root->father = node;
        received_root->left = nullptr;
        received_root->right = orig_right_son_of_node;
        if(orig_right_son_of_node != nullptr){
            orig_right_son_of_node->father = node;
        }
    }
    updateRankAlongPath(received_root);
}

template<typename data_t, typename rank_t>
void RankTree<data_t, rank_t>::swapNonRoot(RankTreeNode<data_t, rank_t>* node1, RankTreeNode<data_t, rank_t>* node2){
    /*
     this function assumes that node2 is the smallest node that is bigger than node1.
     this means that node2 does not have a left son.
     (if it had a left son, it would be smaller than node2, we would choose it instead.)
    */
    RankTreeNode<data_t, rank_t>* orig_father_of_node2 = node2->father;
    RankTreeNode<data_t, rank_t>* orig_right_son_of_node2 = node2->right;

    if(node1->right != node2){  // case: node2 isn't right son of node1
        // move node2 to node1's previous location
        node2->right = node1->right;
        if(node1->right){
            node1->right->father = node2;
        }
        node2->left = node1->left;
        if(node1->left){
            node1->left->father = node2;
        }
        node2->father = node1->father;
        if(node1->isALeftSon()){
            node1->father->left = node2;
        }
        else if(node1->isARightSon()){
            node1->father->right = node2;
        }

        // move node1 to node2's previous location
        node1->father = orig_father_of_node2;
        orig_father_of_node2->left = node1;  // node2 is left son, if it was a right son we would choose its father instead.
        node1->left = nullptr;
        node1->right = orig_right_son_of_node2;
        if(orig_right_son_of_node2 != nullptr){
            orig_right_son_of_node2->father = node1;
        }
    }
    else if(node1->right == node2){ // case: node2 is right son of node1
        // move node2 to node1's previous location
        node2->right = node1;
        node2->left = node1->left;
        if(node1->left){
            node1->left->father = node2;
        }
        node2->father = node1->father;
        if(node1->isALeftSon()){
            node1->father->left = node2;
        }
        else if(node1->isARightSon()){
            node1->father->right = node2;
        }

        // move node1 to node2's previous location
        node1->father = node2;
        node1->left = nullptr;
        node1->right = orig_right_son_of_node2;
        if(orig_right_son_of_node2 != nullptr){
            orig_right_son_of_node2->father = node1;
        }
    }
    updateRankAlongPath(node1);
}

template<typename data_t, typename rank_t>
int RankTree<data_t, rank_t>::findRequiredEmptyTreeHeight(int tree_size) {
    return ceil(log2(tree_size + 1) - 1);
}

template<typename data_t, typename rank_t>
int RankTree<data_t, rank_t>::findNumOfNodesToDelete(int tree_size){
    int tree_height =findRequiredEmptyTreeHeight(tree_size);
    double num_of_nodes_to_delete = (exp2(1 + tree_height) - 1) - tree_size;
    int num_of_nodes_to_delete_int = ceil(num_of_nodes_to_delete);
    return num_of_nodes_to_delete_int;
}

template<typename data_t, typename rank_t>
RankTreeNode<data_t, rank_t>* RankTree<data_t, rank_t>::createEmptyTree(int* height, int scale){
    // recursion break point condition
    if(*height == -1){
        (*height)++;
        return nullptr;
    }

    // create new node to add to tree
    RankTreeNode<data_t, rank_t>* node = new RankTreeNode<data_t, rank_t>(scale);
    RankTreeNode<data_t, rank_t>* temp_node;

    // create left sub_tree with (height-1)
    *(height) = *(height) - 1;
    temp_node = createEmptyTree(height, scale);
    node->left = temp_node;
    if(temp_node){
        temp_node->father = node;
    }

    // create right sub_tree with (height-1)
    *(height) = *(height) - 1;
    temp_node = createEmptyTree(height, scale);
    node->right = temp_node;
    if(temp_node){
        temp_node->father = node;
    }

    // after creating my sub_trees, update my height
    node->updateHeight();
    (*height)++;
    return node;
}

template<typename data_t, typename rank_t>
bool RankTree<data_t, rank_t>::removeExtraTreeNodes(RankTreeNode<data_t, rank_t>* node, int* num_of_nodes_to_delete,
                                                    int* tree_height){
    // recursion end condition
    if (*num_of_nodes_to_delete == 0 || node == nullptr){
        return false;
    }

    // remove node if it's a leaf and there are still nodes to remove
    if (node->isLeaf() && *num_of_nodes_to_delete > 0){
        delete node;
        (*num_of_nodes_to_delete)--;
        return true;
    }

    // remove nodes from right sub_tree
    RankTreeNode<data_t, rank_t>* right_son = node->getRight();
    if(right_son != nullptr){
        if (removeExtraTreeNodes(right_son, num_of_nodes_to_delete, tree_height)){
            node->right = nullptr;
        }
    }

    // remove nodes from left sub_tree
    RankTreeNode<data_t, rank_t>* left_son = node->getLeft();
    if(left_son != nullptr){
        if (removeExtraTreeNodes(left_son, num_of_nodes_to_delete, tree_height)){
            node->left = nullptr;
        }
    }

    // after removing nodes from both sub_trees, update the height of curr node
    node->updateHeight();
    return false;
}

template<typename data_t, typename rank_t>
void RankTree<data_t, rank_t>::putTreeToArray(RankTreeNode<data_t, rank_t>* node, RankTreeNode<data_t,
                                              rank_t>* array[], int* i) {
    if(node == nullptr){
        return;
    }

    putTreeToArray(node->left, array, i);
    array[*i] = node;
    (*i)++;
    putTreeToArray(node->right, array, i);
}

template<typename data_t, typename rank_t>
void RankTree<data_t, rank_t>::putArrayIntoTree(RankTreeNode<data_t, rank_t>** node, RankTreeNode<data_t,
                                                rank_t>*** array, int* i){
    if (!(*node)) {
        return;
    }

    if((*node)->left){
        putArrayIntoTree(&(*node)->left, array, i);
    }

    (*node)->data = (*(*array)[*i]).getData();
    (*node)->resetRank();
    (*i)++;

    if ((*node)->right){
        putArrayIntoTree(&(*node)->right, array, i);
    }
}

template<typename data_t, typename rank_t>
void RankTree<data_t, rank_t>::mergeArrays(RankTreeNode<data_t, rank_t>* arr1[], RankTreeNode<data_t, rank_t>* arr2[],
                                           RankTreeNode<data_t, rank_t>* merged_arr[], int arr1_size, int arr2_size) {
    int i= 0, j = 0, k = 0;

    // go over arrays 1 and 2, insert the element with the smaller data to array3
    while(i < arr1_size && j < arr2_size){
        if (*(((RankTreeNode<data_t, rank_t>*)arr1[i])->getData()) < *(((RankTreeNode<data_t, rank_t>*)arr2[j])->getData())){
            merged_arr[k] = arr1[i];
            i++;
        }
        else {
            merged_arr[k] = arr2[j];
            j++;
        }
        k++;
    }

    // check which of the arrays we finished scanning, add the remaining elements from other array to array3
    if((arr1_size-i) > (arr2_size-j)){
        while(k < arr1_size + arr2_size) {
            merged_arr[k] = arr1[i];
            i++;
            k++;
        }
    }
    else{
        while(k < arr1_size + arr2_size) {
            merged_arr[k] = arr2[j];
            j++;
            k++;
        }
    }
}

template<typename data_t, typename rank_t>
void RankTree<data_t, rank_t>::updateRankAlongPath(RankTreeNode<data_t, rank_t>* node){
    node->updateRank();
    if (node->father != nullptr){
        updateRankAlongPath(node->father);
    }
}

template<typename data_t, typename rank_t>
RankTreeNode<data_t, rank_t> *RankTree<data_t, rank_t>::getLeftMostNode() {
    RankTreeIterator<data_t, rank_t> iter = begin();
    if(iter.getPtr() == nullptr) {
        return nullptr;
    }

    if(iter.checkNullLeft()){
        return iter.node_ptr;
    }
    iter.goLeft();
    while(iter.node_ptr){
        if(iter.checkNullLeft()){
            return iter.node_ptr;
        }
        iter.goLeft();
    }

    return iter.node_ptr;
}

template<typename data_t, typename rank_t>
RankTreeNode<data_t, rank_t> *RankTree<data_t, rank_t>::getRightMostNode() {
    RankTreeIterator<data_t, rank_t> iter = begin();
    if(iter.node_ptr == nullptr) {
        return nullptr;
    }

    if(iter.checkNullRight()){
        return iter.node_ptr;
    }
    iter.goRight();
    while(iter.node_ptr){
        if(iter.checkNullRight()){
            return iter.node_ptr;
        }
        iter.goRight();
    }

    return iter.node_ptr;
}




#endif //WET2_RANK_TREE_H
