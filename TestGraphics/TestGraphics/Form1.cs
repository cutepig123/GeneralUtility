//http://book.51cto.com/art/200811/98820.htm

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
        }

        double mScale;
        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            Graphics dc = e.Graphics;
            e.Graphics.ScaleTransform((float)mScale, (float)mScale);
            
            Pen bluePen = new Pen(Color.Blue, 3);
            dc.DrawRectangle(bluePen, 0, 0, 50, 50);
            Pen redPen = new Pen(Color.Red, 2);
            dc.DrawEllipse(redPen, 0, 50, 80, 60);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (mScale > 4)
                mScale = 1.0 / 4;
            else
                mScale = mScale * 2;
            Invalidate();
        }
    }
}
