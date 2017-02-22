//
// Created by Gary on 03-Feb-17.
//

#include <vector>
#include <cmath>
#include "Histogram2D.h"
#include "Types.h"

#define U_RANGE 256
#define V_RANGE 256
#define MAX_BINS 400
#define SIGNIFICANT_BIN_THRESHOLD 0.005

GreenChroma::GreenChroma() {
    this->filtered_ = false;
    this->numBins = 0;
}

void GreenChroma::createHistogram(std::vector<int> u_vals, std::vector<int> v_vals) {
    this->numBins = 0;
    // Create the histogram
    this->green_ = Histogram2D<int>(u_vals, v_vals);
    // Filter the histogram
    this->green_.filterBins(MAX_BINS, SIGNIFICANT_BIN_THRESHOLD);

    // Now modify the filtered bins so that indexes match to u&v value
    this->filtered_bins_ = std::vector<std::vector<bool> >(U_RANGE);
    for (int i = 0; i < U_RANGE; i++) {
        this->filtered_bins_.at(i) = std::vector<bool>(V_RANGE);
    }

    for (int i = 0; i < U_RANGE; i++) {
        for (int j = 0; j < V_RANGE; j++) {
            if (this->green_.isFiltered(i, j)) {
                this->filtered_bins_.at(i).at(j) = true;
                this->numBins++;
            }
        }
    }

    // Further processing on the filtered bins (testing)
    smoothPoints(this->filtered_bins_);
    removeOutliers(this->filtered_bins_);

    this->filtered_ = true;
}

bool GreenChroma::binsExist(){
    return this->filtered_;
}

bool GreenChroma::isWhite(int y_val, int u_val, int v_val) {
    // The L^1 function space has a unit circle shaped by a diamond,
    // which is similar to the uv_plane white diamond
    // We want all uv-points within a L^p=1 radius of the centre of the uv plane

    if (y_val <= this->y_expv_) return false;

    int lp1_norm = abs(u_val - 128) + abs(v_val - 128);

    return (lp1_norm < this->y_radius_);
}

void GreenChroma::smoothPoints(std::vector<std::vector<bool> > &plane) {
    // Simple smoothing - flips points horizontally or vertically between two flipped points.
    for (int i = 1; i < plane.size() - 1; i++) {
        for (int j = 1; j < plane.at(i).size() - 1; j++) {
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
                if(plane.at(i).at(j) == false){
                    this->numBins++;
                }
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
                    if(plane.at(i).at(j) == true){
                        this->numBins--;
                    }
                    plane.at(i).at(j) = false;
                }
            }
        }
    }
}

void GreenChroma::setGreen(int u, int v){
    if(this->numBins >= MAX_GREEN){
        return;
    }
    if(this->filtered_bins_.at(u).at(v) == false){
        this->numBins++;
    }
    this->filtered_bins_.at(u).at(v) = true;
}
void GreenChroma::unsetGreen(int u, int v){
    if(this->numBins < 1){
        return;
    }
    if(this->filtered_bins_.at(u).at(v) == true){
        this->numBins--;
    }
    this->filtered_bins_.at(u).at(v) = false;
}

void GreenChroma::setGreenArray(std::vector<std::vector<bool> > new_vals){
    this->filtered_bins_ = new_vals;
}

int GreenChroma::getNumBins(){
    return this->numBins;
}

void GreenChroma::fillPoints(int occurences){
    for(int i = 0 ; i < occurences ; i++){
        smoothPoints(this->filtered_bins_);
    }
}

void GreenChroma::sanityCheck(){
    removeOutliers(this->filtered_bins_);
}
