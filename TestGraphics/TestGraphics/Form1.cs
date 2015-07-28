/*
todo
 * 
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
            imagePanel1.AddLine();
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

        private void getToolStripMenuItem_Click(object sender, EventArgs e)
        {
            String text =String.Format("\r\n");
            for(int i=0; i<imagePanel1.Pts.Count; i++)
            {
                string s =string.Format("{0}: {1} {2}\r\n", i,
                    imagePanel1.Pts[i].X, imagePanel1.Pts[i].Y);

                text += s;
            }
            string promptValue = Prompt.ShowDialog("All points of the line:", "Line",text);
        }

        private void toolStripMenuItem2_Click(object sender, EventArgs e)
        {
            MessageBox.Show("Not impl");
        }
    }

    //http://stackoverflow.com/questions/5427020/prompt-dialog-in-windows-forms
    public static class Prompt
    {
        public static string ShowDialog(string text, string caption, string cont)
        {
            Form prompt = new Form();
            prompt.Width = 200;
            prompt.Height = 550;
            prompt.FormBorderStyle = FormBorderStyle.FixedDialog;
            prompt.Text = caption;
            prompt.StartPosition = FormStartPosition.CenterScreen;
            Label textLabel = new Label() { Left = 50, Top = 20, Text = text };
            TextBox textBox = new TextBox() { Left = 50, Top = 50, Width = 100, Height =400 };
            textBox.Multiline = true;
            textBox.ScrollBars = ScrollBars.Both;
            textBox.Text = cont;
            Button confirmation = new Button() { Text = "Ok", Left = 50, Width = 100, Top = 450, DialogResult = DialogResult.OK };
            confirmation.Click += (sender, e) => { prompt.Close(); };
            prompt.Controls.Add(textBox);
            prompt.Controls.Add(confirmation);
            prompt.Controls.Add(textLabel);
            prompt.AcceptButton = confirmation;

            return prompt.ShowDialog() == DialogResult.OK ? textBox.Text : "";
        }
    }
}
