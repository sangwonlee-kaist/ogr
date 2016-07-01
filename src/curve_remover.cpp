#include "curve_remover.hpp"

class CurveRemover::impl
    {
public:
     impl() = default;
    ~impl() = default;

    bool isRemoved;
    cv::Mat inputImage;
    cv::Mat outputImage;
    };

CurveRemover::CurveRemover() : pImpl {new impl}
    {
    pImpl->isRemoved = false;
    }

CurveRemover::~CurveRemover()
    {

    }

void
CurveRemover::remove()
    {
    if (pImpl->isRemoved)
        return;

    if (pImpl->inputImage.empty())
        throw std::invalid_argument {"CurveRemover::remove(): Empty image."};

    

    pImpl->isRemoved = true;
    }

void
CurveRemover::setImage(const cv::Mat& inputImage)
    {
    pImpl->inputImage = inputImage;
    pImpl->isRemoved = false;
    }

cv::Mat
CurveRemover::getImage()
    {
    if (not pImpl->isRemoved)
        this->remove();

    return pImpl->outputImage.clone();
    }
