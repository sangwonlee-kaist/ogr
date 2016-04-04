#pragma once

#include "config.hpp"

class AxesParser
    {
public:
     AxesParser();
    ~AxesParser();

private:
    class impl;
    std::unique_ptr<impl> pImpl;
    };
