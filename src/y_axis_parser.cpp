#include "y_axis_parser.hpp"

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
              double& fixelHeight)
    {
    // Calculate the Height of fixel from figure ticks.
    // Once you get the fixel height, 
    // you can calculate the distance between two
    // fixels in real world unit.

    cv::Mat img = numberImage.clone();

    cv::GaussianBlur(img, img, cv::Size (3, 3), 0);

    if (img.channels() == 3)
        cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);

    cv::adaptiveThreshold(
        img, // image source.
        img, // destination.
        255, // max per each fixel.
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

    cv::findContours(img,
        contours,
        hierarchy,
        cv::RETR_EXTERNAL,          // I do not know.
        cv::CHAIN_APPROX_TC89_KCOS); // I do not know. See the reference.

    // Approximate contours to polygons + get bounding rects and circles.
    // What the..? What does he say...?
    // I think this means the transform contour to some polygon...?
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
                 return cv::norm(cv::boundingRect(c1).br()) < 
                        cv::norm(cv::boundingRect(c2).br());
                 });    // Merge small contours.

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

        cv::Point  leftBr {leftRect.br()};
        cv::Point rightBl {rightRect.tl().x, rightRect.br().y};

        if (cv::norm(leftBr - rightBl) < maxContourWidth)
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
#endif

    // Sort merged contour by y direction.
    std::sort(mergedContours.begin(), mergedContours.end(),
              [](const std::vector<cv::Point>& c1,
                 const std::vector<cv::Point>& c2)
                 {
                 return cv::boundingRect(c1).y > cv::boundingRect(c2).y;
                 });

    tesseract::TessBaseAPI tessBaseAPI;
    tessBaseAPI.Init(nullptr, "eng");

#ifdef DEBUG
    for (int i = 0; i < mergedContours.size(); ++i)
        {
        cv::Mat num = numberImage(cv::boundingRect(mergedContours[i]));
        cv::resize(num, num, cv::Size (100, 100));
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
        cv::resize(num, num, cv::Size (100, 100));
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
        numCenters.push_back((rect.y + rect.y + rect.height) / 2);
        }

    fixelHeight  = ( numValues[1]  - numValues[0]) /
                  (numCenters[1] - numCenters[0]);
    offsetValue = numValues[0] - numCenters[0] * fixelHeight;

    //std::cout << "Fixel Height  = " << fixelHeight  << std::endl;
    //std::cout << "Offset value = " << offsetValue << std::endl;
    PRTTXT(fixelHeight)
    PRTTXT(offsetValue)

#ifdef DEBUG
    // Simple test... predict fourth contour values.
    cv::Rect rect4 = cv::boundingRect(mergedContours[3]);
    std::cout << "Expected value = 4" << std::endl;
    std::cout << "Result   value = " << offsetValue + (rect4.y +
        rect4.height / 2) * fixelHeight << std::endl;
#endif
    // Neglect! ---------------------------------------------------------
    // This is an test region.
    // New algorithm.
    // Original source from
    // http://stackoverflow.com/questions/23506105/extracting-text-opencv
    // ------------------------------------------------------------------
    }
}

class YAxisParser::impl
    {
public:
     impl() = default;
    ~impl() = default;

    bool isParsed;
    cv::Mat axisImage;
    std::string label;
    // offset value: the real value of 0 fixel in axis image.
    // So real world value = offsetValue + fixel * fixelHeight.
    double offsetValue;
    double fixelHeight;
    };

YAxisParser::YAxisParser() : pImpl {new impl}
    {
    pImpl->isParsed = false;
    }

YAxisParser::~YAxisParser()
    {

    }

void
YAxisParser::setImage(const cv::Mat& axisImage)
    {
    pImpl->axisImage = axisImage.clone();
    // You have to parse for new image.
    pImpl->isParsed = false;
    }

void
YAxisParser::parse()
    {
    if (pImpl->isParsed)
        {
        // You do not have to parse twice for the same image.
        return;
        }

    if (pImpl->axisImage.empty())
        {
        throw std::invalid_argument
            {"YAxisParser::parse(): No input image."};
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
    int numCols = pImpl->axisImage.cols;


    std::vector<int> zeroHistogram (numCols);

    for (int colIndex = 0; colIndex < numCols; ++colIndex)
        {
        cv::Mat col = binaryImage.col(colIndex);
        zeroHistogram[colIndex] =
            std::count(col.begin<uchar>(), col.end<uchar>(), 0);
        } 

#ifdef DEBUG
    {
    int maxHistogram = *std::max_element(zeroHistogram.begin(), 
                                         zeroHistogram.end());
    
    cv::Mat canvas = cv::Mat::ones (maxHistogram, numCols, CV_8UC3);

    for (int i = 0; i < numCols; ++i)
        {
        cv::line(canvas, cv::Point(i, maxHistogram), 
                         cv::Point(i, maxHistogram - zeroHistogram[i]),
                         cv::Scalar(0, 255, 0));
        }

    cv::imshow("zero histogram on y axis", canvas); cv::waitKey();
    } 
#endif

    std::vector<int> zeroSignals (zeroHistogram.size());
    std::transform(zeroHistogram.begin(), zeroHistogram.end(), // Source.
                   zeroSignals.begin(), // Destination. 
                   [](int val) 
                       {
                       return (val > 0 ? 1 : 0);
                       });

    std::vector<int> signalDiffs (zeroSignals.size());
    std::adjacent_difference(zeroSignals.begin(), zeroSignals.end(),
                             signalDiffs.begin());

    int tickEndColIndex = numCols;
    std::vector< std::pair<int, int> > regionBeginEnd;     
    {
    int sigBeg, sigEnd;
    for (int i = 0; i < signalDiffs.size(); ++i)
        {
        const int& sig = signalDiffs[i];
        if (sig == 1)
            {
            sigBeg = i;
            }
        else if (sig == -1)
            {
            sigEnd = i;
            regionBeginEnd.push_back({sigBeg, sigEnd});
            sigBeg = sigEnd = 0;
            }

        if (i == signalDiffs.size() - 1)
            {
            if (sigBeg != 0 and sigEnd == 0)
                tickEndColIndex = sigBeg;
            }
        }    
    }

    auto maxRegionPairIterator =
        max_element(regionBeginEnd.begin(), regionBeginEnd.end(), 
            [](const std::pair<int, int>& p1, const std::pair<int, int>& p2)
                {
                return p1.second - p1.first < p2.second - p2.first;
                });

    int maxRegionWidth = maxRegionPairIterator->second - 
                         maxRegionPairIterator->first;

    PRTTXT(maxRegionWidth)

    // Merge small regions.
    std::vector< std::pair<int, int> > mergedRegionBeginEnd;
    {
    int beg = regionBeginEnd[0].first;
    int end = regionBeginEnd[0].second;
    for (int i = 0; i < regionBeginEnd.size() - 1; ++i)
        {
        std::pair<int, int>&  leftPair = regionBeginEnd[i];
        std::pair<int, int>& rightPair = regionBeginEnd[i + 1];

        // Merge them but do not put to array 
        // (possibility of additional merging).
        PRTTXT(rightPair.first - leftPair.second)
        if (rightPair.first - leftPair.second < maxRegionWidth / 3)
            {
            end = rightPair.second; 
            }
        else
            {
            mergedRegionBeginEnd.push_back({beg, end});
            // prepair next step.
            beg = rightPair.first;
            end = rightPair.second;
            }

        if (i == regionBeginEnd.size() - 2)
            {
            mergedRegionBeginEnd.push_back({beg, end});
            }
        }
    }

    if (mergedRegionBeginEnd.size() != 2)
        {
        throw std::logic_error 
            {"YAxisParser::parse(): region seperation fails."};
        }
    
#ifdef DEBUG

    for (auto& pa : mergedRegionBeginEnd)
        std::cout << pa.first << ", " << pa.second << std::endl;

#endif

    int numberBeginColIndex = mergedRegionBeginEnd[1].second;
    int numberEndColIndex   = mergedRegionBeginEnd[1].first;
    int labelBeginColIndex  = mergedRegionBeginEnd[0].second;
    int labelEndColIndex    = mergedRegionBeginEnd[0].first;
    // Include some additional white space.
    numberBeginColIndex = (numberBeginColIndex +    tickEndColIndex) / 2;
    numberEndColIndex   = (numberEndColIndex   + labelBeginColIndex) / 2;
    labelBeginColIndex  = (numberEndColIndex   + labelBeginColIndex) / 2;
    labelEndColIndex    = (                        labelEndColIndex) / 2;

    int numRows = pImpl->axisImage.rows;

    cv::Mat numberImage =
        pImpl->axisImage(cv::Rect(numberEndColIndex,
                            0,
                            numberBeginColIndex - numberEndColIndex,
                            numRows)).clone();
    PRTIMG(numberImage)

    analyzeNumberImage(numberImage, pImpl->offsetValue, pImpl->fixelHeight);

    cv::Mat labelImage =
        pImpl->axisImage(cv::Rect(labelEndColIndex,
                            0,
                            labelBeginColIndex - labelEndColIndex,
                            numRows)).clone();
    // Lotate 90 degree for clockwise.
    cv::flip(labelImage.t(), labelImage, 1);

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
YAxisParser::getLabel()
    {
    if (not pImpl->isParsed)
        this->parse();

    return pImpl->label;
    }

double
YAxisParser::getOffsetValue()
    {
    if (not pImpl->isParsed)
        this->parse();

    return pImpl->offsetValue;
    }

double
YAxisParser::getFixelHeight()
    {
    if (not pImpl->isParsed)
        this->parse();

    return pImpl->fixelHeight;
    }
