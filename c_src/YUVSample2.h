//
// Created by red on 27-Jan-17.
//

#ifndef CHROMATICITY_YUVSAMPLE2_H
#define CHROMATICITY_YUVSAMPLE2_H

/*
 * Alternative YUV Sampling Method Test
 */

#include <deque>
#include <stdexcept>
#include "Histogram.h"
#include "Histogram2D.h"
#include "Polynomial.h"

#define MAX_SAMPLE_SIZE 537600

class YUVSample2 {
private:
    std::deque<float> u_vals_;
    std::deque<float> v_vals_;
    std::deque<float> y_vals_;
    Histogram<float> y_histogram_;
    Histogram2D<float> uv_histogram_;

public:
    YUVSample2();

    // Samples an image
    void sampleImage(const std::string&path);

    // Classify an image according to this sample
    void classifyImage(std::string path, std::string out_path);

    // Create / recreate a histogram according to the g_vals
    void createHistogram();

    // Print the histogram to cout
    void showUVHistogram();
};


#endif //CHROMATICITY_YUVSAMPLE2_H
