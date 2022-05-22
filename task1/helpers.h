#pragma once
#include <vector>

template<typename T>
using vector2d = std::vector<std::vector<T>>;

template <typename T>
using vector3d = std::vector<vector2d<T>>;

template<typename T>
void resize(vector2d<T>& data, size_t first_dim, size_t second_dim)
{
    data.resize(first_dim);
    for (auto& item: data){
        item.resize(second_dim);
    }
}

template<typename T>
void resize(vector3d<T>& data,
            size_t first_dim,
            size_t second_dim,
            size_t thirst_dim)
{
    data.resize(first_dim);
    for(auto& item: data){
        resize(item, second_dim, thirst_dim);
    }
}

template <typename T>
bool hasSameSizes(const vector2d<T>& left,
                 const vector2d<T>& right)
{
    bool result = left.size() == right.size();

    if ( result == true ){
        for (decltype (left.size()) index = 0;
             index < left.size() && result == true;
             ++index)
        {
            result = left[index].size() == right[index].size();
        }
    }
    return result;
}

template <typename T>
bool hasSameSizes(const vector3d<T>& left,
                 const vector3d<T>& right)
{
    bool result = left.size() == right.size();
    if ( result == true ){
        for (decltype (left.size()) index = 0;
             index < left.size() && result == true;
             ++index)
        {
            result = hasSameSizes(left[index], right[index]);
        }
    }
    return result;
}


template <typename T>
bool hasSameSizes( const vector2d<T>& origin,
                   size_t first_dim,
                   size_t second_dim )
{
    bool result = origin.size() == first_dim;

    if ( result == true ){
        for (decltype (origin.size()) index = 0;
             index < origin.size() && result == true;
             ++index)
        {
            result = origin[index].size() == second_dim;
        }
    }
    return result;
}


template <typename T>
bool hasSameSizes( const vector3d<T>& origin,
                   size_t first_dim,
                   size_t second_dim,
                   size_t thirth_dim)
{
    bool result = origin.size() == first_dim;

    if ( result == true ){
        for (decltype (origin.size()) index = 0;
             index < origin.size() && result == true;
             ++index)
        {
            result = hasSameSizes(origin.at(index), second_dim, thirth_dim);
        }
    }
    return result;
}

struct CraftComponent
{
    float min_x;
    float min_y;
    float width;
    float height;
    int square;
};
