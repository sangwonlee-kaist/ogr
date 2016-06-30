#pragma once

#include "config.hpp"
#include "debug_helper.hpp"

class PointDetector
    {
public:
     PointDetector();
    ~PointDetector();
    void setImage(const cv::Mat& dataImage);
    void detect();
    std::vector<cv::Point> getPoints();
private:
    class impl;
    std::unique_ptr<impl> pImpl;
    };
