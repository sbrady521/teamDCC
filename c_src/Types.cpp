//
// Created by Gary on 03-Feb-17.
//

#include <vector>
#include <cmath>
#include "Types.h"

#define U_RANGE 256
#define V_RANGE 256
#define MAX_BINS 400

#define SIGNIFICANT_BIN_THRESHOLD 0.005

#define MIN_CLUSTER_THRESHOLD 5
#define DBSCAN_EPSILON 10

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

void GreenChroma::clusterBins() {
    // Apply DBSCAN to bins

    ColourCluster curr_cluster;
    std::vector<ColourCluster> cluster_list = std::vector<ColourCluster>();
    std::vector<std::vector<bool> > visited_points = std::vector<std::vector<bool> >(this->filtered_bins_.size());

    for (int i = 0; i < this->filtered_bins_.size(); i++) {
        visited_points.at(i) = std::vector<bool>(this->filtered_bins_.at(i).size());
    }

    for (int i = 0; i <this->filtered_bins_.size(); i++) {
        for (int j = 0; j < this->filtered_bins_.at(i).size(); j++) {
            if (this->filtered_bins_.at(i).at(j) == false) continue;
            if (visited_points.at(i).at(j) == true) continue;

            visited_points.at(i).at(j) = true;

            // Get Neighbouring points
            std::vector<std::pair<int, int> > neighbours = regionQuery(i, j, DBSCAN_EPSILON);
            if (neighbours.size() < MIN_CLUSTER_THRESHOLD) continue;

            curr_cluster = ColourCluster(cUNCLASSIFIED, U_RANGE, V_RANGE);
            expandCluster(i, j, neighbours, curr_cluster, visited_points, cluster_list);
            cluster_list.push_back(curr_cluster);
        }
    }

    // Guess which colour each cluster is

    // Find the biggest cluster. That's probably green.
    // The second biggest cluster is probably white/black (hopefully)
    // Disregard other clusters for now.
    ColourCluster largest;
    ColourCluster second_largest;

    int largest_cluster_size = 0;
    int second_largest_cluster_size = 0;

    bool found_largest;
    bool found_second_largest;

    for (int i = 0; i < cluster_list.size(); i++) {
        ColourCluster curr_cluster = cluster_list.at(i);
        if (curr_cluster.size() > largest_cluster_size) {
            second_largest = largest;
            second_largest_cluster_size = largest_cluster_size;

            largest = curr_cluster;
            largest_cluster_size = curr_cluster.size();

            if (found_largest) found_second_largest = true;
            found_largest = true;
        } else if (curr_cluster.size() >  second_largest_cluster_size) {
            second_largest = curr_cluster;
            second_largest_cluster_size = curr_cluster.size();
            found_second_largest = true;
        }
    }

    if (found_largest) {
        largest.setColour(cFIELD_GREEN);
        green_cluster_ = largest;
        green_cluster_exists_ = true;
    }
    if (found_second_largest) {
        second_largest.setColour(cWHITE);
        white_cluster_ = second_largest;
        white_cluster_exists_ = true;
    }
}

void GreenChroma::expandCluster(int x, int y,
                                std::vector<std::pair<int, int> >& neighbours,
                                ColourCluster& cluster,
                                std::vector<std::vector<bool> >& visited_points,
                                std::vector<ColourCluster>& cluster_list) {
    cluster.addPoint(x, y);
    for (int i = 0; i < neighbours.size(); i++) {
       std::pair<int, int> curr_point = neighbours.at(i);
        int curr_x = curr_point.first;
        int curr_y = curr_point.second;

        if (visited_points.at(curr_x).at(curr_y) == false) {
            visited_points.at(curr_x).at(curr_y) = true;

            std::vector<std::pair<int, int> > curr_neighbours = regionQuery(curr_x, curr_y, DBSCAN_EPSILON);
            if (curr_neighbours.size() >= MIN_CLUSTER_THRESHOLD) {
                neighbours.insert(neighbours.end(), curr_neighbours.begin(), curr_neighbours.end());
            }

            bool is_clustered = false;
            for (int j = 0; j < cluster_list.size(); j++) {
                if (cluster_list.at(j).inCluster(curr_x, curr_y)) {
                    is_clustered = true;
                    break;
                }
            }

            if (!is_clustered) cluster.addPoint(curr_x, curr_y);
        }
    }
}

std::vector<std::pair<int, int> > GreenChroma::regionQuery(int x, int y, int distance) {
    std::vector<std::pair<int, int> > neighbours = std::vector<std::pair<int, int> >();

    for (int x_pos = ((x - distance) > 0) ? x - distance : 0;
         x_pos < ((x + distance) < this->filtered_bins_.size()) ? x + distance : this->filtered_bins_.size();
         x_pos++) {
        for (int y_pos = ((y - distance) > 0) ? y - distance : 0;
             y_pos < ((y + distance) < this->filtered_bins_.at(x_pos).size()) ? y + distance : this->filtered_bins_.at(x_pos).size();
             y_pos++) {
            if (this->filtered_bins_.at(x_pos).at(y_pos)) {
                neighbours.push_back(std::make_pair(x_pos, y_pos));
            }
        }
    }

    return neighbours;
}

bool GreenChroma::binsExist(){
    return this->filtered_;
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
