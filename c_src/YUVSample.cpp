//
// Created by red on 19-Jan-17.
//

#include <iostream>
#include <vector>
#include <deque>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "Sample.h"
#include "YUVSample.h"
#include "Histogram.h"
#include "Polynomial.h"

// constructor for sample
YUVSample::YUVSample() {
}

YUVSample::~YUVSample() {
}

void YUVSample::createHistogram() {
    Histogram<float> u_histogram = Histogram<float>(this->u_vals_);
    Histogram<float> v_histogram = Histogram<float>(this->v_vals_);
    this->u_histogram_ = u_histogram;
    this->v_histogram_ = v_histogram;
}

void YUVSample::showUVals() {
    for (std::deque<float>::iterator it = this->u_vals_.begin();
         it != this->u_vals_.end(); it++) {
        std::cout << *it << ' ';
    }
    std::cout << std::endl;
}

void YUVSample::showUHistogram() {
    std::cout << "Showing U Histogram" << std::endl;
    this->u_histogram_.showHistogram();
}

void YUVSample::showVHistogram() {
    std::cout << "Showing V Histogram" << std::endl;
    this->v_histogram_.showHistogram();
}

void YUVSample::sampleImage(const std::string &path) {
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
    for (int y_val = n_rows*0.65; y_val < n_rows; y_val++) {
        for (int x_val = 0; x_val < n_cols; x_val++) {
            int u_val = img.at<cv::Vec3b>(y_val, x_val)[1];
            int v_val = img.at<cv::Vec3b>(y_val, x_val)[2];

            // If the sample vector has exceeded its max size remove the oldest datapoint
            if (u_vals_.size() > MAX_SAMPLE_SIZE) {
                this->u_vals_.pop_front();
                this->v_vals_.pop_front();
            }
            // append g_chroma value to vector for future analysis
            this->u_vals_.push_back(u_val);
            this->v_vals_.push_back(v_val);
        }
    }

    img.release();
}

void YUVSample::classifyImage(std::string path, std::string out_path) {
    //float minRange; float maxRange;
    //this->hue_histogram_.getPeakRange(HSV_GREEN_DENSITY_THRESHOLD, minRange, maxRange);

    //float minSatRange; float maxSatRange;
    //this->sat_histogram_.getPeakRange(HSV_SAT_GREEN_DENSITY_THRESHOLD, minSatRange, maxSatRange);

    //Polynomial Fitting

    double minURange; double maxURange;
    Polynomial1V u_model = this->u_histogram_.fitPolynomial(3);
    u_model.maxAreaWindow(this->u_histogram_.getMinBin(),
                        this->u_histogram_.getMaxBin(), 30, minURange, maxURange);

    double minVRange; double maxVRange;
    Polynomial1V v_model = this->v_histogram_.fitPolynomial(3);
    v_model.maxAreaWindow(this->v_histogram_.getMinBin(),
                           this->v_histogram_.getMaxBin(), 30, minVRange, maxVRange);

    cv::Mat img = cv::imread(path, cv::IMREAD_COLOR);
    if (!img.data) {
        std::cerr << "Exception at classifyImage for file,'" << path << "'" << std::endl;
        throw no_img_data("No Image Data...");
    }

    //std::cout << "U range: " << minURange << " " << maxURange << std::endl;
    //std::cout << "V range: " << minVRange << " " << maxVRange << std::endl;

    int n_rows = img.rows;
    int n_cols = img.cols;

    cv::cvtColor(img, img, cv::COLOR_BGR2YUV);

    cv::Mat new_img(n_rows, n_cols, CV_8UC3, cv::Scalar(0,0,0));

    for (int y_val = 0; y_val < n_rows; y_val++) {
        for (int x_val = 0; x_val < n_cols; x_val++) {
            int u_val = img.at<cv::Vec3b>(y_val, x_val)[1];
            int v_val = img.at<cv::Vec3b>(y_val, x_val)[2];
            int y_val_col = img.at<cv::Vec3b>(y_val, x_val)[0];



            if (u_val >= minURange && u_val <= maxURange && v_val >= minVRange && v_val <= maxVRange
                && y_val_col < 160 && y_val_col > 75) {
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
