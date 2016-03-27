#pragma once

#include "StdConfig.hpp"
//#include <OpencvConfig.hpp>

class AxesDetector
    {
public:
     AxesDetector();
    ~AxesDetector();
    void getGraphImage();
private:
    class impl;
    std::unique_ptr<impl> pImpl;
    };
