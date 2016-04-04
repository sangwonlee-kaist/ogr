#include "axes_parser.hpp"

class AxesParser::impl
    {
public:
     impl() = default;
    ~impl() = default; 
    };

AxesParser::AxesParser() : pImpl {new impl}
    {

    }

AxesParser::~AxesParser()
    {

    }


