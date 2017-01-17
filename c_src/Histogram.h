//
// Created by red on 06-Dec-16.
//

#ifndef CHROMATICITY_HISTOGRAM_H
#define CHROMATICITY_HISTOGRAM_H

#include <iostream>
#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>
#include "Polynomial.h"

template <typename T>
class Histogram {
private:
    std::vector<double> bins_; //holds upper bounds for each bin
    std::vector<int> counts_; //holds the count for each bin
    std::vector<double> density_; //holds the density of each bin
    int size_; //number of bins
    int num_data_; // number of data points

public:
    // constructors
    Histogram();

    Histogram(std::vector<T> &values);

    Histogram(std::deque<T> &values);

    Histogram(const Histogram &obj);

    Histogram& operator=(const Histogram& obj);

    // destructor
    ~Histogram();

    // print histogram data to cout
    void showHistogram();

    // Get min bin
    double getMinBin();

    // Get max bin
    double getMaxBin();

    // get the bin range for the maximum peak in the histogram
    void getPeakRange(double threshold, T &minRange, T &maxRange);

    // append data to the histogram
    void appendData(std::vector<T> &values);

    // Find the expected value of the histogram
    double expectedValue();
    double E();
    
    // Find the variance of the histogram

    // Fit polynomial to histogram
    Polynomial1V fitPolynomial(int degree);
};

#include "Histogram.tcc"

#endif //CHROMATICITY_HISTOGRAM_H
