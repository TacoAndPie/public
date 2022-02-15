#ifndef WET2_UNION_H
#define WET2_UNION_H


#include <cstdlib>
#include <stdexcept>
#include "up_tree_node.h"
#include "rank_tree.h"


/*
 * Union DS expects index values from 0 to size (not including size).
 * */

template <typename data_t>
class Union {
    int size;
    UpTreeNode<data_t>** array_base;
public:

    Union() = default;
    explicit Union(int new_size);
    ~Union() = default;
    ReturnValue makeSet(int index, data_t data);
    ReturnValue findNodeRepByID(int index, UpTreeNode<data_t>** node);
    ReturnValue getNodeByID(int index, UpTreeNode<data_t>** node);
    ReturnValue findDataPtrByIndex(int index, data_t* data_ptr);
    ReturnValue unify(int index1, int index2);
};

/* union gets num of elements in structure, creates new array with correct size, creates new node for each element
 (each element gets its own tree with only one node), and saves the node pointer in the array element index
 union doesn't create new groups, only array with correct amount of nodes, user needs to create actual new groups
 and insert each group to the correct node in array.*/
template<typename data_t>
Union<data_t>::Union(int new_size) : size(new_size) {
    // create empty array of node pointers
    array_base = new UpTreeNode<data_t>*[size];
    if(!array_base){
      //  throw std::bad_alloc();
    }
    // create empty node for each array element and insert its pointer to the array
    for (int i = 0; i < size; i++){
        UpTreeNode<data_t>* new_node = new UpTreeNode<data_t>();
        *(array_base + i) = new_node;
    }
}


template <typename data_t>
ReturnValue Union<data_t>::makeSet(int index, data_t data) {
    if (index < 0 || index >= size){
        return MY_INVALID_INPUT;
    }

    UpTreeNode<data_t>* node_ptr = new UpTreeNode<data_t>(data);
    if (node_ptr == nullptr){
        return MY_ALLOCATION_ERROR;
    }

    *(array_base + index) = node_ptr;
    return MY_SUCCESS;
}

template <typename data_t>
ReturnValue Union<data_t>::findNodeRepByID(int index, UpTreeNode<data_t>** node){
    // check input
    if (index < 0 || index >= size){
        return MY_INVALID_INPUT;
    }

    // start at array base (array starts with index=0)
    UpTreeNode<data_t>* curr_node = array_base[index];

    // find rep node for the upTree
    while (!(curr_node->isRoot())){
        curr_node = curr_node->getFather();
    }

    if (curr_node == nullptr){
        return ELEMENT_DOES_NOT_EXIST;
    }

    ReturnValue res;
    // shrink routes - update all nodes along route to point to root of upTree
    UpTreeNode<data_t>* root = curr_node; // at this point, curr_node is the root
    UpTreeNode<data_t>* father;
    curr_node = *(array_base + index); // reset cur_node to the original node of index
    while (!(curr_node->isRoot())){
        father = curr_node->getFather();
        if (!father->isRoot()){
            res = curr_node->setFather(root);
            if ( res != MY_SUCCESS){
                return res;
            }
        }
        curr_node = father;
    }

    // root is the node rep of the upTree, so that's the node we return
    *node = root;
    return MY_SUCCESS;
}

template <typename data_t>
ReturnValue Union<data_t>::getNodeByID(int index, UpTreeNode<data_t>** node){
    // check input
    if (index < 0 || index >= size){
        return MY_INVALID_INPUT;
    }

    // start at array base (array starts with index=0)
    *node = array_base[index];
    return MY_SUCCESS;
}

template <typename data_t>
ReturnValue Union<data_t>::findDataPtrByIndex(int index, data_t* data_ptr){
    UpTreeNode<data_t>* node;
    ReturnValue res = findNodeRepByID(index, &node);
    if (res != MY_SUCCESS){
        return res;
    }

    *data_ptr = node->getData();
    return MY_SUCCESS;
}

template <typename data_t>
ReturnValue Union<data_t>::unify(int index1, int index2){
    // check input
    if (index1 < 0 || index1 >= size || index2 < 0 || index2 >= size){
        return MY_INVALID_INPUT;
    }

    // check if same element(no need to merge anything)
    if (index1 == index2){
        return MY_SUCCESS;
    }

    // find rep node of each element (elements are saved in reversed tree)
    UpTreeNode<data_t>* node1;
    UpTreeNode<data_t>* node2;
    ReturnValue res = findNodeRepByID(index1, &node1);
    if (res == MY_INVALID_INPUT || res == ELEMENT_DOES_NOT_EXIST){
        return res;
    }
    res = findNodeRepByID(index2, &node2);
    if (res == MY_INVALID_INPUT || res == ELEMENT_DOES_NOT_EXIST){
        return res;
    }

    // check if both elements have the same rep node (if so, no need to merge)
    if (node1 == node2){
        return MY_SUCCESS;
    }

    // check which node has "smaller" data, and merge to the smaller one.
    if (*node1 < *node2) {
        res = node2->mergeToMe(node1);
    }
    else {
        res = node1->mergeToMe(node2);
    }

    return res;
}


#endif //WET2_UNION_H
