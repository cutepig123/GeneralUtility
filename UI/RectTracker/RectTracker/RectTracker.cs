// Copy Right 2004 - feel free to do anything with this code.
//
// this project was created using the methods shown in:
// Creating Bitmap Regions for Forms and Buttons - http://www.thecodeproject.com/csharp/bmprgnform.asp
// 

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;
using System.Drawing.Drawing2D;

namespace RectTrackerSharp
{
	/// <summary>
	/// Summary description for RectTracker.
	/// </summary>
	public class RectTracker : System.Windows.Forms.UserControl
	{
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;
		//original rect
		Rectangle baseRect;
		
		//new rect with bounds
		Rectangle ControlRect;

		//all small rects
		Rectangle[] SmallRect = new Rectangle[8];

		//sqare size
		Size Sqare = new Size(6,6);
		
		//graphics object
		Graphics g;

		//the control that is tracked
		Control currentControl;

		// To store the location of previous mouse left click in the form
		// so that we can use it to calculate the new form location during dragging
		private Point prevLeftClick;

		// To determine if it is the first time entry for every dragging of the form
		private bool isFirst = true;

		//defines the color surrounding the tracker
		Color MyBackColor = Color.Wheat;
		
		//defines the color of the sqares
		Color SqareColor = Color.White;

		//defines the line color of the sqares 
		Color SqareLineColor = Color.Black;

		//The Control the tracker is used on 
		public Control Control
		{
			get {return currentControl;}
			set 
			{
				//currentControl = value;
				//Rect = currentControl.Bounds;
			}
		}
		
		/// <summary>
		/// keep track of which border of the box is to be resized.
		/// </summary>
		enum RESIZE_BORDER
		{
			RB_NONE = 0,
			RB_TOP = 1,
			RB_RIGHT = 2,
			RB_BOTTOM = 3,
			RB_LEFT = 4,
			RB_TOPLEFT = 5,
			RB_TOPRIGHT = 6,
			RB_BOTTOMLEFT = 7,
			RB_BOTTOMRIGHT = 8
		}

		//currently resize mouse location
		private RESIZE_BORDER CurrBorder;

		/// <summary>
		/// The current position of the rectangle in client coordinates (pixels).
		/// </summary>
		public Rectangle Rect
		{
			get {return baseRect;}
			set 
			{
				int X = Sqare.Width;
				int Y = Sqare.Height;
				int Height = value.Height;
				int Width = value.Width;
				baseRect = new Rectangle(X,Y,Width,Height);
				SetRectangles();
			}
			
		}
		

		/// <summary>
		/// Set the Sqares positions
		/// </summary>
		void SetRectangles()
		{
			//TopLeft
			SmallRect[0] = new Rectangle(new Point(baseRect.X - Sqare.Width,baseRect.Y - Sqare.Height),Sqare);
			//TopRight
			SmallRect[1] = new Rectangle(new Point(baseRect.X + baseRect.Width,baseRect.Y - Sqare.Height),Sqare);
			//BottomLeft
			SmallRect[2] = new Rectangle(new Point(baseRect.X - Sqare.Width,baseRect.Y + baseRect.Height),Sqare);
			//BottomRight
			SmallRect[3] = new Rectangle(new Point(baseRect.X + baseRect.Width,baseRect.Y + baseRect.Height),Sqare);
			//TopMiddle
			SmallRect[4] = new Rectangle(new Point(baseRect.X + (baseRect.Width/2) - (Sqare.Width/2) ,baseRect.Y - Sqare.Height),Sqare);
			//BottomMiddle
			SmallRect[5] = new Rectangle(new Point(baseRect.X + (baseRect.Width/2) - (Sqare.Width/2) ,baseRect.Y + baseRect.Height),Sqare);
			//LeftMiddle
			SmallRect[6] = new Rectangle(new Point(baseRect.X - Sqare.Width,baseRect.Y + (baseRect.Height/2) - (Sqare.Height/2)),Sqare);
			//RightMiddle
			SmallRect[7] = new Rectangle(new Point(baseRect.X + baseRect.Width,baseRect.Y + (baseRect.Height/2) - (Sqare.Height/2)),Sqare);
			
			//the whole tracker rect
			ControlRect = new Rectangle(new Point(0,0),this.Bounds.Size);
		}

		/// <summary>
		/// Draws Sqares
		/// </summary>
		public void Draw()
		{
			try
			{
				//draw sqares
				g.FillRectangles(Brushes.White,SmallRect);
				//fill sqares
				g.DrawRectangles(Pens.Black,SmallRect);
			}

			catch (Exception ex)
			{
				Console.WriteLine(ex.Message);
			}
		}

		/// <summary>
		/// Check point position to see if it's on tracker
		/// </summary>
		/// <param name="point">Current Point Position</param>
		/// <returns></returns>
		public bool Hit_Test(Point point)
		{
			//Check if the point is somewhere on the tracker
			if (!ControlRect.Contains(point))
			{
				//should never happen
				Cursor.Current = Cursors.Arrow;
				
				return false;
			}
			else if(SmallRect[0].Contains(point))
			{
				Cursor.Current = Cursors.SizeNWSE;
				CurrBorder = RESIZE_BORDER.RB_TOPLEFT;
			}
			else if(SmallRect[3].Contains(point))
			{
				Cursor.Current = Cursors.SizeNWSE;
				CurrBorder = RESIZE_BORDER.RB_BOTTOMRIGHT;
			}
			else if(SmallRect[1].Contains(point))
			{
				Cursor.Current = Cursors.SizeNESW;
				CurrBorder = RESIZE_BORDER.RB_TOPRIGHT;
			}
			else if(SmallRect[2].Contains(point))
			{
				Cursor.Current = Cursors.SizeNESW;
				CurrBorder = RESIZE_BORDER.RB_BOTTOMLEFT;
			}
			else if(SmallRect[4].Contains(point))
			{
				Cursor.Current = Cursors.SizeNS;
				CurrBorder = RESIZE_BORDER.RB_TOP;
			}
			else if(SmallRect[5].Contains(point))
			{
				Cursor.Current = Cursors.SizeNS;
				CurrBorder = RESIZE_BORDER.RB_BOTTOM;
			}
			else if(SmallRect[6].Contains(point))
			{
				Cursor.Current = Cursors.SizeWE;
				CurrBorder = RESIZE_BORDER.RB_LEFT;
			}
			else if(SmallRect[7].Contains(point))
			{
				Cursor.Current = Cursors.SizeWE;
				CurrBorder = RESIZE_BORDER.RB_RIGHT;
			}
			else if(ControlRect.Contains(point))
			{
				Cursor.Current = Cursors.SizeAll;
				CurrBorder = RESIZE_BORDER.RB_NONE;
				
			}
			
			
			return true;
		}

		/// <summary>
		/// Check point position to see if it's on tracker
		/// </summary>
		/// <param name="x">X position</param>
		/// <param name="y">Y position</param>
		/// <returns></returns>
		public bool Hit_Test(int x, int y)
		{
			return Hit_Test(new Point(x,y));
		}
			
		/// <summary>
		/// Cunstructor
		/// </summary>
		/// <param name="theControl">The Control we want to use the Tracker on</param>
		public RectTracker(Control theControl)
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();
		

			//set the tracked control 
			currentControl = theControl;
			
			//Create the tracker
			Create();
			
		}

		public RectTracker()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();
		}

		/// <summary>
		/// Setup the tracker
		/// </summary>
		private void Create()
		{
			//create tracker bounds
			int X = currentControl.Bounds.X - Sqare.Width;
			int Y = currentControl.Bounds.Y - Sqare.Height;
			int Height = currentControl.Bounds.Height + (Sqare.Height*2);
			int Width = currentControl.Bounds.Width + (Sqare.Width*2);
			
			//set bounds
			this.Bounds = new Rectangle(X,Y,Width+1,Height+1);
			
			this.BringToFront();

			//create inside rect bounds
			Rect = currentControl.Bounds;

			//create transparent area around the control
			this.Region = new Region(BuildFrame());

			//create graphics
			g = this.CreateGraphics();

		}

		/// <summary>
		/// Transparents the control area in the tracker
		/// </summary>
		/// <returns>Graphics Path made for transparenting</returns>
		private GraphicsPath BuildFrame()
		{
			//make the tracker to "contain" the control like this:
			//
			//
			//+++++++++++++++++++++++
			//+						+
			//+						+
			//+	 	 CONTROL		+
			//+						+
			//+						+
			//+++++++++++++++++++++++
			//

			GraphicsPath path = new GraphicsPath();

			//Top Rectagle
			path.AddRectangle(new Rectangle(0,0,currentControl.Width + (Sqare.Width*2)+1,Sqare.Height+1));;
			//Left Rectangle
			path.AddRectangle(new Rectangle(0,Sqare.Height+1,Sqare.Width+1,currentControl.Bounds.Height + Sqare.Height+1));
			//Bottom Rectagle
			path.AddRectangle(new Rectangle(Sqare.Width+1,currentControl.Bounds.Height + Sqare.Height,currentControl.Width + Sqare.Width +1,Sqare.Height+1));
			//Right Rectagle
			path.AddRectangle(new Rectangle(currentControl.Width + Sqare.Width,Sqare.Height+1,Sqare.Width+1,currentControl.Height-2));
			
			return path;
		}

	



		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			// 
			// RectTracker
			// 
			this.BackColor = System.Drawing.Color.Wheat;
			this.Name = "RectTracker";
			this.Size = new System.Drawing.Size(64, 56);
			this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.RectTracker_MouseUp);
			this.Paint += new System.Windows.Forms.PaintEventHandler(this.RectTracker_Paint);
			this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.RectTracker_MouseMove);

		}
		#endregion

		
		
		public void Mouse_Move(object sender,System.Windows.Forms.MouseEventArgs e)
		{
			//minimum size for the control is 8x8
			if (currentControl.Height < 8)
			{
				currentControl.Height = 8;
				return;
			}
			else if (currentControl.Width < 8)
			{
				currentControl.Width = 8;
				return;
			}
					
			switch(this.CurrBorder)
			{
				case RESIZE_BORDER.RB_TOP:
					currentControl.Height = currentControl.Height - e.Y + prevLeftClick.Y;
					if (currentControl.Height > 8)
					currentControl.Top = currentControl.Top + e.Y - prevLeftClick.Y;					
					break;
				case RESIZE_BORDER.RB_TOPLEFT:
					currentControl.Height = currentControl.Height - e.Y + prevLeftClick.Y;
					if (currentControl.Height > 8)
					currentControl.Top =currentControl.Top + e.Y - prevLeftClick.Y;
					currentControl.Width = currentControl.Width - e.X + prevLeftClick.X;
					if (currentControl.Width > 8)
					currentControl.Left =currentControl.Left + e.X - prevLeftClick.X;
					break;
				case RESIZE_BORDER.RB_TOPRIGHT:
					currentControl.Height = currentControl.Height - e.Y + prevLeftClick.Y;
					if (currentControl.Height > 8)
					currentControl.Top = currentControl.Top + e.Y - prevLeftClick.Y;
					currentControl.Width = currentControl.Width + e.X - prevLeftClick.X;
					break;
				case RESIZE_BORDER.RB_RIGHT:
					currentControl.Width = currentControl.Width + e.X - prevLeftClick.X;
					break;
				case RESIZE_BORDER.RB_BOTTOM:
					currentControl.Height = currentControl.Height + e.Y - prevLeftClick.Y;
					break;
				case RESIZE_BORDER.RB_BOTTOMLEFT:
					currentControl.Height = currentControl.Height + e.Y - prevLeftClick.Y;
					currentControl.Width = currentControl.Width - e.X + prevLeftClick.X;
					if (currentControl.Width > 8)
						currentControl.Left = currentControl.Left + e.X - prevLeftClick.X;
					break;
				case RESIZE_BORDER.RB_BOTTOMRIGHT:
					currentControl.Height = currentControl.Height + e.Y - prevLeftClick.Y;
					currentControl.Width = currentControl.Width + e.X - prevLeftClick.X;
					break;
				case RESIZE_BORDER.RB_LEFT:
					currentControl.Width = currentControl.Width - e.X + prevLeftClick.X;
					if (currentControl.Width > 8)
					currentControl.Left = currentControl.Left + e.X - prevLeftClick.X;
					break;
				case RESIZE_BORDER.RB_NONE:
					currentControl.Location = new Point(currentControl.Location.X + e.X - prevLeftClick.X, currentControl.Location.Y + e.Y - prevLeftClick.Y);
					break;
				
			}

		}

		private void RectTracker_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			Cursor = Cursors.Arrow;
			
			if (e.Button == MouseButtons.Left)
			{
				// If this is the first mouse move event for left click dragging of the form,
				// store the current point clicked so that we can use it to calculate the form's
				// new location in subsequent mouse move events due to left click dragging of the form
				if(isFirst == true)
				{
					// Store previous left click position
					prevLeftClick = new Point(e.X, e.Y);

					// Subsequent mouse move events will not be treated as first time, until the
					// left mouse click is released or other mouse click occur
					isFirst = false;
				}
				else
				{
					//hide tracker
					this.Visible = false;
					//forward mouse position to append changes
					Mouse_Move(this,e);
					// Store new previous left click position
					prevLeftClick = new Point(e.X, e.Y);
				}
				
				
			}
			else
			{
				// This is a new mouse move event so reset flag
				isFirst = true;
				//show the tracker
				this.Visible = true;
				
				//update the mouse pointer to other cursors by checking it's position
				Hit_Test(e.X,e.Y);
			}

			
		}

		private void RectTracker_Paint(object sender, System.Windows.Forms.PaintEventArgs e)
		{
			//redraw sqares
			Draw();
		}

		private void RectTracker_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			//the mouse is up, recreate the control to append changes
			Create();
			this.Visible = true;
			
		}

	}
}
