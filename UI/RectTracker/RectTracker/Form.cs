using System;
using System.Drawing;
using System.Reflection;
using System.Runtime;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace RectTrackerSharp
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.Button button2;
		private System.Windows.Forms.TextBox textBox1;
		private System.Windows.Forms.Label label1;

		RectTracker CSharpTracker;
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

			CSharpTracker = new RectTracker();
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
			System.Configuration.AppSettingsReader configurationAppSettings = new System.Configuration.AppSettingsReader();
			this.button1 = new System.Windows.Forms.Button();
			this.button2 = new System.Windows.Forms.Button();
			this.textBox1 = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(102, 47);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(88, 40);
			this.button1.TabIndex = 0;
			this.button1.Text = "button1";
			this.button1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button1_MouseDown);
			// 
			// button2
			// 
			this.button2.Cursor = System.Windows.Forms.Cursors.SizeAll;
			this.button2.Location = new System.Drawing.Point(41, 161);
			this.button2.Name = "button2";
			this.button2.Size = new System.Drawing.Size(181, 75);
			this.button2.TabIndex = 1;
			this.button2.Text = "button2";
			
			this.button2.MouseDown += new System.Windows.Forms.MouseEventHandler(this.button2_MouseDown);
			// 
			// textBox1
			// 
			this.textBox1.Location = new System.Drawing.Point(88, 16);
			this.textBox1.Name = "textBox1";
			this.textBox1.TabIndex = 2;
			this.textBox1.Text = "textBox1";
			this.textBox1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.textBox1_MouseDown);
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(166, 102);
			this.label1.Name = "label1";
			this.label1.TabIndex = 4;
			this.label1.Text = "label1";
			this.label1.MouseDown += new System.Windows.Forms.MouseEventHandler(this.label1_MouseDown);
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.CausesValidation = ((bool)(configurationAppSettings.GetValue("Form1.CausesValidation", typeof(bool))));
			this.ClientSize = new System.Drawing.Size(292, 273);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.label1,
																		  this.textBox1,
																		  this.button2,
																		  this.button1});
			this.Name = "Form1";
			this.Text = "Form1";
			this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Form1_MouseUp);
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

		
		private void Form1_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			
			this.Controls.Remove(CSharpTracker);
			
		}


		private void button2_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			button2.BringToFront();
			
			button2.Capture = false;
			if(this.Controls.Contains(CSharpTracker))
				this.Controls.Remove(CSharpTracker);
			CSharpTracker = new RectTracker((Control)button2);
			
			this.Controls.Add(CSharpTracker);
			CSharpTracker.BringToFront();
			CSharpTracker.Draw();
		}

		
		private void button1_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			button1.BringToFront();
			button1.Capture = false;
			if(this.Controls.Contains(CSharpTracker))
				this.Controls.Remove(CSharpTracker);
			CSharpTracker = new RectTracker(button1);
			
			this.Controls.Add(CSharpTracker);
			CSharpTracker.BringToFront();
			CSharpTracker.Draw();
			
		}

		
		

		private void label1_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			label1.BringToFront();
			
			label1.Capture = false;
			if(this.Controls.Contains(CSharpTracker))
				this.Controls.Remove(CSharpTracker);
			CSharpTracker = new RectTracker(label1);
			
			this.Controls.Add(CSharpTracker);
			CSharpTracker.BringToFront();
			CSharpTracker.Draw();
		}

		private void textBox1_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			//TODO: on tracker, if control is a textbox, and Multililne is false,
			//only allow to resize width
			textBox1.BringToFront();
			
			textBox1.Capture = false;
			if(this.Controls.Contains(CSharpTracker))
				this.Controls.Remove(CSharpTracker);
			CSharpTracker = new RectTracker(textBox1);
			
			this.Controls.Add(CSharpTracker);
			CSharpTracker.BringToFront();
			CSharpTracker.Draw();
		}

	

		

	


	}
}
