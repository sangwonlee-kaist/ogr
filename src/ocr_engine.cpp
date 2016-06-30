#include "ocr_engine.hpp"

#include "leptonica/allheaders.h"
#include "tesseract/baseapi.h"

class OcrEngine::impl
    {
public:
     impl() = default;
    ~impl() = default;

    tesseract::TessBaseAPI* tessBaseAPI;
    cv::Mat textImage;
    };

OcrEngine::OcrEngine() : pImpl {new impl}
    {
    pImpl->tessBaseAPI = new tesseract::TessBaseAPI {};
    if (pImpl->tessBaseAPI->Init(nullptr, "eng"))
        {
        throw std::runtime_error
            {"OcrEngine::OcrEngine(): Init tesseract fails"};
        }
    }

OcrEngine::~OcrEngine()
    {
    pImpl->tessBaseAPI->End();
    }

void
OcrEngine::setImage(const cv::Mat& textImage)
    {
    // real memory copy
    pImpl->textImage = textImage.clone();
    // send image memory to tesseract api.
    pImpl->tessBaseAPI->SetImage(
        static_cast<uchar*>(pImpl->textImage.data),
        pImpl->textImage.size().width,
        pImpl->textImage.size().height,
        pImpl->textImage.channels(),
        pImpl->textImage.step1());
    }

std::string
OcrEngine::getText()
    {
    char* rawText = pImpl->tessBaseAPI->GetUTF8Text();
    // copy to string.
    std::string text {rawText}; 
    // release used memory
    delete[] rawText;

    return text;
    }