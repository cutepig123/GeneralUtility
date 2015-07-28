//http://www.codeproject.com/Articles/26532/A-Zoomable-and-Scrollable-PictureBox
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace YLScsImage
{
    public partial class ImagePanel : UserControl
    {
        public ImagePanel()
        {
            InitializeComponent();

            // Set the value of the double-buffering style bits to true.
            this.SetStyle(ControlStyles.AllPaintingInWmPaint |
              ControlStyles.UserPaint | ControlStyles.ResizeRedraw |
              ControlStyles.UserPaint | ControlStyles.DoubleBuffer, true);

            Pts.Add(new Point(10 * 5, 10 * 3));
            Pts.Add(new Point(10 * 5, 15 * 3));
            Pts.Add(new Point(20 * 5, 10 * 3));
            Pts.Add(new Point(30 * 5, 5 * 3));
        }

		//Image mImage;
        //float zoom;
        private List<PointF> Pts = new List<PointF>();
        private int object_radius = 3;
        private int over_dist_squared = 5;
        enum Mode{
            Mode_Default,
            Mode_EditPt
        };

        Mode mMode = Mode.Mode_Default;
        private int mEditPtIdx = -1;

        int viewRectWidth, viewRectHeight; // view window width and height

        float zoom = 1.0f;
        public float Zoom
        {
            get { return zoom; }
            set
            {
                if (value < 0.001f) value = 0.001f;
                zoom = value;

                displayScrollbar();
                setScrollbarValues();
                Invalidate();
            }
        }

        Size canvasSize = new Size(60, 40);
        public Size CanvasSize
        {
            get { return canvasSize; }
            set
            {
                canvasSize = value;
                displayScrollbar();
                setScrollbarValues();
                Invalidate();
            }
        }

        Bitmap image;
        public Bitmap Image
        {
            get { return image; }
            set 
            {
                image = value;
                displayScrollbar();
                setScrollbarValues(); 
                Invalidate();
            }
        }

        InterpolationMode interMode = InterpolationMode.HighQualityBilinear;
        public InterpolationMode InterpolationMode
        {
            get{return interMode;}
            set{interMode=value;}
        }
        
        protected override void OnLoad(EventArgs e)
        {
            displayScrollbar();
            setScrollbarValues();
            base.OnLoad(e);
        }

        protected override void OnResize(EventArgs e)
        {
            displayScrollbar();
            setScrollbarValues();
            base.OnResize(e);
        }
        protected override void OnPaint(PaintEventArgs e)
        {
             base.OnPaint(e);

            //draw image
            if(image!=null)
            {
                Rectangle srcRect,distRect;
                Point pt=new Point((int)(hScrollBar1.Value/zoom),(int)(vScrollBar1.Value/zoom));
                if (canvasSize.Width * zoom < viewRectWidth && canvasSize.Height * zoom < viewRectHeight)
                    srcRect = new Rectangle(0, 0, canvasSize.Width, canvasSize.Height);  // view all image
                else srcRect = new Rectangle(pt, new Size((int)(viewRectWidth / zoom), (int)(viewRectHeight / zoom))); // view a portion of image

                //distRect=new Rectangle((int)(-srcRect.Width/2),-srcRect.Height/2,srcRect.Width,srcRect.Height); // the center of apparent image is on origin
                distRect = new Rectangle(0, 0, srcRect.Width, srcRect.Height); 
 
                //Matrix mx=new Matrix(); // create an identity matrix
                //mx.Scale(zoom,zoom); // zoom image
                //mx.Translate(viewRectWidth/2.0f,viewRectHeight/2.0f, MatrixOrder.Append); // move image to view window center

                Graphics g=e.Graphics;
                g.InterpolationMode=interMode;
                g.ScaleTransform(zoom, zoom);
                //g.Transform=mx;
                g.DrawImage(image,distRect,srcRect, GraphicsUnit.Pixel);
            }

            e.Graphics.ResetTransform();
            e.Graphics.TranslateTransform(-this.hScrollBar1.Value, -this.vScrollBar1.Value );

            for (int i = 0; i < Pts.Count - 1; i++)
            {
                // Draw the segment.
                e.Graphics.DrawLine(Pens.Blue, ScalePointF(Pts[i], zoom), ScalePointF(Pts[i + 1], zoom));
            }

            // Draw the end points.
            foreach (PointF pt in Pts)
            {
                Rectangle rect = new Rectangle(
                    (int)(pt.X * zoom - object_radius), (int)(pt.Y * zoom - object_radius),
                    2 * object_radius + 1, 2 * object_radius + 1);
                e.Graphics.FillEllipse(Brushes.White, rect);
                e.Graphics.DrawEllipse(Pens.Black, rect);
            }

        }

        private void displayScrollbar()
        {
            viewRectWidth = this.Width;
            viewRectHeight = this.Height;

            if (image != null) canvasSize = image.Size;

            // If the zoomed image is wider than view window, show the HScrollBar and adjust the view window
            if (viewRectWidth > canvasSize.Width*zoom)
            {
                hScrollBar1.Visible = false;
                viewRectHeight = Height;
            }
            else
            {
                hScrollBar1.Visible = true;
                viewRectHeight = Height - hScrollBar1.Height - statusStrip1.Height;
            }

            // If the zoomed image is taller than view window, show the VScrollBar and adjust the view window
            if (viewRectHeight > canvasSize.Height*zoom)
            {
                vScrollBar1.Visible = false;
                viewRectWidth = Width;
            }
            else
            {
                vScrollBar1.Visible = true;
                viewRectWidth = Width - vScrollBar1.Width;
            }

            // Set up scrollbars
            statusStrip1.Location = new Point(0, Height - statusStrip1.Height);
            hScrollBar1.Location = new Point(0, Height - hScrollBar1.Height - statusStrip1.Height);
            hScrollBar1.Width = viewRectWidth;
            vScrollBar1.Location = new Point(Width - vScrollBar1.Width, 0);
            vScrollBar1.Height = viewRectHeight;
        }

        private void setScrollbarValues()
        {
            // Set the Maximum, Minimum, LargeChange and SmallChange properties.
            this.vScrollBar1.Minimum = 0;
            this.hScrollBar1.Minimum = 0;

            // If the offset does not make the Maximum less than zero, set its value. 
            if ((canvasSize.Width * zoom - viewRectWidth) > 0)
            {
                this.hScrollBar1.Maximum =(int)( canvasSize.Width * zoom) - viewRectWidth;
            }
            // If the VScrollBar is visible, adjust the Maximum of the 
            // HSCrollBar to account for the width of the VScrollBar.  
            if (this.vScrollBar1.Visible)
            {
                this.hScrollBar1.Maximum += this.vScrollBar1.Width;
            }
            this.hScrollBar1.LargeChange = this.hScrollBar1.Maximum / 10;
            this.hScrollBar1.SmallChange = this.hScrollBar1.Maximum / 20;

            // Adjust the Maximum value to make the raw Maximum value 
            // attainable by user interaction.
            this.hScrollBar1.Maximum += this.hScrollBar1.LargeChange;

            // If the offset does not make the Maximum less than zero, set its value.    
            if ((canvasSize.Height * zoom - viewRectHeight) > 0)
            {
                this.vScrollBar1.Maximum = (int)(canvasSize.Height * zoom) - viewRectHeight;
            }

            // If the HScrollBar is visible, adjust the Maximum of the 
            // VSCrollBar to account for the width of the HScrollBar.
            if (this.hScrollBar1.Visible)
            {
                this.vScrollBar1.Maximum += this.hScrollBar1.Height;
            }
            this.vScrollBar1.LargeChange = this.vScrollBar1.Maximum / 10;
            this.vScrollBar1.SmallChange = this.vScrollBar1.Maximum / 20;

            // Adjust the Maximum value to make the raw Maximum value 
            // attainable by user interaction.
            this.vScrollBar1.Maximum += this.vScrollBar1.LargeChange;
        }

        private void vScrollBar1_Scroll(object sender, ScrollEventArgs e)
        {
            this.Invalidate();
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

        //PointF ScalePointF(Point pt, float scale)
        //{
        //    PointF p = new PointF();
        //    p.X = pt.X * scale;
        //    p.Y = pt.Y * scale;
        //    return p;
        //}

        //Point ToPoint(PointF pt)
        //{
        //    Point p = new Point();
        //    p.X = (int)pt.X;
        //    p.Y = (int)pt.Y;
        //    return p;
        //}

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

        private void ImagePanel_KeyDown(object sender, KeyEventArgs e)
        {
            //if (e.Control && e.Shift && e.KeyCode == Keys.P)
            //{
            //    MessageBox.Show("Hello");
            //}

            //else if (e.Control && e.KeyCode == Keys.P)
            //{
            //    Zoom = Zoom * 2;
            //}

            //else if (e.Control && e.KeyCode == Keys.O)
            //{
            //    Zoom = Zoom / 2;
            //}

        }

        PointF GetMouseLocation(Point pt)
        {
            PointF p = new PointF();
            p.X = (pt.X + this.hScrollBar1.Value) / zoom;
            p.Y = (pt.Y + this.vScrollBar1.Value) / zoom ;
            return p;
        }

        private void ImagePanel_MouseMove(object sender, MouseEventArgs e)
        {
            if (mMode == Mode.Mode_EditPt)
            {
                Pts[mEditPtIdx] = GetMouseLocation(e.Location);
                Invalidate();
            }

        }

        private void ImagePanel_MouseUp(object sender, MouseEventArgs e)
        {
            this.Cursor = Cursors.Default;
            mMode = Mode.Mode_Default;
            mEditPtIdx = -1;

        }

        private void ImagePanel_MouseDown(object sender, MouseEventArgs e)
        {
            PointF pt = GetMouseLocation(e.Location);
            toolStripStatusLabel1.Text = String.Format("{0} {1}", pt.X, pt.Y);

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

        private void toolStripStatusLabel1_Click(object sender, EventArgs e)
        {

        }

    }
}
