Для работы необходимо создать приложение типа DynamicLibrary и установить в зависимости библиотеки ONNX runtime и dlib

1. Загрузить изображение в масив dlib::array2d<dlib::bgr_pixel> image

2. Создать сессию ONNX runtime
static Ort::Env env(ORT_LOGGING_LEVEL_FATAL, "frcnn_test");
	Ort::SessionOptions session_options;
	session_options.SetIntraOpNumThreads(8);
	session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_BASIC);
	
	session_ = Ort::Session(env, model.data(), model.size(), session_options);
	size_t num_input_nodes = session_.GetInputCount();
	
	input_node_names_.resize(num_input_nodes);
	output_node_names_.resize(session_.GetOutputCount());

	GetInputOutputNodeNames();

	auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

	// create batch lines
	std::vector<std::vector<float>> batch;
	
	
3. создать масив array2d<float> img, сохранить в него изображение image и нормализировать изображение	

for (float& n : img)
	{
		n = (n / 255.0 - 0.588f) / 0.193f;
	}
	
Полученый результат сохраняем в  std::vector<float>  - это будет даные одной линии для  ONNX runtime.
На вход ONNX runtime принимает батч линий, поэтому сохраняем даную линию в вектор и имеем std::vector<std::vector<float>> batch
	
4. инициализровать ONNX runtime и запустить распознавание
(используйте документацию так тут подано не полное решение а лишь частичные шаги)

BATCH_SIZE = 1
CRNN_HEIGHT = 32
	std::vector<int64_t> input_node_dims = { BATCH_SIZE, 1, CRNN_HEIGHT, (int64)(lines[i].nc() * CRNN_HEIGHT / lines[i].nr()) };
		input_tensor_ = Ort::Value::CreateTensor<float>(
			memory_info,
			batch[i].data(),// тот батч который мы создали в п.3
			batch[i].size(),
			input_node_dims.data(),
			input_node_dims.size());

		assert(input_tensor_.IsTensor());

		output_tensors_ = session_.Run(
			Ort::RunOptions{ nullptr },
			input_node_names_.data(),
			&input_tensor_,
			1,
			output_node_names_.data(),
			1);	
			
5. Получить и интерпретировать результат
auto type_info = output_tensors_.front().GetTensorTypeAndShapeInfo();

	alphabet_size и line_len получаем с output_tensors_
	
	float* floatarr_row = (output_tensors_.front().GetTensorMutableData<float>());
	
	Копируете даные из floatarr_row  в трехмерный масв  floatarr[line_len][1][alphabet_size] - параметры можно получить с output_tensors_

	float*** floatarr = new float** [shapes[0]];
	
	копируем по принципу	
	std::copy(floatarr_row + alphabet_size  i, floatarr_row + alphabet_size  (i + 1), floatarr[i][0]);
	
6. floatarr являет собой масив вероятностей букв алфавита для каждого символа строки
делаем обход циклами
1) каждый символ строки это 0 измерение масива
2) порядковый номер буквы алфавита - [2] в масиве
3) [1]  в масиве это номер батча - всегда будет 1

находим букву с максимальной вероятностбю - значение floatarr[][][]

7. иногда бывает дублирование букв которое нужно будет потом убрать

8. Полученый текст вернуть пользователю (сохранить в файл или просто в переменную)
	
	