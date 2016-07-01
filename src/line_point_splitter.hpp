#pragma once

#include "config.hpp"
#include "debug_helper.hpp"

class LinePointSplitter
    {
public:
     LinePointSplitter();
    ~LinePointSplitter();

    void remove();
    void setImage(const cv::Mat& inputImage);
    cv::Mat getLineImage();
    cv::Mat getPointImage();
private:
    class impl;
    std::unique_ptr<impl> pImpl;
    };