#include "config.hpp"
#include "graph_spliter.hpp"
#include "x_axis_parser.hpp"

int
main(int argc, char* argv[])
    {
    cv::Mat src = cv::imread(argv[1]);
    GraphSpliter graphSpliter;

    graphSpliter.setImage(src);

    cv::imshow("src", src);
    //cv::imshow("x axis", graphSpliter.getXAxisImage());

    XAxisParser xAxisParser;

    xAxisParser.setImage(graphSpliter.getXAxisImage());
    xAxisParser.parseAxis();

    cv::waitKey(0);

    return 0;
    }
