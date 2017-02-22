#include <opencv2/opencv.hpp>
#include <string>
#include <cmath>

#include "Types.h"
#include "GreenChromaClassifier.h"

#define INITIAL 1
#define PROGRESS 2
#define PLAYING 3
#define BASESAMPLESIZE 10000
#define TOPWIDTH 1280
#define TOPHEIGHT 960
#define U_RANGE 256
#define V_RANGE 256

void GreenChromaClassifier::sample(GreenChroma& gc, cv::Mat& top, cv::Mat& bottom, int context) {
    if(gc.binsExist() == false && context == PROGRESS){
        std::cerr << "unable to take progress sample as no initial sample exits" << std::endl;
        context = INITIAL;
    }
    if(context == INITIAL){
        int n_rows = top.rows;
        int n_cols = top.cols;
        for (int y_pos = n_rows*0.70; y_pos < n_rows; y_pos++) {
            for (int x_pos = 0; x_pos < n_cols; x_pos++) {
                int y_val = top.at<cv::Vec3b>(y_pos, x_pos)[0];
                int u_val = top.at<cv::Vec3b>(y_pos, x_pos)[1];
                int v_val = top.at<cv::Vec3b>(y_pos, x_pos)[2];

                this->u_vals_.push_back(u_val);
                this->v_vals_.push_back(v_val);
                this->y_vals_.push_back(y_val)  ;
            }
        }
    }else if(context == PROGRESS){
        int randX;
        int randY;

        //Create temporary UV val array for new values

        /*
        std::vector<std::vector<bool> > filteredUV;
        filteredUV = std::vector<std::vector<bool> >(U_RANGE);
        for (int i = 0; i < U_RANGE; i++) {
            filteredUV.at(i) = std::vector<bool>(V_RANGE);
        }
        */
        int bins_dif = MAX_GREEN - gc.getNumBins();
        float dif_ratio = (float)bins_dif/(float)MAX_GREEN;
        int samplesize = BASESAMPLESIZE * dif_ratio;
        std::cout << "Sample Size: " << samplesize << std::endl;
        for(int pix_counter = 0 ; pix_counter < samplesize ; pix_counter++){
            //Generate random (x, y) coordinates to sample
            randX = rand() % TOPWIDTH;

            //randY must be higher than 200 as unlikely any green pixels lower
            randY = getWeightedPos(200, TOPHEIGHT, 0.85); //Weight towards higher y vals

            int y_val = top.at<cv::Vec3b>(randY, randX)[0];
            int u_val = top.at<cv::Vec3b>(randY, randX)[1];
            int v_val = top.at<cv::Vec3b>(randY, randX)[2];

            //Check if any U V values within 2 are green
            bool validGreen = possiblyGreen(y_val, u_val, v_val, 2, gc);
            if(validGreen){
                gc.setGreen(u_val,v_val);
            }
        }
        //Plug in new values
        /*
        for(int u = 0 ; u < U_RANGE ; u++){
            for(int v = 0 ; v < V_RANGE ; v++){
                if(filteredUV.at(u).at(v) == true){
                    gc.setGreen(u, v);
                }
            }
        }
        */

        this->classifiedSanityCheck(gc, top);

    }else if(context == PLAYING){
        return;
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

    gc.y_expv_ = y_sum / static_cast<double>(this->y_vals_.size());
    gc.y_sd_ = sqrt((y_sum_squared / static_cast<double>(this->y_vals_.size())) - pow(gc.y_expv_, 2));
    gc.y_radius_ = abs(gc.y_expv_ - 128);
}

void GreenChromaClassifier::classify(GreenChroma& gc, cv::Mat& test, cv::Mat& classified) {
    int n_rows = test.rows;
    int n_cols = test.cols;

    int white_min_y = gc.y_expv_ + 3*gc.y_sd_;

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
            } else if (gc.isWhite(y_val, u_val, v_val)) {
                classified.at<cv::Vec3b>(y_pos, x_pos)[0] = 128;
                classified.at<cv::Vec3b>(y_pos, x_pos)[1] = 128;
                classified.at<cv::Vec3b>(y_pos, x_pos)[2] = 128;
            }
        }
    }
}

//This function checks if any YUV values within a certain range are green
bool GreenChromaClassifier::possiblyGreen(int y, int u, int v, int within, GreenChroma& gc){
    for(int u_pos = u - within ; u_pos <= u + within ; u_pos++){
        for(int v_pos = v - within ; v_pos <= v + within ; v_pos++){
            if(u_pos < 0 || u_pos > U_RANGE || v_pos < 0 || v_pos > V_RANGE){ //make sure values arent out of range
                continue;
            }
            if(gc.isFiltered(u_pos, v_pos)){
                return true;
            }
        }
    }
    return false;
}

//Gets a random number between min and max, weighted towards the weight value
//weight to be between 0 and 1, 1 being weight towards the max
//0 being weight towards the min
int GreenChromaClassifier::getWeightedPos(int min, int max, float weight){
    int result = (rand() + min) % max;

    /*
    int range = max - min;
    int idealNum = range*weight + min;
    bool chosen = false;
    bool chanceArray[10];
    for(int i = 0 ; i < 10 ; i++){
        chanceArray[i] = false;
    }
    while(!chosen){
        result = (rand() + min) % max;
        int dif = std::abs(result - idealNum);
        float difRatio = 1 - dif/range;
        int i = 0;
        while(difRatio > 0 && i < 10){
            chanceArray[i] = true;
            difRatio -= 0.1;
        }
        int randIndex = rand() % 10;
        if(chanceArray[randIndex]){
            chosen = true;
        }
    }
    */

    return result;
}

void GreenChromaClassifier::classifiedSanityCheck(GreenChroma &gc, cv::Mat& image){
    int n_rows = image.rows;
    int n_cols = image.cols;

    for(int y_pos = 0 ; y_pos < n_rows*0.02 ; y_pos++){
        for(int x_pos = 0 ; x_pos < n_cols ; x_pos++){
            int y_val = image.at<cv::Vec3b>(y_pos, x_pos)[0];
            int u_val = image.at<cv::Vec3b>(y_pos, x_pos)[1];
            int v_val = image.at<cv::Vec3b>(y_pos, x_pos)[2];

            if(gc.isFiltered(u_val, v_val)){
                gc.unsetGreen(u_val, v_val);
            }
        }
    }
}
