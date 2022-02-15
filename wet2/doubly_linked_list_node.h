#ifndef WET2_DOUBLY_LINKED_LIST_NODE_H
#define WET2_DOUBLY_LINKED_LIST_NODE_H


template<typename data_t> class DoublyLinkedList;
template<typename data_t> class DoublyLinkedListIterator;

template<typename data_t>
class DoublyLinkedListNode {
    data_t* data;
    DoublyLinkedListNode* next;
    DoublyLinkedListNode* prev;
public:
    DoublyLinkedListNode(data_t* data) : data(data), next(nullptr),prev(nullptr){}
    ~DoublyLinkedListNode() = default;
    DoublyLinkedListNode* getNext() { return next; }
    DoublyLinkedListNode* getPrev() { return prev; }
    data_t* getData() { return data; }
    friend class DoublyLinkedList<data_t>;
    friend class DoublyLinkedListIterator<data_t>;
};


#endif //WET2_DOUBLY_LINKED_LIST_NODE_H
