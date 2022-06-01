#include "Calculation.h"
#include <iostream>
#include <set>

//ok
vector3d<float> Calculation::threshold(const vector3d<float> &map,
                                       float customThresh,
                                       size_t height,
                                       size_t width)
{
    vector3d<float> score;
    resize(score, 1, height, width, 0.0f);
//    assert(hasSameSizes(score, map));
    for ( size_t i0 = 0; i0 < score.size(); ++i0)
    {
        for ( size_t i1 = 0; i1 < score[i0].size(); ++i1 )
        {
            for ( size_t i2 = 0; i2 < score[i0][i1].size(); ++i2 )
            {
                score[i0][i1][i2] = map.at(i0).at(i1).at(i2) < customThresh ? 0 : 1;
            }
        }
    }
    return score;
}

//ok
vector3d<float> Calculation::clip(const vector3d<float> &text_score,
                                  const vector3d<float> &link_score,
                                  size_t height,
                                  size_t width)
{
    vector3d<float> text_score_comb;
    resize(text_score_comb, 1, height, width, 0.0f);

//    assert(hasSameSizes(text_score, text_score_comb));
//    assert(hasSameSizes(link_score, text_score_comb));

    for ( size_t i0 = 0; i0 < text_score_comb.size(); ++i0)
    {
        for ( size_t i1 = 0; i1 < text_score_comb[i0].size(); ++i1)
        {
            for ( size_t i2 = 0; i2 < text_score_comb[i0][i1].size(); ++i2)
            {
                auto score  = text_score.at(i0).at(i1).at(i2)
                        + link_score.at(i0).at(i1).at(i2);
                if ( score  <= 0 ) {
                    score = 0;
                } else if ( score >= 1 ){
                    score = 1;
                }
                text_score_comb[i0][i1][i2] = score;
            }
        }
    }
    return text_score_comb;
}

//ok
vector3d<float> Calculation::padInitArray(const vector3d<float> &map,
                                          size_t height,
                                          size_t width)
{
    vector3d<float> lim;
    resize(lim, 1, height + 1, width + 1, 0.0f);
//    assert(hasSameSizes(map, 1, height, width));

    for (size_t i0 = 0; i0 < lim.size(); ++i0)
    {
        for ( size_t i1 = 0; i1 + 1 < lim[i0].size(); ++i1)
        {
            for ( size_t i2 = 0; i2 + 1 < lim[i0][i1].size(); ++i2)
            {
                float result = map.at(i0).at(i1).at(i2) == 0 ? -1 : -5;
                lim[i0][i1+1][i2+1] = result;
            }
        }
    }

    for ( size_t i0 = 0; i0 < lim.size(); ++i0)
    {
        for ( size_t i1 = 0; i1 < height; ++i1 )
        {
            lim[i0][i1][0] = -1;
        }
        for ( size_t i2 = 0; i2 < width; ++i2)
        {
            lim[i0][0][i2] = -1;
        }
    }
    return lim;
}
//ok
vector3d<float> Calculation::padDeleteArray(const vector3d<float> &map,
                                            size_t height,
                                            size_t width)
{
    vector3d<float> lim;
    resize(lim, 1, height - 1, width - 1, 0.0f);
//    assert(hasSameSizes(map, 1, height, width));

    for (size_t i0 = 0; i0 < lim.size(); ++i0)
    {
        for ( size_t i1 = 0; i1 < lim[i0].size(); ++i1)
        {
            for ( size_t i2 = 0; i2 < lim[i0][i1].size(); ++i2)
            {
                float map_score = map.at(i0).at(i1 + 1).at(i2 + 1);
                float score = 0;
                if ( map_score != -1 ){
                    score = map_score;
                }
                lim[i0][i1][i2] = score;
            }
        }
    }
    return lim;
}
//ok
void Calculation::labeling(vector3d<float> &lim,
                           size_t height,
                           size_t width)
{
    float L = 0;
    for (size_t i0 = 0; i0 < lim.size(); ++i0)
    {
        for ( size_t i1 = 1; i1 < lim[i0].size(); ++i1)
        {
            for ( size_t i2 = 1; i2 < lim[i0][i1].size(); ++i2)
            {
                float cp = lim[i0][i1][i2];
                if ( cp == -5 )
                {
                    float v1 = lim[i0][i1 - 1][i2];
                    float v2 = lim[i0][i1][i2 - 1];

                    if ( v2 > 0 ) {
                        lim[i0][i1][i2] = v2;
                        if ( v1 > 0 && (v1 != v2) ){
                            replace(lim, height, width, v2, v1);
                        }
                    } else if ( v1 > 0 && v2 < 0){
                        lim[i0][i1][i2] = v1;
                    } else {
                        ++L;
                        lim[i0][i1][i2] = L;
                    }
                }
            }
        }
    }
}
//ok
int Calculation::reLabeling(vector3d<float> &lim,
                            size_t height,
                            size_t width)
{
    std::vector<float> unique_data = unique(lim, height, width);
    assert(unique_data.size() > 0);

    size_t N = unique_data.size() - 1;
    size_t k = 0;

    for ( size_t v = 0; v < N + 1; ++v)
    {
        float searched  = unique_data[v];
        if ( searched != 0 ) {
            replace(lim, height, width, searched, k);
            ++k;
        }
    }
    return unique_data.size();
}
#include <iostream>
//ok
std::vector<float> Calculation::unique(const vector3d<float> &lim,
                                       size_t height,
                                       size_t width)
{
//    assert(hasSameSizes(lim, 1, height, width));
    std::set<float> unique;
    for (size_t i0 = 0; i0 < 1; ++i0)
    {
        for ( size_t i1 = 0; i1 < height; ++i1)
        {
            for ( size_t i2 = 0; i2 < width; ++i2)
            {
                float cp = lim[i0][i1][i2];
                unique.insert(cp);
            }
        }
    }
    return {unique.begin(), unique.end()};
}
//ok
void Calculation::replace(vector3d<float> &lim,
                          size_t height,
                          size_t width,
                          float equal,
                          float replace)
{
    for (size_t i0 = 0; i0 < 1; ++i0)
    {
        for ( size_t i1 = 1; i1 < height; ++i1)
        {
            for ( size_t i2 = 1; i2 < width; ++i2)
            {
                if ( lim[i0][i1][i2] == equal ){
                    lim[i0][i1][i2] = replace;
                }
            }
        }
    }
}
