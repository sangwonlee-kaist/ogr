#pragma once

#include "config.hpp"
#include "debug_helper.hpp"

class CurveRemover
    {
public:
     CurveRemover();
    ~CurveRemover();

    void remove();
    void setImage(const cv::Mat& inputImage);
    cv::Mat getImage();
private:
    class impl;
    std::unique_ptr<impl> pImpl;
    };