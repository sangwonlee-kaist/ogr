#include "graph_spliter.hpp"

#define DEBUG

#ifdef DEBUG
    #define PRTIMG(x) cv::imshow(#x, x); \
                      cv::waitKey(0);
#else
    #define PRTIMG(x) 
#endif

class GraphSpliter::impl
    {
public:
     impl() = default;
    ~impl() = default;

    bool    isSplited;
    cv::Mat graphImage;
    cv::Mat xAxisImage;
    cv::Mat yAxisImage;
    cv::Mat  dataImage; 
    };

GraphSpliter::GraphSpliter() : pImpl {new impl}
    {
    pImpl->isSplited = false; 
    }

GraphSpliter::~GraphSpliter()
    {

    }

void 
GraphSpliter::setImage(const cv::Mat& graphImage)
    {
    // In OpenCV, = operator of cv::Mat is assignment of ptr.
    // (in real, assignment of reference... might be...)
    // So if you want real copy of image, you must use .clone()
    // or .copyTo() of cv::Mat.
    pImpl->graphImage = graphImage.clone();
    // you have to do split process for new image.
    pImpl->isSplited = false;    
    }

void 
GraphSpliter::splitGraph()
    {
    if (pImpl->isSplited)
        {
        // You do not have to split twice for same image.
        // Nothing to do.
        return;
        }

    // algorithm here!
    // if input image is color image.
    cv::Mat grayImage;
    if (pImpl->graphImage.channels() == 3)
        {
        cv::cvtColor(pImpl->graphImage,
                     grayImage,
                     cv::COLOR_RGB2GRAY);
        }
    else
        {
        grayImage = pImpl->graphImage.clone();
        }

    PRTIMG(grayImage)
    // Make binary image of gray image.
    
    cv::Mat binaryImage;
    cv::adaptiveThreshold(~grayImage,
        binaryImage,
        255,
        cv::ADAPTIVE_THRESH_MEAN_C, 
        cv::THRESH_BINARY,
        5,
        -2);
                          
    PRTIMG(binaryImage)
    
    // vertical line which has the length < minHorizontalSize 
    // is noise.
    // Size unit = Fixel.
    int minHorizontalSize = (pImpl->graphImage).cols / 3;
    int minVerticalSize   = (pImpl->graphImage).rows / 3;

    // Create...
    cv::Mat verticalLine = 
        cv::getStructuringElement(cv::MORPH_RECT, cv::Size(minHorizontalSize, 1));

    
    
    
     
    }

cv::Mat 
GraphSpliter::getXAxisImage()
    {
    if (not (pImpl->isSplited))
        this->splitGraph();
    // I'm not sure what is the proper way
    // gives result image as an output.
    // I'm not use xAxisImage directly.
    return pImpl->xAxisImage.clone();
    }

cv::Mat 
GraphSpliter::getYAxisImage()
    {
    if (not (pImpl->isSplited))
        this->splitGraph();

    return pImpl->yAxisImage.clone();
    }

cv::Mat 
GraphSpliter::getDataImage()
    {
    if (not (pImpl->isSplited))
        this->splitGraph();

    return pImpl->dataImage.clone();
    }
