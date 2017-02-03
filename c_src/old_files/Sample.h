//
// Created by red on 06-Dec-16.
//

#ifndef CHROMATICITY_SAMPLE_H
#define CHROMATICITY_SAMPLE_H

#include <vector>
#include <deque>
#include <stdexcept>
#include "Histogram.h"

#define MAX_SAMPLE_SIZE 537600
#define GREEN_DENSITY_THRESHOLD 0.003

class Sample {
private:

    std::deque<float> green_chroma_vals_;
    Histogram<float> histogram_;

public:
    // Constructor
    Sample();

    // Destructor
    ~Sample();

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
