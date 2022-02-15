#ifndef WET2_RANK_TREE_ITERATOR_H
#define WET2_RANK_TREE_ITERATOR_H


#include "rank_tree_node.h"

template<typename data_t, typename rank_t>
class RankTreeIterator {
    RankTreeNode<data_t, rank_t> *node_ptr;
    friend class RankTree<data_t, rank_t>;
public:
    explicit RankTreeIterator(RankTreeNode<data_t, rank_t> *node_ptr) noexcept: node_ptr(node_ptr) {}
    ~RankTreeIterator() = default;
    bool operator==(RankTreeIterator other_iter) const { return node_ptr == other_iter.node_ptr; }
    bool operator!=(RankTreeIterator other_iter) const { return node_ptr != other_iter.node_ptr; }
    void goFather() { node_ptr = node_ptr->father; }
    void goLeft() { node_ptr = node_ptr->left; }
    void goRight() { node_ptr = node_ptr->right; }
    bool checkNullFather() { return (node_ptr->father == nullptr); }
    bool checkNullLeft() { return (node_ptr->left == nullptr); }
    bool checkNullRight() { return (node_ptr->right == nullptr); }
    data_t getData() const { return *node_ptr->data; }
    RankTreeNode<data_t, rank_t> *getPtr() { return node_ptr; }
};



#endif //WET2_RANK_TREE_ITERATOR_H
