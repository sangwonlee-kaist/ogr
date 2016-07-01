#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <vector>
#include <chrono>

#include "../graph_splitter.hpp"
#include "../line_point_splitter.hpp"
#include "../point_detector.hpp"

int main()
    {
    GraphSplitter graphSplitter;
    graphSplitter.setImage(cv::imread("SH2.png"));

    LinePointSplitter linePointSplitter;
    linePointSplitter.setImage(graphSplitter.getDataImage());
    
    PointDetector pointDetector;
    pointDetector.setImage(linePointSplitter.getPointImage());
    pointDetector.getPoints();

    return 0;
    }