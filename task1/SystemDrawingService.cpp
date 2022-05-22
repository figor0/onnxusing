#include <dlib/array2d.h>
#include <dlib/image_loader/load_image.h>
#include <dlib/image_transforms.h>
#include <dlib/image_saver/image_saver.h>

#include "SystemDrawingService.h"

void SystemDrawingService::drawRectangles(const  std::string &source_name,
                                          const  std::string &out_name,
                                          const dlib::bgr_pixel &color,
                                          const std::vector<dlib::rectangle> &rects)
{
    dlib::array2d<dlib::bgr_pixel> image;
    dlib::load_image(image, source_name);

    for (const auto& rect: rects)
    {
        dlib::draw_rectangle(image, rect, color);
    }
    dlib::save_bmp(image, out_name);
}

void SystemDrawingService::saveImageFromFloat(const std::string &outName,
                                              const vector3d<float> &arr,
                                              size_t width,
                                              size_t height)
{

}

std::vector<std::string> split(const std::string& data,
                               const char delimeter)
{
    std::vector<std::string> strings;
    std::istringstream f(data);
    std::string s;
    while ( getline(f, s, delimeter) ) {
        strings.push_back(s);
    }
    return strings;
}

float specialftos(const std::string &str)
{
    float result = 0;
    auto parts = split(str, 'E');
    auto it = std::find(parts[0].begin(), parts[0].end(), ',');
    if ( it != parts[0].end() ){
        *it = '.';
    }
    if ( parts.size() > 1 ){
        int degree = std::stoi(parts[1]);
        float base = std::stof(parts[0]);
        result = base * std::pow(10, degree);
    } else {
        result = std::stof(parts[0]);
    }
    return result;
}

void SystemDrawingService::loadFloatArrFromFile(const std::string &fileName,
                                                vector3d<float> &arr)
{
    std::ifstream reader(fileName);
    if ( !reader.is_open() )
        throw std::runtime_error("reader are't opened");

    assert( arr.size() > 0 );
    float value = 0;
    auto& fillable_array = arr[0];
    for ( size_t i0 = 0; i0 < fillable_array.size(); ++i0 )
    {
        for ( size_t i1 = 0; i1 < fillable_array[i0].size(); ++i1 )
        {
            if ( reader.eof() == true )
                throw std::runtime_error("wrong reader stream size");
            std::string float_str;
            std::getline(reader, float_str);
            value = specialftos(float_str);
            fillable_array[i0][i1] = value;
        }
    }
}
