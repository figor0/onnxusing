using System.Collections.Generic;
using System.Drawing;
using System.Linq;

namespace task_for_cpp_test
{
    class RecognitionService
    {
        float LOW_TEXT = 0.4f;
        float LINK_THRESHOLD = 0.4f;
        float TEXT_THRESHOLD = 0.4f;

        int height = 0;
        int width = 0;

        // ссылки на два масива даных полученых в результате работы нейронной сети. Даные сохранены в текстовых файлав. 
        // загружаются в конструкторе
        float[,,] textmap;
        float[,,] linkmap;

        public RecognitionService(string imgName, string textmap_name, string linkmap_name)
        {
            using (Bitmap image = new Bitmap(imgName))
            {
                height = image.Height;//672
                width = image.Width;//1024
            }

            //  INITIAL DATA (WE GOT IT FROM ONNX Runtime session) and image data
            textmap = new float[1, height / 2, width / 2];
            linkmap = new float[1, height / 2, width / 2];

            SystemDrawingService.LoadFloatArrFromFile(textmap_name, textmap, width / 2, height / 2);
            SystemDrawingService.LoadFloatArrFromFile(linkmap_name, linkmap, width / 2, height / 2);
        }

        internal List<Rectangle> GetFragmentsCraft()
        {          
            float[,,] text_score = Calculation.Threshold(textmap, LOW_TEXT, height / 2, width / 2);
            float[,,] link_score = Calculation.Threshold(linkmap, LINK_THRESHOLD, height / 2, width / 2);

#if DEBUG
            // даные после Threshold можно сохранить на диск в виде изображения для наглядности
            SystemDrawingService.SaveImageFromFloat("text_score.jpg", textmap, width / 2, height / 2);
            SystemDrawingService.SaveImageFromFloat("link_score.jpg", linkmap, width / 2, height / 2);
#endif

            // объединяем масивы
            float[,,] text_score_comb = Calculation.Clip(text_score, link_score, height / 2, width / 2);

            // добавляем сверху и слева по одной ячейке для дальнейшей разметки
            float[,,] lim = Calculation.PadInitArray(text_score_comb, height / 2, width / 2);
            int new_h = height / 2 + 1;
            int new_w = width / 2 + 1;

            // разметка
            Calculation.Labeling(lim, new_h, new_w);

            // убираем лишние ячейки которые ранее добавляли для разметки
            float[,,] new_lim = Calculation.PadDeleteArray(lim, new_h, new_w);
            new_h--;
            new_w--;

            // повторная разметка
            int N = Calculation.ReLabeling(new_lim, new_h, new_w);

            // извлечение координат
            return GetBoundingBoxes(new_lim, new_h, new_w, textmap, link_score, text_score);
        }

        /// <summary>
        /// MAIN METHOD
        /// </summary>
        private List<Rectangle> GetBoundingBoxes(float[,,] labels, int height, int width, float[,,] textmap, float[,,] link_score, float[,,] text_score)
        {
            List<Rectangle> rect = new List<Rectangle>();
            List<float> x, y;
            List<CraftComponent> stats = new List<CraftComponent>();

            // уникальные числа в размеченом масиве, отсортированы по возростанию
            float[] unique = Calculation.Unique(labels, height, width);// find all unique vales in labels

            // for each unique value take all values
            // for example value = 2, take all 2 from labels array and remember x, y for them
            for (int i = 0; i < unique.Length; i++)
            {
                (x, y) = GetComponent(labels, height, width, unique[i]);
                if (x.Count() != 0 && y.Count() != 0)
                {
                    CraftComponent component = new CraftComponent
                    {
                        min_x = x.Min(),
                        min_y = y.Min(),
                        width = x.Max() - x.Min() + 1,
                        height = y.Max() - y.Min() + 1,
                        square = x.Count()
                    };

                    stats.Add(component);
                }
            }

            for (int k = 1; k < stats.Count; k++)
            {
                float[,,] segmap = new float[1, height, width];

                int size = stats[k].square;
                if (size < 10)//  if number of labels with some value less then 10, don't take this region
                {
                    continue;
                }

                if (MaxFromTextmap(textmap, labels, height, width, k) < TEXT_THRESHOLD)// max from textmap where labels=k
                {
                    continue;
                }

                // segmap[labels==k] = 255
                Replace(segmap, labels, height, width, k);// replace all vales=l on 255

                // segmap[np.logical_and(link_score==1, text_score==0)] = 0
                for (int f = 0; f < 1; f++)
                {
                    for (int i = 0; i < height; i++)
                    {
                        for (int j = 0; j < width; j++)
                        {
                            if (link_score[f, i, j] == 1 && text_score[f, i, j] == 0)
                            {
                                segmap[f, i, j] = 0;
                            }
                        }
                    }
                }

                // calculate coordinates of rectangles
                int min_x = (int)stats[k].min_x;
                int min_y = (int)stats[k].min_y;
                int w = (int)stats[k].width;
                int h = (int)stats[k].height;

                int niter = (int)(System.Math.Sqrt(size * System.Math.Min(w, h) / (w * h)) * 2);
                int sx = min_x - niter;
                int ex = min_x + w + niter + 1;
                int sy = min_y - niter;
                int ey = min_y + h + niter + 1;

                sx = sx < 0 ? 0 : sx;
                sy = sy < 0 ? 0 : sy;
                ex = ex >= width ? width : ex;
                ey = ey >= height ? height : ey;

                int width1 = ex - sx;
                int height1 = ey - sy;

                //  data was twice less then image size so you need to multiply
                // сохраняем полученые даные в структуру Rectangle и в масив координат
                rect.Add(new Rectangle(sx * 2, sy * 2, width1 * 2, height1 * 2));
            }

            // то что мы хотели получить на выходе - координаты областей
            return rect;
        }

        private static (List<float>, List<float>) GetComponent(float[,,] lim, int height, int width, float value)
        {
            List<float> x = new List<float>();
            List<float> y = new List<float>();

            for (int f = 0; f < 1; f++)
            {
                for (int i = 1; i < height; i++)
                {
                    for (int j = 1; j < width; j++)
                    {
                        if (lim[f, i, j] == value)
                        {
                            x.Add(j);
                            y.Add(i);
                        }
                    }
                }
            }

            return (x, y);
        }

        private static float MaxFromTextmap(float[,,] textmap, float[,,] labels, int height, int width, float valueK)
        {
            List<float> res = new List<float>();
            for (int f = 0; f < 1; f++)
            {
                for (int i = 0; i < height; i++)
                {
                    for (int j = 0; j < width; j++)
                    {
                        if (labels[f, i, j] == valueK)
                        {
                            res.Add(textmap[f, i, j]);
                        }
                    }
                }
            }

            return res.Max();
        }

        private static void Replace(float[,,] segmap, float[,,] labels, int height, int width, float valueK)
        {
            for (int f = 0; f < 1; f++)
            {
                for (int i = 0; i < height; i++)
                {
                    for (int j = 0; j < width; j++)
                    {
                        if (labels[f, i, j] == valueK)
                        {
                            segmap[f, i, j] = 255;
                        }
                    }
                }
            }
        }

    }

}
