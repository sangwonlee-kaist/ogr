# Makefile for opencv3 with tesseract-ocr.

CXX        = g++
OPT_LEVEL  = -O3
STD_LEVEL  = -std=c++11 #-DDEBUG

TESS_INCDIR = -I /usr/local/include
TESS_LIBS   = -ltesseract
LEPT_INCDIR = -I /usr/local/include
LEPT_LIBS   = -llept
OPENCV_INCDIR = -I /usr/local/include/opencv2
OPENCV_LIBS   = -lopencv_core       \
                -lopencv_highgui    \
                -lopencv_ml         \
                -lopencv_imgproc    \
                -lopencv_imgcodecs

CXXFLAGS = $(STD_LEVEL)     \
           $(OPT_LEVEL)     \
           $(TESS_INCDIR)   \
           $(TESS_LIBS)     \
           $(LEPT_INCDIR)   \
           $(LEPT_LIBS)     \
           $(OPENCV_INCDIR) \
           $(OPENCV_LIBS)

TARGET   = test.x

all : $(TARGET)

debug: CXX+=-DDEBUG
debug: all

$(TARGET) : test.cpp graph_spliter.o x_axis_parser.o point_detector.o \
                                     y_axis_parser.o
	$(CXX) $^ $(CXXFLAGS) -o $@

graph_spliter.o : graph_spliter.hpp graph_spliter.cpp
	$(CXX) -c graph_spliter.cpp $(OPT_LEVEL) $(STD_LEVEL) $(OPENCV_INCDIR)

x_axis_parser.o : x_axis_parser.hpp x_axis_parser.cpp
	$(CXX) -c x_axis_parser.cpp $(OPT_LEVEL) $(STD_LEVEL) $(OPENCV_INCDIR)    \
                                $(TESS_INCDIR) $(LEPT_INCDIR)

point_detector.o: point_detector.hpp point_detector.cpp
	$(CXX) -c point_detector.cpp $(OPT_LEVEL) $(STD_LEVEL) $(OPENCV_INCDIR)   \
                                 $(TESS_INCDIR) $(LEPT_INCDIR)

y_axis_parser.o : y_axis_parser.hpp y_axis_parser.cpp
	$(CXX) -c y_axis_parser.cpp $(OPT_LEVEL) $(STD_LEVEL) $(OPENCV_INCDIR)    \
                                $(TESS_INCDIR) $(LEPT_INCDIR)

clean:
	rm *.o $(TARGET)
