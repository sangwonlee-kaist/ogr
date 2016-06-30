#include "point_detector.hpp"

#ifdef DEBUG
    #define PRTIMG(x)     cv::imshow(#x, x); cv::waitKey(0);
    #define PRTTXT(x)     std::cout << #x << " = " << x << std::endl;
    #define PRTTXT2(x, y) std::cout <<  x << " = " << y << std::endl;
#else
    #define PRTIMG(x)
    #define PRTTXT(x)
    #define PRTTXT2(x, y)
#endif

class PointDetector::impl
    {
public:
     impl() = default;
    ~impl() = default;
    bool isDetected;
    cv::Mat dataImage;
    std::vector<cv::Point> points;
    };

PointDetector::PointDetector() : pImpl {new impl}
    {
    pImpl->isDetected = false;
    }

PointDetector::~PointDetector()
    {

    }

void
PointDetector::setImage(const cv::Mat& dataImage)
    {
    pImpl->dataImage = dataImage.clone();
    pImpl->isDetected = false;
    }

void
PointDetector::detect()
    {
    if (pImpl->isDetected)
        {
        // Do not have to calculate for same image.
        // Data is cached.
        return;
        }

    if (pImpl->dataImage.empty())
        {
        throw std::invalid_argument
            {"PointDetector::detect(): No input image."};
        }

    PRTIMG(pImpl->dataImage)

    // Make image to gray color.
    cv::Mat grayImage;
    if (pImpl->dataImage.channels() == 3)
        {
        cv::cvtColor(pImpl->dataImage, grayImage, cv::COLOR_BGR2GRAY);
        }
    else
        {
        grayImage = pImpl->dataImage.clone();
        }

    PRTIMG(grayImage)
    // I think blur process is not necessary...
    // The figure of journal is useually clear to figure out.
    // So I did not anyway.
    cv::Mat thresholdImage;
    cv::threshold(grayImage, thresholdImage, 100, 255, cv::THRESH_BINARY);

    std::vector< std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(thresholdImage,
        contours,
        hierarchy,
        CV_RETR_TREE,
        CV_CHAIN_APPROX_SIMPLE,
        cv::Point (0, 0));

    std::vector< std::vector<cv::Point> > polyContours (contours.size());
    std::vector<cv::Rect> boundRect (contours.size());
    for (int i = 0; i < contours.size(); ++i)
        {
        // True means closed polygon.
        cv::approxPolyDP(contours[i], polyContours[i], 3, true);
        boundRect[i] = cv::boundingRect(polyContours[i]);
        }

    // Sort rectangle by x direction.
    std::sort(boundRect.begin(), boundRect.end(),
        [](const cv::Rect& c1, const cv::Rect& c2) {return c1.x < c2.x;});

    // Draw polygonal contour + bonding rects + circles
    cv::RNG rng (12345);
    cv::Mat drawing = cv::Mat::zeros(thresholdImage.size(), CV_8UC3);
    for(int i = 0; i < contours.size(); ++i)
        {
        if (boundRect[i].area() > 500)
            continue;

        cv::Scalar color = 
            cv::Scalar (rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
//        cv::drawContours(drawing, // image
//            polyContours,   // contours
//            i,              // contour index
//            color,          // color
//            1,              // thickness = 1
//            8,              // lineType = LINE_8
//            cv::noArray (), // hierarchy = cv::noArray()
//            0,              // maxLevel = INT_MAX
//            cv::Point ());  // offset = cv::Point ()

        cv::rectangle(drawing, boundRect[i].tl(), boundRect[i].br(),
            color, 2, 8, 0);
        }

    PRTIMG(drawing)

    pImpl->points.resize(0);
    for(int i = 0; i < contours.size(); ++i)
        {
        if (boundRect[i].area() > 500)
            continue;
        // Save center points.
        cv::Point center = (boundRect[i].tl() + boundRect[i].br()) / 2;
        pImpl->points.push_back(center);
        }
    }

std::vector<cv::Point>
PointDetector::getPoints()
    {
    if (not pImpl->isDetected)
        {
        this->detect();
        }

    return pImpl->points;
    }
