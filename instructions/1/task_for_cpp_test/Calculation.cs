using System.Collections.Generic;

namespace task_for_cpp_test
{

    static class Calculation
    {
        const int on_pixel = 255;
        const int off_pixel = 0;

        internal static float[,,] Threshold(float[,,] map, float customThresh, int height, int width)
        {
            float[,,] score = new float[1, height, width];

            for (int i = 0; i < 1; i++)
            {
                for (int j = 0; j < height; j++)
                {
                    for (int k = 0; k < width; k++)
                    {
                        score[i, j, k] = map[i, j, k] < customThresh ? 0 : 1;
                    }
                }
            }

            return score;
        }

        internal static float[,,] Clip( float[,,] text_score,
                                        float[,,] link_score,
                                        int height,
                                        int width)
        {
            float[,,] text_score_comb = new float[1, height, width];

            for (int i = 0; i < 1; i++)
            {
                for (int j = 0; j < height; j++)
                {
                    for (int k = 0; k < width; k++)
                    {
                        text_score_comb[i, j, k] = (text_score[i, j, k] + link_score[i, j, k]);
                        if (text_score_comb[i, j, k] <= 0)
                        {
                            text_score_comb[i, j, k] = 0;
                        }

                        if (text_score_comb[i, j, k] >= 1)
                        {
                            text_score_comb[i, j, k] = 1;
                        }
                    }
                }
            }

            return text_score_comb;
        }

        internal static float[,,] PadInitArray(float[,,] map, int height, int width)
        {
            float[,,] lim = new float[1, height + 1, width + 1];

            for (int i = 0; i < 1; i++)
            {
                for (int j = 0; j < height; j++)
                {
                    for (int k = 0; k < width; k++)
                    {
                        lim[i, j + 1, k + 1] = map[i, j, k] == 0 ? -1 : -5;
                    }
                }
            }

            for (int i = 0; i < 1; i++)
            {
                for (int j = 0; j < height; j++)
                {
                    lim[i, j, 0] = -1;
                }

                for (int k = 0; k < width; k++)
                {
                    lim[i, 0, k] = -1;
                }
            }

            return lim;
        }

        internal static float[,,] PadDeleteArray(float[,,] map, int height, int width)
        {
            float[,,] lim = new float[1, height - 1, width - 1];

            for (int i = 0; i < 1; i++)
            {
                for (int j = 0; j < height - 1; j++)
                {
                    for (int k = 0; k < width - 1; k++)
                    {
                        lim[i, j, k] = map[i, j + 1, k + 1] == -1 ? 0 : map[i, j + 1, k + 1];
                    }
                }
            }

            return lim;
        }

        internal static void Labeling(float[,,] lim, int height, int width)
        {
            float L = 0;
            for (int f = 0; f < 1; f++)
            {
                for (int i = 1; i < height; i++)
                {
                    for (int j = 1; j < width; j++)
                    {
                        float cp = lim[f, i, j];
                        if (cp == -5)
                        {
                            float v1 = lim[f, i - 1, j];
                            float v2 = lim[f, i, j - 1];

                            if (v2 > 0)
                            {
                                lim[f, i, j] = v2;
                                if (v1 > 0 && (v1 != v2))
                                {
                                    Replace(lim, height, width, v2, v1);
                                }
                            }
                            else if (v1 > 0 && v2 < 0)
                            {
                                lim[f, i, j] = v1;
                            }
                            else
                            {
                                L++;
                                lim[f, i, j] = L;
                            }
                        }
                    }
                }
            }
        }

        private static void Replace(float[,,] lim, int height, int width, float equal, float replace)
        {
            for (int f = 0; f < 1; f++)
            {
                for (int i = 1; i < height; i++)
                {
                    for (int j = 1; j < width; j++)
                    {
                        if (lim[f, i, j] == equal)
                        {
                            lim[f, i, j] = replace;
                        }
                    }
                }
            }
        }

        internal static int ReLabeling(float[,,] lim, int height, int width)
        {
            float[] unique = Unique(lim, height, width);

            int N = unique.Length - 1;
            int k = 0;

            for (int v = 0; v < N + 1; v++)
            {
                float searched = unique[v];
                if (searched != 0)
                {
                    Replace(lim, height, width, searched, k);
                }

                k++;
            }

            return unique.Length;
        }

        internal static float[] Unique(float[,,] lim, int height, int width)
        {
            HashSet<float> unique = new HashSet<float>();
            for (int f = 0; f < 1; f++)
            {
                for (int i = 0; i < height; i++)
                {
                    for (int j = 0; j < width; j++)
                    {
                        float cp = lim[f, i, j];
                        unique.Add(cp);
                    }
                }
            }

            float[] arr = new float[unique.Count];
            unique.CopyTo(arr);
            return arr;
        }

    }

}
