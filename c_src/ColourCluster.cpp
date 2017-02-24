//
// Created by red on 23-Feb-17.
//

#include "ColourCluster.h"
#include <stdexcept>

ColourCluster::ColourCluster() {
}

ColourCluster::ColourCluster(Colour colour, int x_size, int y_size) {
    this->colour_ = colour;
    this->num_points_ = 0;

    this->points_ = std::vector<std::vector<bool> >(x_size);
    for (int i = 0; i < x_size; i++) {
        this->points_.at(i) = std::vector<bool>(y_size);
    }

    this->x_range_ = x_size;
    this->y_range_ = y_size;
}

void ColourCluster::addPoint(int x, int y) {
    if (x >= x_range_ || y >= y_range_ || x < 0 || y < 0) {
        throw std::runtime_error("Point coordinates out of range! At ColourCluster::addPoint");
    }

    if (this->points_.at(x).at(y) == false) {
        this->points_.at(x).at(y) = true;
        this->num_points_++;
    }
}

void ColourCluster::setColour(Colour colour) {
    this->colour_ = colour;
}