#include "config.hpp"
#include "graph_spliter.hpp"
#include "x_axis_parser.hpp"
#include "point_detector.hpp"

int
main(int argc, char* argv[])
try {
    if (argc < 2)
        {
        std::cout << "Type file name." << std::endl;
        }

    cv::Mat src = cv::imread(argv[1]);

    if (src.empty())
        {
        std::cout << "File " << argv[1] << " does not exist." << std::endl;
        }

    GraphSpliter graphSpliter;

    graphSpliter.setImage(src);
    graphSpliter.split();

    cv::imshow("src image", src);

    cv::imshow("data", graphSpliter.getDataImage());
    cv::imshow("x axis", graphSpliter.getXAxisImage());
    cv::imshow("y axis", graphSpliter.getYAxisImage());

    PointDetector pointDetector;
    pointDetector.setImage(graphSpliter.getDataImage());
    pointDetector.detect();
    //XAxisParser xAxisParser;
    //xAxisParser.setImage(graphSpliter.getXAxisImage());
    //std::cout << "x label is " << xAxisParser.getLabel() << std::endl;

    cv::waitKey(0);

    return 0;
    }
catch (std::exception& e)
    {
    std::cout << e.what() << std::endl;
    }
