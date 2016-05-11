#pragma once

#include "config.hpp"

class XAxisParser
    {
public:
     XAxisParser();
    ~XAxisParser();
    void setImage(const cv::Mat& axisImage);
    void parse();
    std::string getLabel();
    double getOffsetValue();
    double getPixelWidth();
private:
    class impl;
    std::unique_ptr<impl> pImpl;
    };
