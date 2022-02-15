#ifndef WET2_DYNAMIC_ARRAY_H
#define WET2_DYNAMIC_ARRAY_H

#define ARRAY_START_SIZE    4
#define UP_SCALE            2
#define DOWN_SCALE          0.5
#define NO_SCALE            1
#define MINIMUM_PROPORTION  0.25

#include <cstdlib>
#include <stdexcept>
#include <cmath>
#include "rank_tree.h" // only here because of enum ReturnValue, need to figure out where to place it


typedef enum {FREED = -1, EMPTY, TAKEN} IndexStatus;

template <class data_t>
class DynamicHashTable {
private:
    int max_size;
    int curr_size;
    data_t* array_start_ptr;
    int* graveyard;
    void rescale(double scale);
    int hashBase(data_t* data);
    int hashStep(data_t* data);
    int hash(data_t* data, int k);
    bool isEmptyForInsert(int index);
    void initializeArrays();
    void updateFields(DynamicHashTable* hash_to_copy);

public:
    explicit DynamicHashTable(int size = ARRAY_START_SIZE);
    ~DynamicHashTable();

    ReturnValue insertData(data_t data);
    ReturnValue removeData(data_t data);
    int findIndex(data_t data);
    data_t* getDataPtr(data_t data);
    void clearTable();
    void mergeToMe(DynamicHashTable<data_t>* other_hash_table);
};




template<class data_t>
DynamicHashTable<data_t>::DynamicHashTable(int size) :  max_size(size), curr_size(0) {
    array_start_ptr = new data_t[size];
    if(!array_start_ptr){
        throw std::bad_alloc();
    }
    graveyard = new int[size];
    if(!graveyard){
        throw std::bad_alloc();
    }
    initializeArrays();
}

template<class data_t>
DynamicHashTable<data_t>::~DynamicHashTable() {
    delete[] array_start_ptr;
    delete[] graveyard;
}

template<class data_t>
ReturnValue DynamicHashTable<data_t>::insertData(data_t data) {
    int index;
    int k = 0;
    if (findIndex(data) != -1) {
        return MY_FAILURE;
    }

    while (true) {
        index = hash(&data, k);
        if (isEmptyForInsert(index)) {
            break;
        }
        k++;
    }
    array_start_ptr[index] = data;
    curr_size++;
    graveyard[index] = TAKEN;

    if (curr_size == (max_size/2)) {
        rescale(UP_SCALE);
    }
    return MY_SUCCESS;
}

template<class data_t>
ReturnValue DynamicHashTable<data_t>::removeData(data_t data) {
    int index = findIndex(data);
//    int k = 0;
    if (index == -1) {
        return MY_FAILURE;
    }

    curr_size--;
    graveyard[index] = FREED;
    array_start_ptr[index] = nullptr;


    if((curr_size <= ceil(MINIMUM_PROPORTION*max_size)) && (DOWN_SCALE*max_size >= ARRAY_START_SIZE)){
        rescale(DOWN_SCALE);
    }else if((curr_size <= ceil(MINIMUM_PROPORTION*max_size)) && (DOWN_SCALE*max_size < ARRAY_START_SIZE)){
        rescale(NO_SCALE);
    }
    return MY_SUCCESS;
}



template<class data_t>
int DynamicHashTable<data_t>::findIndex(data_t data) {
    int index;
    int k = 0;

    while(true){
        index = hash(&data, k);
        if(graveyard[index] == EMPTY){
            return -1;
        }
        if(graveyard[index] == TAKEN && data == &(array_start_ptr[index])){
            return index;
        }
        k++;
    }
}

template<class data_t>
data_t *DynamicHashTable<data_t>::getDataPtr(data_t data) {
    int index = findIndex(data);
    if (index == -1) {
        return nullptr;
    }

    return (array_start_ptr + index);
}

template<class data_t>
void DynamicHashTable<data_t>::rescale(double scale) {
    int new_size = max_size*scale;
    DynamicHashTable* new_hash = new DynamicHashTable<data_t>(new_size);
    if(!new_hash){
        throw std::bad_alloc();
    }
    for(int i = 0; i < max_size; i++) {
        if(graveyard[i] != TAKEN){
            continue;
        }
        else{
            new_hash->insertData(*(array_start_ptr + i));
        }
    }
    updateFields(new_hash);
    delete new_hash;
}

template<typename data_t>
void DynamicHashTable<data_t>::clearTable() {
    initializeArrays();
}

template<class data_t>
int DynamicHashTable<data_t>::hashBase(data_t* data) {
    return data->getKey() % max_size;
}

template<class data_t>
int DynamicHashTable<data_t>::hashStep(data_t* data) {
    return 1;
}

template<class data_t>
int DynamicHashTable<data_t>::hash(data_t* data, int k) {
    int num = k * hashStep(data);
    num += hashBase(data);
    return (num % max_size);
}

template<class data_t>
bool DynamicHashTable<data_t>::isEmptyForInsert(int index) {
    if(graveyard[index] != TAKEN){
        return true;
    }
    return false;
}

template<class data_t>
void DynamicHashTable<data_t>::initializeArrays() {
    for (int i = 0; i < max_size; ++i) {
        *(array_start_ptr + i) = nullptr;
        graveyard[i] = EMPTY;
    }
    curr_size = 0;
}

template<class data_t>
void DynamicHashTable<data_t>::updateFields(DynamicHashTable* hash_to_copy) {
    max_size = hash_to_copy->max_size;
    curr_size = hash_to_copy->curr_size;
    delete[] array_start_ptr;
    array_start_ptr = hash_to_copy->array_start_ptr;
    delete[] graveyard;
    graveyard = hash_to_copy->graveyard;
    hash_to_copy->array_start_ptr = nullptr;
    hash_to_copy->graveyard = nullptr;
}

template<class data_t>
void DynamicHashTable<data_t>::mergeToMe(DynamicHashTable<data_t>* other_hash_table) {
    if (other_hash_table == nullptr){
        return;
    }

    for (int i = 0; i < other_hash_table->max_size; i++){
        if (other_hash_table->graveyard[i] == TAKEN){
            this->insertData(other_hash_table->array_start_ptr[i]);
        }
    }
}


#endif //WET2_DYNAMIC_ARRAY_H
