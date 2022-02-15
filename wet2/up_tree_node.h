#ifndef WET2_UP_TREE_NODE_H
#define WET2_UP_TREE_NODE_H

#include "rank_tree.h"

template<typename data_t> class UpTree;
template<typename data_t> class UpTreeIterator;

template<typename data_t>
class UpTreeNode {
    data_t data;
    bool is_root;
    UpTreeNode<data_t>* father;
    int size;

    void setRoot(bool is_new_root) { is_root = is_new_root; }

public:
    UpTreeNode() : data(nullptr), father(nullptr), is_root(true), size(1) {}
    explicit UpTreeNode(data_t new_data) : data(new_data), father(nullptr), is_root(true), size(1) {}
    ~UpTreeNode() = default;
    UpTreeNode* getFather() { return father; }
    data_t getData() { return data; }
    int getSize() { return size; }
    bool isRoot() { return is_root; }
    ReturnValue setFather(UpTreeNode<data_t>* new_father);
    void setData(data_t new_data) { data = new_data; }
    ReturnValue mergeToMe(UpTreeNode<data_t>* other_node);
    bool operator<(UpTreeNode<data_t> node2);

    friend class UpTree<data_t>;
    friend class UpTreeIterator<data_t>;
};

template<typename data_t>
ReturnValue UpTreeNode<data_t>::setFather(UpTreeNode<data_t>* new_father){
    if (new_father == this) {
        return MY_FAILURE;
    }

    setRoot(new_father == nullptr);
    father = new_father;
    return MY_SUCCESS;
}

template<typename data_t>
ReturnValue UpTreeNode<data_t>::mergeToMe(UpTreeNode<data_t>* other_node){
    // check input
    if (other_node == nullptr){
        return MY_INVALID_INPUT;
    }

    // check both are roots, we only merge nodes that are reps of their upTree
    if (!this->is_root || !other_node->is_root){
        return MY_FAILURE;
    }

    // merge data (actual object) of other node in to this node
    *(this->data) += *(other_node->getData());

    // update size of this (UpTreeNode)
    this->size += other_node->size;

    // update other_node father to this (other node is no longer root)
    return other_node->setFather(this);
}

template<typename data_t>
bool UpTreeNode<data_t>::operator<(UpTreeNode<data_t> node2){
    if(size < node2.size){
        return true;
    }
    return false;
}


#endif //WET2_UP_TREE_NODE_H
