/*
 issues
 * 1. 如果去掉button，则autoscroll不工作
 * 2. autoscroll的scrollbar大小不对
 * 3. Refresh有闪屏现象
 * 4. 如何让statusbar不随着scrollbar移动？
 */

//http://book.51cto.com/art/200811/98820.htm
//https://msdn.microsoft.com/en-us/library/k0fsyd4e%28v=vs.110%29.aspx
//http://csharphelper.com/blog/2014/10/draw-and-move-line-segments-in-c/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TestGraphics
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void imagePanel1_Load(object sender, EventArgs e)
        {

        }

        private void imagePanel1_Click(object sender, EventArgs e)
        {
            
        }

        private void openImageToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                Bitmap b = new Bitmap(openFileDialog1.FileName);
                //imagePanel1.CanvasSize = b.Size;
                imagePanel1.Image = b;
            }
        }

        private void addLineToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void fileToolStripMenuItem_Click(object sender, EventArgs e)
        {

        }

        private void zoomOutToolStripMenuItem_Click(object sender, EventArgs e)
        {
            imagePanel1.Zoom /= 2;
        }

        private void zoomInToolStripMenuItem_Click(object sender, EventArgs e)
        {
            imagePanel1.Zoom *= 2;
        }
    }
}
