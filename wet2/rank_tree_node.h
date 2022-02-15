#ifndef WET2_RANK_TREE_NODE_H
#define WET2_RANK_TREE_NODE_H

#include <algorithm>

using std::max;

template<typename data_t, typename rank_t> class RankTree;
template<typename data_t, typename rank_t> class RankTreeIterator;

template<typename data_t, typename rank_t>
class RankTreeNode {
    data_t* data;
    rank_t rank;
    int height;
    RankTreeNode* father;
    RankTreeNode* left;
    RankTreeNode* right;
    
public:
    explicit RankTreeNode(int scale): data(new data_t()), father(nullptr), left(nullptr), right(nullptr), height(0), rank(rank_t(scale)) {}
    explicit RankTreeNode(data_t* data, int scale): data(data), father(nullptr), left(nullptr), right(nullptr), height(0), rank(rank_t(scale)) {}
    ~RankTreeNode();
    RankTreeNode* getFather() { return father; }
    RankTreeNode* getLeft() { return left; }
    RankTreeNode* getRight() { return right; }
    data_t* getData() { return data; }
    rank_t getRank() { return rank; }
    bool isLeaf(); 
    bool onlyHaveLeftSon();  
    bool onlyHaveRightSon();  
    bool haveTwoSons();  
    bool isALeftSon();  
    bool isARightSon();
    void updateHeight();
    void updateRank();
    void resetRank();
    int getBF();

    static void recursiveNodeDeletion(RankTreeNode<data_t, rank_t>* node);
    friend class RankTree<data_t, rank_t>;
    friend class RankTreeIterator<data_t, rank_t>;
};

template<typename data_t, typename rank_t>
RankTreeNode<data_t, rank_t>::~RankTreeNode(){
    father = nullptr;
    left = nullptr;
    right = nullptr;
    data = nullptr;
}

template<typename data_t, typename rank_t>
bool RankTreeNode<data_t, rank_t>::isLeaf(){
    if(!right && !left){
        return true;
    }
    return false;
}

template<typename data_t, typename rank_t>
bool RankTreeNode<data_t, rank_t>::onlyHaveLeftSon(){
    if(!right && left){
        return true;
    }
    return false;
}

template<typename data_t, typename rank_t>
bool RankTreeNode<data_t, rank_t>::onlyHaveRightSon(){
    if(right && !left){
        return true;
    }
    return false;
}

template<typename data_t, typename rank_t>
bool RankTreeNode<data_t, rank_t>::haveTwoSons(){
    if(right && left){
        return true;
    }
    return false;
}

template<typename data_t, typename rank_t>
bool RankTreeNode<data_t, rank_t>::isALeftSon(){
    if(father == nullptr){
        return false;
    }
    if(!father->left){
        return false;
    }
    if(father->left->getData() == this->getData()){
        return true;
    }
    return false;
}

template<typename data_t, typename rank_t>
bool RankTreeNode<data_t, rank_t>::isARightSon(){
    if(father == nullptr){
        return false;
    }
    if(!father->right){
        return false;
    }
    if(father->right->getData() == data){
        return true;
    }
    return false;
}

template<typename data_t, typename rank_t>
void RankTreeNode<data_t, rank_t>::updateHeight() {
    int left_height = (left == nullptr) ? -1 : left->height;
    int right_height = (right == nullptr) ? -1 : right->height;

    height = 1 + max(left_height, right_height);
}

template<typename data_t, typename rank_t>
void RankTreeNode<data_t, rank_t>::updateRank() {
    resetRank();
    if (isLeaf()){
        return;
    }
    if(onlyHaveLeftSon()){
        rank += left->rank;
        return;
    }
    if(onlyHaveRightSon()){
        rank += right->rank;
        return;
    }
    if(haveTwoSons()){
        rank += left->rank;
        rank += right->rank;
        return;
    }
}

template<typename data_t, typename rank_t>
void RankTreeNode<data_t, rank_t>::resetRank() {
    rank.initializeRank(*data);
}


template<typename data_t, typename rank_t>
int RankTreeNode<data_t, rank_t>::getBF() {
    int left_height = (left == nullptr) ? -1 : left->height;
    int right_height = (right == nullptr) ? -1 : right->height;
    return (left_height - right_height);
}

template<typename data_t, typename rank_t>
void RankTreeNode<data_t, rank_t>::recursiveNodeDeletion(RankTreeNode<data_t, rank_t>* node) {
    if (node == nullptr) {
        return;
    }
    recursiveNodeDeletion(node->left);
    recursiveNodeDeletion(node->right);

    if (node->isALeftSon()) {
        node->father->left = nullptr;
    }
    else {
        if (node->isARightSon()) {
            node->father->right = nullptr;
        }
    }
    delete node;
}
#endif //WET2_RANK_TREE_NODE_H
