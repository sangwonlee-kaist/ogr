# Makefile for opencv3 with tesseract-ocr.

CXX        = g++
OPT_LEVEL  = -O3
STD_LEVEL  = -std=c++11
TESS_DIR   = -I /usr/local/include/tesseract \
             -ltesseract
LEPT_DIR   = -I /usr/local/include/leptonica \
             -llept
OPENCV_INCDIR = -I /usr/local/include/opencv2
OPENCV_LIBS   = -lopencv_core       \
                -lopencv_highgui    \
                -lopencv_ml         \
                -lopencv_imgproc    \
                -lopencv_imgcodecs

CXXFLAGS = $(STD_LEVEL)  \
           $(OPT_LEVEL)  \
           $(TESS_DIR)   \
           $(LEPT_DIR)   \
           $(OPENCV_INCDIR) \
           $(OPENCV_LIBS)

TARGET   = test.x

all : $(TARGET)

$(TARGET) : test.cpp graph_spliter.o axes_parser.o
	$(CXX) $^ $(CXXFLAGS) -o $@

graph_spliter.o : graph_spliter.hpp graph_spliter.cpp
	$(CXX) -c graph_spliter.cpp $(OPT_LEVEL) $(STD_LEVEL) $(OPENCV_INCDIR)

axes_parser.o : axes_parser.hpp axes_parser.cpp
	$(CXX) -c axes_parser.cpp $(OPT_LEVEL) $(STD_LEVEL) $(OPENCV_INCDIR)

clean:
	rm *.o $(TARGET)
