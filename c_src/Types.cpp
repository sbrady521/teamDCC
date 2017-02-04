//
// Created by Gary on 03-Feb-17.
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
    this->green_ = Histogram2D<int>(u_vals, v_vals);

    // Filter the histogram
    this->green_.filterBins(MAX_BINS);

    // Now modify the filtered bins so that indexes match to u&v value
    this->filtered_bins_ = std::vector<std::vector<bool> >(U_RANGE);
    for (int i = 0; i < U_RANGE; i++) {
        this->filtered_bins_.at(i) = std::vector<bool>(V_RANGE);
    }

    for (int i = 0; i < U_RANGE; i++) {
        for (int j = 0; j < V_RANGE; j++) {
            if (this->green_.isFiltered(i, j)) {
                this->filtered_bins_.at(i).at(j) = true;
            }
        }
    }
}

bool GreenChroma::isFiltered(int u_val, int v_val) {
    return this->filtered_bins_.at(u_val).at(v_val);
    //return this->green_.isFiltered(u_val, v_val);
}
