cmake_minimum_required(VERSION 2.8)

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2)
set(SRCS
	ReadMe.txt
	RectTrack.cpp
	RectTrack.h
	RectTrack.rc
	RectTrackDlg.cpp
	RectTrackDlg.h
	Resource.h
	stdafx.cpp
	stdafx.h
	trckrect.cpp
	trckrect.h
)

add_executable(MFCRectTrack WIN32 ${SRCS})
