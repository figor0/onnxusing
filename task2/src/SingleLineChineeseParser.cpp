#include <neurolang/SingleLineChineeseParser.h>
#include <dlib/image_transforms.h>
#include <neurolang/helpers.h>

namespace neurolang
{
using SLCP = SingleLineChineeseParser;
struct SLCP::Private
{
    RecognitionSettings<wchar_t> settings;
    std::shared_ptr<Ort::Env> env;
    std::shared_ptr<Ort::SessionOptions> options;
    std::unique_ptr<Ort::Session> session;

    std::vector<std::string> input_nodes_names;
    std::vector<std::string> output_nodes_names;

};

constexpr int batch_size = 1;
constexpr int crnn_height = 32;

SLCP::SingleLineChineeseParser()
    :
    m_impl(std::make_unique<Private>())
{
    defaultSetup();
}

SLCP::SingleLineChineeseParser(std::shared_ptr<Ort::Env> env,
                               const RecognitionSettings<wchar_t> &info,
                               std::shared_ptr<Ort::SessionOptions> opt)
    :
    m_impl(std::make_unique<Private>())
{
    setEnv(env);
    setOptions(opt);
    setUpInfo(info);
}

SingleLineChineeseParser::~SingleLineChineeseParser()
{

}

void SLCP::setEnv(std::shared_ptr<Ort::Env> env)
{
    if ( env == nullptr)
        throw std::runtime_error("setEnv - wtf MAN!!!");
    m_impl->env = env;
}

void SLCP::setUpInfo(const RecognitionSettings<wchar_t>& info)
{
    m_impl->settings.model = info.model;
    m_impl->settings.alphabet = info.alphabet;

    m_impl->session = std::make_unique<Ort::Session>(
                                                *m_impl->env,
                                                 m_impl->settings.model.data(),
                                                 m_impl->settings.model.size(),
                                                *m_impl->options);
}

void SLCP::setOptions(std::shared_ptr<Ort::SessionOptions> opt)
{
    if ( opt == nullptr)
        throw std::runtime_error("setOpt - wtf MAN!!!");
    m_impl->options = opt;
}

void SingleLineChineeseParser::defaultSetup()
{
    m_impl->options = std::make_shared<Ort::SessionOptions>();
    m_impl->options->SetInterOpNumThreads(8);
    m_impl->options->SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_BASIC);

    m_impl->env = std::make_shared<Ort::Env>(ORT_LOGGING_LEVEL_WARNING,
                                             "frcnn_test");
}

inline void normalize(dlib::array2d<float>& image)
{
    for (auto& pixel: image)
    {
        pixel = ( pixel / 255.0 - 0.588f) / 0.193f;
    }
}

inline std::vector<float> prepare(const dlib::array2d<float>& data)
{
    decltype (prepare({})) retval;
    retval.reserve(data.nc() * data.nr());
    for (const auto& item: data)
    {
        retval.push_back(item);
    }
    return retval;
}
std::vector<char*> getInputNodeNames(const Ort::Session& session)
{
    /// input_node_names - inn
    std::vector<char*> inn;
    auto inn_size = session.GetInputCount();
    inn.reserve(inn_size);
    Ort::AllocatorWithDefaultOptions allocator;
    for(decltype (inn_size) index = 0; index < inn_size; ++index)
    {
        char* raw_name = session.GetInputName(index, allocator);
        inn.push_back(raw_name);
    }
    return inn;
}

std::vector<char*> getOutputNodeNames(const Ort::Session& session)
{
    std::vector<char*> onn;
    auto onn_size = session.GetInputCount();
    onn.reserve(onn_size);
    Ort::AllocatorWithDefaultOptions allocator;
    for(decltype (onn_size) index = 0; index < onn_size; ++index)
    {
        onn.push_back(session.GetOutputName(index, allocator));
    }
    return onn;
}

std::vector<std::vector<Ort::Value>> runSession(Ort::Session& session,
                                   std::vector<std::vector<float>>& batch_lines,
                                   const Ort::MemoryInfo& info,
                                   std::vector<char*>& input_node_names,
                                   std::vector<char*>& output_node_names)
{
    std::vector<std::vector<Ort::Value>> retval;
    for ( decltype (batch_lines.size()) index = 0;
          index < batch_lines.size();
          ++index )
    {
        std::vector<int64_t> input_node_dims = { batch_size, 1, crnn_height, 146 };
        auto input_tensor = Ort::Value::CreateTensor<float>(
                    info,
                    batch_lines[index].data(),
                    batch_lines[index].size(),
                    input_node_dims.data(),
                    input_node_dims.size());
        assert(input_tensor.IsTensor());
        Ort::RunOptions opt;
        auto output_tensors = session.Run(opt,
                                         input_node_names.data(),
                                         &input_tensor,
                                         1,
                                         output_node_names.data(),
                                         1);
        retval.push_back(std::move(output_tensors));
    }
    return retval;
}

std::basic_string<wchar_t> SLCP::parse(const dlib::array2d<dlib::bgr_pixel> &img)
{
    dlib::array2d<float> grey_img;
    dlib::assign_image(grey_img, img);

    normalize(grey_img);
    auto preparedImage = prepare(grey_img);

    std::vector<std::vector<float>> lines_batch;
    lines_batch.push_back(preparedImage);

    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator,
                                                  OrtMemTypeDefault);

    auto input_node_names = getInputNodeNames(*m_impl->session);
    auto output_node_names = getOutputNodeNames(*m_impl->session);

    auto out_tesors = runSession(*m_impl->session,
                                 lines_batch,
                                 memory_info,
                                 input_node_names,
                                 output_node_names);

    assert(out_tesors.front().front().IsTensor());

    auto& tensor = out_tesors.front().front();
    auto type_info = tensor.GetTensorTypeAndShapeInfo();
    auto shapes = type_info.GetShape();

    float* floatarr_row = tensor.GetTensorMutableData<float>();

    const int alphabet_size = shapes[2];//6736;
    const int line_len = shapes[0];
    vector3d<float> floatarr;
    resize(floatarr, line_len, 1, alphabet_size);

    for (int index = 0; index < line_len; ++index )
    {
        std::copy(floatarr_row + alphabet_size * index,
                  floatarr_row + alphabet_size * (index + 1),
                  floatarr[index][0].data());
    }

    std::wstring result;
    for (size_t i = 0; i < floatarr.size(); ++i)
    {
        auto begin = floatarr[i][0].begin();
        auto end = floatarr[i][0].end();
        size_t max_index = std::min_element(begin, end) - begin;
        result.push_back(m_impl->settings.alphabet[max_index]);
    }
    return result;
}

}
