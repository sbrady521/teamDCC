#include <iostream>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "Sample.h"
#include "HSVSample.h"
#include "YUVSample.h"
#include "Polynomial.h"
#include "Histogram2D.h"

void tests(void);

int main(int argc, char** argv ) {
    if (argc != 4) {
        if (argc == 2 && strcmp(argv[1], "TEST") == 0) {
            tests();
            return EXIT_SUCCESS;
        } else {
            std::cerr << "Usage: ./Chromatacity <dir that you want to sample on> <dir with the images you want to classify> <rgchroma|hsv|yuv>" << std::endl;
            return 1;
        }
    }

    char *samplePath = argv[1];
    char *classifyPath = argv[2];
    char *arg3 = argv[3];

    std::vector<std::string> sampleFiles;
    std::vector<std::string> classifyFiles;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(samplePath)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            std::string path_str = std::string(ent->d_name);
            if (path_str.find("_classified.png") != std::string::npos) continue;
            sampleFiles.push_back(std::string(samplePath) + std::string(ent->d_name));
        }
        closedir(dir);
    } else {
        /* could not open directory */
        return EXIT_FAILURE;
    }

    if ((dir = opendir(classifyPath)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
            std::string path_str = std::string(ent->d_name);
            if (path_str.find("_classified.png") != std::string::npos) continue;
            classifyFiles.push_back(std::string(classifyPath) + path_str);
        }
        closedir(dir);
    } else {
        /* could not open directory */
        return EXIT_FAILURE;
    }

    int num_files = sampleFiles.size() < classifyFiles.size() ? sampleFiles.size() : classifyFiles.size();

    //for (int i = 0; i < num_files; i++) std::cout << sampleFiles[i] << std::endl;


    if (strcmp(arg3, "hsv") == 0) {
        HSVSample green = HSVSample();
        for (int i = 0; i < num_files; i++) {
            green.sampleImage(sampleFiles[i]);
            std::string out_file = classifyFiles[i] + std::string("_hsv_classified.png");
            green.createHistogram();
            //green.showHueHistogram();
            green.classifyImage(classifyFiles[i], out_file);
        }
    } else if (strcmp(arg3, "yuv") == 0) {
        YUVSample green = YUVSample();
        for (int i = 0; i < num_files; i++) {
            std::cout << "Image " << i + 1 << " name: " << classifyFiles[i] << std::endl;  
            green.sampleImage(sampleFiles[i]);
            std::string out_file = classifyFiles[i] + std::string("_yuv_classified.png");
            green.createHistogram();
            //green.showUHistogram(); green.showVHistogram();
            green.classifyImage(classifyFiles[i], out_file);
        }
    } else {
        Sample green = Sample();
        for (int i = 0; i < num_files; i++) {
            green.sampleImage(sampleFiles[i]);

            std::string out_file = classifyFiles[i] + std::string("_rgchroma_classified.png");
            green.createHistogram();
            green.classifyImage(classifyFiles[i], out_file);
        }
    }
    return EXIT_SUCCESS;
}


void tests(void) {
    /*
    std::cout << "Testing building histograms..." << std::endl;

    std::vector<int> vec1;
    std::vector<int> vec2;
    std::vector<int> vec3;

    for (int i = 0; i < 5; i++) {
        vec1.push_back(i);
        vec2.push_back(i);
        vec3.push_back(i);
    }

    for (int i = 0; i < 10; i++) {
        vec2.push_back(i);
        vec3.push_back(i);
    }

    for (int i = 0; i < 100; i++) {
        vec3.push_back(i);
    }

    for (int i = 80; i < 100; i++) {
        vec3.push_back(i); vec3.push_back(i);
    }

    for (int i = 80; i < 100; i++) {
        vec3.push_back(i); vec3.push_back(i);
    }

    for (int i = 100; i < 1000; i++) vec3.push_back(i);

    Histogram<int> test1 = Histogram<int>(vec1);

    std::vector<int> vec4;
    vec4.push_back(6);
    test1.appendData(vec4);

    std::vector<int> vec5;
    vec5.push_back(8); vec5.push_back(8); vec5.push_back(8); vec5.push_back(8);
    test1.appendData(vec5);
    test1.showHistogram();

    int min; int max;
    test1.getPeakRange(0.15, min, max);
    std::cout << min << " " << max << std::endl;
    return;
     */

    /*
    std::cout << "Testing Polynomial Fitting..." << std::endl;
    std::vector<double> x1 = std::vector<double>();
    std::vector<double> y1 = std::vector<double>();

    for (int i = 1; i <= 4; i++) x1.push_back(i);
    y1.push_back(6); y1.push_back(5); y1.push_back(7); y1.push_back(10);

    Polynomial1V p1 = polyFit(x1, y1, 3);
    p1.showPolynomial();

    double min; double max;
    p1.maxAreaWindow(0, 5, 1, min, max);
    std::cout << "Min area " << min << "max area " << max << std::endl;
     */

    std::cout << "Testing 2D Histogram..." << std::endl;

    std::vector<double> vec1;

    for (int i = 0; i < 5; i++) {
        vec1.push_back(i);
    }

    std::vector<double> vec2;
    vec2.push_back(2); vec2.push_back(3); vec2.push_back(1); vec2.push_back(1); vec2.push_back(4);

    Histogram2D<double> test1 = Histogram2D<double>(vec1, vec2);
    test1.showHistogram();
}
