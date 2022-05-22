#pragma once

#include <fstream>
#include <dlib/geometry/rectangle.h>
#include "helpers.h"


float specialftos(const std::string& str);
std::vector<std::string> split(const std::string& data,
                               const char delimeter);
class SystemDrawingService
{
public:
    static void drawRectangles(const std::string& source_name,
                               const std::string& out_name,
                               const dlib::bgr_pixel& color,
                               const std::vector<dlib::rectangle>& rects);

/// not used
    static void saveImageFromFloat(const std::string& outName,
                                   const vector3d<float>& arr,
                                   size_t width,
                                   size_t height);
///   not used
//    static Bitmap arrayToBitmap(std::vector<std::vector<unsigned char>> array);


    static void loadFloatArrFromFile(const std::string& fileName,
                                     vector3d<float>& arr);
};
