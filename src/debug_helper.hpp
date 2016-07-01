#pragma once

#include "config.hpp"

#define OGR_DEBUG

#ifdef OGR_DEBUG
    #define PRTIMG_HELPER(x, y)         \
        {                               \
        std::cout << "Image: " << std::setw(20) << #x << \
            ", File: " << std::setw(20) << __FILE__ <<   \
            ", Line: " << std::setw(20) << __LINE__ <<   \
            std::endl;                  \
        cv::imshow(#x, y);              \
        cv::waitKey();                  \
        }

    #define PRTTXT_HELPER(x, y)            \
        {                                  \
        std::cout << "Variable: " << std::setw(20) << #x << \
            ", Value: " << std::setw(20) << y <<            \
            ", File: " << std::setw(20) << __FILE__ <<      \
            ", Line: " << std::setw(20) << __LINE__ <<      \
            std::endl;                     \
        }

    #define PRTIMG(x)     PRTIMG_HELPER(x, x)
    #define PRTTXT(x)     PRTTXT_HELPER(x, x)
    #define PRTIMG2(x, y) PRTIMG_HELPER(x, y)
    #define PRTTXT2(x, y) PRTTXT_HELPER(x, y)

    #define DEBUG_ONLY_BEGIN [&]() {
    #define DEBUG_ONLY_END   }();

    #define DEBUG_SHOW_IMG(x) PRTIMG(x)
    #define DEBUG_SHOW_VAR(x) PRTTXT(x)
#else
    #define PRTIMG(x)
    #define PRTTXT(x)
    #define PRTIMG2(x, y)
    #define PRTTXT2(x, y)
    #define DEBUG_ONLY(x)

    #define DEBUG_ONLY_BEGIN [&]() {
    #define DEBUG_ONLY_END   };
#endif
