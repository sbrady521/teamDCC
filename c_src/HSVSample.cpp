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
#include "HSVSample.h"
#include "Histogram.h"

// constructor for sample
Sample::Sample() {
}

Sample::~Sample() {
}

void Sample::createHistogram() {
    Histogram<float> histogram = Histogram<float>(this->hsv_hue_vals_);
    this->histogram_ = histogram;
}

void Sample::showChromaVals() {
    for (std::deque<float>::iterator it = this->hsv_hue_vals_.begin();
         it != this->hsv_hue_vals_.end(); it++) {
        std::cout << *it << ' ';
    }
    std::cout << std::endl;
}

void Sample::showHistogram() {
    this->histogram_.showHistogram();
}

void Sample::sampleImage(const std::string &path) {
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

    // Loop through each pixel and calculate g_chroma value
    for (int y_val = n_rows*0.65; y_val < n_rows; y_val++) {
        for (int x_val = 0; x_val < n_cols; x_val++) {
            hue_val = img.at<cv::Vec3b>(y_val, x_val)[0];

            // If the sample vector has exceeded its max size remove the oldest datapoint
            if (hsv_hue_vals_.size() > MAX_SAMPLE_SIZE) {
                this->hsv_hue_vals_.pop_front();
            }

            // append g_chroma value to vector for future analysis
            this->hsv_hue_vals_.push_back(hue_val*255);
        }
    }

    img.release();
}

void Sample::classifyImage(std::string path, std::string out_path) {
    float minRange; float maxRange;
    this->histogram_.getPeakRange(GREEN_DENSITY_THRESHOLD, minRange, maxRange);

    // make the range more generous
    cv::Mat img = cv::imread(path, cv::IMREAD_COLOR);
    if (!img.data) {
        std::cerr << "Exception at classifyImage for file,'" << path << "'" << std::endl;
        throw no_img_data("No Image Data...");
    }

    int n_rows = img.rows;
    int n_cols = img.cols;

    cv::cvtColor(img, img, cv::COLOR_BGR2HSV);

    cv::Mat new_img(n_rows, n_cols, CV_8UC3, cv::Scalar(0,0,0));

    std::cout << minRange << " " << maxRange << std::endl;

    for (int y_val = 0; y_val < n_rows; y_val++) {
        for (int x_val = 0; x_val < n_cols; x_val++) {
            hue_val = img.at<cv::Vec3b>(y_val, x_val)[0];

            if (hue_val >= minRange && hue_val <= maxRange) {
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

void Sample::writeChromaVals(std::string path) {
    std::ofstream out;
    out.open(path.c_str());
    for (std::deque<float>::iterator it = this->hsv_hue_vals_.begin();
         it != this->hsv_hue_vals_.end(); it++) {
        out << *it << ' ';
    }
    out << std::endl;
    out.close();
}