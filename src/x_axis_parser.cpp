#include "x_axis_parser.hpp"

//#define DEBUG

#ifdef DEBUG
    #define PRTIMG(x)     cv::imshow(#x, x); cv::waitKey();
    #define PRTTXT(x)     std::cout << #x << " = " << x << std::endl;
    #define PRTIMG2(x, y) cv::imshow(#x, y); cv::waitKey();
    #define PRTTXT2(x, y) std::cout <<  x << " = " << y << std::endl;
#else
    #define PRTIMG(x)
    #define PRTTXT(x)
    #define PRTIMG2(x, y)
    #define PRTTXT2(x, y)
#endif

namespace // Helper functions.
{
void
analyzeNumberImage(const cv::Mat& numberImage,
              double& offsetValue,
              double& pixelWidth)
    {
    // Calculate the width of pixel from figure ticks.
    // Once you get the pixel width, you can calculate the distance between two
    // pixels in real world unit.

    cv::Mat img = numberImage.clone();

    cv::GaussianBlur(img, img, cv::Size (3, 3), 0);

    if (img.channels() == 3)
        cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);

    cv::adaptiveThreshold(
        img, // image source.
        img, // destination.
        255, // max per each pixel.
        cv::ADAPTIVE_THRESH_MEAN_C, // kernel style.
        cv::THRESH_BINARY, // threshold method.
        21,  // Size of a pixel neighborhood.
        10); // Constant subtracted from the mean or weighted mean.

    img = ~img;

    PRTIMG(img);

    // Find initial contours from obtained points.
    std::vector< std::vector<cv::Point> > contours;
    // I do not know what it is.
    std::vector<cv::Vec4i> hierarchy;

    // Make rough contours.
    // Canny edge detector also available?
    cv::findContours(img,
        contours,
        hierarchy,
        cv::RETR_EXTERNAL,           // I do not know.
        cv::CHAIN_APPROX_TC89_KCOS); // I do not know. See the reference.

    // Approximate contours to polygons + get bounding rects and circles.
    // What the..? What does he say...?
    // I think this means the transform contour to some polygon...?
    // Remove noise?
    std::vector< std::vector<cv::Point> > polyContours (contours.size());

    for (int i = 0; i < contours.size(); ++i)
        {
        // true maens closed polygon.
        cv::approxPolyDP(contours[i], polyContours[i], 3, true);
        }

    // Sort poly contour by x direction.
    std::sort(polyContours.begin(), polyContours.end(),
              [](const std::vector<cv::Point>& c1,
                 const std::vector<cv::Point>& c2)
                 {
                 return cv::boundingRect(c1).x < cv::boundingRect(c2).x;
                 });

    // Make rect of polyContour.
    std::vector<cv::Rect> polyContourRects (polyContours.size());
    for (int i = 0; i < polyContours.size(); ++i)
        {
        polyContourRects[i] = cv::boundingRect(polyContours[i]);
        }

#ifdef DEBUG
    // Small points are detected.
    // We can merge this with nearest contours.
    {
    // Display for debug.
    cv::Mat dispImage = img.clone();
    cv::cvtColor(dispImage, dispImage, cv::COLOR_GRAY2BGR);
    int colorInterval = 255 / polyContours.size();
    //for (int i = 0; i < polyContours.size(); i++)
    for (int i = 0; i < polyContourRects.size(); i++)
        {
        //cv::Rect rect = cv::boundingRect(polyContours[i]);

        cv::rectangle(dispImage,
                      polyContourRects[i].tl(),
                      polyContourRects[i].br(),
                      cv::Scalar (0, 255, colorInterval * i), // BGR, so green.
                      2);
        }

    PRTIMG(dispImage)
    }
#endif

    // Merge small contours.

    // If two distance between two adjacent contours is smaller than
    // the width of largest width of contours, they are continuous charaters
    // (this means that they are two characters in a word).

    // Find largest width of contours.
    int maxContourWidth =
        std::max_element(polyContourRects.begin(), polyContourRects.end(),
            [](const cv::Rect& r1, const cv::Rect& r2)
                {
                return r1.width < r2.width;
                }
            )->width;

    PRTTXT(maxContourWidth)

    std::vector< std::vector<cv::Point> > mergedContours {polyContours[0]};
    for (int i = 0; i < polyContours.size() - 1; i++)
        {
        const cv::Rect&  leftRect = polyContourRects[i];
        const cv::Rect& rightRect = polyContourRects[i + 1];

        if (std::abs(leftRect.br().x - rightRect.tl().x) < maxContourWidth)
            {
            // these two characters are in a word.
            // merge contour.
            for (const auto point : polyContours[i + 1])
                mergedContours.back().push_back(point);
            }
        else
            {
            // Prepare next step.
            // Add empty std::vector<cv::Point>.
            mergedContours.push_back(polyContours[i + 1]);
            }
        }

    // Get bounding rects.
    std::vector<cv::Rect> boundRect (mergedContours.size());
    // This is not needed at this time.
    // std::vector<cv::Point> center   (contours.size());
    for (int i = 0; i < mergedContours.size(); ++i)
        boundRect[i] = cv::boundingRect(mergedContours[i]);

#ifdef DEBUG
    {
    // Display for debug.
    cv::Mat dispImage = img.clone();
    cv::cvtColor(dispImage, dispImage, cv::COLOR_GRAY2BGR);
    for (int i = 0; i < mergedContours.size(); i++)
        {
        //if (boundRect[i].area() < 100)
        //    continue;

        cv::rectangle(dispImage,
                      boundRect[i].tl(),
                      boundRect[i].br(),
                      cv::Scalar (0, 255, 0),
                      2);
        }

    PRTIMG(dispImage)
    }
#endif

    // Sort merged contour by x direction.
    std::sort(mergedContours.begin(), mergedContours.end(),
              [](const std::vector<cv::Point>& c1,
                 const std::vector<cv::Point>& c2)
                 {
                 return cv::boundingRect(c1).x < cv::boundingRect(c2).x;
                 });

    tesseract::TessBaseAPI tessBaseAPI;
    tessBaseAPI.Init(nullptr, "eng");

#ifdef DEBUG
    for (int i = 0; i < mergedContours.size(); ++i)
        {
        cv::Mat num = numberImage(cv::boundingRect(mergedContours[i]));
        tessBaseAPI.SetImage(static_cast<uchar*>(num.data),
                        num.size().width,
                        num.size().height,
                        num.channels(),
                        num.step1());
        // Use unique pointer to prevent memory loss.
        std::unique_ptr<char> numberString (tessBaseAPI.GetUTF8Text(),
                                            std::default_delete<char>());
        PRTIMG(num)
        std::stringstream ss;
        double number = 0;
        ss << numberString.get();
        ss >> number;
        std::cout << "Number = " << number << std::endl;
        }
#endif

    // Throw away first and last element.
    // Two points are sufficient to figure out unit system.
    std::vector<double> numValues;
    std::vector<double> numCenters;
    for (auto& i : {1,2})
        {
        cv::Rect rect = cv::boundingRect(mergedContours[i]);
        cv::Mat num  = numberImage(rect);
        tessBaseAPI.SetImage(static_cast<uchar*>(num.data),
                        num.size().width,
                        num.size().height,
                        num.channels(),
                        num.step1());
        // Use unique pointer to prevent memory loss.
        std::unique_ptr<char> numberString (tessBaseAPI.GetUTF8Text(),
                                            std::default_delete<char>());

        std::stringstream ss;
        double number = 0;
        // lexical cast
        ss << numberString.get(); ss >> number;
        numValues.push_back(number);
        numCenters.push_back((rect.x + rect.x + rect.width) / 2);
        }

    pixelWidth  = ( numValues[1]  - numValues[0]) /
                  (numCenters[1] - numCenters[0]);
    offsetValue = numValues[0] - numCenters[0] * pixelWidth;

    //std::cout << "Pixel width  = " << pixelWidth  << std::endl;
    //std::cout << "Offset value = " << offsetValue << std::endl;
    PRTTXT(pixelWidth)
    PRTTXT(offsetValue)

#ifdef DEBUG
    // Simple test... predict fourth contour values.
    cv::Rect rect4 = cv::boundingRect(mergedContours[4]);
    std::cout << "Expected value = 4" << std::endl;
    std::cout << "Result   value = " << offsetValue + (rect4.x +
        rect4.width / 2) * pixelWidth << std::endl;
#endif
    // Neglect! ---------------------------------------------------------
    // This is an test region.
    // New algorithm.
    // Original source from
    // http://stackoverflow.com/questions/23506105/extracting-text-opencv
    // ------------------------------------------------------------------
    }
}

class XAxisParser::impl
    {
public:
     impl() = default;
    ~impl() = default;

    bool isParsed;
    cv::Mat axisImage;
    std::string label;
    // offset value: the real value of 0 pixel in axis image.
    // So real world value = offsetValue + pixel * pixelWidth.
    double offsetValue;
    double pixelWidth;
    };

XAxisParser::XAxisParser() : pImpl {new impl}
    {
    pImpl->isParsed = false;
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
XAxisParser::parse()
    {
    if (pImpl->isParsed)
        {
        // You do not have to parse twice for the same image.
        return;
        }

    if (pImpl->axisImage.empty())
        {
        throw std::invalid_argument
            {"XAxisParser::parse(): No input image."};
        }

    PRTIMG(pImpl->axisImage)

    // Remove unwanted xticks.
    // First, Make image to gray color.
    cv::Mat grayImage;
    if (pImpl->axisImage.channels() == 3)
        {
        cv::cvtColor(pImpl->axisImage, grayImage, cv::COLOR_BGR2GRAY);
        }
    else
        {
        grayImage = pImpl->axisImage.clone();
        }

    PRTIMG(grayImage)

    // Second, Change gray to binary.
    cv::Mat binaryImage;
    cv::adaptiveThreshold(~grayImage,
        binaryImage,
        255,
        cv::ADAPTIVE_THRESH_GAUSSIAN_C,
        cv::THRESH_BINARY_INV,
        3, // Large kernel size to reduce noise (I am not sure).
       -3  // Larage value to remove noise (I am not sure).
        );

    PRTIMG(binaryImage)

    // Seperate image to 3 region.
    // 1. tic region.
    // 2. tic value region.
    // 3. label region.
    int numRows = pImpl->axisImage.rows;

    // Find first non zero row index.
    int firstNonZeroRowIndex = 0;
    for (int rowIndex = 0; rowIndex < numRows; ++rowIndex)
        {
        cv::Mat row = binaryImage.row(rowIndex);
        if (std::count(row.begin<uchar>(), row.end<uchar>(), 0) > 0)
            {
            firstNonZeroRowIndex = rowIndex;
            break;
            }
        }

    bool isTickRegion   = true;
    bool isNumberRegion = false;
    bool isLabelRegion  = false;
    bool isEmptyRegion  = false;
    int tickEndRowIndex     = 0;
    int numberBeginRowIndex = 0;
    int numberEndRowIndex   = 0;
    int labelBeginRowIndex  = 0;
    int labelEndRowIndex    = numRows;

    for (int rowIndex = firstNonZeroRowIndex; rowIndex < numRows; ++rowIndex)
        {
        cv::Mat row = binaryImage.row(rowIndex);
        int numZeros = std::count(row.begin<uchar>(), row.end<uchar>(), 0);

        if (isTickRegion)
            {
            if (not isEmptyRegion && numZeros == 0)
                {
                isEmptyRegion = true;
                tickEndRowIndex = rowIndex - 1;
                }
            else if (isEmptyRegion && numZeros != 0)
                {
                isEmptyRegion = false;
                isTickRegion  = false;
                isNumberRegion = true;
                isLabelRegion  = false;
                numberBeginRowIndex = rowIndex;
                }
            }

        if (isNumberRegion)
            {
            if (not isEmptyRegion && numZeros == 0)
                {
                isEmptyRegion = true;
                numberEndRowIndex = rowIndex - 1;
                }
            else if (isEmptyRegion && numZeros != 0)
                {
                isEmptyRegion = false;
                isTickRegion  = false;
                isNumberRegion = false;
                isLabelRegion  = true;
                labelBeginRowIndex = rowIndex;
                }
            }

        if (isLabelRegion)
            {
            // Nohing to do...
            if (not isEmptyRegion && numZeros == 0)
                {
                isEmptyRegion = true;
                labelEndRowIndex = rowIndex - 1;
                }
            }
        }

    // Include some additional white space.
    numberBeginRowIndex = (numberBeginRowIndex +    tickEndRowIndex) / 2;
    numberEndRowIndex   = (numberEndRowIndex   + labelBeginRowIndex) / 2;
    labelBeginRowIndex  = (numberEndRowIndex   + labelBeginRowIndex) / 2;
    labelEndRowIndex    = (numRows             +   labelEndRowIndex) / 2;

    int numCols = pImpl->axisImage.cols;

    cv::Mat numberImage =
        pImpl->axisImage(cv::Rect(0,
                            numberBeginRowIndex,
                            numCols,
                            numberEndRowIndex - numberBeginRowIndex)).clone();

    PRTIMG(numberImage)

//    // Change image to c string.
//    tessBaseAPI.SetImage(static_cast<uchar*>(numberImage.data),
//                    numberImage.size().width,
//                    numberImage.size().height,
//                    numberImage.channels(),
//                    numberImage.step1());
//    // Use unique pointer to prevent memory loss.
//    std::unique_ptr<char> numberString (tessBaseAPI.GetUTF8Text(),
//                                        std::default_delete<char>());
//
//    std::cout << numberString.get() << std::endl;

    analyzeNumberImage(numberImage, pImpl->offsetValue, pImpl->pixelWidth);

    cv::Mat labelImage =
        pImpl->axisImage(cv::Rect(0,
                            labelBeginRowIndex,
                            numCols,
                            labelEndRowIndex - labelBeginRowIndex)).clone();

    PRTIMG(labelImage)

    tesseract::TessBaseAPI tessBaseAPI;
    tessBaseAPI.Init(nullptr, "eng");
    tessBaseAPI.SetImage(static_cast<uchar*>(labelImage.data),
                    labelImage.size().width,
                    labelImage.size().height,
                    labelImage.channels(),
                    labelImage.step1());
    // Use unique pointer to prevent memory loss.
    std::unique_ptr<char> labelString (tessBaseAPI.GetUTF8Text(),
                                       std::default_delete<char>());
    std::stringstream ss;
    ss << labelString.get();
    // copy labelString to member.
    std::getline(ss, pImpl->label);

    //std::cout << pImpl->label << std::endl;
    PRTTXT(pImpl->label)

    pImpl->isParsed = true;
    }

std::string
XAxisParser::getLabel()
    {
    if (not pImpl->isParsed)
        this->parse();

    return pImpl->label;
    }

double
XAxisParser::getOffsetValue()
    {
    if (not pImpl->isParsed)
        this->parse();

    return pImpl->offsetValue;
    }

double
XAxisParser::getPixelWidth()
    {
    if (not pImpl->isParsed)
        this->parse();

    return pImpl->pixelWidth;
    }
