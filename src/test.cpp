#include "config.hpp"
#include "graph_splitter.hpp"
#include "line_point_splitter.hpp"
#include "x_axis_parser.hpp"
#include "point_detector.hpp"
#include "y_axis_parser.hpp"

int
main(int argc, char* argv[])
try {
    std::cout << "Type file name." << std::endl;
    std::string filename;
    std::getline(std::cin, filename);

    cv::Mat src = cv::imread(filename);

    if (src.empty())
        {
        std::cout << "File " << filename << " does not exist." << std::endl;
        return 1;
        }

    cv::imshow("input image", src);
    cv::waitKey();

    GraphSplitter graphSplitter;

    graphSplitter.setImage(src);
//    graphSplitter.split();

//    cv::imshow("src image", src);
//
//    cv::imshow("data", graphSplitter.getDataImage());
//    cv::imshow("x axis", graphSplitter.getXAxisImage());
//    cv::imshow("y axis", graphSplitter.getYAxisImage());

    XAxisParser xAxisParser;
    xAxisParser.setImage(graphSplitter.getXAxisImage());
    //std::cout << "x label is " << xAxisParser.getLabel() << std::endl;

    YAxisParser yAxisParser;
    yAxisParser.setImage(graphSplitter.getYAxisImage());
    //std::cout << "y label is " << yAxisParser.getLabel() << std::endl;

    double xOffset     = xAxisParser.getOffsetValue();
    double pixelWidth = xAxisParser.getPixelWidth();

    double yOffset     = yAxisParser.getOffsetValue();
    double pixelHeight = yAxisParser.getPixelHeight();


    LinePointSplitter linePointSplitter;
    linePointSplitter.setImage(graphSplitter.getDataImage());
    PointDetector pointDetector;
    pointDetector.setImage(linePointSplitter.getPointImage());

    std::vector<cv::Point> points;
    points = pointDetector.getPoints();

    std::cout << std::setw(10) << xAxisParser.getLabel() <<
                 std::setw(10) << yAxisParser.getLabel() <<
                 std::endl;
    std::cout << "--------------------" << std::endl;
    for (auto& point : points)
        std::cout << std::setw(10)                     <<
                     std::setprecision(2)              <<
                     std::setiosflags(std::ios::fixed) <<
                     xOffset + point.x * pixelWidth    <<
                     std::setw(10)                     <<
                     std::setprecision(2)              <<
                     std::setiosflags(std::ios::fixed) <<
                     yOffset + point.y * pixelHeight   <<
                     std::endl;
    cv::waitKey();

    return 0;
    }
catch (std::exception& e)
    {
    std::cout << e.what() << std::endl;
    }
