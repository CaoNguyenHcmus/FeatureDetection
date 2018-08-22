/*
 *
 *  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
 *
 *  By downloading, copying, installing or using the software you agree to this license.
 *  If you do not agree to this license, do not download, install,
 *  copy or use the software.
 *
 *
 *                           License Agreement
 *                For Open Source Computer Vision Library
 *
 * Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
 * Copyright (C) 2009-2011, Willow Garage Inc., all rights reserved.
 * Copyright (C) 2014-2015, Itseez Inc., all rights reserved.
 * Third party copyrights are property of their respective owners.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *   * Redistribution's of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   * Redistribution's in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   * The name of the copyright holders may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * This software is provided by the copyright holders and contributors "as is" and
 * any express or implied warranties, including, but not limited to, the implied
 * warranties of merchantability and fitness for a particular purpose are disclaimed.
 * In no event shall the Intel Corporation or contributors be liable for any direct,
 * indirect, incidental, special, exemplary, or consequential damages
 * (including, but not limited to, procurement of substitute goods or services;
 * loss of use, data, or profits; or business interruption) however caused
 * and on any theory of liability, whether in contract, strict liability,
 * or tort (including negligence or otherwise) arising in any way out of
 * the use of this software, even if advised of the possibility of such damage.
 *
 */

#include <iostream>
#include <limits>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include "../include/detectionMethod.hpp"
#include "../include/imageStyle.hpp"
#include "../include/matchingMethod.hpp"
#include "../include/method.h"
#include "../include/miniFunction.hpp"
#include "../include/videoMatching.hpp"

enum Mode
{
    FEATURE_DETECTION = 1,
    IMAGE_MATCHING = 2,
    VIDEO_MATCHING = 3
};

int main(void)
{
    int mode = FEATURE_DETECTION;
    double tempInputMode = 0; // Error handling if input not an int
again:
{
    while ((std::cout << "Choose mode:\n"
                      << "[1] Feature detection\n"
                      << "[2] Image Matching\n"
                      << "[3] Matching in video\n"
                      << "Your choice: ") &&
           !(std::cin >> tempInputMode))
    {
        std::cin.clear();                                                   // Clear bad input flag
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard input
        std::cout << "\nError! Please choose available mode.\n\n";
    }
}

    mode = static_cast<int>(tempInputMode); // Make sure mode variable is an int
    int method = 2; //-- Create method variable and set default value
    switch (mode)
    {
        case FEATURE_DETECTION:
        {
            std::cout << "\nFeature detection mode.\n";

            cv::Mat src = inputAndCheckValidImage("Input image path: ");

            method = chooseMethod();
            featureDetection(src, method);

            break;
        }
        case IMAGE_MATCHING:
        {
            std::cout << "\nImage Matching mode.\n";

            cv::Mat srcObject = inputAndCheckValidImage("Input object image path: ");
            cv::Mat imgTemp = inputAndCheckValidImage("Input scene image path: ");

            int imageType = DEFAULT_IMAGE;
            cv::Mat srcScene = returnSrc(imgTemp, imageType);

            int drawLinker = checkChoice("Link Features? (y/n): ");
            int drawHomography = checkChoice("Draw Homography? (y/n): ");

            method = chooseMethod();
            matchingImage(srcObject, srcScene, drawLinker, drawHomography, method);

            break;
        }
        case VIDEO_MATCHING:
        {
            std::cout << "\nMatching in video mode.\n";

            cv::Mat srcObject = inputAndCheckValidImage("Input object image path: ");

            std::string videoPath;
        inputVideo:
        {
            videoPath = inputAndOutString("Input video path: ");
        }
            cv::VideoCapture cap(videoPath);
            if (!cap.isOpened())
            {
                std::cout << "Error reading video!\n";
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                goto inputVideo;
            }

            method = chooseMethod();
            matchVideo(srcObject, cap, method);

            break;
        }
        default:
            std::cout << "\nError! Please choose available mode.\n\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            goto again;
    }

    return 0;
}
