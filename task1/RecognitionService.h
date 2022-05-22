#pragma once
#include <inttypes.h>
#include <string>
#include <vector>
#include "helpers.h"
#include <list>

#include <dlib/image_loader/load_image.h>
#include <dlib/array2d.h>
#include <dlib/utility.h>

#include "SystemDrawingService.h"
#include "Calculation.h"
#include <algorithm>

class RecognitionService
{
public:
    RecognitionService(const std::string& imageFilePath,
                       const std::string& textMapFilePath,
                       const std::string& linkMapFilePath);
    std::vector<dlib::rectangle> getFragmentsCraft();
private:
    std::vector<dlib::rectangle> getBoudingBoxes(const vector3d<float>& labels,
                                               size_t height,
                                               size_t width,
                                               const vector3d<float>& textmap,
                                               const vector3d<float>& link_score,
                                               const vector3d<float>& text_score);
    static std::pair<std::vector<float>, std::vector<float>> getComponent(const vector3d<float>& lim,
                                                                      size_t height,
                                                                      size_t width,
                                                                      float value);
    static float maxFromTextMap(const vector3d<float>& textMap,
                                const vector3d<float>& labels,
                                size_t height,
                                size_t width,
                                float value);
    //\valueKey equvalence
    static void replace( vector3d<float>& segmap,
                         const vector3d<float>& labels,
                         const size_t height,
                         const size_t width,
                         const float floatValueK);

    const float m_low_text = 0.4f;
    const float m_link_threshold = 0.4f;
    const float m_text_threshold = 0.4f;
    size_t m_height = 0;
    size_t m_width = 0;

    vector3d<float> m_textmap;
    vector3d<float> m_linkmap;
};
