#pragma once

#include "config.hpp"

class GraphSplitter
    {
public:
     GraphSplitter();
    ~GraphSplitter();
    void setImage(const cv::Mat& graphImage);
    void split();
    cv::Mat getXAxisImage();
    cv::Mat getYAxisImage();
    cv::Mat getDataImage();
private:
    class impl;
    std::unique_ptr<impl> pImpl;
    };
