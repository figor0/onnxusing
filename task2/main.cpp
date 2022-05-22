#include <dlib/algs.h>
#include <dlib/array.h>
#include <dlib/array2d.h>
#include <dlib/image_loader/load_image.h>

#include <onnxruntime/core/session/onnxruntime_cxx_api.h>

static Ort::Env env(ORT_LOGGING_LEVEL_INFO, "frcnn_test");

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

const int batch_size = 1;
const int crnn_height = 32;

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

int main()
{
//    dlib::array2d<dlib::bgr_pixel> image;
    dlib::array2d<float> image;
    dlib::load_image(image, "norm_img_line.png");

//    dlib::assign_image(greyScaleImage, image);

//    normalize(image);
    auto preparedImgData = prepare(image);
    std::vector<std::vector<float>> lines_batch;
    lines_batch.push_back(preparedImgData);

    Ort::SessionOptions session_options;
    session_options.SetIntraOpNumThreads(8);
    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_BASIC);
    Ort::Session session_(env, "model_chinese_new.onnx", session_options);

    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    auto input_node_names = getInputNodeNames(session_);
    auto output_node_names = getOutputNodeNames(session_);

    auto out_tesors = runSession(session_,
                                 lines_batch,
                                 memory_info,
                                 input_node_names,
                                 output_node_names);

    assert(out_tesors.front().front().IsTensor());

    auto& tensor = out_tesors.front().front();
    auto typeInfo = tensor.GetTensorTypeAndShapeInfo();

    float* data = tensor.GetTensorMutableData<float>();
    return 0;
}
