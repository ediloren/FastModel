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


// Global.cpp: implementation of global functions
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Global.h"
#include "ErrorHandler.h"

#define MAXLINELEN	1024

CErrorHandler error;

// this function handles all error messages
// TBC warning: no check on m_cCharBuffer overflows! 
void GlbErrorMsg(const char *fmt,...)
{
	char m_cCharBuffer[MAXLINELEN];

	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsprintf(m_cCharBuffer, fmt, arg_ptr);
	va_end(arg_ptr);
  
	strcat(m_cCharBuffer, "\n");
	error.m_pOutputWnd->OutputText(m_cCharBuffer);
}

// this function handles all warning messages 
// TBC warning: no check on m_cCharBuffer overflows! 
void GlbWarningMsg(const char *fmt,...)
{
	char m_cCharBuffer[MAXLINELEN];

	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsprintf(m_cCharBuffer, fmt, arg_ptr);
	va_end(arg_ptr);

	strcat(m_cCharBuffer, "\n");
	error.m_pOutputWnd->OutputText(m_cCharBuffer);
}

// this function handles all system errors
// TBC warning: no check on m_cCharBuffer overflows! 
void GlbSysErrorMsg(const char *fmt,...)
{
	char m_cCharBuffer[MAXLINELEN];

	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsprintf(m_cCharBuffer, fmt, arg_ptr);
	va_end(arg_ptr);
  
	strcat(m_cCharBuffer, "\n");
	error.m_pOutputWnd->OutputText(m_cCharBuffer);
}

// this function handles all debug messages
// TBC warning: no check on m_cCharBuffer overflows! 
void GlbDbgErrorMsg(const char *fmt,...)
{
	char m_cCharBuffer[MAXLINELEN];

	va_list arg_ptr;
	va_start(arg_ptr, fmt);
	vsprintf(m_cCharBuffer, fmt, arg_ptr);
	va_end(arg_ptr);
  
	strcat(m_cCharBuffer, "\n");
	error.m_pOutputWnd->OutputText(m_cCharBuffer);
}

