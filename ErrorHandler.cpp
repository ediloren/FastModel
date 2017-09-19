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


// ErrorHandler.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include <stdio.h>

#include "ErrorHandler.h"

// static members initialization
CMainFrame *CErrorHandler::m_pMainFrame = NULL;
COutputWnd *CErrorHandler::m_pOutputWnd = NULL;

// this function collects all normal errors from C++ language functions 
// TBC warning: no check on m_cCharBuffer overflows! 
void CErrorHandler::ErrorMsg(const char *fmt,...)
{
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsprintf(m_cCharBuffer, fmt, arg_ptr);
	va_end(arg_ptr);
  
	strcat(m_cCharBuffer, "\n");
	m_pOutputWnd->OutputText(m_cCharBuffer);
}

// Warning: as this function ADDS A CHAR at the end of the text buffer,
// the buffer must be long enough to accomodate it!
void CErrorHandler::ErrorWrite(char *text)
{
	strcat(text, "\n");
	m_pOutputWnd->OutputText(text);
}

/* this function handles all warning messages */
void CErrorHandler::WarningMsg(const char *fmt,...)
{
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsprintf(m_cCharBuffer, fmt, arg_ptr);
	va_end(arg_ptr);

	strcat(m_cCharBuffer, "\n");
	m_pOutputWnd->OutputText(m_cCharBuffer);
}

/* this function handles all system errors */
void CErrorHandler::SysErrorMsg(const char *fmt,...)
{
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsprintf(m_cCharBuffer, fmt, arg_ptr);
	va_end(arg_ptr);
  
	strcat(m_cCharBuffer, "\n");
	m_pOutputWnd->OutputText(m_cCharBuffer);
}

// this function collects all errors from C language functions 
extern "C" int commonErrorMsg(const char *fmt,...) 
{
	class CErrorHandler errorObj;
	char buffer[1024];

	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsprintf(buffer, fmt, arg_ptr);
	va_end(arg_ptr);

	errorObj.ErrorWrite(buffer);
    return(0);
}

void CErrorHandler::SetMainFrame(CMainFrame *mainframe)
{
	m_pMainFrame = mainframe;
	m_pOutputWnd = &(mainframe->m_wndOutput);
}
