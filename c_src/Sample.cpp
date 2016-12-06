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
    std::cout << "Constructing Sample..." << std::endl;
}

void Sample::createHistogram() {
    Histogram<int> *histogram = new Histogram<int>(this->green_chroma_vals_);

    this->histogram_ = histogram;
}

void Sample::showHistogram() {
    this->histogram_->showHistogram();
}

void Sample::SampleImage(const std::string &path) {
    std::cout << "Sampling Image..." << std::endl;
    cv::Mat img = cv::imread(path, cv::IMREAD_COLOR);

    if (!img.data) throw no_img_data("No Image Data...");

    int n_rows = img.rows;
    int n_cols = img.cols;

    for (int y_val = n_rows*0.625; y_val < n_rows; y_val++) {
        for (int x_val = 0; x_val < n_cols; x_val++) {
            float rgb_sum = 0;

            for (int i = 0; i < 3; i++) {
                rgb_sum += img.at<cv::Vec3b>(x_val, y_val)[i];
            }

            if (rgb_sum == 0) continue;

            float g_chroma = img.at<cv::Vec3b>(x_val, y_val)[1]/rgb_sum;

            if (g_chroma <= 0.2) continue;

            this->green_chroma_vals_.push_back(static_cast<int>(g_chroma*255));
        }
    }
}

void Sample::classifyImage(const std::string &path, const std::string &out_path) {

}