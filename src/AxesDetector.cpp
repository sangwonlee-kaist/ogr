#include "AxesDetector.hpp"

class AxesDetector::impl
    {
public:
     impl() = default;
    ~impl() = default;
    int a;
    };

AxesDetector::AxesDetector()
    :
    pImpl {new impl}
    {
    pImpl->a = 0;
    }

AxesDetector::~AxesDetector()
    {

    }

void
AxesDetector::getGraphImage()
    {
    ++(pImpl->a);
    std::cout << 2.0 * (pImpl->a) << std::endl;
    }

