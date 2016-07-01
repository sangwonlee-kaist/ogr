#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <vector>
#include <chrono>

#include "../line_point_splitter.hpp"
#include "../point_detector.hpp"

int main()
    {
    cv::Mat image = cv::imread("real5.png");

    LinePointSplitter linePointSplitter;
    linePointSplitter.setImage(image);
    cv::Mat pointImage = linePointSplitter.getPointImage();

    PointDetector pointDetector;
    pointDetector.setImage(pointImage);
    pointDetector.getPoints();

    return 0;
    }