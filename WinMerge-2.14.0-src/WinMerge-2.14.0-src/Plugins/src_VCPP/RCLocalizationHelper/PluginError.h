/** 
 * @file  PluginError.h
 *
 * @brief Declaration of global Plugin Error functions
 *
 * These display a string (either specified or from string resouce)
 * via a MessageBox.
 * @todo These need to be altered to store error in property,
 * when Plugin Errors are implemented.
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id: PluginError.h 3135 2006-03-06 03:01:38Z elsapo $

#ifndef PluginError_h_included
#define PluginError_h_included

void PluginError(int id);
void PluginErrorFmt(int idfmt, ...);


#endif // PluginError_h_included
