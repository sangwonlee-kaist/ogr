#pragma once
#include "config.hpp"

class PointDetector
    {
public:
     PointDetector();
    ~PointDetector();
    void detect();
private:
    class impl;
    std::unique_ptr<impl> pImpl;
    };
