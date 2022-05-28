#pragma once
#include <memory>

#include <onnxruntime/core/session/onnxruntime_cxx_api.h>
#include <dlib/array2d.h>

namespace neurolang
{
template<typename Code>
struct RecognitionSettings
{
    std::string model;
    std::basic_string<Code> alphabet;
};

template<typename Code>
class ITextParsingService
{
public:
    virtual void setUpInfo(const RecognitionSettings<Code>& info) = 0;
    virtual void setEnv(std::shared_ptr<Ort::Env> env) = 0;
    virtual void setOptions(std::shared_ptr<Ort::SessionOptions> opt) = 0;

    virtual std::basic_string<Code>
        parse(const dlib::array2d<dlib::bgr_pixel>& image) = 0;
};
}
