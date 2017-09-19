/***************************************************************************
*                                                                          *
*   Copyright (c) 2017                                                     *
*   FastFieldSolvers S.R.L.  http://www.fastfieldsolvers.com               *
*                                                                          *
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU Lesser General Public License (LGPL)     *
*   as published by the Free Software Foundation; either version 2 of      *
*   the License, or (at your option) any later version.                    *
*   for detail see the LICENCE text file.                                  *
*                                                                          *
*   This program is distributed in the hope that it will be useful,        *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU Library General Public License for more details.                   *
*                                                                          *
*   You should have received a copy of the GNU Library General Public      *
*   License along with this program; if not, write to the Free Software    *
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307   *
*   USA                                                                    *
*                                                                          *
***************************************************************************/


// ControlSolvers.cpp : implementation file
//
// This is a class used to manage FastHenry and FastCap processes 
//
// An instance of this class is kept in CFastModelApp, then it is used by
// CFastModelApp (initialization), CFastModelDoc (FastHenry, FastCap run),
// CMainFrm (close consoles)

#include "stdafx.h"
#include "ControlSolvers.h"
#include "FastModel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// global pointer to FastCap main window
HWND FCHwnd;
// global pointer to FasterCap main window
HWND FRCHwnd;
// global pointer to FastHenry main window
HWND FHHwnd;

/////////////////////////////////////////////////////////////////////////////
// CControlSolvers


////////////////////////////////////////////////////////////
//
//  FastHenry communication functions
//


void CControlSolvers::StartFastHenry() 
{
	HKEY rkey;
	char app_path[_MAX_PATH];
	unsigned long len;
	CString path;

	// warninigs (all implemented):
	
	// 1) before sending messages to created process, must be sure
	//    it has started; can use WaitFor... (see codeproject article)

	// 2) then, before communicating again, can test is hwnd is valid using
	//    IsWindow(); if no longer valid (window closed) must launch another 
	//    process

	// 3) note that the created process, as uses threads, must shut down 
	//    working thread on the WM_CLOSE event (and wait for effective closure <- this one not
	//    implemented)

	// 4) to close remote app, send WM_CLOSE 


	FHHwnd = NULL;

	// Get FastHenry2 installation path
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\FastFieldSolvers\\FastHenry2\\Settings"), 0,
		KEY_READ, &rkey) != ERROR_SUCCESS) {
		AfxMessageBox("Cannot find FastHenry2 'Settings' key in Registry\nPlease try installing FastHenry2 again", MB_ICONSTOP);
	}
	else {
		len = _MAX_PATH;
		if ( RegQueryValueEx( rkey, _T("Path"), NULL, NULL, (unsigned char *)app_path, &len) != ERROR_SUCCESS ) {
			AfxMessageBox("Cannot find FastHenry2 path in Registry\nPlease try installing FastHenry2 again", MB_ICONSTOP);
		}
		else {
			path = app_path;
		}
	}
	RegCloseKey(rkey);

	// form full application path name
	path += _T("\\FastHenry2.exe");

	// check that FastHenry exe is in current directory
	FILE *fp = fopen( (LPCTSTR)path, "r");
	if( fp == NULL ) {
		AfxMessageBox("Cannot find FastHenry2 in its install directory\nPlease try installing FastHenry2 again", MB_ICONWARNING);
		m_bIsFastHenry = FALSE;
		return;
	}
	fclose(fp);

	m_bIsFastHenry = TRUE;

	STARTUPINFO stup;
	PROCESS_INFORMATION procInfo;

	stup.cb = sizeof(STARTUPINFO); 
    stup.lpReserved = NULL; 
    stup.lpDesktop = NULL; 
    stup.lpTitle = NULL; 
    stup.dwX = 0; 
    stup.dwY = 0; 
    stup.dwXSize = 0; 
    stup.dwYSize = 0; 
    stup.dwXCountChars = 0; 
    stup.dwYCountChars = 0; 
    stup.dwFillAttribute = 0; 
    stup.dwFlags = 0; 
    stup.wShowWindow = 0; 
    stup.cbReserved2 = 0; 
    stup.lpReserved2 = NULL; 
    stup.hStdInput = NULL; 
    stup.hStdOutput = NULL; 
    stup.hStdError = NULL; 

	CreateProcess((LPCTSTR)path, "",
		NULL,  // process security attributes
		NULL,   // thread security attributes
		  FALSE,  // handle inheritance flag
		  CREATE_NEW_CONSOLE, // creation flags
		  NULL,  // pointer to new environment block
		  NULL,   // pointer to current directory name
		  &stup,  // pointer to STARTUPINFO
		  &procInfo  // pointer to PROCESS_INFORMATION
	);

	m_dwFastHenryProcessId = procInfo.dwProcessId; 
    m_dwFastHenryThreadId = procInfo.dwThreadId;

	// wait for the FastHenry process to start
	WaitForInputIdle(procInfo.hProcess, INFINITE);

	// find the Window handle of FastHenry
	FindFastHenryHWND();

	// position window under all topmost windows
	::SetWindowPos( FHHwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

	// debug only
	//sprintf(msg, "Proc %x, thread %x", m_dwProcessId, m_dwThreadId);
	//AfxMessageBox(msg);

}

static BOOL CALLBACK EnumFHWindowsProc(
	HWND hwnd,      // handle to parent window
	LPARAM lParam   // application-defined value
	)
{

	char buf1[256], buf2[256]; 
	// char buf3[256], msg[512]; // debug
	int numChars;
	DWORD threadId, procId;
	
	numChars = GetClassName(hwnd, buf1, 256);

	numChars = GetWindowText(hwnd, buf2, 256);

	procId = 1;
	threadId = GetWindowThreadProcessId(hwnd, &procId);
	
	// debug only
	// sprintf(buf3, "Thread %x, proc %x", threadId, procId);

	// FastHenry title contains also the name
	// of the input file. So a test can be made only on the LAST chars
	// of title for matching with 'FastHenry2'.
	if( procId == (DWORD)lParam && (strncmp(buf2 + strlen(buf2) - 10, "FastHenry2", 10) == 0) )  {
		// debug only
		//sprintf(msg, "%s %s %s Hwnd %x", buf1, buf2, buf3, hwnd);
		//AfxMessageBox(msg);

		FHHwnd = hwnd;

		return FALSE;
	}

	return TRUE;

}

void CControlSolvers::FindFastHenryHWND() 
{
	BOOL ans;

	ans = EnumWindows(EnumFHWindowsProc, m_dwFastHenryProcessId);


	if ( FHHwnd == NULL ) {
		AfxMessageBox("Internal error: unable to Enum windows\nFastHenry2 is not usable", MB_ICONERROR);
		m_bIsFastHenry = FALSE;
	}
}

void CControlSolvers::RunFastHenry(CString pathName) 
{

	if( m_bIsFastHenry == FALSE ) {
		AfxMessageBox("Sorry cannot run FastHenry2:\nprogram has not been initialized", MB_ICONWARNING);
		return;
	}

	// check if FastHenry is still open
	if( !IsWindow(FHHwnd) ) {
		// if not, must restart 
		StartFastHenry();
		if( m_bIsFastHenry == FALSE ) {
			// if not able to restart, return; no message because already
			// given with StartFastHenry
			return;
		}
	}

	// bring FastHenry window in front 
	::BringWindowToTop(FHHwnd);

	// then send message
	CString dataToSend = "path name ";
	dataToSend += pathName;

	COPYDATASTRUCT cpd;
	LRESULT copyDataResult;

	cpd.dwData = 0;
	cpd.cbData = dataToSend.GetLength() + 1;
	cpd.lpData = (void*)dataToSend.GetBuffer(cpd.cbData);

	copyDataResult = ::SendMessage(FHHwnd, WM_COPYDATA, (WPARAM)AfxGetApp()->m_pMainWnd->GetSafeHwnd(), (LPARAM)&cpd);
	dataToSend.ReleaseBuffer();
}


void CControlSolvers::CloseFH() 
{

	if( m_bIsFastHenry = TRUE ) {
		// close remote FastHenry console
		if( IsWindow(FHHwnd) ) {
			LPARAM val = ::SendMessage(FHHwnd, WM_CLOSE, 0, 0);
		}
		// otherwise, colsole already closed, do nothing!		
	}
}

////////////////////////////////////////////////////////////
//
//  FastCap communication functions
//


void CControlSolvers::StartFastCap() 
{
	HKEY rkey;
	char app_path[_MAX_PATH];
	unsigned long len;
	CString path;

	// warninigs (all implemented):
	
	// 1) before sending messages to created process, must be sure
	//    it has started; can use WaitFor... (see codeproject article)

	// 2) then, before communicating again, can test is hwnd is valid using
	//    IsWindow(); if no longer valid (window closed) must launch another 
	//    process

	// 3) note that the created process, as uses threads, must shut down 
	//    working thread on the WM_CLOSE event (and wait for effective closure <- this one not
	//    implemented)

	// 4) to close remote app, send WM_CLOSE 

	FCHwnd = NULL;

	// Get FastCap2 installation path
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\FastFieldSolvers\\FastCap2\\Settings"), 0,
		KEY_READ, &rkey) != ERROR_SUCCESS) {
		AfxMessageBox("Cannot find FastCap2 'Settings' key in Registry\nPlease try installing FastCap2 again", MB_ICONSTOP);
	}
	else {
		len = _MAX_PATH;
		if ( RegQueryValueEx( rkey, _T("Path"), NULL, NULL, (unsigned char *)app_path, &len) != ERROR_SUCCESS ) {
			AfxMessageBox("Cannot find FastCap2 path in Registry\nPlease try installing FastCap2 again", MB_ICONSTOP);
		}
		else {
			path = app_path;
		}
	}
	RegCloseKey(rkey);

	// form full application path name
	path += _T("\\FastCap2.exe");

	// check that FastCap exe is in current directory
	FILE *fp = fopen( (LPCTSTR)path, "r");
	if( fp == NULL ) {
		AfxMessageBox("Cannot find FastCap2 in its install directory\nPlease try installing FastCap2 again", MB_ICONWARNING);
		m_bIsFastCap = FALSE;
		return;
	}
	fclose(fp);

	m_bIsFastCap = TRUE;

	STARTUPINFO stup;
	PROCESS_INFORMATION procInfo;

	stup.cb = sizeof(STARTUPINFO); 
    stup.lpReserved = NULL; 
    stup.lpDesktop = NULL; 
    stup.lpTitle = NULL; 
    stup.dwX = 0; 
    stup.dwY = 0; 
    stup.dwXSize = 0; 
    stup.dwYSize = 0; 
    stup.dwXCountChars = 0; 
    stup.dwYCountChars = 0; 
    stup.dwFillAttribute = 0; 
    stup.dwFlags = 0; 
    stup.wShowWindow = 0; 
    stup.cbReserved2 = 0; 
    stup.lpReserved2 = NULL; 
    stup.hStdInput = NULL; 
    stup.hStdOutput = NULL; 
    stup.hStdError = NULL; 

	CreateProcess((LPCTSTR)path, "",
		NULL,  // process security attributes
		NULL,   // thread security attributes
		  FALSE,  // handle inheritance flag
		  CREATE_NEW_CONSOLE, // creation flags
		  NULL,  // pointer to new environment block
		  NULL,   // pointer to current directory name
		  &stup,  // pointer to STARTUPINFO
		  &procInfo  // pointer to PROCESS_INFORMATION
	);

	m_dwFastCapProcessId = procInfo.dwProcessId; 
    m_dwFastCapThreadId = procInfo.dwThreadId;

	// wait for the FastCap process to start
	WaitForInputIdle(procInfo.hProcess, INFINITE);

	// find the Window handle of FastCap
	FindFastCapHWND();

	// position window under all topmost windows
	::SetWindowPos( FCHwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

	// debug only
	//sprintf(msg, "Proc %x, thread %x", m_dwProcessId, m_dwThreadId);
	//AfxMessageBox(msg);

}

static BOOL CALLBACK EnumFCWindowsProc(
	HWND hwnd,      // handle to parent window
	LPARAM lParam   // application-defined value
	)
{

	char buf1[256], buf2[256]; 
	// char buf3[256], msg[512]; // debug
	int numChars;
	DWORD threadId, procId;
	
	numChars = GetClassName(hwnd, buf1, 256);

	numChars = GetWindowText(hwnd, buf2, 256);

	procId = 1;
	threadId = GetWindowThreadProcessId(hwnd, &procId);
	
	// debug only
	// sprintf(buf3, "Thread %x, proc %x", threadId, procId);

	// FastCap title contains also the name
	// of the input file. So a test can be made only on the LAST chars
	// of title for matching with 'FastCap2'.
	if( procId == (DWORD)lParam && (strncmp(buf2 + strlen(buf2) - 8, "FastCap2", 8) == 0) )  {
		// debug only
		//sprintf(msg, "%s %s %s Hwnd %x", buf1, buf2, buf3, hwnd);
		//AfxMessageBox(msg);

		FCHwnd = hwnd;

		return FALSE;
	}

	return TRUE;

}

void CControlSolvers::FindFastCapHWND() 
{
	BOOL ans;

	ans = EnumWindows(EnumFCWindowsProc, m_dwFastCapProcessId);

	if ( FCHwnd == NULL ) {
		AfxMessageBox("Internal error: unable to Enum windows\nFastCap2 is not usable", MB_ICONERROR);
		m_bIsFastCap = FALSE;
	}
	
}

void CControlSolvers::RunFastCap(CString pathName) 
{

	if( m_bIsFastCap == FALSE ) {
		AfxMessageBox("Sorry cannot run FastCap2:\nprogram has not been initialized", MB_ICONWARNING);
		return;
	}

	// check if FastCap is still open
	if( !IsWindow(FCHwnd) ) {
		// if not, must restart 
		StartFastCap();
		if( m_bIsFastCap == FALSE ) {
			// if not able to restart, return; no message because already
			// given with StartFastCap
			return;
		}
	}

	// bring FastCap window in front 
	::BringWindowToTop(FCHwnd);

	// then send message
	CString dataToSend = "path name ";
	dataToSend += pathName;

	COPYDATASTRUCT cpd;
	LRESULT copyDataResult;

	cpd.dwData = 0;
	cpd.cbData = dataToSend.GetLength() + 1;
	cpd.lpData = (void*)dataToSend.GetBuffer(cpd.cbData);

	copyDataResult = ::SendMessage(FCHwnd, WM_COPYDATA, (WPARAM)AfxGetApp()->m_pMainWnd->GetSafeHwnd(), (LPARAM)&cpd);
	dataToSend.ReleaseBuffer();
}


void CControlSolvers::CloseFC() 
{

	if( m_bIsFastCap = TRUE ) {
		// close remote FastCap console
		if( IsWindow(FCHwnd) ) {
			LPARAM val = ::SendMessage(FCHwnd, WM_CLOSE, 0, 0);
		}
		// otherwise, colsole already closed, do nothing!		
	}

}

////////////////////////////////////////////////////////////
//
//  FasterCap communication functions
//


void CControlSolvers::StartFasterCap() 
{
	HKEY rkey;
	char app_path[_MAX_PATH];
	unsigned long len;
	CString path;

	// warninigs (all implemented):
	
	// 1) before sending messages to created process, must be sure
	//    it has started; can use WaitFor... (see codeproject article)

	// 2) then, before communicating again, can test is hwnd is valid using
	//    IsWindow(); if no longer valid (window closed) must launch another 
	//    process

	// 3) note that the created process, as uses threads, must shut down 
	//    working thread on the WM_CLOSE event (and wait for effective closure <- this one not
	//    implemented)

	// 4) to close remote app, send WM_CLOSE 

	FRCHwnd = NULL;

	// Get FasterCap installation path
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\FastFieldSolvers\\FasterCap\\Settings"), 0,
		KEY_READ, &rkey) != ERROR_SUCCESS) {
		AfxMessageBox("Cannot find FasterCap 'Settings' key in Registry\nPlease try installing FasterCap again", MB_ICONSTOP);
	}
	else {
		len = _MAX_PATH;
		if ( RegQueryValueEx( rkey, _T("Path"), NULL, NULL, (unsigned char *)app_path, &len) != ERROR_SUCCESS ) {
			AfxMessageBox("Cannot find FasterCap path in Registry\nPlease try installing FasterCap again", MB_ICONSTOP);
		}
		else {
			path = app_path;
		}
	}
	RegCloseKey(rkey);

	// form full application path name
	path += _T("\\FasterCap.exe");

	// check that FasterCap exe is in current directory
	FILE *fp = fopen( (LPCTSTR)path, "r");
	if( fp == NULL ) {
		AfxMessageBox("Cannot find FasterCap in its install directory\nPlease try installing FasterCap again", MB_ICONWARNING);
		m_bIsFasterCap = FALSE;
		return;
	}
	fclose(fp);

	m_bIsFasterCap = TRUE;

	STARTUPINFO stup;
	PROCESS_INFORMATION procInfo;

	stup.cb = sizeof(STARTUPINFO); 
    stup.lpReserved = NULL; 
    stup.lpDesktop = NULL; 
    stup.lpTitle = NULL; 
    stup.dwX = 0; 
    stup.dwY = 0; 
    stup.dwXSize = 0; 
    stup.dwYSize = 0; 
    stup.dwXCountChars = 0; 
    stup.dwYCountChars = 0; 
    stup.dwFillAttribute = 0; 
    stup.dwFlags = 0; 
    stup.wShowWindow = 0; 
    stup.cbReserved2 = 0; 
    stup.lpReserved2 = NULL; 
    stup.hStdInput = NULL; 
    stup.hStdOutput = NULL; 
    stup.hStdError = NULL; 

	CreateProcess((LPCTSTR)path, "",
		NULL,  // process security attributes
		NULL,   // thread security attributes
		  FALSE,  // handle inheritance flag
		  CREATE_NEW_CONSOLE, // creation flags
		  NULL,  // pointer to new environment block
		  NULL,   // pointer to current directory name
		  &stup,  // pointer to STARTUPINFO
		  &procInfo  // pointer to PROCESS_INFORMATION
	);

	m_dwFasterCapProcessId = procInfo.dwProcessId; 
    m_dwFasterCapThreadId = procInfo.dwThreadId;

	// Changed the way to launch FasterCap, to be compatible with a console application
	// (wxWidgets porting of FasterCap starts as a console, to launch a GUI only if
	// the user has not requested to launch FasterCap in text only mode).
	// The modification consists in not using anymore WaitForInputIdle()
	// in CControlSolvers::StartFasterCap(), since WaitForInputIdle() returns immediately
	// for a console application (remark: old VS 6.0 docs are wrong, GetLastError()
	// does not return the error code for WaitForInputIdle(), and the function does not work
	// if the process in the function parameter is a console, not if the caller is a console)
	//
	// wait for the FasterCap process to start
	//WaitForInputIdle(procInfo.hProcess, INFINITE);

	// find the Window handle of FasterCap
	FindFasterCapHWND();

	// position window under all topmost windows
	::SetWindowPos( FRCHwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

	// debug only
	//sprintf(msg, "Proc %x, thread %x", m_dwProcessId, m_dwThreadId);
	//AfxMessageBox(msg);

}

static BOOL CALLBACK EnumFRCWindowsProc(
	HWND hwnd,      // handle to parent window
	LPARAM lParam   // application-defined value
	)
{

	char buf1[256], buf2[256]; 
	// char buf3[256], msg[512]; // debug
	int numChars;
	DWORD threadId, procId;
	
	numChars = GetClassName(hwnd, buf1, 256);

	numChars = GetWindowText(hwnd, buf2, 256);

	procId = 1;
	threadId = GetWindowThreadProcessId(hwnd, &procId);
	
	// debug only
	// sprintf(buf3, "Thread %x, proc %x", threadId, procId);

	// FasterCap title contains also the name
	// of the input file. So a test can be made only on the LAST chars
	// of title for matching with 'FasterCap'.
	if( procId == (DWORD)lParam && (strncmp(buf2 + strlen(buf2) - 9, "FasterCap", 9) == 0) )  {
		// debug only
		//sprintf(msg, "%s %s %s Hwnd %x", buf1, buf2, buf3, hwnd);
		//AfxMessageBox(msg);

		FRCHwnd = hwnd;

		return FALSE;
	}

	return TRUE;

}

void CControlSolvers::FindFasterCapHWND() 
{
	BOOL ans;
	int i;

	// since for FasterCap we cannot use WaitForInputIdle(), test and wait for a reasonable time until
	// the FasterCap window is available
	for(i=0; i<10 && FRCHwnd == NULL; i++) {
		ans = EnumWindows(EnumFRCWindowsProc, m_dwFasterCapProcessId);
		Sleep(100);
	}

	if ( FRCHwnd == NULL ) {
		AfxMessageBox("Internal error: unable to Enum windows\nFasterCap is not usable", MB_ICONERROR);
		m_bIsFasterCap = FALSE;
	}
	
}

void CControlSolvers::RunFasterCap(CString pathName) 
{

	if( m_bIsFasterCap == FALSE ) {
		AfxMessageBox("Sorry cannot run FasterCap:\nprogram has not been initialized", MB_ICONWARNING);
		return;
	}

	// check if FasterCap is still open
	if( !IsWindow(FRCHwnd) ) {
		// if not, must restart 
		StartFasterCap();
		if( m_bIsFasterCap == FALSE ) {
			// if not able to restart, return; no message because already
			// given with StartFasterCap
			return;
		}
	}

	// bring FasterCap window in front 
	::BringWindowToTop(FRCHwnd);

	// then send message
	CString dataToSend = "path name ";
	dataToSend += pathName;

	COPYDATASTRUCT cpd;
	LRESULT copyDataResult;

	cpd.dwData = 0;
	cpd.cbData = dataToSend.GetLength() + 1;
	cpd.lpData = (void*)dataToSend.GetBuffer(cpd.cbData);

	copyDataResult = ::SendMessage(FRCHwnd, WM_COPYDATA, (WPARAM)AfxGetApp()->m_pMainWnd->GetSafeHwnd(), (LPARAM)&cpd);
	dataToSend.ReleaseBuffer();
}


void CControlSolvers::CloseFRC() 
{

	if( m_bIsFasterCap = TRUE ) {
		// close remote FasterCap console
		if( IsWindow(FRCHwnd) ) {
			LPARAM val = ::SendMessage(FRCHwnd, WM_CLOSE, 0, 0);
		}
		// otherwise, colsole already closed, do nothing!		
	}
}



