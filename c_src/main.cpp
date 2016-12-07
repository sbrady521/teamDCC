#include <iostream>
#include "Sample.h"

int main(int argc, char** argv ) {
    Sample *test = new Sample();
    try {
        test->SampleImage("bottom_camera/botFram0001.jpg");
    } catch (std::runtime_error &ex) {
        std::cerr << ex.what() << std::endl;
    }

    test->createHistogram();
    //test->showHistogram();
    //test->showChromaVals();
    std::string path = "top_camera/topFram0001.jpg";
    std::cout << path << std::endl;
    std::string out_path = std::string("top_camera/topFram0001_rgchroma.png");
    test->classifyImage(path, out_path);
}