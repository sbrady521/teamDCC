#include <opencv2/opencv.hpp>
#include <string>
#include <cmath>

#include "Types.h"
#include "GreenChromaClassifier.h"

void GreenChromaClassifier::sample(GreenChroma& gc, cv::Mat& top, cv::Mat& bottom) {
    int n_rows = top.rows;
    int n_cols = top.cols;

    for (int y_pos = n_rows*0.5; y_pos < n_rows; y_pos++) {
        for (int x_pos = 0; x_pos < n_cols; x_pos++) {
            int y_val = top.at<cv::Vec3b>(y_pos, x_pos)[0];
            int u_val = top.at<cv::Vec3b>(y_pos, x_pos)[1];
            int v_val = top.at<cv::Vec3b>(y_pos, x_pos)[2];

            this->y_vals_.push_back(y_val);
            this->u_vals_.push_back(u_val);
            this->v_vals_.push_back(v_val);
        }
    }
}

void GreenChromaClassifier::model(GreenChroma & gc) {
    gc.createHistogram(this->u_vals_, this->v_vals_);

    // Calculate the expected value and SD of the y_values
    double y_sum, y_sum_squared;
    for (int i = 0; i < this->y_vals_.size(); i++) {
        y_sum += this->y_vals_.at(i);
        y_sum_squared += pow(this->y_vals_.at(i), 2);
    }

    this->y_expv_ = y_sum / static_cast<double>(this->y_vals_.size());
    this->y_sd_ = sqrt((y_sum_squared / static_cast<double>(this->y_vals_.size())) - pow(this->y_expv_, 2));
}

void GreenChromaClassifier::classify(GreenChroma& gc, cv::Mat& test, cv::Mat& classified) {
    int n_rows = test.rows;
    int n_cols = test.cols;

    int white_min_y = this->y_expv_ + 2*this->y_sd_;

    for (int y_pos = 0; y_pos < n_rows; y_pos++) {
        for (int x_pos = 0; x_pos < n_cols; x_pos++) {
            int y_val = test.at<cv::Vec3b>(y_pos, x_pos)[0];
            int u_val = test.at<cv::Vec3b>(y_pos, x_pos)[1];
            int v_val = test.at<cv::Vec3b>(y_pos, x_pos)[2];

            //std::cerr << u_val << " " << v_val << std::endl;

            if (gc.isFiltered(u_val, v_val)) {
                classified.at<cv::Vec3b>(y_pos, x_pos)[0] = 0;
                classified.at<cv::Vec3b>(y_pos, x_pos)[1] = 255;
                classified.at<cv::Vec3b>(y_pos, x_pos)[2] = 0;
            } else if (y_val >= white_min_y) {
                classified.at<cv::Vec3b>(y_pos, x_pos)[0] = 128;
                classified.at<cv::Vec3b>(y_pos, x_pos)[1] = 128;
                classified.at<cv::Vec3b>(y_pos, x_pos)[2] = 128;
            }
        }
    }
}