using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.IO;

namespace StockAnalysis
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void download(string url, string file)
        {
            WebClient webClient = new WebClient();
            webClient.DownloadFile(url, file);
        }

        private string getData(string url )
        {
            WebClient webClient = new WebClient();
            webClient.DownloadFile(url, "temp.txt");

            string content = System.IO.File.ReadAllText("temp.txt");
            return content;
        }

        double getValue1(string s1, string key)
        {
            int p1 = s1.IndexOf(key);
            int p2 = s1.IndexOf("<span>", p1);
            p2 += "<span>".Length;
            int p3 = s1.IndexOf("</span>", p2);
            string value1 = s1.Substring(p2, p3 - p2);
            string value12=value1.Replace("B", "e9");
            double dValue1 = Convert.ToDouble(value12);
            return dValue1;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            // download data of https://hk.finance.yahoo.com/q?s=0231.HK
            string s1 = getData("https://hk.finance.yahoo.com/q?s=0231.HK");
            //MessageBox.Show(s1);
            double dValue1 = getValue1(s1,"成交量:");
            double dValue2 = getValue1(s1, "市值:");
            
            int x = 1;

            // parse data

            // check whether it matched the rule
        }

        // Download all
        private void button2_Click(object sender, EventArgs e)
        {
            for(int i=1; i<9999; i++)
            {
                string s;
                s =String.Format("{0}.HK", i.ToString("D4"));

                label1.Text =s;

                if (!Directory.Exists(s))
                {
                    Directory.CreateDirectory(s);

                    try
                    {
                        download("https://hk.finance.yahoo.com/q?s=" + s, s + "\\1.txt");
                        download("http://real-chart.finance.yahoo.com/table.csv?s=" + s, s + "\\1.csv");
                    }
                    catch { }
                }
            }
        }

        // volume anaysis
        private void button3_Click(object sender, EventArgs e)
        {
            //read files & parse
            //log the max(volume/total)
        }
    }
}
