//
// Created by red on 16-Jan-17.
//

#ifndef CHROMATICITY_POLYNOMIAL_H
#define CHROMATICITY_POLYNOMIAL_H

#include <Eigen/Dense>
#include <vector>
#include <cmath>
#include <iostream>
#include <stdexcept>

// Wrapper Class for VectorXf
class Polynomial1V {
private:
    int degree_; // The Degree of the Polynomial
    Eigen::VectorXf polynomial_; // The Coordinate Vector of the Polynomial

public:
    inline Polynomial1V(Eigen::VectorXf v);

    // Print the Coordinate Vector of the Polynomial
    inline void showPolynomial();

    // Evaluate the polynomial at some x value
    inline double eval(double x);

    // Integrate the Polynomial
    inline Polynomial1V integrate();

    // Find the maximum of area under the curve between x_start, x_end,
    // where the area is a window under the curve range units long
    // Assigns the starting x_value and ending x_value to min and max
    // Areas below the curve are counted as negative areas.
    inline void maxAreaWindow(double x_start, double x_end, double window_range, double &min, double &max);
};

template <typename T>
Polynomial1V polyFit(std::vector<T> x_data, std::vector<T> y_data, int degree);

#include "Polynomial.tcc"

#endif //CHROMATICITY_POLYNOMIAL_H
