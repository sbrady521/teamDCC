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
    std::vector<T> bins_; //holds upper bounds for each bin
    std::vector<int> counts_; //holds the count for each bin
    std::vector<double> density_; //holds the density of each bin
    int size_; //number of bins

public:
    // constructors
    Histogram();

    Histogram(std::vector<T> &values);

    Histogram(const Histogram &obj);

    Histogram& operator=(const Histogram& obj);

    // destructor
    ~Histogram();

    // print histogram data to cout
    void showHistogram();

    // get the bin range for the maximum peak in the histogram
    void getPeakRange(double threshold, T &minRange, T &maxRange);
};

#include "Histogram.tcc"

#endif //CHROMATICITY_HISTOGRAM_H
