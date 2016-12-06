#include <iostream>
#include "Sample.h"

int main(int argc, char** argv ) {
    Sample test;
    try {
        test.SampleImage("test.bmp");
    } catch (std::runtime_error &ex) {
        std::cerr << ex.what() << std::endl;
    }

    test.createHistogram();
    test.showHistogram();
}