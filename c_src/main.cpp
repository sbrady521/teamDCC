#include <iostream>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "Sample.h"

int main(int argc, char** argv ) {
    if (argc != 4) {
        std::cerr << "Usage: ./Chromaticity sampleDir classifyDir outputDir" << std::endl;
        return 1;
    }

    char *samplePath = argv[1];
    char *classifyPath = argv[2];
    char *outPath = argv[3];

    std::vector<std::string> sampleFiles;
    std::vector<std::string> classifyFiles;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (samplePath)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            if (strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0) continue;
            sampleFiles.push_back(std::string(ent->d_name));
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
            classifyFiles.push_back(std::string(ent->d_name));
        }
        closedir (dir);
    } else {
        /* could not open directory */
        return EXIT_FAILURE;
    }

    int num_files = sampleFiles.size() < classifyFiles.size() ? sampleFiles.size() : classifyFiles.size();

    for (int i = 0; i < num_files; i++) std::cout << sampleFiles[i] << std::endl;

    int samplecnt = 0;

    /*
    Sample test = Sample();
    try {
        test.SampleImage("bottom_camera/botFram0001.jpg");
    } catch (std::runtime_error &ex) {
        std::cerr << ex.what() << std::endl;
    }

    test.createHistogram();
    test.showHistogram();
    std::string path = "top_camera/topFram0001.jpg";
    std::cout << path << std::endl;
    std::string out_path = std::string("top_camera/topFram0001_rgchroma.png");
    test.classifyImage(path, out_path);
    */
}
