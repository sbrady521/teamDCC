#include <string>

#include "Types.h"
#include "GreenChromaClassifier.h"

void GreenChromaClassifier::fit(GreenChroma& gc, cv::Mat& top, cv::Mat& bottom) {

    int n_rows = top.rows;
    int n_cols = top.cols;

    for (int y_val = n_rows*0.5; y_val < n_rows; y_val++) {
        for (int x_val = 0; x_val < n_cols; x_val++) {
            int y_val_col = top.at<cv::Vec3b>(y_val, x_val)[0];
            int u_val = top.at<cv::Vec3b>(y_val, x_val)[1];
            int v_val = top.at<cv::Vec3b>(y_val, x_val)[2];

            // If the sample vector has exceeded its max size remove the oldest datapoint
            this->u_vals_.push_back(u_val);
            this->v_vals_.push_back(v_val);
            this->y_vals_.push_back(y_val_col);
        }
    }

    gc.createHistogram(this->u_vals_, this->v_vals_);
}

void GreenChromaClassifier::classify(GreenChroma& gc, cv::Mat& test, cv::Mat& classified) {
    int n_rows = test.rows;
    int n_cols = test.cols;

    for (int y_val = 0; y_val < n_rows; y_val++) {
        for (int x_val = 0; x_val < n_cols; x_val++) {
            int y_val_col = test.at<cv::Vec3b>(y_val, x_val)[0];
            int u_val = test.at<cv::Vec3b>(y_val, x_val)[1];
            int v_val = test.at<cv::Vec3b>(y_val, x_val)[2];

            if (gc.isFiltered(u_val, v_val)) {
                classified.at<cv::Vec3b>(y_val, x_val)[0] = 255;
                classified.at<cv::Vec3b>(y_val, x_val)[1] = 112;
                classified.at<cv::Vec3b>(y_val, x_val)[2] = 132;
            }
        }
    }
}