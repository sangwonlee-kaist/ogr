#include "graph_splitter.hpp"

class GraphSplitter::impl
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

GraphSplitter::GraphSplitter() : pImpl {new impl}
    {
    pImpl->isSplited = false;
    }

GraphSplitter::~GraphSplitter()
    {

    }

void
GraphSplitter::setImage(const cv::Mat& graphImage)
    {
    // In OpenCV, = operator of cv::Mat is assignment of ptr.
    // (in real, assignment of reference... might be...)
    // So if you want real copy of image, you must use .clone()
    // or .copyTo() of cv::Mat.
    pImpl->graphImage = graphImage.clone();
    // you have to split for new image.
    pImpl->isSplited = false;
    }

void
GraphSplitter::split()
    {
    if (pImpl->isSplited)
        {
        // You do not have to split twice for the same image.
        // Everything is cached.
        // So nothing to do.
        return;
        }

    // algorithm here!
    // If input image is color image,
    // change it to gray image.
    cv::Mat grayImage;
    if (pImpl->graphImage.channels() == 3)
        {
        cv::cvtColor(pImpl->graphImage, grayImage, cv::COLOR_BGR2GRAY);
        }
    else
        {
        grayImage = pImpl->graphImage.clone();
        }

    DEBUG_SHOW_IMG(grayImage)
    // Make binary image of gray image.

    cv::Mat binaryImage;
    cv::adaptiveThreshold(~grayImage,
        binaryImage,
        255,
        cv::ADAPTIVE_THRESH_MEAN_C,
        cv::THRESH_BINARY,
        5,
       -2);

    DEBUG_SHOW_IMG(binaryImage)

    // Size unit = Fixel.

    // Create horizontal line image...
    cv::Mat horizontalImage = binaryImage.clone();
    // 33.3% of the graph height.
    int minHorizontalSize   = (pImpl->graphImage).cols / 3;

    cv::Mat horizontalLine =
        cv::getStructuringElement(cv::MORPH_RECT,
                                  cv::Size(minHorizontalSize, 1));

    cv::erode(horizontalImage, horizontalImage,
              horizontalLine , cv::Point(-1, -1));

    cv::dilate(horizontalImage, horizontalImage,
               horizontalLine , cv::Point(-1, -1));

    horizontalImage = ~horizontalImage;
    DEBUG_SHOW_IMG(horizontalImage)

    // Create vertical line image...
    cv::Mat verticalImage = binaryImage.clone();
    // 33.3% of the graph width.
    int minVerticalSize   = (pImpl->graphImage).rows / 3;

    cv::Mat verticalLine =
        cv::getStructuringElement(cv::MORPH_RECT,
                                  cv::Size(1, minVerticalSize));

    cv::erode(verticalImage, verticalImage,
              verticalLine , cv::Point(-1, -1));

    cv::dilate(verticalImage, verticalImage,
               verticalLine , cv::Point(-1, -1));

    verticalImage = ~verticalImage;
    DEBUG_SHOW_IMG(verticalImage)

    // Find longest lines.
    int numRows     = pImpl->graphImage.rows;
    int numHalfRows = numRows / 2;
    int xAxisRowIndex = 0;
    int maxNumZeros = 0;

    // Find longest horizontal line at bottom side.
    // Searching direction: bottom to top.
    maxNumZeros = 0;
    for (int rowIndex = numRows - 1; rowIndex >= numHalfRows; --rowIndex)
        {
        cv::Mat row  = horizontalImage.row(rowIndex);
        int numZeros = std::count(row.begin<uchar>(), row.end<uchar>(), 0);

        if (numZeros >= maxNumZeros)
            {
            maxNumZeros = numZeros;
            xAxisRowIndex = rowIndex;
            }
        }

    int dataImageWidth =  maxNumZeros;

    // For invaild input image.
    if (dataImageWidth == 0)
        {
        throw std::invalid_argument
            {"GraphSplitter::split(): No x axis in graph."};
        }

    int numCols     = pImpl->graphImage.cols;
    int numHalfCols = numCols / 2;
    int yAxisColIndex = 0;
    // Find longest vertical line at left side.
    // Searching direction: left to right.
    maxNumZeros = 0;
    for (int colIndex = 0; colIndex < numHalfCols; ++colIndex)
        {
        cv::Mat col = verticalImage.col(colIndex);
        int numZeros = std::count(col.begin<uchar>(), col.end<uchar>(), 0);

        if (numZeros > maxNumZeros)
            {
            maxNumZeros = numZeros;
            yAxisColIndex = colIndex;
            }
        }

    int dataImageHeight = maxNumZeros;

    // For invaild input image.
    if (dataImageHeight == 0)
        {
        throw std::invalid_argument
            {"GraphSplitter::split(): No y axis in graph."};
        }

    // Check whether top frame exists or not.
    bool isTopFrameExist = false;
    int topFrameRowIndex = 0;

    maxNumZeros = 0;
    for (int rowIndex = 0; rowIndex < numHalfRows; ++rowIndex)
        {
        cv::Mat row  = horizontalImage.row(rowIndex);
        int numZeros = std::count(row.begin<uchar>(), row.end<uchar>(), 0);

        if (numZeros >= maxNumZeros)
            {
            maxNumZeros = numZeros;
            topFrameRowIndex = rowIndex;
            }
        }

    if (maxNumZeros > 0.9 * dataImageWidth)
        {
        isTopFrameExist = true;
        // More precise height of data image.
        dataImageHeight  = xAxisRowIndex - topFrameRowIndex;
        }

    // Check whether right frame exists or not.
    bool isRightFrameExist = false;
    int rightFrameColIndex = 0;

    maxNumZeros = 0;
    for (int colIndex = numCols - 1; colIndex >= numHalfCols; --colIndex)
        {
        cv::Mat col  = verticalImage.col(colIndex);
        int numZeros = std::count(col.begin<uchar>(), col.end<uchar>(), 0);

        if (numZeros >= maxNumZeros)
            {
            maxNumZeros = numZeros;
            rightFrameColIndex = colIndex;
            }
        }

    if (maxNumZeros > 0.9 * dataImageHeight)
        {
        isRightFrameExist = true;
        // More precise width of data image.
        dataImageWidth  = rightFrameColIndex - yAxisColIndex;
        }

    /*          top
     *     +------------+
     *   l |            | r
     *   e |            | i
     *   f |    data    | g
     *   t |  (image)   | h
     *     |            | t
     *     +------------+
     *         bottom
     */

    int dataBottomRowIndex = xAxisRowIndex;
    int dataTopRowIndex    = xAxisRowIndex - dataImageHeight;
    int dataLeftColIndex   = yAxisColIndex;
    int dataRightColIndex  = yAxisColIndex + dataImageWidth;

    // Extract data image.
    pImpl->dataImage =
        pImpl->graphImage(cv::Rect(dataLeftColIndex, dataTopRowIndex,
                                   dataImageWidth,   dataImageHeight)).clone();

    DEBUG_SHOW_IMG(pImpl->dataImage)

    // Extract x axis image.
    int xAxisImageWidth  = dataImageWidth;
    int xAxisImageHeight = numRows - dataBottomRowIndex;

    pImpl->xAxisImage =
        pImpl->graphImage(cv::Rect(dataLeftColIndex,
                                   dataBottomRowIndex,
                                   xAxisImageWidth,
                                   xAxisImageHeight)).clone();

    DEBUG_SHOW_IMG(pImpl->xAxisImage)

    // Extract y axis image.
    int yAxisImageWidth  = dataLeftColIndex;
    int yAxisImageHeight = dataImageHeight;

    pImpl->yAxisImage =
        pImpl->graphImage(cv::Rect(0,
                                   dataTopRowIndex,
                                   yAxisImageWidth,
                                   yAxisImageHeight)).clone();

    DEBUG_SHOW_IMG(pImpl->yAxisImage)

    pImpl->isSplited = true;
    }

cv::Mat
GraphSplitter::getXAxisImage()
    {
    if (not (pImpl->isSplited))
        this->split();
    // I'm not sure what is the proper way
    // gives result image as an output.
    // I'm not use xAxisImage directly.
    return pImpl->xAxisImage.clone();
    }

cv::Mat
GraphSplitter::getYAxisImage()
    {
    if (not (pImpl->isSplited))
        this->split();

    return pImpl->yAxisImage.clone();
    }

cv::Mat
GraphSplitter::getDataImage()
    {
    if (not (pImpl->isSplited))
        this->split();

    return pImpl->dataImage.clone();
    }
