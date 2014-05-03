using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Speech.Synthesis;
using System.IO;
using System.Diagnostics;

namespace text_to_speech
{
    public partial class Form1 : Form
    {
        SpeechSynthesizer reader;
        System.Windows.Forms.Timer timer;

        public Form1()
        {
            InitializeComponent();

        }
        private void Form1_Load(object sender, EventArgs e)
        {
            reader = new SpeechSynthesizer();
            
            //string scul = "en-US";
            //System.Globalization.CultureInfo culinfo = new System.Globalization.CultureInfo(scul);
            //foreach (InstalledVoice voice in reader.GetInstalledVoices(culinfo))
            //{
            //    Trace.WriteLine(voice.VoiceInfo.Name);
            //    reader.SelectVoice(voice.VoiceInfo.Name);
            //}
            button2.Enabled = false;
            button3.Enabled = false;
            button4.Enabled = false;
            textBox1.ScrollBars = ScrollBars.Both;

            timer = new Timer();
            textBox2.Text = "0";
            checkBox1.Checked = true;
            
        }

        //SPEAK TEXT
        private void button1_Click(object sender, EventArgs e)
        {
            reader.Dispose();
            if (textBox1.Text != "")
            {

                reader = new SpeechSynthesizer();
                reader.SpeakAsync(textBox1.Text);
                label2.Text = "SPEAKING";
                button2.Enabled = true;
                button4.Enabled = true;
                reader.SpeakCompleted += new EventHandler<SpeakCompletedEventArgs>(reader_SpeakCompleted);
            }
            else
            {
                MessageBox.Show("Please enter some text in the textbox", "Message", MessageBoxButtons.OK);
            }
        }

        void reader_SpeakCompleted(object sender, SpeakCompletedEventArgs e)
        {
            label2.Text = "IDLE";
        }

        //PAUSE
        private void button2_Click(object sender, EventArgs e)
        {
            if (reader != null)
            {
                if (reader.State == SynthesizerState.Speaking)
                {
                    reader.Pause();
                    label2.Text = "PAUSED";
                    button3.Enabled = true;

                }
            }
        }

        //RESUME
        private void button3_Click(object sender, EventArgs e)
        {
            if (reader != null)
            {
                if (reader.State == SynthesizerState.Paused)
                {
                    reader.Resume();
                    label2.Text = "SPEAKING";
                }
                button3.Enabled = false;
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            if (reader != null)
            {
                reader.Dispose();
                label2.Text = "IDLE";
                button2.Enabled = false;
                button3.Enabled = false;
                button4.Enabled = false;
            }
        }

        private void button5_Click(object sender, EventArgs e)
        {
            openFileDialog1.ShowDialog();
        }

        private void openFileDialog1_FileOk(object sender, CancelEventArgs e)
        {
            textBox1.Text =  File.ReadAllText(openFileDialog1.FileName.ToString());

        }

        private int latest_hr = -1;

        private void checkBox1_CheckedChanged()
        {
            latest_hr = -1;

            if (checkBox1.Checked)
            {
                timer.Interval = 1000;
                timer.Tick += new EventHandler(ShowMessage_TimerEventProcessor);
                timer.Start();
            }
            else
                timer.Stop();
        }

       

        private void ShowMessage_TimerEventProcessor(Object sender, EventArgs e)
        {
            DateTime dt = DateTime.Now;
            label5.Text = dt.ToLongTimeString();
            int expectMin =Int16.Parse(textBox2.Text);
            if( dt.Minute >= expectMin && dt.Minute < expectMin+1 && latest_hr!=dt.Hour )
            {
                string s = "  现在时间是" + label5.Text + "\n";
                
                s = s + s + s + s + s;
                reader.SpeakAsync( s);
                latest_hr = dt.Hour;
            }
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            checkBox1_CheckedChanged();
        }

     
    }
}
