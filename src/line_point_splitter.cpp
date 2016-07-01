#include "line_point_splitter.hpp"

class LinePointSplitter::impl
    {
public:
     impl() = default;
    ~impl() = default;

    bool isRemoved;
    cv::Mat inputImage;
    cv::Mat lineImage;
    cv::Mat pointImage;
    };

LinePointSplitter::LinePointSplitter() : pImpl {new impl}
    {
    pImpl->isRemoved = false;
    }

LinePointSplitter::~LinePointSplitter()
    {

    }

void
LinePointSplitter::remove()
    {
    if (pImpl->isRemoved)
        return;

    if (pImpl->inputImage.empty())
        throw std::invalid_argument {"LinePointSplitter::remove(): Empty image."};

    DEBUG_SHOW_IMG(pImpl->inputImage)

    cv::Mat grayImage;
    if (pImpl->inputImage.channels() == 3) // is color
        {
        cv::cvtColor(pImpl->inputImage, grayImage, cv::COLOR_BGR2GRAY);
        }
    else
        {
        grayImage = pImpl->inputImage.clone();
        }

    // When the value of cv::THRESH_OTSU is defined, 
    // the threshold function returns the optimal 
    // threshold value obtained by the Otsu's algorithm.
    // 0 (threshold value parameter) is ignored.
    cv::Mat binaryImage;
    cv::threshold(grayImage, binaryImage, 0, 255, cv::THRESH_OTSU + cv::THRESH_BINARY_INV);

    DEBUG_SHOW_IMG(binaryImage);

    cv::Mat element5 {5, 5, CV_8U, cv::Scalar {1}};
    cv::Mat pointImage;
    cv::morphologyEx(binaryImage, pointImage, cv::MORPH_OPEN, element5);
    DEBUG_SHOW_IMG(pointImage);
    
    cv::Mat dilatedImage;
    cv::Mat element7 {7, 7, CV_8U, cv::Scalar {1}};
    cv::dilate(pointImage, dilatedImage, element7);    

    cv::Mat lineImage = binaryImage - dilatedImage;
    DEBUG_SHOW_IMG(lineImage);
 
    pImpl->lineImage = lineImage;
    pImpl->pointImage = pointImage;
    pImpl->isRemoved = true;
    }

void
LinePointSplitter::setImage(const cv::Mat& inputImage)
    {
    pImpl->inputImage = inputImage;
    pImpl->isRemoved = false;
    }

cv::Mat
LinePointSplitter::getLineImage()
    {
    if (not pImpl->isRemoved)
        this->remove();

    return pImpl->lineImage.clone();
    }

cv::Mat
LinePointSplitter::getPointImage()
    {
    if (not pImpl->isRemoved)
        this->remove();

    return pImpl->pointImage.clone();
    }