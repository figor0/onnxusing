using System;
using System.Collections.Generic;
using System.Drawing;

namespace task_for_cpp_test
{
    class Program
    {
        // Дано набор даных (полученых с тестового изображения) )и ряд алгоритмов, применив которые к этим даным, можно получить
        // координаты областей с текстом на тестовом изображении

        // Необходимо портировать код на С++
        // Консольное приложение на С++ должно считать даные с файлов, обработать их и вернуть результат в виде координат
        // В идеале результат необходимо отрисовать на изображении
        // Для работы с изображениями мы используем Dlib библиотеку
        // Так же можно обойтись без отрисовки, тогда ширина и высота изображения (тут) задаются константами
        // Результат можно сравнить с результатом в папке results В корне проектаю
        static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");

            // стартовое предобработаное изображение (используется тут для наглядности, отрисовки результата,
            // извлечения параметров высоты и ширины
            string imgFileName = "resources/test_img.png";

            // результат работы нейронной сети для даного изображения - два масива с Float числами, сохранен в соответсвующие файлы
            // в конструкторе RecognitionService масивы считаны из файлов, для дальнейшей обработки алгоритмами
            string textmap_name = "resources/textmap.txt";
            string linkmap_name = "resources/linkmap.txt";

            RecognitionService service = new RecognitionService(imgFileName, textmap_name, linkmap_name);

            // основная работа над даными, в следствие которой должен быть список (масив) координат областей с текстом (словами)
            List<Rectangle> rectangles = service.GetFragmentsCraft();

            // для наглядности отрисовываем координаты областей в виде прямоугольников на базовом изображении
            // как вариант вывести в консоль
            SystemDrawingService.DrawRectangles(imgFileName, "craft_rectangles.jpg", Color.Green, rectangles);
        }
    }
}

