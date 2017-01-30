#include <string>
#include <opencv2/opencv.hpp>

#include "Types.h"

class GreenChromaClassifier {
private:
    std::vector<int> u_vals_;
    std::vector<int> v_vals_;
    std::vector<int> y_vals_;
public:
	void fit(GreenChroma&, cv::Mat&, cv::Mat&);
	void predict(GreenChroma& gc, cv::Mat&, cv::Mat&);
};