#include "config.hpp"
#include "graph_spliter.hpp"

int
main(int argc, char* argv[])
    {
    cv::Mat src = cv::imread("test.png");
    GraphSpliter graphSpliter;

    graphSpliter.setImage(src);
   
    cv::imshow("data", graphSpliter.getDataImage());
    cv::imshow("x axis", graphSpliter.getXAxisImage());
    cv::imshow("y axis", graphSpliter.getYAxisImage());
 
    cv::waitKey(0);

    return 0;
    }
