#include <string>
#include <opencv2/opencv.hpp>

#include "Types.h"

class GreenChromaClassifier {
private:
    std::vector<int> u_vals_;
    std::vector<int> v_vals_;
    std::vector<int> y_vals_;

    bool closeTo(bool[][] filteredUV, int u, int v, int within);

public:
    void sample(GreenChroma&, cv::Mat&, cv::Mat&);
    void model(GreenChroma&);
    void classify(GreenChroma& gc, cv::Mat&, cv::Mat&);
    void progressSample(GreenChroma& gc, cv::Mat& top, cv::Mat& bottom);
    void initialSample(GreenChroma& gc, cv::Mat& top, cv::Mat& bottom);
    bool isEmpty();
};
