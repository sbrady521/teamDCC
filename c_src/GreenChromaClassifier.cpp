#include <opencv2/opencv.hpp>
#include <string>

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

            this->y_sum_ += y_val;
        }
    }
}

void GreenChromaClassifier::model(GreenChroma & gc) {
    gc.createHistogram(this->u_vals_, this->v_vals_);
}

void GreenChromaClassifier::classify(GreenChroma& gc, cv::Mat& test, cv::Mat& classified) {
    int n_rows = test.rows;
    int n_cols = test.cols;

    double y_avg = this->y_sum_ / this->u_vals_.size();

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
            } else if (y_val > 180) {
                classified.at<cv::Vec3b>(y_pos, x_pos)[0] = 128;
                classified.at<cv::Vec3b>(y_pos, x_pos)[1] = 128;
                classified.at<cv::Vec3b>(y_pos, x_pos)[2] = 128;
            }
        }
    }
}