#include "../ocr_engine.hpp"

int
main(int, char* [])
    {
    cv::Mat textImage;

    OcrEngine ocrEngine;
    // Sample #1
    textImage = cv::imread("sample.png");
    ocrEngine.setImage(textImage);
    std::cout << ocrEngine.getText() << std::endl;
    // Sample #2
    textImage = cv::imread("sample2.png");
    ocrEngine.setImage(textImage);
    std::cout << ocrEngine.getText() << std::endl;

    std::cin.get();

    return 0;
    }