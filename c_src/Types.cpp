//
// Created by Gary on 30/01/2017.
//

#include <vector>
#include "Histogram2D.h"
#include "Types.h"

#define U_RANGE 256
#define V_RANGE 256
#define MAX_BINS 400

GreenChroma::GreenChroma() {
}

void GreenChroma::createHistogram(std::vector<int> u_vals, std::vector<int> v_vals) {
    // Initialize the desired bin vectors
    std::vector<double> u_bin = std::vector<double>(U_RANGE);
    std::vector<double> v_bin = std::vector<double>(V_RANGE);

    // Fill each bin with the desired bin values
    for (int i = 0; i <= 255; i++) {
        u_bin.at(i) = i;
        v_bin.at(i) = i;
    }

    // Create the histogram
    this->green_ = Histogram2D<int>(u_vals, v_vals, u_bin, v_bin);

    // Filter the histogram
    this->green_.filterBins(MAX_BINS);
}

bool GreenChroma::isFiltered(int u_val, int v_val) {
    std::vector<std::vector<bool> > filtered_bins = this->green_.getFilteredBins();
    return filtered_bins.at(u_val).at(v_val);
}