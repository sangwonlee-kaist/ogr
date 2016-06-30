#pragma once

#include "config.hpp"

class OcrEngine
    {
public:
     OcrEngine();
    ~OcrEngine();
    void setImage(const cv::Mat& textImage);
    std::string getText();
private:
    class impl;
    std::unique_ptr<impl> pImpl;
    };