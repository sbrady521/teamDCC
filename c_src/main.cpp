#include <iostream>
#include <dirent.h>
#include <string.h>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <chrono>
#include "Types.h"
#include "GreenChromaClassifier.h"

typedef std::vector<std::string> svtr;

void openFilesTest(std::string&, svtr&, svtr&, svtr&);
void openFilesTrain(std::string&, svtr&);
void process_training(GreenChromaClassifier&, GreenChroma&, std::string&, std::string&);
double process_testing(GreenChromaClassifier&, GreenChroma&, std::string&, std::string&, std::string&);


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
    std::string imgDirTrainTop = imgDirTrain + "/top/";
    std::string imgDirTrainBottom = imgDirTrain + "/bottom/";

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

    // For each of the training top/bottom camera pairs, sample data from them
    std::cout << "Sampling Images..." << std::endl;
    auto t_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < trainFilesTop.size(); i++) {
        process_training(gcc, gc, trainFilesTop[i], trainFilesBottom[i]);
    }
    gc.fillPoints(4);
    std::cout << "total bins found: " << gc.getNumBins() << std::endl;
    auto t_end = std::chrono::high_resolution_clock::now();
    std::cout << "Sampling took "
              << std::chrono::duration<double, std::milli>(t_end-t_start).count()
              << " ms\n";


    std::cout << "Model creation took "
              << std::chrono::duration<double, std::milli>(t_end-t_start).count()
              << " ms\n";

    // Using our fitted classifier, generate results for the test images
    std::cout << "Classifying images..." << std::endl;
    std::vector<double> allfs(0);
    for (int i = 0; i < testFilesRaw.size(); i++) {
        double value = process_testing(gcc, gc, testFilesRaw[i], testFilesClassified[i], testFilesAnnotated[i]);
        allfs.push_back(value);
    }

    double average = 0;
    for (int i = 0; i < allfs.size(); ++i) {
        average += allfs.at(i);
    }
    std::cout << "Average F1: " << (average / (double)allfs.size()) << std::endl << std::endl;

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

    if(gc.binsExist()){ //Progress sample
        std::cout << "taking progress sample" << std::endl;
        gcc.sample(gc, imgTop, imgBottom, 2);
    }else{ //Initial sample
        std::cout << "taking Initial sample" << std::endl;
        gcc.sample(gc, imgTop, imgBottom, 1);
        gcc.model(gc);
    }

    imgTop.release();
    imgBottom.release();
}

double process_testing(GreenChromaClassifier& gcc, GreenChroma& gc, std::string& pathRaw, std::string& pathClassified, std::string& pathAnnotated) {
    cv::Mat imgTest = cv::imread(pathRaw, cv::IMREAD_COLOR);
    cv::Mat imgAnnotated = cv::imread(pathAnnotated, cv::IMREAD_COLOR);
    cv::Mat imgClassified(960, 1280, CV_8UC3, cv::Scalar(255,255,255));

    if (!imgTest.data) {
        std::cerr << "Exception at classifyImage for file,'" << pathRaw << "'" << std::endl;
        throw std::runtime_error("No Image Data...");
    }
    if (!imgAnnotated.data) {
        std::cerr << "Exception at classifyImage for file,'" << pathAnnotated << "'" << std::endl;
        throw std::runtime_error("No Image Data...");
    }

    cv::cvtColor(imgTest, imgTest, cv::COLOR_BGR2YUV);

    auto t_start = std::chrono::high_resolution_clock::now();
    gcc.classify(gc, imgTest, imgClassified);
    auto t_end = std::chrono::high_resolution_clock::now();
    std::cout << "Classification took "
              << std::chrono::duration<double, std::milli>(t_end-t_start).count()
              << " ms\n";

    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    bool status = imwrite(pathClassified, imgClassified, compression_params);
    if (!status) {
        std::cerr << "Failed to write image '" << pathClassified << "'" << std::endl;
    }

    double tp = 0;
    double fp = 0;
    double tn = 0;
    double fn = 0;
    for (int i = 0; i < 960; ++i) {
        for (int j = 0; j < 1280; ++j) {
            bool annotatedGreen = false;
            bool classifiedGreen = false;
            if (imgAnnotated.at<cv::Vec3b>(i,j)[2] < 1) {
                annotatedGreen = true;
            }
            if (imgClassified.at<cv::Vec3b>(i,j)[1] == 255) {
                classifiedGreen = true;
            }
            if (annotatedGreen && classifiedGreen) tp++;
            if (!annotatedGreen && classifiedGreen) fp++;
            if (!annotatedGreen && !classifiedGreen) tn++;
            if (annotatedGreen && !classifiedGreen) fn++;
        }
    }

    double precision = 0;
    double recall = 0;
    double f1 = 0;
    if (tp + fp > 0) {
        precision = (tp) / (tp + fp);
    }
    if (tp + fn > 0) {
        recall = (tp) / (tp + fn);
    }
    if (precision + recall > 0) {
        f1 = (precision * recall * 2) / (precision + recall);
    }

    std::cout << pathRaw << std::endl;
    std::cout << "tp(" << tp << ") fp(" << fp << ") tn(" << tn << ") fn(" << fn << ")" << std::endl;
    std::cout << "Precision: " << precision << std::endl;
    std::cout << "Recall: " << recall << std::endl;
    std::cout << "F1 Score: " << f1 << std::endl;
    std::cout << std::endl;

    imgTest.release();
    imgAnnotated.release();

    return f1;

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
        std::cerr << "Error opening directory for test files" << std::endl;
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
        std::cerr << "Error opening directory for training files" << std::endl;
        exit(1);
    }
}
