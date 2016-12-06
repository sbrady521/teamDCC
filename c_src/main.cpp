#include <iostream>
#include "Sample.h"

int main(int argc, char** argv ) {
    Sample test;
    try {
        test.SampleImage("bottom_camera/botFram0001.jpg");
    } catch (std::runtime_error &ex) {
        std::cerr << ex.what() << std::endl;
    }

    test.createHistogram();
    //test.showHistogram();
    test.classifyImage("top_camera/topFram0001.jpg", "top_camera/topFram0001_rgchroma.png");
}