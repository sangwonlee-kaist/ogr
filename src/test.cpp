#include "config.hpp"
#include "graph_spliter.hpp"

int
main(int argc, char* argv[])
    {
    cv::Mat src = cv::imread("test.png");
    GraphSpliter graphSpliter;

    graphSpliter.setImage(src);
    graphSpliter.splitGraph();
    
    return 0;
    }
