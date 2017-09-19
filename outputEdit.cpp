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


// OutputEdit.cpp : implementation file
//
// This is a CEdit-derived class to manage I/O from output window 


#include "stdafx.h"
#include "fastmodel.h"
#include "outputEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// COutputEdit

COutputEdit::COutputEdit()
{
	m_pLastCmd = m_pCurrentCmd = m_pNewCmd = new CCmdHistory;
}

COutputEdit::~COutputEdit()
{
	// upon destroy, must free the memory allocated for history
	// TBC warning: should remove ALL history!!
	delete  m_pLastCmd, m_pCurrentCmd, m_pNewCmd;


}


BEGIN_MESSAGE_MAP(COutputEdit, CEdit)
	//{{AFX_MSG_MAP(COutputEdit)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COutputEdit message handlers

void COutputEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
#ifndef FM_DEMO
	int lastpos, lastlinenum, lastlineindex, startPos, endPos;

	// if not waiting for user input, cannot write in edit control
	if(m_bWaitingForInput == FALSE)
		return;

	lastlinenum = GetLineCount();
	lastlineindex = LineIndex(lastlinenum-1);
	lastpos = GetWindowTextLength();

	switch(nChar) {

		case VK_LEFT:
			GetSel(startPos, endPos);
			if (startPos <= lastlineindex + m_iPromptLen) {
				SetSel(lastlineindex + m_iPromptLen, lastlineindex + m_iPromptLen);
				// routine ends here!
				return;
			}

			break;

		case VK_RIGHT:
			GetSel(startPos, endPos);
			if (endPos < lastlineindex + m_iPromptLen) {
				SetSel(lastpos, lastpos);
				// routine ends here!
				return;
			}

			break;

		case VK_UP:
			// if not at the end of the list
			if(m_pCurrentCmd->m_pPrev != NULL) {
				m_pCurrentCmd = m_pCurrentCmd->m_pPrev;
				// replace command line with last one
				SetSel(lastlineindex + m_iPromptLen, lastpos);
				ReplaceSel(m_pCurrentCmd->m_strCommand);
			}

			// routine ends here anyway!
			return;

		case VK_DOWN:
			// if not at the end of the list
			if(m_pCurrentCmd->m_pNext != NULL) {
				m_pCurrentCmd = m_pCurrentCmd->m_pNext;
				// replace command line with last one
				SetSel(lastlineindex + m_iPromptLen, lastpos);
				ReplaceSel(m_pCurrentCmd->m_strCommand);
			}

			// routine ends here anyway!
			return;

		case VK_DELETE:
			GetSel(startPos, endPos);
			if(startPos < lastlineindex + m_iPromptLen &&
				endPos >= lastlineindex + m_iPromptLen) {
				SetSel(lastlineindex + m_iPromptLen, endPos);
			}
			else if (startPos < lastlineindex + m_iPromptLen &&
						endPos < lastlineindex + m_iPromptLen) {
				SetSel(lastlineindex + m_iPromptLen, lastlineindex + m_iPromptLen);
				// routine ends here!
				return;
			}

			break;

		// handle ctrl-C event (stop CINT)
		case VK_CANCEL:


			break;

		default:
			
			break;
	}

#endif
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void COutputEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
#ifndef FM_DEMO
	int lastpos, lastlinenum, lastlineindex, startPos, endPos, len;

	// if not waiting for user input, cannot write in edit control
	if(m_bWaitingForInput == FALSE)
		return;

	lastlinenum = GetLineCount();
	lastlineindex = LineIndex(lastlinenum-1);
	lastpos = GetWindowTextLength();

	switch(nChar) {
		case '\r':

			// error condition: number of chars in edit ctrl 
			// is smaller than expected
			if(lastlineindex + m_iPromptLen > lastpos) {
				m_strInputLine = _T("");
			}
			else {
				len =  LineLength(lastlineindex);
				if(len >= 0) {
					// GetLine uses char buffer, so to use CString some conversion
					// is required (note that after having had CString buffer access
					// the buffer must be released)
					GetLine(lastlinenum-1, m_strInputLine.GetBufferSetLength(len), len+1);
					m_strInputLine.ReleaseBuffer();

					m_strInputLine = m_strInputLine.Right(len - m_iPromptLen);
				} 
				else
					// error!
					m_strInputLine = _T("");
			}

			// manually add newline in last position
			SetSel(lastpos, lastpos);
			ReplaceSel("\r\n");

			// save command in history list
			m_pNewCmd->m_strCommand = m_strInputLine;
			m_pLastCmd = m_pNewCmd;
			m_pNewCmd = new CCmdHistory;
			m_pNewCmd->m_pPrev = m_pLastCmd;
			m_pLastCmd->m_pNext = m_pNewCmd;
			m_pCurrentCmd = m_pNewCmd;

			// signal that a line of text has been entered by the user
			::SetEvent(m_hndGetsEvent);

			// routine ends here!
			return;

		// virtual-key code VK_BACK is the same as ascii BACKSPACE
		case VK_BACK:
			GetSel(startPos, endPos);
			if(startPos < lastlineindex + m_iPromptLen &&
				endPos > lastlineindex + m_iPromptLen) {
				SetSel(lastlineindex + m_iPromptLen, endPos);
			}
			else if (startPos <= lastlineindex + m_iPromptLen &&
						endPos <= lastlineindex + m_iPromptLen) {
				SetSel(lastlineindex + m_iPromptLen, lastlineindex + m_iPromptLen);
				// routine ends here!
				return;
			}

			break;

		default:
			GetSel(startPos, endPos);
			if(startPos < lastlineindex + m_iPromptLen &&
				endPos >= lastlineindex + m_iPromptLen) {
				SetSel(endPos, endPos);
			}
			else if (startPos < lastlineindex + m_iPromptLen &&
						endPos < lastlineindex + m_iPromptLen) {
				SetSel(lastpos, lastpos);
			}
			
			break;
	}
	
#endif

	CEdit::OnChar(nChar, nRepCnt, nFlags);

}
