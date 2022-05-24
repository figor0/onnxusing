#include <dlib/algs.h>
#include <dlib/array.h>
#include <dlib/array2d.h>
#include <dlib/image_loader/load_image.h>

#include <onnxruntime/core/session/onnxruntime_cxx_api.h>
#include <codecvt>
#include <neurolang/helpers.h>

#include <neurolang/SingleLineChineeseParser.h>

template<typename Code>
std::string loadAllFile(std::ifstream& reader)
{
    if ( reader.is_open() == false )
        throw std::runtime_error("Unable open file");

    reader.seekg(0, std::ios::end);
    size_t size = reader.tellg();
    std::string result(size, ' ');
    reader.seekg(0);

    reader.read(result.data(), size);
    return result;
}

int main()
{
    std::ifstream model_reader("model_chinese_new.onnx",
                         std::ios_base::binary | std::ios_base::in);

    std::string model = loadAllFile<wchar_t>(model_reader);

    std::wstring chin_alphabet = readAlphabet<wchar_t>("alphabet_chinese.txt");

    SingleLineChineeseParser service;
    service.setUpInfo({model, chin_alphabet});

    dlib::array2d<dlib::bgr_pixel> image;
    dlib::load_image(image, "norm_img_line.png");

    auto result = service.parse(image);

    std::wcout << result << std::endl;

    return 0;
}
