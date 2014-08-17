using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Xml;
using System.IO;
using System.Diagnostics;

namespace Zm200Tool
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Button button1;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Form1()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.button1 = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(96, 96);
			this.button1.Name = "button1";
			this.button1.TabIndex = 0;
			this.button1.Text = "button1";
			this.button1.Click += new System.EventHandler(this.button1_Click);
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(292, 266);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.button1});
			this.Name = "Form1";
			this.Text = "Form1";
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}

		delegate void Operation(string sFile);

		void SrchFiles(string sDir, Operation op)
		{
			foreach(string d in Directory.GetDirectories(sDir))
			{
				foreach(string f in Directory.GetFiles(d))
				{
					if( f.ToLower().EndsWith(".vcproj") )
					{
						op(f);
					}
				}

				SrchFiles(d, op);
			}
		}

		void ModifyXml(string sFile)
		{
			Debug.WriteLine(sFile);

			XmlDocument doc =new XmlDataDocument();
			//string sFile ="G:/Itf/Source/WireTrace/Itf_WireTrace.vcproj";
			System.IO.File.Copy(sFile, sFile + ".bk");

			doc.Load(sFile);
			XmlElement root = doc.DocumentElement;
			XmlNodeList nl= root.SelectNodes("//Tool[@Name='VCCLCompilerTool']");
			for(int i=0; i<nl.Count; i++)
			{
				XmlNode node =nl.Item(i);

				if( node.Attributes["AdditionalOptions"] != null )
				{
					string s =node.Attributes["AdditionalOptions"].Value;
					if( s.IndexOf("/Zm")<0)
						node.Attributes["AdditionalOptions"].Value ="/Zm200 " + s;

					Debug.WriteLine(s + " --> " + node.Attributes["AdditionalOptions"].Value);
				}
				else
				{
					XmlAttribute attr =node.OwnerDocument.CreateAttribute("AdditionalOptions");
					attr.Value ="/Zm200 ";
					node.Attributes.Append(attr);

					Debug.WriteLine( " --> " + node.Attributes["AdditionalOptions"].Value);
				}
				//node.Attributes.Append(
				for( int j=0; j<node.Attributes.Count; j++)
				{
					//XmlAttribute nodeAttr =node.Attributes.Item(j);
					//nodeAttr.
					//nodeAttr.InnerText
				}
			}

			doc.Save(sFile);
		}
	
		
		private void button1_Click(object sender, System.EventArgs e)
		{
			
			Operation op =new Operation(ModifyXml);

			SrchFiles("g:\\", op);
		}
	}

}