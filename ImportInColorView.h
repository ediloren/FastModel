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


#if !defined(AFX_IMPORTINCOLORVIEW_H__1FC21723_9309_11D5_9282_F4E514C1148B__INCLUDED_)
#define AFX_IMPORTINCOLORVIEW_H__1FC21723_9309_11D5_9282_F4E514C1148B__INCLUDED_

// ImportInColorView.h : header file
//

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Lexer.h"
#include "ErrorHandler.h"
#include "ColorEditView.h"
#ifndef C3D_GEOMETRY_DEFS
	#include "Geometry.h"
#endif

#define FALSE 0 
#define TRUE 1 

#ifndef MAX
#define MAX(A,B)  ( (A) > (B) ? (A) : (B) )
#endif

// define types of import operations; 
// note that values >= 256 are free to be defined by overloaded classes
#define IICV_COLOR_LINES			1
#define IICV_CONVERT_TO_MODEL		2
#define IICV_FILE_SOURCE			4
#define IICV_GENERATE_ERRORS		8

// define result types for processLine function
#define IICV_ERROR		-1
#define IICV_OK			0
#define IICV_CONTINUE	1
#define IICV_NEWLINE	2

// these are RTF color codes
#define IICV_COMMENT_COLOR "\\cf0 "
#define IICV_CONTROL_COLOR "\\cf1 "
#define IICV_ELEMENT_COLOR "\\cf2 "
#define IICV_ERROR_COLOR "\\cf3 "
#define IICV_DEFAULT_COLOR "\\cf4 "

// defines for syncNewLine
#define IICV_SYNC_NO_ERROR		0
#define IICV_SYNC_WITH_ERROR	1

#define IICV_MAX_LINE_LEN	256 
#define IICV_MAX_IMPORT_LEN	4096
#define IICV_MAX_ERROR_MSG	32
#define IICV_MAX_CHAR_BUF	1024

class CImportInColorView
{

public:
	virtual int import(unsigned int operation, FILE *fp,
		CColorEditView *colorEditView, C3DShell *model,
		long baselineno, long startline, long endline);
	void genError(int errnum, const char *fmt,...);
	void genWarning(int warnum, const char *fmt,...);

	unsigned int m_uiOperation;
	int m_iInputBufPos, m_iCutChars;
	long m_lViewLineNum, m_lStartLine, m_lEndLine;
	CColorEditView *m_pColorEditView;

protected:
	CImportInColorView();     
	~CImportInColorView();	
	virtual void setColor(unsigned int colorIndex);
	void copyToRTF();
	void syncNewLine(char err);
	void syncAndAddNewLine(int color);
	void skipLines(int nlines, int color);
	virtual char processLine();
	virtual int getTok();
	virtual int readTok(unsigned int *position);
	virtual int moveForward(unsigned int *position);

	char m_sImportText[IICV_MAX_IMPORT_LEN], m_sErrorMsg[IICV_MAX_ERROR_MSG];
	int m_iTok;
	CErrorHandler m_error;
	long m_lBaseLN, m_lLineNum;
	C3DShell *m_pShell;
	char m_pCharBuffer[IICV_MAX_CHAR_BUF];
	FILE *m_fp;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTINCOLORVIEW_H__1FC21723_9309_11D5_9282_F4E514C1148B__INCLUDED_)
