#ifndef TYPES
#define TYPES
#define MAX_GREEN 2000

#include <vector>
#include "Colours.h"
#include "Histogram2D.h"
#include "ColourCluster.h"

class GreenChroma {
private:
    Histogram2D<int> green_;

    void removeOutliers(std::vector<std::vector<bool> > &plane);
    void smoothPoints(std::vector<std::vector<bool> > &plane);
    std::vector<std::pair<int, int> > regionQuery(int x, int y, int distance);
    void expandCluster(int x, int y,
                       std::vector<std::pair<int, int> >& neighbours,
                       ColourCluster& cluster,
                       std::vector<std::vector<bool> >& visited_points,
                       std::vector<ColourCluster>& cluster_list);

    int numBins;
public:
    void fillPoints(int occurences);
    std::vector<std::vector<bool> > filtered_bins_;
    bool filtered_;

    ColourCluster green_cluster_;
    ColourCluster white_cluster_;

    bool green_cluster_exists_;
    bool white_cluster_exists_;

    double y_expv_;
    double y_sd_;
    double y_radius_;

    GreenChroma();

    void createHistogram(std::vector<int> u_vals, std::vector<int> v_vals);

    void clusterBins();

    bool binsExist();
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

    inline bool isWhite(int y_val, int u_val, int v_val) {
        // The L^1 function space has a unit circle shaped by a diamond,
        // which is similar to the uv_plane white diamond
        // We want all uv-points within a L^p=1 radius of the centre of the uv plane

        if (y_val <= this->y_expv_) return false;

        int lp1_norm = abs(u_val - 128) + abs(v_val - 128);

        return (lp1_norm < this->y_radius_);
    }

    void sanityCheck();
};

#endif
