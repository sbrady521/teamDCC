#include <string>
#include <opencv2/opencv.hpp>

#include "Types.h"

class GreenChromaClassifier {
public:
	void fit(GreenChroma&, cv::Mat&, cv::Mat&);
	void predict(GreenChroma& gc, cv::Mat&, cv::Mat&);
private:
};