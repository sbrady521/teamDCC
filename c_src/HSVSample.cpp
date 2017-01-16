//
// Created by red on 18-Dec-16.
//

#include <iostream>
#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "Sample.h"
#include "HSVSample.h"
#include "Histogram.h"
#include "Polynomial.h"

// constructor for sample
HSVSample::HSVSample() {
}

HSVSample::~HSVSample() {
}

void HSVSample::createHistogram() {
    Histogram<float> hue_histogram = Histogram<float>(this->hsv_hue_vals_);
    Histogram<float> sat_histogram = Histogram<float>(this->hsv_sat_vals_);
    this->hue_histogram_ = hue_histogram;
    this->sat_histogram_ = sat_histogram;
}

void HSVSample::showChromaVals() {
    for (std::deque<float>::iterator it = this->hsv_sat_vals_.begin();
         it != this->hsv_sat_vals_.end(); it++) {
        std::cout << *it << ' ';
    }
    std::cout << std::endl;
}

void HSVSample::showSatHistogram() {
    std::cout << "Showing Saturation Histogram" << std::endl;
    this->sat_histogram_.showHistogram();
}

void HSVSample::showHueHistogram() {
    std::cout << "Showing Hue Histogram" << std::endl;
    this->hue_histogram_.showHistogram();
}

void HSVSample::sampleImage(const std::string &path) {
    // Load image using OpenCV
    cv::Mat img = cv::imread(path, cv::IMREAD_COLOR);

    if (!img.data) {
        std::cerr << "Exception at classifyImage for file,'" << path << "'" << std::endl;
        throw no_img_data("No Image Data...");
    }

    // Get im dimensions
    int n_rows = img.rows;
    int n_cols = img.cols;

    cv::cvtColor(img, img, cv::COLOR_BGR2HSV);

    // Loop through each pixel and calculate hue value
    for (int y_val = 0; y_val < n_rows; y_val++) {
        for (int x_val = 0; x_val < n_cols; x_val++) {
            int hue_val = img.at<cv::Vec3b>(y_val, x_val)[0];
            int sat_val = img.at<cv::Vec3b>(y_val, x_val)[1];

            // If the sample vector has exceeded its max size remove the oldest datapoint
            if (hsv_hue_vals_.size() > MAX_SAMPLE_SIZE) {
                this->hsv_hue_vals_.pop_front();
                this->hsv_sat_vals_.pop_front();
            }
            // append g_chroma value to vector for future analysis
            this->hsv_hue_vals_.push_back(hue_val);
            this->hsv_sat_vals_.push_back(sat_val);
        }
    }

    img.release();
}

void HSVSample::classifyImage(std::string path, std::string out_path) {
    double minRange; double maxRange;
    //this->hue_histogram_.getPeakRange(HSV_GREEN_DENSITY_THRESHOLD, minRange, maxRange);
    Polynomial1V model = this->hue_histogram_.fitPolynomial(2);
    model.maxAreaWindow(this->hue_histogram_.getMinBin(), \
                        this->hue_histogram_.getMaxBin(), 20, minRange, maxRange );

    float minSatRange; float maxSatRange;
    this->sat_histogram_.getPeakRange(HSV_SAT_GREEN_DENSITY_THRESHOLD, minSatRange, maxSatRange);

    cv::Mat img = cv::imread(path, cv::IMREAD_COLOR);
    if (!img.data) {
        std::cerr << "Exception at classifyImage for file,'" << path << "'" << std::endl;
        throw no_img_data("No Image Data...");
    }

    int n_rows = img.rows;
    int n_cols = img.cols;

    cv::cvtColor(img, img, cv::COLOR_BGR2HSV);

    cv::Mat new_img(n_rows, n_cols, CV_8UC3, cv::Scalar(0,0,0));

    // Feathering
    std::cout << "Hue Range " << minRange << " " << maxRange << std::endl;
    std::cout << "Hue Vector ";
    model.showPolynomial() ;
    std::cout << "Mininum Sat " << minSatRange << std::endl;

    for (int y_val = 0; y_val < n_rows; y_val++) {
        for (int x_val = 0; x_val < n_cols; x_val++) {
            int hue_val = img.at<cv::Vec3b>(y_val, x_val)[0];
            int sat_val = img.at<cv::Vec3b>(y_val, x_val)[1];

            if (hue_val >= minRange && hue_val <= maxRange && sat_val > minSatRange) {
                new_img.at<cv::Vec3b>(y_val, x_val)[0] = 255;
                new_img.at<cv::Vec3b>(y_val, x_val)[1] = 112;
                new_img.at<cv::Vec3b>(y_val, x_val)[2] = 132;
            }
        }
    }

    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);


    bool status = imwrite(out_path, new_img, compression_params);
    if (!status) std::cerr << "Failed to write image '" << out_path << "'" << std::endl;
}

void HSVSample::writeHueVals(std::string path) {
    std::ofstream out;
    out.open(path.c_str());
    for (std::deque<float>::iterator it = this->hsv_hue_vals_.begin();
         it != this->hsv_hue_vals_.end(); it++) {
        out << *it << ' ';
    }
    out << std::endl;
    out.close();
}