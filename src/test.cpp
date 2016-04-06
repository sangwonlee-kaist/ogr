#include "config.hpp"
#include "graph_spliter.hpp"
#include "x_axis_parser.hpp"

int
main(int argc, char* argv[])
try {
    cv::Mat src = cv::imread(argv[1]);
    GraphSpliter graphSpliter;

    graphSpliter.setImage(src);

    cv::imshow("src", src);
    cv::imshow("data", graphSpliter.getDataImage());
    cv::imshow("x axis", graphSpliter.getXAxisImage());
    cv::imshow("y axis", graphSpliter.getYAxisImage());

    XAxisParser xAxisParser;

    xAxisParser.setImage(graphSpliter.getXAxisImage());
    xAxisParser.parse();

    return 0;
    }
catch (std::exception& e)
    {
    std::cout << e.what() << std::endl;
    }
