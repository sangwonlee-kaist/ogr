#include "x_axis_parser.hpp"

#define DEBUG

#ifdef DEBUG
    #define PRTIMG(x) cv::imshow(#x, x); cv::waitKey(0);
#else
    #define PRTIMG(x)
#endif

class XAxisParser::impl
    {
public:
     impl() = default;
    ~impl() = default; 
    
    bool isParsed;
    cv::Mat axisImage;
    tesseract::TessBaseAPI tessBaseAPI;
    };

XAxisParser::XAxisParser() : pImpl {new impl}
    {
    pImpl->isParsed = false;
    // Initialize with english.
    pImpl->tessBaseAPI.Init(nullptr, "eng");
    }

XAxisParser::~XAxisParser()
    {

    }

void 
XAxisParser::setImage(const cv::Mat& axisImage)
    {
    pImpl->axisImage = axisImage.clone();
    // You have to parse for new image.
    pImpl->isParsed = false;
    }

void
XAxisParser::parseAxis()
    {
    if (pImpl->isParsed)
        {
        // You do not have to parse twice for the same image.
        return;
        }

    PRTIMG(pImpl->axisImage) 

    pImpl->isParsed = true; 
    }
