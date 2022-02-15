#ifndef WET2_HISTOGRAM_H
#define WET2_HISTOGRAM_H

#include <stdexcept>

class Histogram {
public:
    int size;
    int* hist;
    Histogram(int size);
    ~Histogram() = default;
    void clearHistogram() const;
    void increaseElement(int index) const;
    void decreaseElement(int index) const;
    Histogram& operator+=(Histogram& other_hist);
    Histogram& operator-=(Histogram& other_hist);
    int getVal(int index);
};


#endif //WET2_HISTOGRAM_H
