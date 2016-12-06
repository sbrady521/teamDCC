//
// Created by red on 06-Dec-16.
//

#ifndef CHROMATICITY_SAMPLE_H
#define CHROMATICITY_SAMPLE_H

#include <vector>
#include "Histogram.h"

class Sample {
private:
    std::vector<int> green_chroma_vals_;

    void getChromaticityRange(int &min, int &max);

    Histogram<int> createHistogram();

public:
    // Constructor
    Sample();

    // Samples an image
    void SampleImage(const std::string&path);

    // Classify an image according to this sample
    void classifyImage(const std::string&path, const std::string&out_path);
};

#endif //CHROMATICITY_SAMPLE_H
