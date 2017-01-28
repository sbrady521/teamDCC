//
// Created by red on 27-Jan-17.
//

#include "YUVSample2.h"
#include <iostream>
#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <ctime>
#include "Sample.h"
#include "Histogram.h"
#include "Histogram2D.h"
#include "Polynomial.h"

YUVSample2::YUVSample2() {}

void YUVSample2::createHistogram() {
    Histogram<float> y_histogram = Histogram<float>(this->y_vals_);
    Histogram2D<float> uv_histogram = Histogram2D<float> (this->u_vals_, this->v_vals_);
    this->uv_histogram_ = uv_histogram;
    this->y_histogram_ = y_histogram;
}

void YUVSample2::showUVHistogram() {
    std::cout << "Showing UV Histogram" << std::endl;
    this->uv_histogram_.showHistogram();
}

void YUVSample2::sampleImage(const std::string &path) {
    // Load image using OpenCV
    cv::Mat img = cv::imread(path, cv::IMREAD_COLOR);

    if (!img.data) {
        std::cerr << "Exception at classifyImage for file,'" << path << "'" << std::endl;
        throw no_img_data("No Image Data...");
    }

    // Get im dimensions
    int n_rows = img.rows;
    int n_cols = img.cols;

    cv::cvtColor(img, img, cv::COLOR_BGR2YUV);

    // Loop through each pixel and calculate hue value
    double ySum = 0;
    int pixCounter = 0;
    for (int y_val = n_rows*0.65; y_val < n_rows; y_val++) {
        for (int x_val = 0; x_val < n_cols; x_val++) {
            int u_val = img.at<cv::Vec3b>(y_val, x_val)[1];
            int v_val = img.at<cv::Vec3b>(y_val, x_val)[2];
            int y_val_col = img.at<cv::Vec3b>(y_val, x_val)[0];
            ySum += y_val_col;
            pixCounter++;

            // If the sample vector has exceeded its max size remove the oldest datapoint
            if (u_vals_.size() > MAX_SAMPLE_SIZE) {
                this->u_vals_.pop_front();
                this->v_vals_.pop_front();
                this->y_vals_.pop_front();
            }
            // append g_chroma value to vector for future analysis
            this->u_vals_.push_back(u_val);
            this->v_vals_.push_back(v_val);
            this->y_vals_.push_back(y_val_col);
        }
    }
    img.release();
}

void YUVSample2::classifyImage(std::string path, std::string out_path) {
    std::clock_t c_start = std::clock();
    this->uv_histogram_.filterBins(400);
    std::clock_t c_end = std::clock();

    std::cout << std::fixed << std::setprecision(2) << "CPU time used: "
              << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " ms\n";

    cv::Mat img = cv::imread(path, cv::IMREAD_COLOR);
    if (!img.data) {
        std::cerr << "Exception at classifyImage for file,'" << path << "'" << std::endl;
        throw no_img_data("No Image Data...");
    }

    int n_rows = img.rows;
    int n_cols = img.cols;

    cv::cvtColor(img, img, cv::COLOR_BGR2YUV);

    cv::Mat new_img(n_rows, n_cols, CV_8UC3, cv::Scalar(0,0,0));
    for (int y_val = 0; y_val < n_rows; y_val++) {
        for (int x_val = 0; x_val < n_cols; x_val++) {
            int y_val_col = img.at<cv::Vec3b>(y_val, x_val)[0];
            int u_val = img.at<cv::Vec3b>(y_val, x_val)[1];
            int v_val = img.at<cv::Vec3b>(y_val, x_val)[2];
            if (this->uv_histogram_.isFiltered(u_val, v_val)) {
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