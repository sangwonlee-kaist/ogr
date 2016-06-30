#pragma once

#include "config.hpp"
#include "ocr_engine.hpp"
#include "debug_helper.hpp"

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
