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
#include <vector>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include "../include/miniFunction.hpp"
#include "../include/method.h"

void saveImage(std::string imageName, cv::Mat image)
{
    std::vector<int> compressionParams;
    compressionParams.push_back(cv::ImwriteFlags::IMWRITE_JPEG_QUALITY);
    compressionParams.push_back(100);
    cv::imwrite(imageName, image, compressionParams);
}

int chooseMethod()
{
    int method = SURF_METHOD;

again:
{
    while ((std::cout << "\nPlease choose detection method:\n"
                      << "[1] SIFT\n"
                      << "[2] SURF\n"
                      << "[3] FAST\n"
                      << "Your choice: ") &&
           !(std::cin >> method))
    {
        std::cout << "Error! Please choose available method!\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

    switch (method)
    {
    case SIFT_METHOD:
        return SIFT_METHOD;
        break;
    case SURF_METHOD:
        return SURF_METHOD;
        break;
    case FAST_METHOD:
        return FAST_METHOD;
        break;
    default:
        std::cout << "Error! Please choose available method!\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        goto again;
    }
}

std::string inputAndOutString(std::string printScreen)
{
    std::string path;
    std::cout << printScreen;
    std::cin >> path;

    return path;
}

cv::Mat inputAndCheckValidImage(std::string outScene)
{
    std::string path;
    cv::Mat temp;
    while (1)
    {
        path = inputAndOutString(outScene);
        temp = cv::imread(path, cv::ImreadModes::IMREAD_COLOR);

        if (!temp.data)
        {
            std::cout << "Error reading image! Re-enter!\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        else
        {
            break;
        }
    }
    return temp;
}

bool checkChoice(std::string printScreen)
{
    bool temp;
    std::string choice;
again:
{
    choice = inputAndOutString(printScreen);
}

    if (!choice.compare(0, 1, "y") || !choice.compare(0, 1, "Y"))
    {
        return temp = true;
    }
    else if (!choice.compare(0, 1, "n") || !choice.compare(0, 1, "N"))
    {
        return temp = false;
    }
    else
    {
        std::cout << "Error! Please choose (y/n)!\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        goto again;
    }
}
