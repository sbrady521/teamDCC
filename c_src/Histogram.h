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
    T *bins_; //holds upper bounds for each bin
    T *counts_; //holds the count for each bin
    double *density_; //holds the density of each bin
    int size_; //number of bins

public:
    //constructors
    Histogram();

    Histogram(std::vector<T> &values);

    // destructor
    ~Histogram();

    // print histogram data to cout
    void showHistogram();

    // get the bin range for the maximum peak in the histogram
    void getPeakRange(T &min, T &max);
};

template <typename T>
Histogram<T>::Histogram() {

}

template <typename T>
Histogram<T>::Histogram(std::vector<T> &values) {
    // get the number of data points (length of values)
    int data_points = values.size();

    // get the range of the values
    T min = *std::min_element(values.begin(), values.end());
    T max = *std::max_element(values.begin(), values.end());
    T range = max - min;

    // The number of bins will be equal to the floor of the range (for now)
    int bin_num = range;

    // TODO: check for bad_alloc
    this->bins_ = new T[bin_num];
    this->counts_ = new T[bin_num];
    this->density_ = new double[bin_num];

    // fill arrays with zeroes
    std::fill_n(this->counts_, bin_num, 0);
    std::fill_n(this->density_, bin_num, 0.0);

    // calculate the interval between each bin
    double interval = static_cast<double>(range) /static_cast<double>(bin_num);

    // iterate through the bin intervals. Set each element of bins_
    // to be the upper bound of the bin.
    // For example if you have 1,2,3,4,5; 5 bins of interval 1
    // bins_[0] = 2; bins_[1] = 3; ... bins_[4] = 6;
    T bin_ubound = min + interval;
    for (int bin_cnt = 0; bin_cnt < bin_num; bin_cnt++) {
        this->bins_[bin_cnt] = bin_ubound;
        bin_ubound += interval;
    }

    // calculate how much each data point contributes to the density
    double density_incr = 1.0/static_cast<double>(data_points);

    // iterate through data points and increment counts_ and density_ appropriately
    for (typename std::vector<T>::iterator it = values.begin(); it != values.end(); it++) {
        int bin_pos = (*it - min)/interval;
        counts_[bin_pos]++;
        density_[bin_pos] += density_incr;
    }

    // Set size
    this->size_ = bin_num;
}

template <typename T>
Histogram<T>::~Histogram() {
    delete[] this->bins_; delete[] this->counts_; delete[] this->density_;
}

template <typename T>
void Histogram<T>::showHistogram() {
    for (int i = 0; i < this->size_; i++) {
        std::cout << "Bin maximum: " << this->bins_[i] << "; Bin Density: " << this->density_[i] << std::endl;
    }
}

template <typename T>
void Histogram<T>::getPeakRange(T &min, T &max) {

}

#endif //CHROMATICITY_HISTOGRAM_H
