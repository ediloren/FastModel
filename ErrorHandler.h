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


// ErrorHandler.h : interface of the CErrorHandler class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#define EH_MAXLINELEN	1024

#include "MainFrm.h"

class CErrorHandler
{
	friend void GlbErrorMsg(const char *fmt,...);
	friend void GlbWarningMsg(const char *fmt,...);
	friend void GlbSysErrorMsg(const char *fmt,...);
	friend void GlbDbgErrorMsg(const char *fmt,...);

public:
	void ErrorMsg(const char *fmt,...);
	void ErrorWrite(char *text);
	void SysErrorMsg(const char *fmt,...);
	void WarningMsg(const char *fmt,...);
	void SetMainFrame(CMainFrame *mainframe);

protected:
	static CMainFrame *m_pMainFrame;
	static COutputWnd *m_pOutputWnd;
	char m_cCharBuffer[EH_MAXLINELEN];
};

#endif
