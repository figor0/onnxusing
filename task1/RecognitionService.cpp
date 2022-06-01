#include "RecognitionService.h"
#include <string>

RecognitionService::RecognitionService(const std::string &imageFilePath,
                                       const std::string &textMapFilePath,
                                       const std::string &linkMapFilePath)
{
    dlib::array2d<dlib::bgr_pixel> bitmap;
    dlib::load_image(bitmap, imageFilePath);

    m_height = bitmap.nr();
    m_width = bitmap.nc();

    size_t half_height = m_height/2;
    size_t half_width = m_width/2;

    resize(m_textmap, 1, half_height, half_width, 0.0f);
    resize(m_linkmap, 1, half_height, half_width, 0.0f);

    SystemDrawingService::loadFloatArrFromFile(textMapFilePath,
                                               m_textmap);
    SystemDrawingService::loadFloatArrFromFile(linkMapFilePath,
                                               m_linkmap);
}

///\todo use N
std::vector<dlib::rectangle> RecognitionService::getFragmentsCraft()
{
    auto text_score = Calculation::threshold(m_textmap,
                                             m_low_text,
                                             m_height/2,
                                             m_width/2);
    auto link_score = Calculation::threshold(m_linkmap,
                                             m_link_threshold,
                                             m_height/2,
                                             m_width/2);
//#if DEBUG
    SystemDrawingService::saveImageFromFloat("text_score",
                                             m_textmap,
                                             m_width/2,
                                             m_height/2);
    SystemDrawingService::saveImageFromFloat("link_score",
                                             m_linkmap,
                                             m_width/2,
                                             m_height/2);
//#endif
    auto text_score_comb = Calculation::clip(text_score,
                                             link_score,
                                             m_height/2,
                                             m_width/2);
    auto lim = Calculation::padInitArray(text_score_comb,
                                         m_height/2,
                                         m_width/2);
    size_t new_h = m_height/2 + 1;
    size_t new_w = m_width/2 + 1;

    Calculation::labeling(lim, new_h, new_w);
    auto new_lim = Calculation::padDeleteArray(lim, new_h, new_w);
    new_h--;
    new_w--;

    size_t N = Calculation::reLabeling(new_lim, new_h, new_w);
    (void)N;
    return getBoudingBoxes(new_lim,
                           new_h,
                           new_w,
                           m_textmap,
                           link_score,
                           text_score);
}

std::vector<dlib::rectangle>
        RecognitionService::getBoudingBoxes( const vector3d<float> &labels,
                                             size_t height,
                                             size_t width,
                                             const vector3d<float> &textmap,
                                             const vector3d<float> &link_score,
                                             const vector3d<float> &text_score)
{
    std::vector<dlib::rectangle> rects;
    //        std::vector<float> x,y;
    std::vector<CraftComponent> stats;

    std::vector<float> unique = Calculation::unique(labels, height, width);
    stats.reserve(unique.size());

    for (size_t i = 0; i < unique.size(); ++i)
    {
        auto unique_elem = unique[i];
        auto [x, y] = getComponent(labels, height, width, unique_elem);
        if ( x.size() != 0 && y.size() != 0 )
        {
            int min_x = *std::min_element(x.begin(), x.end());
            int max_x = *std::max_element(x.begin(), x.end());

            int min_y = *std::min_element(y.begin(), y.end());
            int max_y = *std::max_element(y.begin(), y.end());

            CraftComponent component;
            component.min_x = min_x;
            component.min_y = min_y;

            component.width = max_x - min_x + 1;
            component.height = max_y - min_y + 1;

            component.square = x.size();

            stats.push_back(component);
        }
    }
    for (size_t k = 1; k < stats.size(); ++k)
    {
        vector3d<float> segmap;
        resize(segmap, 1, height, width, 0.0f);

        int size = stats[k].square;
        if ( size < 10 ){
            continue;
        }

        float max_from_text_map = maxFromTextMap(textmap,
                                                 labels,
                                                 height,
                                                 width,
                                                 k);
        if ( max_from_text_map < m_text_threshold )
        {
            continue;
        }

        replace(segmap, labels, height, width, k);

        for (size_t i0 = 0; i0 < 1; ++i0)
        {
            for ( size_t i1 = 0; i1 < height; ++i1)
            {
                for (size_t i2 = 0; i2 < width; ++i2)
                {
                    float link_value = link_score[i0][i1][i2];
                    float text_value = text_score[i0][i1][i2];
                    if ( link_value == 1 && text_value == 0 )
                    {
                        segmap[i0][i1][i2] = 0;
                    }
                }
            }
        }
        int min_x = stats[k].min_x;
        int min_y = stats[k].min_y;
        int w = stats[k].width;
        int h = stats[k].height;

        int niter = std::sqrt(size * std::min(w, h) / (w * h)) * 2;
        int sx = min_x - niter;
        int ex = min_x + w + niter + 1;
        int sy = min_y - niter;
        int ey = min_y + h + niter + 1;

        sx = sx < 0 ? 0 : sx;
        sy = sy < 0 ? 0 : sy;
        ex = ex >= (int)width ? (int)width : ex;
        ey = ey >= (int)height ? (int)height : ey;

//        int width1 = ex - sx;
//        int height1 = ey - sy;
        auto rect = dlib::rectangle{sx * 2, sy * 2, ex * 2, ey * 2};
        rects.push_back(rect);
    }
    return rects;
}

std::pair<std::vector<float>, std::vector<float> >
    RecognitionService::getComponent(const vector3d<float> &lim,
                                     size_t height,
                                     size_t width,
                                     float value)
{
    std::pair<std::vector<float>, std::vector<float>> retval;
    auto &x = retval.first;
    auto &y = retval.second;

    assert(hasSameSizes(lim, 1, height, width));
    for (size_t i0 = 0; i0 < lim.size() ; ++i0)
    {
        for ( size_t i1 = 0; i1 < lim[i0].size(); ++i1)
        {
            for ( size_t i2 = 0; i2 < lim[i0][i1].size(); ++i2)
            {
                float lim_value = lim[i0][i1][i2];
                if ( lim_value == value ){
                    x.push_back(i2);
                    y.push_back(i1);
                }
            }
        }
    }
    return retval;
}

float RecognitionService::maxFromTextMap(const vector3d<float> &textMap,
                                         const vector3d<float> &labels,
                                         size_t height,
                                         size_t width,
                                         float valueK)
{
    std::vector<float> retval;
    retval.reserve(height * width);

    assert(hasSameSizes(labels, 1, height, width));
    assert(hasSameSizes(textMap, 1, height, width));
    for (size_t i0 = 0; i0 < labels.size() ; ++i0)
    {
        for ( size_t i1 = 0; i1 < labels[i0].size(); ++i1)
        {
            for ( size_t i2 = 0; i2 < labels[i0][i1].size(); ++i2)
            {
                if ( labels[i0][i1][i2] == valueK){
                    retval.push_back(textMap[i0][i1][i2]);
                }
            }
        }
    }
    float max_value = 0;
    auto max_value_it = std::max_element(retval.begin(), retval.end());
    if ( max_value_it != retval.end() ){
        max_value = *max_value_it;
    }
    return max_value;
}

void RecognitionService::replace(vector3d<float> &segmap,
                                 const vector3d<float> &labels,
                                 const size_t height,
                                 const size_t width,
                                 const float floatValueK)
{
    assert(hasSameSizes(labels, 1, height, width));
    assert(hasSameSizes(segmap, 1, height, width));
    for (size_t i0 = 0; i0 < labels.size() ; ++i0)
    {
        for ( size_t i1 = 0; i1 < labels[i0].size(); ++i1)
        {
            for ( size_t i2 = 0; i2 < labels[i0][i1].size(); ++i2)
            {
                if ( labels[i0][i1][i2] == floatValueK){
                    segmap[i0][i1][i2] = 255;
                }
            }
        }
    }
}
