//
// Created by red on 23-Feb-17.
//

#ifndef CHROMATICITY_COLORCLUSTER_H
#define CHROMATICITY_COLORCLUSTER_H

#include "Colours.h"
#include <vector>
#include <stdint.h>

class ColourCluster {
private:
    Colour colour_;
    std::vector<std::vector<bool> > points_;

    int x_range_;
    int y_range_;

public:
    ColourCluster(Colour colour, int x_size, int y_size);

    // Add a u-v value to the current cluster.
    void addPoint(int x, int y);
    inline void addColour(uint8_t y, uint8_t u, uint8_t v) {
        addPoint(u, v);
    }

    inline bool inCluster(int x, int y) {
        return this->points_.at(x).at(y);
    }

    inline bool inCluster(uint8_t y, uint8_t u, uint8_t v) {
        return inCluster(u, v);
    }
};


#endif //CHROMATICITY_COLORCLUSTER_H
