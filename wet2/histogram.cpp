#include "histogram.h"


Histogram::Histogram(int new_size){
    if (new_size <= 0){
        throw std::exception();
    }
    size = new_size;
    hist = new int[size];
    if (!hist){
        throw std::bad_alloc();
    }
    clearHistogram();
}

Histogram& Histogram::operator+=(Histogram& other_hist){
    if (other_hist.hist == nullptr){
        throw std::exception();
    }
    for (int i = 0; i < size; i++){
        hist[i] += other_hist.hist[i];
    }
    return *this;
}
Histogram& Histogram::operator-=(Histogram& other_hist){
    if (other_hist.hist == nullptr){
        throw std::exception();
    }
    for (int i = 0; i < size; i++){
        hist[i] -= other_hist.hist[i];
    }
    return *this;
}

void Histogram::clearHistogram() const {
    for (int i = 0; i < size; i++){
        hist[i] = 0;
    }
}

void Histogram::increaseElement(int index) const {
    if (index < 0 || index >= size){
        throw std::exception();
    }
    hist[index]++;
}

void Histogram::decreaseElement(int index) const {
    if (index < 0 || index >= size){
        throw std::exception();
    }
    (hist[index])--;
}

int Histogram::getVal(int index) {
    if (index >= 0 && index < size) {
        return hist[index];
    }

    return 0; // if index is out of bounds, returning 0 will not affect the overall result
}
