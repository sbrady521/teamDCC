#include <iostream>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "Sample.h"
#include "HSVSample.h"

void tests(void);

int main(int argc, char** argv ) {
    if (argc != 4) {
        if (argc == 2 && strcmp(argv[1], "TEST") == 0) {
            tests();
            return EXIT_SUCCESS;
        } else {
            std::cerr << "Usage: ./Chromaticity sampleDir classifyDir <rgchroma|hsv>" << std::endl;
            return 1;
        }
    }

    char *samplePath = argv[1];
    char *classifyPath = argv[2];
    char *arg3 = argv[3];
    bool rgchroma;

    if (strcmp(arg3, "hsv") == 0) rgchroma = false;
    else rgchroma = true;

    std::vector<std::string> sampleFiles;
    std::vector<std::string> classifyFiles;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (samplePath)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            if (strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0) continue;
            sampleFiles.push_back(std::string(samplePath) + std::string(ent->d_name));
        }
        closedir (dir);
    } else {
        /* could not open directory */
        return EXIT_FAILURE;
    }

    if ((dir = opendir (classifyPath)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            if (strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0) continue;
            std::string path_str = std::string(ent->d_name);
            if (path_str.find("_classified.png") != std::string::npos) continue;
            classifyFiles.push_back(std::string(classifyPath) + path_str);
        }
        closedir (dir);
    } else {
        /* could not open directory */
        return EXIT_FAILURE;
    }

    int num_files = sampleFiles.size() < classifyFiles.size() ? sampleFiles.size() : classifyFiles.size();

    //for (int i = 0; i < num_files; i++) std::cout << sampleFiles[i] << std::endl;


    if (rgchroma) {
        Sample green = Sample();
        for (int i = 0; i < num_files; i++) {
            green.sampleImage(sampleFiles[i]);

            std::string out_file = classifyFiles[i] + std::string("_rgchroma_classified_.png");
            green.createHistogram();
            green.classifyImage(classifyFiles[i], out_file);
        }
    } else {
        HSVSample green = HSVSample();
        for (int i = 0; i < num_files; i++) {
            green.sampleImage(sampleFiles[i]);

            std::string out_file = classifyFiles[i] + std::string("_hsv_classified.png");
            green.createHistogram();
            green.classifyImage(classifyFiles[i], out_file);
        }
    }
    return EXIT_SUCCESS;
}


void tests(void) {
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
    test1.showHistogram();

    Histogram<int> test2 = Histogram<int>(vec2);

    test2.showHistogram();

    int min, max;
    test2.getPeakRange(0.1, min, max);
    std::cout << min << " " << max << std::endl;

    Histogram<int> *test3 = new Histogram<int>(vec3);

    test3->showHistogram();
    test3->getPeakRange(0.01, min, max);
    std::cout << min << " " << max << std::endl;
    delete test3;

    Histogram<int> assigned = test2;
    assigned.showHistogram();

    assigned.getPeakRange(0.1, min, max);
    std::cout << min << " " << max << std::endl;

    std::vector<int> vec4 = std::vector<int>();
    vec4.push_back(1);
    assigned.appendData(vec4);

    assigned.showHistogram();

    vec4.push_back(2);
    assigned.appendData(vec4);

    std::cout << std::endl;

    assigned.showHistogram();

    std::vector<int> vec5 = std::vector<int>();
    vec5.push_back(10);
    assigned.appendData(vec5);
    std::cout << std::endl;

    assigned.showHistogram();

    std::vector<int> vec6 = std::vector<int>();
    vec6.push_back(12);
    assigned.appendData(vec6);
    std::cout << std::endl;

    assigned.showHistogram();
    return;
}