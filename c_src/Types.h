#ifndef TYPES
#define TYPES

#include <vector>
#include "Histogram2D.h"

class GreenChroma {
private:
    Histogram2D<int> green_;

public:
    GreenChroma();

    void createHistogram(std::vector<int> u_vals, std::vector<int> v_vals);

    bool isFiltered(int u_val, int v_val);
};

#endif
