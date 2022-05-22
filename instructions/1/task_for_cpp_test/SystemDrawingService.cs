using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.IO;

namespace task_for_cpp_test
{
    class SystemDrawingService
    {       

        internal static void DrawRectangles(string sourceName,
            string outName, Color c, List<Rectangle> rects)
        {
            using (Bitmap bmp = new Bitmap(sourceName))
            {
                Graphics gBmp = Graphics.FromImage(bmp);
                gBmp.CompositingMode = CompositingMode.SourceCopy;
                Pen pen = new Pen(c, 1f);
                foreach (Rectangle rect in rects)
                {
                    gBmp.DrawRectangle(pen, rect);
                }
                bmp.Save($"../{outName}");
            }
        }

        // internal static void SaveImageFromFloat (string outName,
        //                                          float[,,] arr,
        //                                          int width,
        //                                          int height)
        // {
        //     byte[][,] out_img = new byte[3][,];
        //     for (int ch = 0; ch < 3; ++ch)
        //     {
        //         out_img[ch] = new byte[height, width];
        //     }

        //     for (int i = 0; i < width; i++)
        //     {
        //         for (int j = 0; j < height; j++)
        //         {
        //             out_img[0][j, i] = (byte)(arr[0, j, i] * 255);
        //         }
        //     }


        //     using (Bitmap bmp = ArrayToBitmap(out_img))
        //     {
        //         bmp.Save($"../{outName}");
        //     }
        // }

        // internal static Bitmap ArrayToBitmap(byte[][,] array)
        // {
        //     int height = array[0].GetLength(0);
        //     int width = array[0].GetLength(1);
        //     int channel = array.Length;

        //     Bitmap bmp = new Bitmap(width, height, PixelFormat.Format32bppArgb);
        //     BitmapData bd = bmp.LockBits(new Rectangle(0, 0, width, height), ImageLockMode.WriteOnly, PixelFormat.Format32bppArgb);
        //     int nOffset = (bd.Stride - bd.Width * 4);

        //     unsafe
        //     {
        //         byte* p = (byte*)bd.Scan0;

        //         for (int y = 0; y < height; y++)
        //         {
        //             for (int x = 0; x < width; x++)
        //             {
        //                 if (channel == 1)
        //                 {
        //                     p[0] = p[1] = p[2] = (byte)Math.Min(Math.Max(array[0][y, x], Byte.MinValue), Byte.MaxValue);
        //                     p[3] = 255;
        //                 }
        //                 else
        //                 {
        //                     p[0] = (byte)Math.Min(Math.Max(array[0][y, x], Byte.MinValue), Byte.MaxValue);
        //                     p[1] = (byte)Math.Min(Math.Max(array[1][y, x], Byte.MinValue), Byte.MaxValue);
        //                     p[2] = (byte)Math.Min(Math.Max(array[2][y, x], Byte.MinValue), Byte.MaxValue);
        //                     p[3] = 255;
        //                 }
        //                 p += 4;
        //             }
        //             p += nOffset;
        //         }
        //     }

        //     bmp.UnlockBits(bd);
        //     return bmp;
        // }

        internal static void LoadFloatArrFromFile(string fileName, float[,,] arr, int width, int height)
        {
            int i = 0, j = 0;
            using (FileStream file = File.Open(fileName, FileMode.Open))
            {
                using (StreamReader reader = new StreamReader(file))
                {
                    foreach (float value in arr)
                    {
                        string line = reader.ReadLine();
                        arr[0,i,j] =  float.Parse(line);

                        j++;
                        if(j == width)
                        {
                            j = 0;
                            i++;
                        }

                    }
                }
            }
        }
    }
}
