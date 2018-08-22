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
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "../include/imageStyle.hpp"
#include "../include/method.h"
#include "../include/miniFunction.hpp"

/* 
cv::Mat imgSheared(cv::Mat srcScreen)
{
    int r1, c1;     // transformed point
    int rows, cols; // original image rows and columns
    rows = srcScreen.rows;
    cols = srcScreen.cols;

    float Bx = 0.5; // amount of shearing in x-axis
    float By = 0;   // amount of shearing in y-axis

    int maxXOffset = abs(cols * Bx);
    int maxYOffset = abs(rows * By);

    cv::Mat out = cv::Mat::ones(srcScreen.rows + maxYOffset, srcScreen.cols + maxXOffset, srcScreen.type()); // create output image to be the same as the source

    for (int r = 0; r < out.rows; ++r) // loop through the image
    {
        for (int c = 0; c < out.cols; ++c)
        {
            r1 = r + c * By - maxYOffset; // map old point to new
            c1 = r * Bx + c - maxXOffset;

            if (r1 >= 0 && r1 <= rows && c1 >= 0 && c1 <= cols) // check if the point is within the boundaries
            {
                out.at<uchar>(r, c) = srcScreen.at<uchar>(r1, c1); // set value
            }
        }
    }

    return out;
}
 */

cv::Mat imgSheared(cv::Mat srcScreen)
{
    cv::Mat temp;
    srcScreen.copyTo(temp);
    cv::Mat M(2, 3, CV_32F);

    M.at<float>(0, 0) = 1;
    M.at<float>(0, 1) = 0;
    M.at<float>(0, 2) = 0;

    M.at<float>(1, 0) = 0.5;
    M.at<float>(1, 1) = 1;
    M.at<float>(1, 2) = 0;

    cv::warpAffine(temp, temp, M, cv::Size(temp.cols, temp.rows));
    cv::imshow("Sheared_Image", temp);
    return temp;
}

cv::Mat imgRotated(cv::Mat srcScreen)
{
    //-- Image parameters
    std::string imageName("../output/rotatedImage.jpg");

    //-- Show the original image
    cv::namedWindow("Original Image", cv::WindowFlags::WINDOW_AUTOSIZE);
    cv::imshow("Original Image", srcScreen);

    std::string windowName = "Rotated Image";
    cv::namedWindow(windowName, cv::WindowFlags::WINDOW_AUTOSIZE);

    //-- Create track-bar with parameter
    int iAngle = 180;
    int iScale = 50;
    cv::createTrackbar("Angle", windowName, &iAngle, 360);
    cv::createTrackbar("Scale", windowName, &iScale, 100);

    int iImageHeight = srcScreen.rows / 2;
    int iImageWidth = srcScreen.cols / 2;

    cv::Mat imgRotated;
    int key = 0;
    while (1)
    {
        key = cv::waitKey(30);
        cv::Mat matRotation = cv::getRotationMatrix2D(cv::Point(iImageWidth, iImageHeight), (iAngle - 180), iScale / 50.0);

        //-- Rotate the image
        cv::warpAffine(srcScreen, imgRotated, matRotation, srcScreen.size());

        cv::imshow(windowName, imgRotated);

        if (static_cast<char>(key) == 'z' || static_cast<char>(key) == 'Z') //-- "z" key
        {
            saveImage(imageName, imgRotated);
        }

        if (static_cast<char>(key) == 27) //-- Esc key
        {
            cv::destroyAllWindows();
            break;
        }
    }
    cv::Mat out = imgRotated;
    return out;
}

cv::Mat imgCompressed(cv::Mat srcScreen, float compressPercent)
{
    std::string imageName = "../output/compressedImage.jpg";
    std::vector<int> compressionParams;

    compressionParams.push_back(cv::ImwriteFlags::IMWRITE_JPEG_QUALITY);
    compressionParams.push_back(compressPercent);

    cv::imwrite(imageName, srcScreen, compressionParams);
    cv::Mat temp = cv::imread("../output/compressedImage.jpg", cv::ImreadModes::IMREAD_COLOR);

    cv::Mat out;
    if (temp.channels() != 3)
    {
        cv::cvtColor(temp, out, cv::ColorConversionCodes::COLOR_BGRA2BGR, 0);
    }
    else
    {
        out = temp;
    }
    return out;
}

cv::Mat returnSrc(cv::Mat imgTemp, int imageType)
{
    cv::Mat srcScene;
askImageType:
{
    while ((std::cout << "Choose image type:\n"
                      << "[0] Default\n"
                      << "[1] Compress Image\n"
                      << "[2] Rotate image\n"
                      << "[3] Sheared image\n"
                      << "Your choice: ") &&
           !(std::cin >> imageType))
    {
        std::cin.clear();                                                   //clear bad input flag
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //discard input
        std::cout << "\nError! Please choose available type!\n";
    }
}

    switch (imageType)
    {
    case DEFAULT_IMAGE:
    {
        srcScene = imgTemp;
        break;
    }
    case COMPRESS_IMAGE:
    {
        float compressPercent;

    inputPercent:
    {
        std::cout << "\nInput % compress between 1 and 100: ";
        std::cin >> compressPercent;
    }

        if (compressPercent < 0 || compressPercent > 100 || std::cin.fail())
        //-- check if input percent out of range or not a number
        {
            std::cout << "You must enter valid number between 1 and 100!\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            goto inputPercent;
        }

        srcScene = imgCompressed(imgTemp, compressPercent);
        break;
    }
    case ROTATE_IMAGE:
    {
        srcScene = imgRotated(imgTemp);
        break;
    }
    case SHEARED_IMAGE:
    {
        srcScene = imgSheared(imgTemp);
        break;
    }
    default:
    {
        std::cout << "\nError! Please choose available type!\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        goto askImageType;
    }
    }
    return srcScene;
}
