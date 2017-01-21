//
// Created by red on 19-Jan-17.
//

#ifndef CHROMATICITY_YUVSAMPLE_H
#define CHROMATICITY_YUVSAMPLE_H


//
// Created by red on 18-Dec-16.
//

#include <deque>
#include <stdexcept>
#include "Histogram.h"
#include "Polynomial.h"

#define MAX_SAMPLE_SIZE 537600

class YUVSample {
private:
    std::deque<float> u_vals_;
    std::deque<float> v_vals_;
    Histogram<float> u_histogram_;
    Histogram<float> v_histogram_;

public:
    // Constructor
    YUVSample();

    // Destructor
    ~YUVSample();

    // Samples an image
    void sampleImage(const std::string&path);

    // Classify an image according to this sample
    void classifyImage(std::string path, std::string out_path);

    // Show green_chroma_vals_ vector
    void showUVals();

    // Create / recreate a histogram according to the g_vals
    void createHistogram();

    // Print the histogram to cout
    void showUHistogram();
    void showVHistogram();
};


#endif //CHROMATICITY_YUVSAMPLE_H
