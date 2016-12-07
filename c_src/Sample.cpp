//
// Created by red on 06-Dec-16.
//
#include <iostream>
#include <vector>
#include <cmath>
#include <opencv2/opencv.hpp>
#include "Sample.h"
#include "Histogram.h"

// constructor for sample
Sample::Sample() {
}

Sample::~Sample() {
    std::cerr << "destructing sample..." << std::endl;
}

void Sample::createHistogram() {
    Histogram<int> *histogram = new Histogram<int>(this->green_chroma_vals_);
    this->histogram_ = histogram;
}

void Sample::showChromaVals() {
    for (std::vector<int>::iterator it = this->green_chroma_vals_.begin();
            it != this->green_chroma_vals_.end(); it++) {
        std::cout << *it << ' ';
    }
    std::cout << std::endl;
}

void Sample::showHistogram() {
    this->histogram_->showHistogram();
}

void Sample::SampleImage(const std::string &path) {
    // Load image using OpenCV
    cv::Mat img = cv::imread(path, cv::IMREAD_COLOR);

    if (!img.data) throw no_img_data("No Image Data...");

    // Get im dimensions
    int n_rows = img.rows;
    int n_cols = img.cols;

    // Loop through each pixel and calculate g_chroma value
    for (int y_val = n_rows*0.625; y_val < n_rows; y_val++) {
        for (int x_val = 0; x_val < n_cols; x_val++) {
            float rgb_sum = 0;

            for (int i = 0; i < 3; i++) {
                rgb_sum += img.at<cv::Vec3b>(y_val, x_val)[i];
            }

            if (rgb_sum == 0) continue;

            float g_chroma = img.at<cv::Vec3b>(y_val, x_val)[1]/rgb_sum;

            if (g_chroma <= 0.2) continue;

            // append g_chroma value to vector for future analysis
            this->green_chroma_vals_.push_back(static_cast<int>(g_chroma*255));
        }
    }

    img.release();
}

void Sample::classifyImage(std::string path, std::string out_path) {
    std::cout << "Classifying image" << std::endl;
    int minRange; int maxRange;
    this->histogram_->getPeakRange(0.035, minRange, maxRange);

    // make the range more generous
    minRange = minRange - 7; maxRange = maxRange + 100;

    cv::Mat img = cv::imread(path, cv::IMREAD_COLOR);
    if (!img.data) throw no_img_data("No Image Data...");

    // TODO sample this image too (ie sample top image)

    // avoid white values / clearly not green values
    if (minRange < 87) minRange = 87;

    int n_rows = img.rows;
    int n_cols = img.cols;

    cv::Mat new_img(n_rows, n_cols, CV_8UC3, cv::Scalar(0,0,0));

    std::cout << minRange << " " << maxRange << std::endl;

    for (int y_val = n_rows; y_val < n_rows; y_val++) {
        for (int x_val = 0; x_val < n_cols; x_val++) {
            float rgb_sum = 0;

            for (int i = 0; i < 3; i++) {
                rgb_sum += img.at<cv::Vec3b>(y_val, x_val)[i];
            }

            if (rgb_sum == 0) continue;

            float g_chroma = img.at<cv::Vec3b>(y_val, x_val)[1]/rgb_sum;

            if (g_chroma >= minRange && g_chroma <= maxRange) {
                new_img.at<cv::Vec3b>(y_val, x_val)[0] = 255;
                new_img.at<cv::Vec3b>(y_val, x_val)[1] = 112;
                new_img.at<cv::Vec3b>(y_val, x_val)[2] = 132;
            }
        }
    }

    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);


    imwrite(out_path, new_img, compression_params);
}