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
#include <stdio.h>
#include <string>
#include <vector>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/videoio/videoio.hpp>
#include "../include/method.h"
#include "../include/miniFunction.hpp"
#include "../include/videoMatching.hpp"

void matchVideo(cv::Mat srcObject, cv::VideoCapture cap, int method)
{
    cv::Mat frame;
    int key = 0;
    int i = 0;
    int j = 0;
    std::string imageName;
    std::string fileExtension(".jpg");
    int getFps = cap.get(cv::VideoCaptureProperties::CAP_PROP_FPS);

    //-- Save frame from Video to image files
    std::cout << "Save frames to images...\n";
    while (1)
    {
        key = cv::waitKey(10);
        cap.read(frame);
        if (frame.empty() || (char)key == 27)
        {
            cv::destroyWindow("Streaming");
            break;
        }

        cv::imshow("Streaming", frame);
        imageName = "../temp/frame" + std::to_string(i) + fileExtension;
        saveImage(imageName, frame);
        ++i;
    }

    //-- Declare variables
    std::string scenePath, tempPath;
    cv::Mat imageObject;
    std::string videoPath;
    std::vector<cv::KeyPoint> objectKeypoints;
    cv::Mat objectDescriptors;
    std::vector<std::vector<cv::KeyPoint>> sceneKeypoints;
    std::vector<cv::Mat> srcScene, sceneDescriptors;

    cv::cvtColor(srcObject, imageObject, cv::ColorConversionCodes::COLOR_BGR2GRAY, 0);

    std::cout << "Detect and compute keypoints...\n";
    switch (method)
    {
    case SIFT_METHOD:
    {
        videoPath = "../output/siftVideo.avi";
        cv::Ptr<cv::Feature2D> detector = cv::xfeatures2d::SIFT::create();

        detector->detectAndCompute(imageObject, cv::Mat(), objectKeypoints, objectDescriptors);

        for (j = 0; j < i; ++j)
        {
            std::cout << "Process image " << j + 1 << "\n";
            //-- Input file
            scenePath = "../temp/frame" + std::to_string(j) + fileExtension;
            cv::Mat tempImgScene = cv::imread(scenePath, cv::ImreadModes::IMREAD_COLOR);

            cv::Mat imageScene;
            cv::cvtColor(tempImgScene, imageScene, cv::ColorConversionCodes::COLOR_BGR2GRAY, 0);

            std::vector<cv::KeyPoint> tempKeypoints;
            cv::Mat tempDescriptors;
            detector->detectAndCompute(imageScene, cv::Mat(), tempKeypoints, tempDescriptors);

            sceneKeypoints.push_back(tempKeypoints);
            sceneDescriptors.push_back(tempDescriptors);
            srcScene.push_back(tempImgScene);

            const char *tempImgPath = scenePath.c_str();
            if (remove(tempImgPath) != 0)
            {
                std::cout << "Error delete image " << j + 1 << "\n";
            }
        }
        break;
    }
    case SURF_METHOD:
    {
        videoPath = "../output/surfVideo.avi";
        cv::Ptr<cv::Feature2D> detector = cv::xfeatures2d::SURF::create();

        detector->detectAndCompute(imageObject, cv::Mat(), objectKeypoints, objectDescriptors);

        for (j = 0; j < i; ++j)
        {
            std::cout << "Process image " << j + 1 << "\n";
            //-- Input file
            scenePath = "../temp/frame" + std::to_string(j) + fileExtension;
            cv::Mat tempImgScene = cv::imread(scenePath, cv::ImreadModes::IMREAD_COLOR);

            cv::Mat imageScene;
            cv::cvtColor(tempImgScene, imageScene, cv::ColorConversionCodes::COLOR_BGR2GRAY, 0);

            std::vector<cv::KeyPoint> tempKeypoints;
            cv::Mat tempDescriptors;
            detector->detectAndCompute(imageScene, cv::Mat(), tempKeypoints, tempDescriptors);

            sceneKeypoints.push_back(tempKeypoints);
            sceneDescriptors.push_back(tempDescriptors);
            srcScene.push_back(tempImgScene);

            const char *tempImgPath = scenePath.c_str();
            if (remove(tempImgPath) != 0)
            {
                std::cout << "Error delete image " << j + 1 << "\n";
            }
        }
        break;
    }
    case FAST_METHOD:
    {
        videoPath = "../output/fastideo.avi";
        cv::Ptr<cv::Feature2D> detector = cv::FastFeatureDetector::create();
        cv::Ptr<cv::Feature2D> extractor = cv::BRISK::create();

        detector->detect(imageObject, objectKeypoints);
        extractor->compute(imageObject, objectKeypoints, objectDescriptors);
        if (objectDescriptors.type() != CV_32F)
        {
            cv::Mat temp;
            objectDescriptors.convertTo(temp, CV_32F);
            objectDescriptors = temp;
        }

        for (j = 0; j < i; ++j)
        {
            std::cout << "Process image " << j + 1 << "\n";
            //-- Input file
            scenePath = "../temp/frame" + std::to_string(j) + fileExtension;
            cv::Mat tempImgScene = cv::imread(scenePath, cv::ImreadModes::IMREAD_COLOR);

            cv::Mat imageScene;
            cv::cvtColor(tempImgScene, imageScene, cv::ColorConversionCodes::COLOR_BGR2GRAY, 0);

            std::vector<cv::KeyPoint> tempKeypoints;
            cv::Mat tempDescriptors;

            detector->detect(imageScene, tempKeypoints);
            extractor->compute(imageScene, tempKeypoints, tempDescriptors);
            if (tempDescriptors.type() != CV_32F)
            {
                cv::Mat temp;
                tempDescriptors.convertTo(temp, CV_32F);
                tempDescriptors = temp;
            }

            sceneKeypoints.push_back(tempKeypoints);
            sceneDescriptors.push_back(tempDescriptors);
            srcScene.push_back(tempImgScene);

            const char *tempImgPath = scenePath.c_str();
            if (remove(tempImgPath) != 0)
            {
                std::cout << "Error delete image " << j + 1 << "\n";
            }
        }
        break;
    }
    default:
        break;
    }

    std::cout << "Matching images...\n";
    for (j = 0; j < i; ++j)
    {
        std::cout << "Matching image " << j + 1 << "\n";
        cv::FlannBasedMatcher matcher;
        std::vector<std::vector<cv::DMatch>> matches;
        matcher.knnMatch(objectDescriptors, sceneDescriptors[j], matches, 2);

        std::vector<cv::DMatch> goodMatches;
        unsigned int n = 0;
        for (n = 0; n < matches.size(); ++n)
        {
            const float ratio = 0.8;
            if (matches[n][0].distance < ratio * matches[n][1].distance)
            {
                goodMatches.push_back(matches[n][0]);
            }
        }

        std::vector<cv::Point2f> obj;
        std::vector<cv::Point2f> scene;

        for (n = 0; n < goodMatches.size(); ++n)
        {
            //-- Get the key points from the good matches
            obj.push_back(objectKeypoints[goodMatches[n].queryIdx].pt);
            scene.push_back(sceneKeypoints[j][goodMatches[n].trainIdx].pt);
        }

        cv::Mat H = findHomography(obj, scene, CV_RANSAC);

        std::vector<cv::Point2f> objectCorners(4);
        objectCorners[0] = cv::Point(0, 0);
        objectCorners[1] = cv::Point(imageObject.cols, 0);
        objectCorners[2] = cv::Point(imageObject.cols, imageObject.rows);
        objectCorners[3] = cv::Point(0, imageObject.rows);
        std::vector<cv::Point2f> sceneCorners(4);
        cv::perspectiveTransform(objectCorners, sceneCorners, H);

        cv::line(srcScene[j], sceneCorners[0], sceneCorners[1], cv::Scalar(0, 255, 0), 4);
        cv::line(srcScene[j], sceneCorners[1], sceneCorners[2], cv::Scalar(0, 255, 0), 4);
        cv::line(srcScene[j], sceneCorners[2], sceneCorners[3], cv::Scalar(0, 255, 0), 4);
        cv::line(srcScene[j], sceneCorners[3], sceneCorners[0], cv::Scalar(0, 255, 0), 4);

        //-- Save files
        imageName = "../temp/processed" + std::to_string(j) + fileExtension;
        saveImage(imageName, srcScene[j]);
    }

    std::string srcPath("../temp/processed");

    //-- Create an encoder
    cv::VideoWriter out(videoPath, cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), getFps, cv::Size(720, 406));

    //-- Take frame from files and save to video
    std::cout << "Writing to video...\n";
    for (int n = 0; n < i; ++n)
    {
        imageName = srcPath + std::to_string(n) + fileExtension;

        frame = cv::imread(imageName, cv::ImreadModes::IMREAD_COLOR);
        out.write(frame);

        // const char *tempImgPath = tempPath.c_str();
        // system(tempImgPath);
        const char *tempImgPath = imageName.c_str();
        if (remove(tempImgPath) != 0)
        {
            std::cout << "Error delete image " << n + 1 << "\n";
        }
    }

    //-- Show result video
    std::cout << "Result video.\n";
    cv::VideoCapture newCap(videoPath);
    while (1)
    {
        key = cv::waitKey(10);
        newCap.read(frame);
        if (frame.empty() || static_cast<char>(key) == 27)
        {
            cv::destroyWindow("Streaming");
            break;
        }
        cv::imshow("Streaming", frame);
    }

    cv::waitKey(0);
    std::cout << "Done.\n";
    cv::destroyAllWindows();
}
