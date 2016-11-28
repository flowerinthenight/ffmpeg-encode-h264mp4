using NSH264Encoder;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace H264MP4Encode
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            int width = 640;
            int height = 480;
            int fps = 30;
            H264Encoder enc = new H264Encoder();
            enc.SetupEncode(@"c:\users\idril\downloads\encoded.mp4", width, height, fps);

            Bitmap image = new Bitmap(width, height);
            Graphics g = Graphics.FromImage(image);
            g.FillRectangle(Brushes.Black, 0, 0, width, height);
            Brush[] brushList = new Brush[] { Brushes.Green, Brushes.Red, Brushes.Yellow, Brushes.Pink, Brushes.LimeGreen };
            Random rnd = new Random();

            for (int i = 0; i < 180; i++)
            {
                int rndTmp = rnd.Next(1, 3);
                g.FillRectangle(brushList[i % 5], (i % width) * 2, (i % height) * 0.5f, i % 30, i % 30);
                g.FillRectangle(brushList[i % 5], (i % width) * 2, (i % height) * 2, i % 30, i % 30);
                g.FillRectangle(brushList[i % 5], (i % width) * 0.5f, (i % height) * 2, i % 30, i % 30);
                g.Save();

                Rectangle rect = new Rectangle(0, 0, image.Width, image.Height);
                BitmapData bmpData = image.LockBits(rect, ImageLockMode.ReadOnly, PixelFormat.Format24bppRgb);

                // Get the address of the first line.
                IntPtr ptr = bmpData.Scan0;

                // Declare an array to hold the bytes of the bitmap.
                int bytes = Math.Abs(bmpData.Stride) * image.Height;
                byte[] rgbValues = new byte[bytes];

                // Copy the RGB values into the array.
                System.Runtime.InteropServices.Marshal.Copy(ptr, rgbValues, 0, bytes);

                enc.WriteFrame(rgbValues);

                // Unlock the bits.
                image.UnlockBits(bmpData);
            }

            enc.CloseEncode();
            MessageBox.Show("Test encode done.");
        }
    }
}
