#include <opencv2/opencv.hpp>
#include <string>
#include <cmath>

#include "Types.h"
#include "GreenChromaClassifier.h"

#define INITIAL 1;
#define PROGRESS 2;
#define TOTALPIXELS 1228800;
#define SAMPLESIZE 61440;
#define TOPWIDTH 1280;
#define TOPHEIGHT 960;

void GreenChromaClassifier::sample(GreenChroma& gc, cv::Mat& top, cv::Mat& bottom, int context) {
    if(context == INITIAL){
        int n_rows = top.rows;
        int n_cols = top.cols;
        for (int y_pos = n_rows*0.5; y_pos < n_rows; y_pos++) {
            for (int x_pos = 0; x_pos < n_cols; x_pos++) {
                int y_val = top.at<cv::Vec3b>(y_pos, x_pos)[0];
                int u_val = top.at<cv::Vec3b>(y_pos, x_pos)[1];
                int v_val = top.at<cv::Vec3b>(y_pos, x_pos)[2];

                this->u_vals_.push_back(u_val);
                this->v_vals_.push_back(v_val);
                this->y_vals_.push_back(y_val;
            }
        }
    }else if(context == PROGRESS){
        //Ensure a valid classification exists
        

        int randX;
        int randY;
        for(int pix_counter = 0 ; pix_counter < SAMPLESIZE ; pix_counter++){
            //Generate random (x, y) coordinates to sample
            randX = rand() % TOPWIDTH;

            //randY must be higher than 200 as unlikely any green pixels lower
            randY = rand() % (TOPHEIGHT - 200);
            randY = randY + 200;

            int y_val = top.at<cv::Vec3b>(randY, randX)[0];
            int u_val = top.at<cv::Vec3b>(randY, randX)[1];
            int v_val = top.at<cv::Vec3b>(randY, randX)[2];

            bool validGreen = possiblyGreen()



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

    int white_min_y = this->y_expv_ + 3*this->y_sd_;

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
