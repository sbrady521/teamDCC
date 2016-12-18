#include <iostream>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "Sample.h"
#include "HSVSample.h"

int main(int argc, char** argv ) {
    if (argc != 4) {
        std::cerr << "Usage: ./Chromaticity sampleDir classifyDir <rgchroma|hsv>" << std::endl;
        return 1;
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
}
