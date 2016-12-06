//
// Created by red on 06-Dec-16.
//

#ifndef CHROMATICITY_HISTOGRAM_H
#define CHROMATICITY_HISTOGRAM_H

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

template <typename T>
class Histogram {
private:
    T *bins_;
    T *counts_;
    int size_;

public:
    Histogram();

    Histogram(std::vector<T> &values);

    void showHistogram();
};

template <typename T>
Histogram<T>::Histogram() {

}

template <typename T>
Histogram<T>::Histogram(std::vector<T> &values) {
    int data_points = values.size();

    T min = *std::min_element(values.begin(), values.end());
    T max = *std::max_element(values.begin(), values.end());
    T range = max - min;

    int bin_num = range;

    this->bins_ = new T[bin_num];
    this->counts_ = new T[bin_num];
    std::fill_n(this->counts_, bin_num, 0);

    double interval = static_cast<double>(range) /static_cast<double>(bin_num);

    T bin_ubound = min + interval;
    for (int bin_cnt = 0; bin_cnt < bin_num; bin_cnt++) {
        this->bins_[bin_cnt] = bin_ubound;
        bin_ubound += interval;
    }

    for (typename std::vector<T>::iterator it = values.begin(); it != values.end(); it++) {
        int bin_pos = (*it - min)/interval;
        counts_[bin_pos]++;
    }

    this->size_ = bin_num;
}

template <typename T>
void Histogram<T>::showHistogram() {
    for (int i = 0; i < this->size_; i++) {
        std::cout << "Bin maximum: " << this->bins_[i] << "; Bin Count: " << this->counts_[i] << std::endl;
    }
}

#endif //CHROMATICITY_HISTOGRAM_H
