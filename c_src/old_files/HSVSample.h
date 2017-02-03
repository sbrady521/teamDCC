//
// Created by red on 18-Dec-16.
//

#ifndef CHROMATICITY_HSVSAMPLE_H
#define CHROMATICITY_HSVSAMPLE_H

#include <deque>
#include <stdexcept>
#include "Histogram.h"
#include "Polynomial.h"

#define MAX_SAMPLE_SIZE 537600
#define HSV_GREEN_DENSITY_THRESHOLD 0.03
#define HSV_SAT_GREEN_DENSITY_THRESHOLD 0.1

class HSVSample {
private:
    std::deque<float> hsv_hue_vals_;
    std::deque<float> hsv_sat_vals_;
    Histogram<float> hue_histogram_;
    Histogram<float> sat_histogram_;

public:
    // Constructor
    HSVSample();

    // Destructor
    ~HSVSample();

    // Samples an image
    void sampleImage(const std::string&path);

    // Classify an image according to this sample
    void classifyImage(std::string path, std::string out_path);

    // Show green_chroma_vals_ vector
    void showChromaVals();

    // Create / recreate a histogram according to the g_vals
    void createHistogram();

    // Print the histogram to cout
    void showSatHistogram();
    void showHueHistogram();

    // Write green_chroma_vals_ to file
    void writeHueVals(std::string path);
};

#endif //CHROMATICITY_HSVSAMPLE_H
