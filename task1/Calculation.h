#pragma once

#include <vector>
#include <assert.h>
#include <unordered_set>
#include "helpers.h"

void print( const std::vector<float>& data);
void print(const std::vector<std::vector<float>>& data);

class Calculation
{
public:
    static vector3d<float> threshold( const vector3d<float>& map,
                      float customThresh,
                      size_t height,
                      size_t width);
    static vector3d<float> clip( const vector3d<float>& text_score,
                                 const vector3d<float>& link_score,
                                 size_t height,
                                 size_t width);
    static vector3d<float> padInitArray(const vector3d<float>& map,
                                        size_t height,
                                        size_t width);
    static vector3d<float> padDeleteArray(const vector3d<float>& map,
                                          size_t height,
                                          size_t width);

    static void labeling(vector3d<float>& lim,
                         size_t height,
                         size_t width);
    static int reLabeling(vector3d<float>& lim,
                          size_t height,
                          size_t width);
    static std::vector<float> unique(const vector3d<float>& lim,
                                size_t height,
                                size_t width);

private:
    static void replace(vector3d<float>& lim,
                        size_t height,
                        size_t width,
                        float equal,
                        float replace);

};
