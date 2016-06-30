#pragma once

#include "config.hpp"
#include "ocr_engine.hpp"
#include "debug_helper.hpp"

class YAxisParser
    {
public:
     YAxisParser();
    ~YAxisParser();
    void setImage(const cv::Mat& axisImage);
    void parse();
    std::string getLabel();
    double getOffsetValue();
    double getPixelHeight();
private:
    class impl;
    std::unique_ptr<impl> pImpl;
    };
