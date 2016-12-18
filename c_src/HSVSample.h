//
// Created by red on 18-Dec-16.
//

#ifndef CHROMATICITY_HSVSAMPLE_H
#define CHROMATICITY_HSVSAMPLE_H

#include <deque>
#include <stdexcept>
#include "Histogram.h"

#define MAX_SAMPLE_SIZE 537600
#define GREEN_DENSITY_THRESHOLD 0.003

class HSVSample {
private:
    std::deque<float> hsv_hue_vals_;
    Histogram<float> histogram_;

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
    void showHistogram();

    // Write green_chroma_vals_ to file
    void writeChromaVals(std::string path);
};

#endif //CHROMATICITY_HSVSAMPLE_H
