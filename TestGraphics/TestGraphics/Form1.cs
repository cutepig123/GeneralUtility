/*
 issues
 * 1. 如果去掉button，则autoscroll不工作
 * 2. autoscroll的scrollbar大小不对
 * 3. Refresh有闪屏现象
 * 4. 如何让statusbar不随着scrollbar移动？
 */

//http://book.51cto.com/art/200811/98820.htm
//https://msdn.microsoft.com/en-us/library/k0fsyd4e%28v=vs.110%29.aspx

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
            mScale = 1;
            this.AutoScroll = true;
            this.KeyPreview = true;
            mImage = new Bitmap("1.bmp");
        }

        Image mImage;
        double mScale;

        protected override void OnScroll(ScrollEventArgs se)
        {
            base.OnScroll(se);
            Invalidate();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            Graphics dc = e.Graphics;
            e.Graphics.ScaleTransform((float)mScale, (float)mScale);
            e.Graphics.TranslateTransform (-this.HorizontalScroll.Value, -this.VerticalScroll.Value);
            e.Graphics.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor;

            e.Graphics.DrawImage(
                mImage,
                new Rectangle(10, 10, mImage.Width, mImage.Height),  // destination rectangle  
                0,
                0,           // upper-left corner of source rectangle
                mImage.Width,       // width of source rectangle
                mImage.Height,      // height of source rectangle
                GraphicsUnit.Pixel,
                null);

            Pen bluePen = new Pen(Color.Blue, 3);
            dc.DrawRectangle(bluePen, 0, 0, 50, 50);
            Pen redPen = new Pen(Color.Red, 2);
            dc.DrawEllipse(redPen, 0, 50, 80, 60);

        }

        void Scale(bool isInc)
        {
            if(isInc)
                mScale = mScale * 2;
            else 
                mScale = mScale / 2;

            this.SetAutoScrollMargin((int)(80 * mScale), (int)(110 * mScale));
            Invalidate();
        }
        
        private void Form1_KeyDown(object sender, KeyEventArgs e)
        {
            
            if (e.Control && e.Shift && e.KeyCode == Keys.P)
            {
                MessageBox.Show("Hello");
            }

            else if(e.Control && e.KeyCode==Keys.P)
            {
                Scale(true);
            }

            else if (e.Control && e.KeyCode == Keys.O)
            {
                Scale(false);
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Scale(mScale<4);
        }

        private void Form1_MouseUp(object sender, MouseEventArgs e)
        {

        }

        private void Form1_MouseDown(object sender, MouseEventArgs e)
        {
            toolStripStatusLabel1.Text = String.Format("{0} {1}", e.X, e.Y);
        }
    }
}
