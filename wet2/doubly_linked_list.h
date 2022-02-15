#ifndef WET2_DOUBLY_LINKED_LIST_H
#define WET2_DOUBLY_LINKED_LIST_H

#include "doubly_linked_list_node.h"
#include "rank_tree.h"

template <typename data_t>
class DoublyLinkedList {
    DoublyLinkedListNode<data_t>* head;
    DoublyLinkedListNode<data_t>* tail;
    int size;
public:
    DoublyLinkedList() : size(0), head(nullptr), tail(nullptr) {}
    ~DoublyLinkedList() = default;
    ReturnValue insert(data_t* new_data);
    ReturnValue remove(DoublyLinkedListNode<data_t>* node_to_remove);
    int getSize() const{ return size; }
    ReturnValue merge_to_me(DoublyLinkedList<data_t> other_list);
    DoublyLinkedListNode<data_t>* getHead() { return head; }
    DoublyLinkedListNode<data_t>* getTail() { return tail; }
};

template <typename data_t>
ReturnValue DoublyLinkedList<data_t>::insert(data_t* new_data){
    DoublyLinkedListNode<data_t>* newNode = new DoublyLinkedListNode<data_t>(new_data);
    if(!newNode){
        return MY_ALLOCATION_ERROR;
    }
    newNode->next = head;
    if(head != nullptr){
        head->prev = newNode;
    }
    else{
        tail = newNode;
    }
    head = newNode;
    size++;
    return MY_SUCCESS;
}

template <typename data_t>
ReturnValue DoublyLinkedList<data_t>::remove(DoublyLinkedListNode<data_t>* node_to_remove){
    //we assume the node does in face exists in the list

    if(size == 0){
        return ELEMENT_DOES_NOT_EXIST;
    }
    if(size == 1){
        head = nullptr;
        tail = nullptr;
    }
    else{
        if(head == node_to_remove){
            head = node_to_remove->next;
            node_to_remove->next = nullptr;
            head->prev = nullptr;
        }
        else if(tail == node_to_remove) {
            tail = node_to_remove->prev;
            node_to_remove->prev = nullptr;
            tail->next = nullptr;
        }
        else{
            node_to_remove->prev->next = node_to_remove->next;
            node_to_remove->next->prev = node_to_remove->prev;

            node_to_remove->next = nullptr;
            node_to_remove->prev = nullptr;
        }
    }
    delete node_to_remove;
    size--;
    return MY_SUCCESS;
}

template <typename data_t>
ReturnValue DoublyLinkedList<data_t>::merge_to_me(DoublyLinkedList<data_t> other_list){
    if(other_list.size == 0){
        return MY_SUCCESS;
    }

    if(size == 0){
        head = other_list.head;
        tail = other_list.tail;
        size = other_list.size;
        return MY_SUCCESS;
    }

    tail->next = other_list.head;
    other_list.head->prev = tail;
    tail = other_list.tail;
    size += other_list.size;
    other_list.head = nullptr;
    other_list.tail = nullptr;
    other_list.size = 0;
    return MY_SUCCESS;
}

#endif //WET2_DOUBLY_LINKED_LIST_H
