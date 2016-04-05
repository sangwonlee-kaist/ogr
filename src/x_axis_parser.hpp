#pragma once

#include "config.hpp"

class XAxisParser
    {
public:
     XAxisParser();
    ~XAxisParser();
    void setImage(const cv::Mat& axisImage);
    void parseAxis();
private:
    class impl;
    std::unique_ptr<impl> pImpl;
    };
