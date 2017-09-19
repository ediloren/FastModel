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


// ImportInColorView.cpp : implementation file
//

#include "stdafx.h"

#include "ImportInColorView.h"
#include "FastModel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImportInColorView

CImportInColorView::CImportInColorView()
{
}

CImportInColorView::~CImportInColorView()
{
}

void CImportInColorView::setColor(unsigned int colorIndex)
{
/*	if (colorIndex == FH_COMMENT)
		strcat(m_pColorEditView->m_sRTFLine, IFH_COMMENT_COLOR);
	else if ( (colorIndex == FH_DOT_UNI) || (colorIndex == FH_DOT_EXT) ||
			(colorIndex == FH_DOT_FRE) || (colorIndex == FH_DOT_EQU) ||
			(colorIndex == FH_DOT_DEF) || (colorIndex == FH_DOT_END) )
		strcat(m_pColorEditView->m_sRTFLine, IFH_DOT_COLOR);
	else if ( (colorIndex == FH_NODE) || (colorIndex == FH_SEGMENT) ||
			(colorIndex == FH_GROUND) )
		strcat(m_pColorEditView->m_sRTFLine, IFH_ELEMENT_COLOR);
	else if ( (colorIndex == FH_ERROR) )
		strcat(m_pColorEditView->m_sRTFLine, IFH_ERROR_COLOR);
	else
		strcat(m_pColorEditView->m_sRTFLine, IFH_DEFAULT_COLOR); */
}

void CImportInColorView::copyToRTF()
{
	strcat(m_pColorEditView->m_sRTFLine, m_sImportText);
	m_sImportText[0] = '\0';
}

// synchronize the input file to the next '\n'
void CImportInColorView::syncNewLine(char err)
{
	// is not error if called for istance at the 
	// end of an instruction and before the '\n'
	// just to be sure to get to next line
	if(err != IICV_SYNC_NO_ERROR) {
		// must print message only if in non-interactive mode
		if (m_uiOperation & IICV_GENERATE_ERRORS) {
			m_error.ErrorMsg("Trying to synchronize again");
		}
	}

	if(m_iTok!=LEXER_NEWLINE && m_iTok!=LEXER_EOF) {

		do {
			m_iTok = getTok();
		} while( m_iTok!=LEXER_NEWLINE && m_iTok!=LEXER_EOF );

	}
}

// skip n lines, coloring them with the same color
// nlines must be > 0; nlines=0 causes a line to be skipped 
// in any case
void CImportInColorView::skipLines(int nlines, int color)
{
	int i;

	// be sure to be at end of previous line
	syncAndAddNewLine(color);

	// then skip nlines-1 lines adding carriage returns
	for(i=0; i<nlines-1 && m_iTok != LEXER_EOF; i++) {

		// skip one line
		getTok();
		syncNewLine(0);

		// color line, adding the newline
		if (m_uiOperation & IICV_COLOR_LINES) {
			setColor(color);
			strcat(m_sImportText, "\\par ");
			copyToRTF();
			// set colored line in view, not to fill input buffer;
			// moreover, the color is already set
			m_pColorEditView->setRTFLine();
		}
	}

	// skip last line and do not add newline
	getTok();
	syncNewLine(0);

	// color line
	if (m_uiOperation & IICV_COLOR_LINES) {
		setColor(color);
		copyToRTF();
	}

}

void CImportInColorView::syncAndAddNewLine(int color)
{
	// position at end of previous line
	syncNewLine(0);				
	// add newline
	if (m_uiOperation & IICV_COLOR_LINES) {
		// set the color 
		setColor(color);
		strcat(m_sImportText, "\\par ");
		copyToRTF();
		// set colored line in view, not to fill input buffer;
		// moreover, the color is already set 
		m_pColorEditView->setRTFLine();
	}
}

void CImportInColorView::genError(int errnum, const char *fmt,...)
{
	va_list arg_ptr;

	// must print error message only if in non-interactive mode
	if (m_uiOperation & IICV_GENERATE_ERRORS) {

		va_start(arg_ptr, fmt);
		vsprintf(m_pCharBuffer, fmt, arg_ptr);
		va_end(arg_ptr);
	
		// no more valid, for how newlines are treated
		//
		// if last token read was a new line, the line numer has been already increased, so the
		// error is referred to previous line
		//if( m_iTok == LEXER_NEWLINE) 
		//	m_error.ErrorMsg("ERROR %s%04i - import module: %s on line %u", m_sErrorMsg, errnum, m_pCharBuffer, m_lBaseLN+m_lLineNum-1);
		//else
		m_error.ErrorMsg("ERROR %s%04i - import module: %s on line %u", m_sErrorMsg, errnum, m_pCharBuffer, m_lBaseLN+m_lLineNum);
	}

}

void CImportInColorView::genWarning(int warnum, const char *fmt,...)
{
	va_list arg_ptr;

	// must print error message only if in non-interactive mode
	if (m_uiOperation & IICV_GENERATE_ERRORS) {

		va_start(arg_ptr, fmt);
		vsprintf(m_pCharBuffer, fmt, arg_ptr);
		va_end(arg_ptr);

		// no more valid, for how newlines are treated
		//
		// if last token read was a new line, the line numer has been already increased, so the
		// error is referred to previous line
		//if( m_iTok == LEXER_NEWLINE) 
		//	m_error.ErrorMsg("Warning %s%04i - import module: %s on line %u", m_sErrorMsg, warnum, m_pCharBuffer, m_lBaseLN+m_lLineNum-1);
		//else
		m_error.ErrorMsg("Warning %s%04i - import module: %s on line %u", m_sErrorMsg, warnum, m_pCharBuffer, m_lBaseLN+m_lLineNum);
	}

}

char CImportInColorView::processLine()
{
	MSG msg;

	// every time a line is processed, the window message queue is processed for
	// pending messages, so paint (and other) operations can be performed during
	// lenghty importing tasks
	
	if(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		switch(msg.message) {
				
		// should process these events if it is needed
		// to handle also other events in client windows;
		// one example could be using a modeless dialog
		// box to show an 'Abort' button
		// Warning: as a side effect, also any other client
		// command is activated, including the toolbar buttons.
		// So the point position should be processed to allow
		// only the wanted operations
//		case WM_LBUTTONDOWN: 
//		case WM_RBUTTONDOWN: 
//		case WM_LBUTTONUP: 
//		case WM_RBUTTONUP: 
//		case WM_KEYDOWN: 

//			::DispatchMessage(&msg);
//			break;

		// used to handle window menu commands
//		case WM_COMMAND: 
			// only 'Abort' is allowed
//			if(LOWORD(msg.wParam) == ID_TEST_ABORT) 
//				::DispatchMessage(&msg);
//			break;

		case WM_PAINT:
		case WM_NCMOUSEMOVE:
		case WM_NCLBUTTONDOWN:
		case WM_NCLBUTTONUP:
		// used for maximize / restore from taskbar
		case WM_SYSCOMMAND:
		// for processing WM_CLOSE messages sent to child windows
		// (e.g. to close old FastCap child views)
		case WM_CLOSE:

			::DispatchMessage(&msg);
			break;
		}
	}

	// used to restore the arrow wait cursor, since above message processing
	// changes the cursor
	((CFastModelApp*)AfxGetApp())->RestoreArrowWaitCursor();

	switch (m_iTok)
	{
		// if EOF, add newline
		case LEXER_EOF :
			// add newline, if reading from file 
			if ((m_uiOperation & IICV_COLOR_LINES) && (m_uiOperation & IICV_FILE_SOURCE) ) 
				strcat(m_sImportText, "\\par ");

			return(IICV_OK);
	
		case LEXER_NEWLINE :
			// if end of line, add newline and increase line counter;
			m_lLineNum++;
			// add newline
			if (m_uiOperation & IICV_COLOR_LINES) {
				strcat(m_sImportText, "\\par ");
				copyToRTF();
			}

			return(IICV_NEWLINE);

		default:
			return(IICV_CONTINUE);

	}
	
	genError(1003, "internal error");
	return(1);
}

// main import function
int CImportInColorView::import(unsigned int operation, FILE *fp,
							  CColorEditView *colorEditView, C3DShell *model,
							  long baselineno, long startline, long endline)
{
	
	ASSERT(colorEditView);

	if ( (operation & IICV_CONVERT_TO_MODEL) && (model == NULL) ) {
		genError(1005, "internal error");
		return(IICV_ERROR);
	}

	if ( (operation & IICV_FILE_SOURCE) && (fp == NULL) ) {
		genError(1006, "internal error");
		return(IICV_ERROR);
	}

	if ( !(operation & IICV_COLOR_LINES) && (endline < startline) ) {
		genError(1007, "internal error");
		return(IICV_ERROR);
	}

	// save type of operation to perform
	m_uiOperation = operation;

	// save view for which we are importing 
	m_pColorEditView = colorEditView;

	// save in class common variable the load archive,
	// the base line number and the pointer to the geometry model
	m_fp = fp;
	m_iTok = LEXER_NULL;
	m_lBaseLN = baselineno;
	m_pShell = model;
	m_lStartLine = startline;
	m_lEndLine = endline;
	m_lViewLineNum = startline;
	m_iInputBufPos = 0;
	m_iCutChars = 0;
	m_sImportText[0] = '\0';
	m_lLineNum = 0;

	//
	// follows import-specific routines 
	//

	// do not forget to return the number of cut chars (in case of line too long)
	// return(m_iCutChars);
	return(IICV_CONTINUE);
}

// virtual
int  CImportInColorView::getTok()
{
	return(0);
}

// virtual
int  CImportInColorView::readTok(unsigned int *position)
{
	return(0);
}

// virtual
int  CImportInColorView::moveForward(unsigned int *position)
{
	return(0);
}


