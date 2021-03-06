#include <string>
#include <opencv2/opencv.hpp>

#include "Types.h"

class GreenChromaClassifier {
private:
    std::vector<int> u_vals_;
    std::vector<int> v_vals_;
    std::vector<int> y_vals_;

    bool possiblyGreen(int y, int u, int v, int within, GreenChroma& gc);
    int getWeightedPos(int min, int max, float weight);
    void classifiedSanityCheck(GreenChroma &gc, cv::Mat& image);

public:
    void sample(GreenChroma&, cv::Mat&, cv::Mat&, int context);
    void model(GreenChroma&);
    void classify(GreenChroma& gc, cv::Mat&, cv::Mat&);
};
