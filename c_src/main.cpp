#include <iostream>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include "Types.h"
#include "GreenChromaClassifier.h"

typedef std::vector<std::string> svtr;

void openFilesTest(std::string&, svtr&, svtr&, svtr&);
void openFilesTrain(std::string&, svtr&);
void process_training(GreenChromaClassifier&, GreenChroma&, std::string&, std::string&);
void process_testing(GreenChromaClassifier&, GreenChroma&, std::string&, std::string&, std::string&);


int main(int argc, char** argv ) {

    // Ensure the correct number arguments are passed in from command line arguments
    if (argc != 3) {
        std::cerr << "Usage: ./Chromatacity <training dir> <testing dir>>" << std::endl;
        return 1;
    }

    // Get command line arguments and create directory paths
    std::string imgDirTrain(argv[1]);
    std::string imgDirTestRaw(argv[2]);
    std::string imgDirTest = imgDirTestRaw + "/";
    std::string imgDirTrainTop = imgDirTrain + "top/";
    std::string imgDirTrainBottom = imgDirTrain + "bottom/";

    // Create string vectors to store file names
    std::vector<std::string> testFilesRaw;
    std::vector<std::string> testFilesAnnotated;
    std::vector<std::string> testFilesClassified;
    std::vector<std::string> trainFilesBottom;
    std::vector<std::string> trainFilesTop;

    // Create the GreenChroma data structure and the classifier
    GreenChroma gc = GreenChroma();
    GreenChromaClassifier gcc = GreenChromaClassifier();

    // Given the directories, open them and store the files in string vectors
    openFilesTest(imgDirTest, testFilesRaw, testFilesAnnotated, testFilesClassified);
    openFilesTrain(imgDirTrainTop, trainFilesTop);
    openFilesTrain(imgDirTrainBottom, trainFilesBottom);

    // Sort all files (particularly the paired folders) to ensure indedxes match up
    std::sort(trainFilesTop.begin(), trainFilesTop.end());
    std::sort(trainFilesBottom.begin(), trainFilesBottom.end());
    std::sort(testFilesRaw.begin(), testFilesRaw.end());
    std::sort(testFilesAnnotated.begin(), testFilesAnnotated.end());
    std::sort(testFilesClassified.begin(), testFilesClassified.end());

    // Ensure we have equal number of top and bottom camera images
    if (trainFilesTop.size() != trainFilesBottom.size()) {
        std::cerr << "Number of files in bottom and top training folder do not match.";
    }

    // Ensure we have as many testing files as we have their annotations
    if (testFilesRaw.size() != testFilesAnnotated.size()) {
        std::cerr << "Number of raw and annotated files in test folder do not match.";
    }

    // For each of the training top/bottom camera pairs, fit our classifier to them
    for (int i = 0; i < trainFilesTop.size(); i++) {
        // Sampling
        process_training(gcc, gc, trainFilesTop[i], trainFilesBottom[i]);
    }
    // Modelling
    std::cout << "Creating a model..." << std::endl;
    gcc.model(gc);

    // Using our fitted classifier, generate results for the test images
    for (int i = 0; i < testFilesRaw.size(); i++) {
        process_testing(gcc, gc, testFilesRaw[i], testFilesClassified[i], testFilesAnnotated[i]);
    }

    return EXIT_SUCCESS;
}

void process_training(GreenChromaClassifier& gcc, GreenChroma& gc, std::string& pathTop, std::string& pathBottom) {
    cv::Mat imgTop = cv::imread(pathTop, cv::IMREAD_COLOR);
    cv::Mat imgBottom = cv::imread(pathBottom, cv::IMREAD_COLOR);

    if (!imgTop.data) {
        std::cerr << "Exception at classifyImage for file,'" << pathTop << "'" << std::endl;
        throw std::runtime_error("No Image Data...");
    }
    if (!imgBottom.data) {
        std::cerr << "Exception at classifyImage for file,'" << pathBottom<< "'" << std::endl;
        throw std::runtime_error("No Image Data...");
    }

    cv::cvtColor(imgTop, imgTop, cv::COLOR_BGR2YUV);
    cv::cvtColor(imgBottom, imgBottom, cv::COLOR_BGR2YUV);

    std::cout << "Sampling " << pathTop << std::endl;

    gcc.sample(gc, imgTop, imgBottom);

    imgTop.release();
    imgBottom.release();
}

void process_testing(GreenChromaClassifier& gcc, GreenChroma& gc, std::string& pathRaw, std::string& pathClassified, std::string& pathAnnotated) {
    cv::Mat imgTest = cv::imread(pathRaw, cv::IMREAD_COLOR);
    cv::Mat imgAnnotated = cv::imread(pathAnnotated, cv::IMREAD_COLOR);
    cv::Mat imgClassified(960, 1280, CV_8UC3, cv::Scalar(0,0,0));

    if (!imgTest.data) {
        std::cerr << "Exception at classifyImage for file,'" << pathRaw << "'" << std::endl;
        throw std::runtime_error("No Image Data...");
    }
    if (!imgAnnotated.data) {
        std::cerr << "Exception at classifyImage for file,'" << pathAnnotated << "'" << std::endl;
        throw std::runtime_error("No Image Data...");
    }

    cv::cvtColor(imgTest, imgTest, cv::COLOR_BGR2YUV);
    cv::cvtColor(imgAnnotated, imgAnnotated, cv::COLOR_BGR2YUV);

    std::cout << "Classifying " << pathRaw << " and saving it to " << pathClassified << std::endl;
    gcc.classify(gc, imgTest, imgClassified);

    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    bool status = imwrite(pathClassified, imgClassified, compression_params);
    if (!status) {
        std::cerr << "Failed to write image '" << pathClassified << "'" << std::endl;
    }

    // TODO: Compare Classified with Annotated

    imgTest.release();
    imgAnnotated.release();

}

void openFilesTest(std::string& testDir, svtr& raw, svtr& annotated, svtr& classified) {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(testDir.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                std::string path_str = std::string(ent->d_name);
                if (path_str.find("_classified.png") == std::string::npos) {
                    if (path_str.find("_ann.png") != std::string::npos) {
                        annotated.push_back(std::string(testDir.c_str()) + std::string(ent->d_name));   
                    } else {
                        raw.push_back(std::string(testDir.c_str()) + std::string(ent->d_name));   

                        std::string rawName = std::string(ent->d_name);
                        rawName = rawName.substr(0, rawName.size() - 4);
                        classified.push_back(std::string(testDir.c_str()) + rawName + "_classified.png");   
                    }
                }
            }
        }
        closedir(dir);
    } else {
        /* could not open directory */
        std::cerr << "Error opening directory for test files at "
                  << testDir << std::endl;
        exit(1);
    }
}

void openFilesTrain(std::string& trainDir, svtr& files) {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(trainDir.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                files.push_back(std::string(trainDir.c_str()) + std::string(ent->d_name));   
            }
        }
        closedir(dir);
    } else {
        /* could not open directory */
        std::cerr << "Error opening directory for train files at "
                  << trainDir << std::endl;
        exit(1);
    }
}