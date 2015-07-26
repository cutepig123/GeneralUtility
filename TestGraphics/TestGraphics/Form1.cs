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
            mScale = 1;
            this.AutoScroll = true;
            this.KeyPreview = true;
            mImage = new Bitmap("1.bmp");
            Pts.Add(new Point(10*5, 10*3));
            Pts.Add(new Point(10 * 5, 15 * 3));
            Pts.Add(new Point(20 * 5, 10 * 3));
            Pts.Add(new Point(30 * 5, 5 * 3));
        }

        Image mImage;
        float mScale;
        private List<PointF> Pts = new List<PointF>();
        private int object_radius = 3;
        private int over_dist_squared = 5;
        enum Mode{
            Mode_Default,
            Mode_EditPt
        };

        Mode mMode = Mode.Mode_Default;
        private int mEditPtIdx = -1;

        protected override void OnScroll(ScrollEventArgs se)
        {
            base.OnScroll(se);
            Invalidate();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            //base.OnPaint(e);

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

            e.Graphics.ResetTransform();
            e.Graphics.TranslateTransform(-this.HorizontalScroll.Value * mScale, -this.VerticalScroll.Value * mScale);
            
            for (int i = 0; i < Pts.Count-1; i++)
            {
                // Draw the segment.
                e.Graphics.DrawLine(Pens.Blue, ScalePointF(Pts[i], mScale), ScalePointF(Pts[i + 1], mScale));
            }

            // Draw the end points.
            foreach (PointF pt in Pts)
            {
                Rectangle rect = new Rectangle(
                    (int)(pt.X * mScale - object_radius), (int)(pt.Y * mScale - object_radius),
                    2 * object_radius + 1, 2 * object_radius + 1);
                e.Graphics.FillEllipse(Brushes.White, rect);
                e.Graphics.DrawEllipse(Pens.Black, rect);
            }

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

        private double FindDistanceToPointSquared(PointF a, PointF b)
        {
            return Math.Sqrt((a.X - b.X) * (a.X - b.X) + (a.Y - b.Y) * (a.Y - b.Y));
        }

        PointF ScalePointF(PointF pt, float scale)
        {
            PointF p = new PointF();
            p.X = pt.X * scale;
            p.Y = pt.Y * scale;
            return p;
        }

        PointF ScalePointF(Point pt, float scale)
        {
            PointF p = new PointF();
            p.X = pt.X * scale;
            p.Y = pt.Y * scale;
            return p;
        }

        Point ToPoint(PointF pt)
        {
            Point p = new Point();
            p.X = (int)pt.X;
            p.Y = (int)pt.Y;
            return p;
        }

        private bool MouseIsOverEndpoint(PointF mouse_pt,
            out int pt_idx)
        {
            for (int i = 0; i < Pts.Count; i++)
            {
                // Check the starting point.
                if (FindDistanceToPointSquared(mouse_pt, (Pts[i])) <
                    over_dist_squared)
                {
                    pt_idx = i;
                    return true;
                }
            }

            pt_idx = -1;
            
            return false;
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

        
        private void Form1_MouseDown(object sender, MouseEventArgs e)
        {
            PointF pt = GetMouseLocation(e.Location);
            toolStripStatusLabel1.Text = String.Format("{0} {1}", pt.X , pt.Y );

            int pt_idx;

            if (MouseIsOverEndpoint(pt, out pt_idx))
            {
                this.Cursor = Cursors.Cross;
                mMode = Mode.Mode_EditPt;
                mEditPtIdx = pt_idx;
            }
            else
            {
                this.Cursor = Cursors.Default;
                mMode = Mode.Mode_Default;
                mEditPtIdx = -1;
            }
        }

        private void Form1_MouseUp(object sender, MouseEventArgs e)
        {
            this.Cursor = Cursors.Default;
            mMode = Mode.Mode_Default;
            mEditPtIdx = -1;
        }
        PointF GetMouseLocation(Point pt)
        {
            PointF p=new PointF();
            p.X = (pt.X) / mScale + this.HorizontalScroll.Value;
            p.Y = (pt.Y) / mScale + this.VerticalScroll.Value;
            return p;
        }
        private void Form1_MouseMove(object sender, MouseEventArgs e)
        {
            if(mMode==Mode.Mode_EditPt)
            {
                Pts[mEditPtIdx] = GetMouseLocation(e.Location);
                Invalidate();
            }
        }
    }
}
