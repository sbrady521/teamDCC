//
// Created by red on 06-Dec-16.
//

#ifndef CHROMATICITY_SAMPLE_H
#define CHROMATICITY_SAMPLE_H

#include <vector>
#include <stdexcept>
#include "Histogram.h"

class Sample {
private:

    std::vector<int> green_chroma_vals_;
    Histogram<int> *histogram_;

public:
    // Constructor
    Sample();

    // Destructor
    ~Sample();

    // Samples an image
    void SampleImage(const std::string&path);

    // Classify an image according to this sample
    void classifyImage(std::string path, std::string out_path);

    // Show green_chroma_vals_ vector
    void showChromaVals();

    // Create / recreate a histogram according to the g_vals
    void createHistogram();

    // Print the histogram to cout
    void showHistogram();
};

class no_img_data : public std::runtime_error {
private:
    std::string what_;

public:
    explicit no_img_data(const std::string& err) :
            std::runtime_error(err), what_(err) {}

    virtual const char* what() const throw() {
        return what_.c_str();
    }

    virtual ~no_img_data() throw() {}
};

#endif //CHROMATICITY_SAMPLE_H
