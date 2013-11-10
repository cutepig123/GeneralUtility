How do I detect which Window is obscuring another 
http://stackoverflow.com/questions/565494/how-do-i-detect-which-window-is-obscuring-another

1. http://bobmoore.mvps.org/Win32/w32tip72.htm

WindowFromPoint, (use a point bounded by one window, and see if you get back that window's handle, or the other one).

For partial obscuration, you can use the clipping system. I discuss this in more detail on my website here

2. http://msdn.microsoft.com/en-us/library/ms632599%28VS.85%29.aspx#zorder
This page talks about the Z ordering of windows. It doesn't mention a function to get the Z order directly, but it does point at GetNextWindow(), which given one window can return the next (or previous, don't let the name fool you) in the Z order. Using that, you should be able to figure it out.
