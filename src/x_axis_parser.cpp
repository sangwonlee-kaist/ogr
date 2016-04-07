#include "x_axis_parser.hpp"

//#define DEBUG

#ifdef DEBUG
    #define PRTIMG(x) cv::imshow(#x, x); cv::waitKey(0);
#else
    #define PRTIMG(x)
#endif


namespace // Helper functions.
{
double
getFixelWidth(const cv::Mat& numberImage)
    {
    // Calculate the width of fixel from figure ticks.
    // Once you get the fixel width, you can calculate the distance between two
    // fixels in real world uint.

    cv::Mat img = numberImage.clone();
//
//    cv::GaussianBlur(img, img, cv::Size (3, 3), 0);
//
//    if (img.channels() == 3)
//        cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
//
//    cv::adaptiveThreshold(
//        img, // image source.
//        img, // destination.
//        255, // max per each fixel.
//        cv::ADAPTIVE_THRESH_MEAN_C, // kernel style.
//        cv::THRESH_BINARY, // threshold method.
//        21,  // Size of a pixel neighborhood.
//        10); // Constant subtracted from the mean or weighted mean.
//
//    img = ~img;
//
//    PRTIMG(img);
//
//    std::vector<cv::Point> numberPoints;
//
//    {
//    auto it  = img.begin<uchar>();
//    auto end = img.end<uchar>();
//
//    for (; it != end; ++it)
//        if (*it != 0) numberPoints.push_back(it.pos());

    // New algorithm.
    // Original source from
    // http://stackoverflow.com/questions/23506105/extracting-text-opencv

    

    }


    return 0.0;
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

    // Change image to c string.
    tesseract::TessBaseAPI tessBaseAPI;
    tessBaseAPI.Init(nullptr, "eng");
    tessBaseAPI.SetImage(static_cast<uchar*>(numberImage.data),
                    numberImage.size().width,
                    numberImage.size().height,
                    numberImage.channels(),
                    numberImage.step1());
    // Use unique pointer to prevent memory loss.
    std::unique_ptr<char> numberString (tessBaseAPI.GetUTF8Text(),
                                        std::default_delete<char>());

    std::cout << numberString.get() << std::endl;
    std::cout << "Label width = " << getFixelWidth(numberImage) << std::endl;

    cv::Mat labelImage =
        pImpl->axisImage(cv::Rect(0,
                            labelBeginRowIndex,
                            numCols,
                            labelEndRowIndex - labelBeginRowIndex)).clone();

    PRTIMG(labelImage)

    tessBaseAPI.SetImage(static_cast<uchar*>(labelImage.data),
                    labelImage.size().width,
                    labelImage.size().height,
                    labelImage.channels(),
                    labelImage.step1());
    // Use unique pointer to prevent memory loss.
    std::unique_ptr<char> labelString (tessBaseAPI.GetUTF8Text(),
                                       std::default_delete<char>());

    // copy labelString to member.
    pImpl->label = std::string(labelString.get());

    std::cout << pImpl->label << std::endl;

    pImpl->isParsed = true;
    }

std::string
XAxisParser::getLabel()
    {
    if (not pImpl->isParsed)
        this->parse();

    return pImpl->label;
    }
