/////////////////////////////////////////////////////////////////////////////
//    WinMerge:  an interactive diff/merge utility
//    Copyright (C) 1997-2000  Thingamahoochie Software
//    Author: Dean Grimm
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////
/**
 *  @file FileTransform.cpp
 *
 *  @brief Implementation of file transformations
 */ 
// ID line follows -- this is updated by SVN
// $Id: FileTransform.cpp 7484 2010-12-28 20:56:59Z gerundt $

#include "StdAfx.h"
#include "FileTransform.h"
#include "Plugins.h"
#include "files.h"
#include "paths.h"
#include "multiformatText.h"
#include "UniMarkdownFile.h"
#include "Environment.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int g_bUnpackerMode = PLUGIN_MANUAL;
int g_bPredifferMode = PLUGIN_MANUAL;

/**
 * @brief Category of transformation : define the transformation events
 *
 * @note USER categories are calls to scriptlets, or VB ActiveX DLL, or VC COM DLL
 * Use text definition : if you add one, nothing to do ; 
 * if you change one, you just have change the dll/scripts for that event
 */
extern LPCWSTR TransformationCategories[] = 
{
	L"BUFFER_PREDIFF",
	L"FILE_PREDIFF",
	L"EDITOR_SCRIPT",
	L"BUFFER_PACK_UNPACK",
	L"FILE_PACK_UNPACK",
	NULL,		// last empty : necessary
};




////////////////////////////////////////////////////////////////////////////////
// transformations : packing unpacking

// known handler
BOOL FileTransform_Packing(String & filepath, PackingInfo handler)
{
	// no handler : return true
	if (handler.pluginName.empty())
		return TRUE;

	storageForPlugins bufferData;
	bufferData.SetDataFileAnsi(filepath.c_str());

	// control value
	BOOL bHandled = FALSE;

	PluginInfo * plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(L"FILE_PACK_UNPACK", handler.pluginName.c_str());
	if (plugin == NULL)
		plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(L"BUFFER_PACK_UNPACK", handler.pluginName.c_str());
	LPDISPATCH piScript = plugin->m_lpDispatch;
	if (handler.bWithFile)
	{
		// use a temporary dest name
		bHandled = InvokePackFile(bufferData.GetDataFileAnsi(),
			bufferData.GetDestFileName(),
			bufferData.GetNChanged(),
			piScript, handler.subcode);
		if (bHandled)
			bufferData.ValidateNewFile();
	}
	else
	{
		bHandled = InvokePackBuffer(*bufferData.GetDataBufferAnsi(),
			bufferData.GetNChanged(),
			piScript, handler.subcode);
		if (bHandled)
			bufferData.ValidateNewBuffer();
	}

	// if this packer does not work, that is an error
	if (bHandled == FALSE)
		return FALSE;

	// if the buffer changed, write it before leaving
	BOOL bSuccess = TRUE;
	if (bufferData.GetNChangedValid() > 0)
	{
		bSuccess = bufferData.SaveAsFile(filepath);
	}

	return bSuccess;
}

// known handler
BOOL FileTransform_Unpacking(String & filepath, const PackingInfo * handler, int * handlerSubcode)
{
	// no handler : return true
	if (handler->pluginName.empty())
		return TRUE;

	storageForPlugins bufferData;
	bufferData.SetDataFileAnsi(filepath.c_str());

	// temporary subcode 
	int subcode;

	// control value
	BOOL bHandled = FALSE;

	PluginInfo * plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(L"FILE_PACK_UNPACK", handler->pluginName.c_str());
	if (plugin == NULL)
		plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(L"BUFFER_PACK_UNPACK", handler->pluginName.c_str());
	if (plugin == NULL)
		return FALSE;

	LPDISPATCH piScript = plugin->m_lpDispatch;
	if (handler->bWithFile)
	{
		CString destFileName = bufferData.GetDestFileName();
		// use a temporary dest name
		bHandled = InvokeUnpackFile(bufferData.GetDataFileAnsi(),
			destFileName,
			bufferData.GetNChanged(),
			piScript, subcode);
		if (bHandled)
			bufferData.ValidateNewFile();
	}
	else
	{
		bHandled = InvokeUnpackBuffer(*bufferData.GetDataBufferAnsi(),
			bufferData.GetNChanged(),
			piScript, subcode);
		if (bHandled)
			bufferData.ValidateNewBuffer();
	}

	// if this unpacker does not work, that is an error
	if (bHandled == FALSE)
		return FALSE;

	// valid the subcode
	*handlerSubcode = subcode;

	// if the buffer changed, write it before leaving
	BOOL bSuccess = TRUE;
	if (bufferData.GetNChangedValid() > 0)
	{
		bSuccess = bufferData.SaveAsFile(filepath);
	}

	return bSuccess;
}


// scan plugins for the first handler
BOOL FileTransform_Unpacking(String & filepath, LPCTSTR filteredText, PackingInfo * handler, int * handlerSubcode)
{
	// PLUGIN_BUILTIN_XML : read source file through custom UniFile
	if (handler->bToBeScanned == PLUGIN_BUILTIN_XML)
	{
		handler->pufile = new UniMarkdownFile;
		handler->textType = _T("xml");
		handler->disallowMixedEOL = true;
		handler->pluginName.erase(); // Make FileTransform_Packing() a NOP
		// Leave bToBeScanned alone so above lines will continue to execute on
		// subsequent calls to this function
		return TRUE;
	}

	storageForPlugins bufferData;
	bufferData.SetDataFileAnsi(filepath.c_str());

	// filename, to test the extension
	CString filename = PathFindFileName(filepath.c_str());

	// control value
	BOOL bHandled = FALSE;

	// get the scriptlet files
	PluginArray * piFileScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"FILE_PACK_UNPACK");

	// MAIN LOOP : call each handler, 
	// stop as soon as we have a success
	int step;
	for (step = 0 ; bHandled == FALSE && step < piFileScriptArray->GetSize() ; step ++)
	{
		PluginInfo & plugin = piFileScriptArray->ElementAt(step);
		if (plugin.m_bAutomatic == FALSE)
			continue;
		if (plugin.TestAgainstRegList(filteredText) == FALSE)
			continue;

		handler->pluginName = plugin.m_name;
		handler->bWithFile = TRUE;
		// use a temporary dest name
		bHandled = InvokeUnpackFile(bufferData.GetDataFileAnsi(),
			bufferData.GetDestFileName(),
			bufferData.GetNChanged(),
			plugin.m_lpDispatch, handler->subcode);
		if (bHandled)
			bufferData.ValidateNewFile();
	}

	// We can not assume that the file is text, so use a safearray and not a BSTR
	// TODO : delete this event ? 	Is anyone going to use this ?

	// get the scriptlet files
	PluginArray * piBufferScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"BUFFER_PACK_UNPACK");

	// MAIN LOOP : call each handler, 
	// stop as soon as we have a success
	for (step = 0 ; bHandled == FALSE && step < piBufferScriptArray->GetSize() ; step ++)
	{
		PluginInfo & plugin = piBufferScriptArray->ElementAt(step);
		if (plugin.m_bAutomatic == FALSE)
			continue;
		if (plugin.TestAgainstRegList(filteredText) == FALSE)
			continue;

		handler->pluginName = plugin.m_name;
		handler->bWithFile = FALSE;
		bHandled = InvokeUnpackBuffer(*bufferData.GetDataBufferAnsi(),
			bufferData.GetNChanged(),
			plugin.m_lpDispatch, handler->subcode);
		if (bHandled)
			bufferData.ValidateNewBuffer();
	}

	if (bHandled == FALSE)
	{
		// we didn't find any unpacker, just hope it is normal Ansi/Unicode
		handler->pluginName = _T("");
		handler->subcode = 0;
		bHandled = TRUE;
	}

	// the handler is now defined
	handler->bToBeScanned = FALSE;

	// assign the sucode
	*handlerSubcode = handler->subcode;

	// if the buffer changed, write it before leaving
	BOOL bSuccess = TRUE;
	if (bufferData.GetNChangedValid() > 0)
	{
		bSuccess = bufferData.SaveAsFile(filepath);
	}

	return bSuccess;
}

////////////////////////////////////////////////////////////////////////////////
// transformation prediffing
    
// known handler
BOOL FileTransform_Prediffing(String & filepath, PrediffingInfo handler, BOOL bMayOverwrite)
{
	// no handler : return true
	if (handler.pluginName.empty())
		return TRUE;

	storageForPlugins bufferData;
	// detect Ansi or Unicode file
	bufferData.SetDataFileUnknown(filepath.c_str(), bMayOverwrite);
	// TODO : set the codepage
	// bufferData.SetCodepage();

	// control value
	BOOL bHandled = FALSE;

	PluginInfo * plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(L"FILE_PREDIFF", handler.pluginName.c_str());
	if (!plugin)
	{
		plugin = CAllThreadsScripts::GetActiveSet()->GetPluginByName(L"BUFFER_PREDIFF", handler.pluginName.c_str());
		if (!plugin)
			return FALSE;
	}
	LPDISPATCH piScript = plugin->m_lpDispatch;
	if (handler.bWithFile)
	{
		// use a temporary dest name
		bHandled = InvokePrediffFile(bufferData.GetDataFileAnsi(),
			bufferData.GetDestFileName(),
			bufferData.GetNChanged(),
			piScript);
		if (bHandled)
			bufferData.ValidateNewFile();
	}
	else
	{
		// probably it is for VB/VBscript so use a BSTR as argument
		bHandled = InvokePrediffBuffer(*bufferData.GetDataBufferUnicode(),
			bufferData.GetNChanged(),
			piScript);
		if (bHandled)
			bufferData.ValidateNewBuffer();
	}

	// if this unpacker does not work, that is an error
	if (bHandled == FALSE)
		return FALSE;

	// if the buffer changed, write it before leaving
	BOOL bSuccess = TRUE;
	if (bufferData.GetNChangedValid() > 0)
	{
		// bufferData changes filepath here to temp filepath
		bSuccess = bufferData.SaveAsFile(filepath);
	}

	return bSuccess;
}


// scan plugins for the first handler
BOOL FileTransform_Prediffing(String & filepath, LPCTSTR filteredText, PrediffingInfo * handler, BOOL bMayOverwrite)
{
	storageForPlugins bufferData;
	// detect Ansi or Unicode file
	bufferData.SetDataFileUnknown(filepath.c_str(), bMayOverwrite);
	// TODO : set the codepage
	// bufferData.SetCodepage();

	// filename, to test the extension
	CString filename = PathFindFileName(filepath.c_str());

	// control value
	BOOL bHandled = FALSE;

	// get the scriptlet files
	PluginArray * piFileScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"FILE_PREDIFF");

	// MAIN LOOP : call each handler, 
	// stop as soon as we have a success
	int step;
	for (step = 0 ; bHandled == FALSE && step < piFileScriptArray->GetSize() ; step ++)
	{
		PluginInfo & plugin = piFileScriptArray->ElementAt(step);
		if (plugin.m_bAutomatic == FALSE)
			continue;
		if (plugin.TestAgainstRegList(filteredText) == FALSE)
			continue;

		handler->pluginName = plugin.m_name;
		handler->bWithFile = TRUE;
		// use a temporary dest name
		bHandled = InvokePrediffFile(bufferData.GetDataFileAnsi(),
			bufferData.GetDestFileName(),
			bufferData.GetNChanged(),
			plugin.m_lpDispatch);
		if (bHandled)
			bufferData.ValidateNewFile();
	}

	// get the scriptlet files
	PluginArray * piBufferScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(L"BUFFER_PREDIFF");

	// MAIN LOOP : call each handler, 
	// stop as soon as we have a success
	for (step = 0 ; bHandled == FALSE && step < piBufferScriptArray->GetSize() ; step ++)
	{
		PluginInfo & plugin = piBufferScriptArray->ElementAt(step);
		if (plugin.m_bAutomatic == FALSE)
			continue;
		if (plugin.TestAgainstRegList(filteredText) == FALSE)
			continue;

		handler->pluginName = plugin.m_name;
		handler->bWithFile = FALSE;
		// probably it is for VB/VBscript so use a BSTR as argument
		bHandled = InvokePrediffBuffer(*bufferData.GetDataBufferUnicode(),
			bufferData.GetNChanged(),
			plugin.m_lpDispatch);
		if (bHandled)
			bufferData.ValidateNewBuffer();
	}

	if (bHandled == FALSE)
	{
		// we didn't find any prediffer, that is OK anyway
		handler->pluginName = _T("");
		bHandled = TRUE;
	}

	// the handler is now defined
	handler->bToBeScanned = FALSE;

	// if the buffer changed, write it before leaving
	BOOL bSuccess = TRUE;
	if (bufferData.GetNChangedValid() > 0)
	{
		bSuccess = bufferData.SaveAsFile(filepath);
	}

	return bSuccess;
}


////////////////////////////////////////////////////////////////////////////////

BOOL FileTransform_NormalizeUnicode(String & filepath, BOOL bMayOverwrite)
{
	String tempDir = env_GetTempPath();
	if (tempDir.empty())
		return FALSE;
	String tempFilepath = env_GetTempFileName(tempDir.c_str(), _T("_WM"));
	if (tempFilepath.empty())
		return FALSE;

	int nFileChanged = 0;
	BOOL bSuccess = UnicodeFileToOlechar(filepath.c_str(), tempFilepath.c_str(), nFileChanged); 
	if (!bSuccess)
		return FALSE;

	if (nFileChanged)
	{
		// we do not overwrite so we delete the old file
		if (bMayOverwrite)
		{
			if (!::DeleteFile(filepath.c_str()))
			{
				LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
					filepath.c_str(), GetSysError(GetLastError()).c_str()));
			}
		}
		// and change the filepath if everything works
		filepath = tempFilepath;
	}
	else
	{
		if (!::DeleteFile(tempFilepath.c_str()))
		{
			LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
				tempFilepath.c_str(), GetSysError(GetLastError()).c_str()));
		}
	}


	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

// for OLECHAR files, transform to UTF8 for diffutils
// TODO : convert Ansi to UTF8 if other file is unicode or uses a different codepage
BOOL FileTransform_UCS2ToUTF8(String & filepath, BOOL bMayOverwrite)
{
	String tempDir = env_GetTempPath();
	if (tempDir.empty())
		return FALSE;
	String tempFilepath = env_GetTempFileName(tempDir.c_str(), _T("_WM"));
	if (tempFilepath.empty())
		return FALSE;

	// TODO : is it better with the BOM or without (just change the last argument)
	int nFileChanged = 0;
	BOOL bSuccess = OlecharToUTF8(filepath.c_str(), tempFilepath.c_str(), nFileChanged, FALSE); 
	if (!bSuccess)
		return FALSE;

	if (nFileChanged)
	{
		// we do not overwrite so we delete the old file
		if (bMayOverwrite)
		{
			if (!::DeleteFile(filepath.c_str()))
			{
				LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
					filepath.c_str(), GetSysError(GetLastError()).c_str()));
			}
		}
		// and change the filepath if everything works
		filepath = tempFilepath;
	}
	else
	{
		if (!::DeleteFile(tempFilepath.c_str()))
		{
			LogErrorString(Fmt(_T("DeleteFile(%s) failed: %s"),
				tempFilepath.c_str(), GetSysError(GetLastError()).c_str()));
		}
	}

	return TRUE;
}



////////////////////////////////////////////////////////////////////////////////
// transformation : TextTransform_Interactive (editor scripts)

void GetFreeFunctionsInScripts(CStringArray & sNamesArray, LPCWSTR TransformationEvent)
{
	// get an array with the available scripts
	PluginArray * piScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(TransformationEvent);

	// fill in these structures
	int nFnc = 0;	
	int iScript;
	for (iScript = 0 ; iScript < piScriptArray->GetSize() ; iScript++)
	{
		PluginInfo & plugin = piScriptArray->ElementAt(iScript);
		LPDISPATCH piScript = plugin.m_lpDispatch;
		BSTR * scriptNamesArray;
		int * scriptIdsArray;
		int nScriptFnc = GetMethodsFromScript(piScript, scriptNamesArray, scriptIdsArray);
		sNamesArray.SetSize(nFnc+nScriptFnc);

		int iFnc;
		for (iFnc = 0 ; iFnc < nScriptFnc ; iFnc++)
			// the CString = operator provides the conversion if UNICODE is not defined
			sNamesArray[nFnc+iFnc] = scriptNamesArray[iFnc];

		delete [] scriptIdsArray;
		delete [] scriptNamesArray;

		nFnc += nScriptFnc;
	}
}

BOOL TextTransform_Interactive(CString & text, LPCWSTR TransformationEvent, int iFncChosen)
{
	if (iFncChosen < 0)
		return FALSE;

	// get an array with the available scripts
	PluginArray * piScriptArray = 
		CAllThreadsScripts::GetActiveSet()->GetAvailableScripts(TransformationEvent);

	int iScript;
	for (iScript = 0 ; iScript < piScriptArray->GetSize() ; iScript++)
	{
		if (iFncChosen < piScriptArray->GetAt(iScript).m_nFreeFunctions)
			// we have found the script file
			break;
		iFncChosen -= piScriptArray->GetAt(iScript).m_nFreeFunctions;
	}

	if (iScript >= piScriptArray->GetSize())
		return FALSE;

	// iFncChosen is the index of the function in the script file
	// we must convert it to the function ID
	int fncID = GetMethodIDInScript(piScriptArray->GetAt(iScript).m_lpDispatch, iFncChosen);

	// execute the transform operation
	BOOL bChanged = FALSE;
	InvokeTransformText(text, bChanged, piScriptArray->GetAt(iScript).m_lpDispatch, fncID);

	return bChanged;
}

