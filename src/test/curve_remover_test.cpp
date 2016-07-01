#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <vector>
#include <chrono>

#include "../line_point_splitter.hpp"

int main()
    {
    cv::Mat image = cv::imread("symbol2.png");

    LinePointSplitter linePointSplitter;
    linePointSplitter.setImage(image);
    cv::Mat opened = linePointSplitter.getPointImage();

    //cv::Mat gray;

    //if (image.empty())
    //    {
    //    std::cerr << "File open error." << std::endl;
    //    return 1;
    //    }

    //cv::imshow("Input image", image);

    //if (image.channels() == 3) // color
    //    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    //else 
    //    gray = image.clone();

    //cv::Mat binary;
    //// When the value of cv::THRESH_OTSU is defined, 
    //// the threshold function returns the optimal 
    //// threshold value obtained by the Otsu's algorithm.
    //// 0 (threshold value parameter) is ignored.
    //cv::threshold(gray, binary, 0, 255, cv::THRESH_OTSU + cv::THRESH_BINARY_INV);

    //cv::imshow("Binary image", binary);
    //cv::imwrite("binary.png", binary);

    //cv::Mat element5 {5, 5, CV_8U, cv::Scalar {1}};
    //cv::Mat opened;
    //cv::morphologyEx(binary, opened, cv::MORPH_OPEN, element5);

    //cv::imshow("Opened binary", opened);
    //cv::imwrite("opened.png", opened);

    //cv::Mat dilated;
    //cv::Mat element7 {7, 7, CV_8U, cv::Scalar {1}};
    //cv::dilate(opened, dilated, element7);
    //cv::Mat curve = binary - dilated;
    //
    //cv::imshow("binary - dilated", curve);
    //cv::imwrite("curves.png", curve);

    std::vector< std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::findContours(opened, contours, hierarchy, 
        CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point {0, 0});

    std::cout << "The number of detected contours = " << contours.size() << std::endl;

    std::vector< std::vector<cv::Point> > polyContours (contours.size());
    for (int i = 0; i < contours.size(); ++i)
        cv::approxPolyDP(contours[i], polyContours[i], 5, true);

    cv::RNG rng (std::chrono::system_clock::now().time_since_epoch().count());
    cv::Mat canvas = image.clone();
    for (int i = 0; i < polyContours.size(); ++i)
        {
        cv::Scalar color
            (rng.uniform(100, 255), rng.uniform(100, 255), rng.uniform(100, 255));
        cv::drawContours(canvas, polyContours, i, color, 2);
        }

    cv::imshow("Detect result", canvas);
    cv::imwrite("detected.png", canvas);

    cv::waitKey();


    return 0;
    }