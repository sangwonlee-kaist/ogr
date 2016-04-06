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
XAxisParser::parseAxis()
    {
    if (pImpl->isParsed)
        {
        // You do not have to parse twice for the same image.
        return;
        }

    if (pImpl->axisImage.empty())
        {
        throw std::invalid_argument
            {"XAxisParser::parseAxis(): No input image."};
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

    int numCols = pImpl->axisImage.cols;
//    cv::rectangle(pImpl->axisImage,
//             cv::Point(0, numberBeginRowIndex),
//             cv::Point(numCols - 1, numberEndRowIndex),
//             cv::Scalar(0, 0, 255));
//
//    cv::rectangle(pImpl->axisImage,
//             cv::Point(0, labelBeginRowIndex),
//             cv::Point(numCols - 1, labelEndRowIndex),
//             cv::Scalar(0, 0, 255));

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

    pImpl->isParsed = true;
    }
