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
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "../include/matchingMethod.hpp"
#include "../include/method.h"
#include "../include/miniFunction.hpp"

constexpr int MAX_GOOD_MATCHES_SHOW = 20;

void matchingImage(cv::Mat srcObject, cv::Mat srcScene, bool linked, bool homo, int method)
{
    cv::Mat objectGray;
    cv::Mat sceneGray;
    cv::cvtColor(srcObject, objectGray, cv::ColorConversionCodes::COLOR_BGR2GRAY, 0);
    cv::cvtColor(srcScene, sceneGray, cv::ColorConversionCodes::COLOR_BGR2GRAY, 0);

    auto start_s = std::chrono::high_resolution_clock::now();
    std::vector<cv::KeyPoint> objectKeypoints, sceneKeypoints;
    cv::Mat objectDescriptors, sceneDescriptors;
    std::string imageName, resultWindowName;
    switch (method)
    {
        case SIFT_METHOD:
        {
            imageName = "../output/siftMaching.jpg";
            resultWindowName = "SIFT Good Matches & Object detection";
            cv::Ptr<cv::Feature2D> detector = cv::xfeatures2d::SIFT::create();
            detector->detectAndCompute(objectGray, cv::Mat(), objectKeypoints, objectDescriptors);
            detector->detectAndCompute(sceneGray, cv::Mat(), sceneKeypoints, sceneDescriptors);
            break;
        }
        case SURF_METHOD:
        {
            imageName = "../output/surfMaching.jpg";
            resultWindowName = "SURF Good Matches & Object detection";
            cv::Ptr<cv::Feature2D> detector = cv::xfeatures2d::SURF::create();
            detector->detectAndCompute(objectGray, cv::Mat(), objectKeypoints, objectDescriptors);
            detector->detectAndCompute(sceneGray, cv::Mat(), sceneKeypoints, sceneDescriptors);
            break;
        }
        case FAST_METHOD:
        {
            imageName = "../output/fastMaching.jpg";
            resultWindowName = "FAST Good Matches & Object detection";
            cv::Ptr<cv::Feature2D> detector = cv::FastFeatureDetector::create();
            cv::Ptr<cv::Feature2D> extractor = cv::BRISK::create();

            detector->detect(objectGray, objectKeypoints);
            extractor->compute(objectGray, objectKeypoints, objectDescriptors);

            detector->detect(sceneGray, sceneKeypoints);
            extractor->compute(sceneGray, sceneKeypoints, sceneDescriptors);

            if (objectDescriptors.type() != CV_32F)
            {
                cv::Mat temp;
                objectDescriptors.convertTo(temp, CV_32F);
                objectDescriptors = temp;
            }
            if (sceneDescriptors.type() != CV_32F)
            {
                cv::Mat temp;
                sceneDescriptors.convertTo(temp, CV_32F);
                sceneDescriptors = temp;
            }
            break;
        }
        default:
            break;
    }

    cv::FlannBasedMatcher matcher;
    std::vector<std::vector<cv::DMatch>> matches;
    matcher.knnMatch(objectDescriptors, sceneDescriptors, matches, 2);

    //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
    std::vector<cv::DMatch> goodMatches;
    unsigned int i = 0; // Count variable

    for (i = 0; i < matches.size(); ++i)
    {
        const float ratio = 0.8; // As in Lowe's paper; can be tuned
        if (matches[i][0].distance < ratio * matches[i][1].distance)
        {
            goodMatches.push_back(matches[i][0]);
        }
    }

    //-- Copy only 10 good matches
    std::vector<cv::DMatch> showedGoodMatches, emptyVec;
    for (i = 0; i < MAX_GOOD_MATCHES_SHOW; ++i)
    {
        showedGoodMatches.push_back(goodMatches[i]);
    }

    cv::Mat imageMatches;
    if (linked == 1)
    {
        cv::drawMatches(srcObject, objectKeypoints, srcScene, sceneKeypoints,
                        showedGoodMatches, imageMatches, cv::Scalar::all(-1), cv::Scalar::all(-1),
                        std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
    }
    else
    {
        cv::drawMatches(srcObject, objectKeypoints, srcScene, sceneKeypoints,
                        emptyVec, imageMatches, cv::Scalar::all(-1), cv::Scalar::all(-1),
                        std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
    }

    auto stop_s = std::chrono::high_resolution_clock::now();

    //-- Show detected matches
    if (homo == 1)
    {
        //-- Localize the object
        std::vector<cv::Point2f> obj;
        std::vector<cv::Point2f> scene;

        for (unsigned int i = 0; i < goodMatches.size(); ++i)
        {
            //-- Get the key-points from the good matches
            obj.push_back(objectKeypoints[goodMatches[i].queryIdx].pt);
            scene.push_back(sceneKeypoints[goodMatches[i].trainIdx].pt);
        }

        cv::Mat H = cv::findHomography(obj, scene, CV_RANSAC);

        //-- Get the corners from the object image (the object to be "detected")
        std::vector<cv::Point2f> objectCorners(4);
        objectCorners[0] = cv::Point(0, 0);
        objectCorners[1] = cv::Point(srcObject.cols, 0);
        objectCorners[2] = cv::Point(srcObject.cols, srcObject.rows);
        objectCorners[3] = cv::Point(0, srcObject.rows);
        std::vector<cv::Point2f> sceneCorners(4);

        cv::perspectiveTransform(objectCorners, sceneCorners, H);

        //-- Draw lines between the corners (the mapped object in the scene image)
        cv::line(imageMatches, sceneCorners[0] + cv::Point2f(srcObject.cols, 0), sceneCorners[1] + cv::Point2f(srcObject.cols, 0), cv::Scalar(0, 255, 0), 4);
        cv::line(imageMatches, sceneCorners[1] + cv::Point2f(srcObject.cols, 0), sceneCorners[2] + cv::Point2f(srcObject.cols, 0), cv::Scalar(0, 255, 0), 4);
        cv::line(imageMatches, sceneCorners[2] + cv::Point2f(srcObject.cols, 0), sceneCorners[3] + cv::Point2f(srcObject.cols, 0), cv::Scalar(0, 255, 0), 4);
        cv::line(imageMatches, sceneCorners[3] + cv::Point2f(srcObject.cols, 0), sceneCorners[0] + cv::Point2f(srcObject.cols, 0), cv::Scalar(0, 255, 0), 4);

        cv::imshow(resultWindowName, imageMatches);
    }
    else
    {
        cv::imshow(resultWindowName, imageMatches);
    }

    saveImage(imageName, imageMatches);

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop_s - start_s).count();
    std::cout << "Process time: " << duration << " ms\n";
    std::cout << "Object keypoints: " << objectKeypoints.size() << "\n";
    std::cout << "Scene keypoints: " << sceneKeypoints.size() << "\n";
    std::cout << "Good matches: " << goodMatches.size() << "\n";
    float matchRate = (goodMatches.size() * 100) / ((objectKeypoints.size() + sceneKeypoints.size()) / 2);
    std::cout << "Matching rate: " << matchRate << "%\n";

    cv::waitKey(0);
}
