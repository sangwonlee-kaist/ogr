#pragma once

#include "config.hpp"

class YAxisParser
    {
public:
     YAxisParser();
    ~YAxisParser();
    void setImage(const cv::Mat& axisImage);
    void parse();
    std::string getLabel();
    double getOffsetValue();
    double getFixelWidth();
private:
    class impl;
    std::unique_ptr<impl> pImpl;
    };
