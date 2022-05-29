#pragma once
#include <neurolang/ITextParsingService.h>

namespace neurolang
{

class SingleLineChineeseParser: public ITextParsingService<wchar_t>
{
public:
    SingleLineChineeseParser();
    SingleLineChineeseParser(std::shared_ptr<Ort::Env> env,
                             const RecognitionSettings<wchar_t>& info,
                             std::shared_ptr<Ort::SessionOptions> opt);
    ~SingleLineChineeseParser();
    void setEnv(std::shared_ptr<Ort::Env> env) override;
    void setUpInfo(const RecognitionSettings<wchar_t> &info) override;
    void setOptions(std::shared_ptr<Ort::SessionOptions> opt) override;

    std::wstring parse(const dlib::array2d<dlib::bgr_pixel> &image) override;
private:
    void allRelease();
    std::vector<char*> getInputNodeNames(const Ort::Session& session);
    std::vector<char*> getOutputNodeNames(const Ort::Session& session);
    struct Private;
    void defaultSetup();
    std::unique_ptr<Private> m_impl;
};

}
