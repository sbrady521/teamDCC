#ifndef TYPES
#define TYPES
#define MAX_GREEN 2000

#include <vector>
#include "Histogram2D.h"

class GreenChroma {
private:
    Histogram2D<int> green_;

    void removeOutliers(std::vector<std::vector<bool> > &plane);
    void smoothPoints(std::vector<std::vector<bool> > &plane);
    int numBins;
public:
    void fillPoints(int occurences);
    std::vector<std::vector<bool> > filtered_bins_;
    bool filtered_;

    double y_expv_;
    double y_sd_;
    double y_radius_;

    GreenChroma();


    bool binsExist();
    void createHistogram(std::vector<int> u_vals, std::vector<int> v_vals);
    void setGreen(int u, int v);
    void unsetGreen(int u, int v);
    void setGreenArray(std::vector<std::vector<bool> > new_vals);
    int getNumBins();

    inline bool isFiltered(int u_val, int v_val) {
        return this->filtered_bins_.at(u_val).at(v_val);
    }

    inline bool isGreen(int y_val, int u_val, int v_val) {
        return this->filtered_bins_.at(u_val).at(v_val);
    }

    bool isWhite(int y_val, int u_val, int v_val);

    void sanityCheck();
};

#endif
