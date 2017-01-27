//
// Created by red on 25-Jan-17.
//

#ifndef CHROMATICITY_HISTOGRAM2D_H
#define CHROMATICITY_HISTOGRAM2D_H

#include <iostream>
#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>
#include <Eigen/Dense>

template <typename T>
class Histogram2D {
private:
    std::vector<double> X1_bins_;
    std::vector<double> X2_bins_;
    std::vector<std::vector<double> > density_;
    std::vector<std::vector<int> > counts_;
    // Variable X1 is the the "outer" variable in the above vectors, X2 the "inner"
    int X1_bin_num_; // Number of bins the first Variable has
    int X2_bin_num_; // Number of bins the second Variable has

    int num_data_; // Number of Data points

public:
    // constructors
    Histogram2D();

    Histogram2D(std::vector<T> &X1_values, std::vector<T> &X2_values);

    Histogram2D(std::deque<T> &X1_values, std::deque<T> &X2_values);

    Histogram2D(const Histogram2D &obj);

    Histogram2D& operator=(const Histogram2D& obj);

    // destructor
    ~Histogram2D();

    void showHistogram();

    double getDensity(T X1_val, T X2_val);
};

#include "Histogram2D.tcc"

#endif //CHROMATICITY_HISTOGRAM2D_H
