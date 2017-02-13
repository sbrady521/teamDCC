//
// Created by Gary on 03-Feb-17.
//

#include <vector>
#include <cmath>
#include "Histogram2D.h"
#include "Types.h"

#define U_RANGE 256
#define V_RANGE 256
#define MAX_BINS 600

GreenChroma::GreenChroma() {
    this->filtered_ = false;
}

void GreenChroma::createHistogram(std::vector<int> u_vals, std::vector<int> v_vals) {
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

    // Further processing on the filtered bins (testing)
    smoothPoints(this->filtered_bins_);
    removeOutliers(this->filtered_bins_);

    this->filtered_ = true;
}

bool GreenChroma::isWhite(int y_val, int u_val, int v_val) {
    // The L^1 function space has a unit circle shaped by a diamond,
    // which is similar to the uv_plane white diamond
    // We want all uv-points within a Lp-1 radius of the centre of the uv plane
    int radius = abs(this->y_expv_ - 128);
    int lp1_norm = abs(u_val) + abs(v_val);

    return (lp1_norm < radius);
}

void GreenChroma::smoothPoints(std::vector<std::vector<bool> > &plane) {
    // Simple smoothing - flips points horizontally or vertically between two flipped points.
    for (int i = 0; i < plane.size() - 1; i++) {
        if (i == 0) continue;

        for (int j = 0; j < plane.at(i).size() - 1; j++) {
            if (j == 0) continue;

            if (
                    (
                !plane.at(i).at(j)    &&
                plane.at(i - 1).at(j) &&
                plane.at(i + 1).at(j)
                    )
                                      ||
                    (
                !plane.at(i).at(j)    &&
                plane.at(i).at(j - 1) &&
                plane.at(i).at(j + 1)
                    )
                                      ||
                    (
                !plane.at(i).at(j)    &&
                plane.at(i - 1).at(j - 1) &&
                plane.at(i + 1).at(j + 1)
                    )
                ) {
                plane.at(i).at(j) = true;
            }
        }
    }
}


void GreenChroma::removeOutliers(std::vector<std::vector<bool> > &plane) {
    // Find the sd of the euclidean inner product of all points
    double inner_product_sum = 0;
    double inner_product_squared_sum = 0;
    double inner_product_expected_value;
    double inner_product_sd;
    double point_cnt = 0;

    for (int i = 0; i < plane.size(); i++) {
        for (int j = 0; j < plane.at(i).size(); j++) {
            if (plane.at(i).at(j)) {
                double inner_product = pow(i, 2) + pow(j, 2);
                inner_product_sum += inner_product;
                inner_product_squared_sum += pow(inner_product, 2);
                point_cnt++;
            }
        }
    }
    inner_product_expected_value = inner_product_sum / point_cnt;
    inner_product_sd = sqrt((inner_product_squared_sum / point_cnt) - pow(inner_product_expected_value, 2));

    // Now calculate how many SD's away from the expected value each point is. If it is 1 or more, remove it.
    for (int i = 0; i < plane.size(); i++) {
        for (int j = 0; j < plane.at(i).size(); j++) {
            if (plane.at(i).at(j)) {
                double inner_product = pow(i, 2) + pow(j, 2);
                if (abs(inner_product - inner_product_expected_value) / inner_product_sd >= 1) {
                    plane.at(i).at(j) = false;
                }
            }
        }
    }
}
