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

#include <chrono>
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "../include/detectionMethod.hpp"

void featureDetection(cv::Mat src, int method)
{
    cv::Mat gray;
    cv::cvtColor(src, gray, cv::ColorConversionCodes::COLOR_BGR2GRAY, 0);

    auto start_s = std::chrono::high_resolution_clock::now();

    //-- Create detector
    std::vector<cv::KeyPoint> keypoints;
    std::string resultWindowName = "SURF detection";

    switch (method)
    {
    case SIFT_METHOD:
    {
        resultWindowName = "SIFT detection";
        cv::Ptr<cv::Feature2D> detector = cv::xfeatures2d::SIFT::create();
        detector->detect(gray, keypoints);
        break;
    }
    case SURF_METHOD:
    {
        resultWindowName = "SURF detection";
        cv::Ptr<cv::Feature2D> detector = cv::xfeatures2d::SURF::create();
        detector->detect(gray, keypoints);
        break;
    }
    case FAST_METHOD:
    {
        resultWindowName = "FAST detection";
        cv::Ptr<cv::Feature2D> detector = cv::FastFeatureDetector::create();
        detector->detect(gray, keypoints);
        break;
    }
    default:
        break;
    }

    //-- Draw key-points
    cv::Mat keypointsImage;
    cv::drawKeypoints(gray, keypoints, keypointsImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);

    auto stop_s = std::chrono::high_resolution_clock::now();

    //-- Show original image and detected image
    cv::namedWindow("Display window", cv::WindowFlags::WINDOW_AUTOSIZE);
    cv::imshow("Display window", src);
    cv::namedWindow(resultWindowName, cv::WindowFlags::WINDOW_AUTOSIZE);
    cv::imshow(resultWindowName, keypointsImage);

    //-- Show processing time, number of keypoints
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop_s - start_s).count();
    std::cout << "Process time: " << duration << " ms\n";
    std::cout << "Keypoints: " << keypoints.size() << "\n";
    cv::waitKey(0);
}
