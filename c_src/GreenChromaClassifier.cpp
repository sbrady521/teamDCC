#include <opencv2/opencv.hpp>
#include <string>

#include "Types.h"
#include "GreenChromaClassifier.h"

void GreenChromaClassifier::sample(GreenChroma& gc, cv::Mat& top, cv::Mat& bottom) {
    int n_rows = top.rows;
    int n_cols = top.cols;

    for (int y_val = n_rows*0.5; y_val < n_rows; y_val++) {
        for (int x_val = 0; x_val < n_cols; x_val++) {
            int y_val_col = top.at<cv::Vec3b>(y_val, x_val)[0];
            int u_val = top.at<cv::Vec3b>(y_val, x_val)[1];
            int v_val = top.at<cv::Vec3b>(y_val, x_val)[2];

            this->u_vals_.push_back(u_val);
            this->v_vals_.push_back(v_val);
            this->y_vals_.push_back(y_val_col);
        }
    }
}

void GreenChromaClassifier::model(GreenChroma & gc) {
    gc.createHistogram(this->u_vals_, this->v_vals_);
}

void GreenChromaClassifier::classify(GreenChroma& gc, cv::Mat& test, cv::Mat& classified) {
    int n_rows = test.rows;
    int n_cols = test.cols;

    for (int y_val = 0; y_val < n_rows; y_val++) {
        for (int x_val = 0; x_val < n_cols; x_val++) {
            int u_val = test.at<cv::Vec3b>(y_val, x_val)[1];
            int v_val = test.at<cv::Vec3b>(y_val, x_val)[2];

            //std::cerr << u_val << " " << v_val << std::endl;

            if (gc.isFiltered(u_val, v_val)) {
                classified.at<cv::Vec3b>(y_val, x_val)[0] = 0;
                classified.at<cv::Vec3b>(y_val, x_val)[1] = 0;
                classified.at<cv::Vec3b>(y_val, x_val)[2] = 0;
            }
        }
    }
}



//Methods for progressive sampling using a 2D array value histogram
void GreenChromaClassifier::initialSample(GreenChroma& gc, cv::Mat& top, cv::Mat& bottom) {
    int n_rows = top.rows;
    int n_cols = top.cols;

    for (int y_pos = n_rows*0.65; y_pos < n_rows; y_pos++) {
        for (int x_pos = 0; x_pos < n_cols; x_pos++) {
            int y_val = top.at<cv::Vec3b>(y_pos, x_pos)[0]
            int u_val = top.at<cv::Vec3b>(y_pos, x_pos)[1];
            int v_val = top.at<cv::Vec3b>(y_pos, x_pos)[2];

            this->u_vals_.push_back(u_val);
            this->v_vals_.push_back(v_val);
            this->y_vals_.push_back(y_val);
        }
    }
}

//Takes a samples based on previous sample data
void GreenChromaClassifier::progressSample(GreenChroma& gc, cv::Mat& top, cv::Mat& bottom){
  int n_rows = top.rows;
  int n_cols = top.cols;

  //Making my own 2D array for now
  //Replace with built in 2D array TODO
  bool[][] filteredUV = new bool[255][255];
  for(int u = 0 ; u < 256 ; u++){
    for(int v = 0 ; v < 256 ; v++){
      filteredUV[u][v] = gc.isFiltered(u, v);
  }

  for (int y_pos = n_rows*0.25; y_pos < n_rows; y_pos++) {
      for (int x_pos = 0; x_pos < n_cols; x_pos++) {
          int y_val = top.at<cv::Vec3b>(y_pos, x_pos)[0]
          int u_val = top.at<cv::Vec3b>(y_pos, x_pos)[1];
          int v_val = top.at<cv::Vec3b>(y_pos, x_pos)[2];

          //Searching for neighbours within 1 U/V value
          int final within = 1;

          if(closeTo(filteredUV, u_val, v_val, within)){
              this->u_vals_.push_back(u_val);
              this->v_vals_.push_back(v_val);
              this->y_vals_.push_back(y_val);
          }
      }
  }
}

bool GreenChromaClassifier::isEmpty(){
    return this->u_vals_.empty() && this->v_vals_.empty() && this->y_vals_.empty();
}

//Checks if a UV value or any UV value within a certain range is defined as green
bool GreenChromaClassifier::closeTo(bool[][] filteredUV, int u, int v, int within){
    for(int u_pos = u - within ; int u_pos < u + within ; u_pos++){
        for(int v_pos = v - within ; int v_pos < v + within ; v_pos++){
            if(filteredUV[u_pos][v_pos]){
                return true;
            }
        }
    }
    return false;
}
