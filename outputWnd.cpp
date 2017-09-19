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


// outputWnd.cpp : implementation file
//

#include "stdafx.h"

#include "outputWnd.h"
#include "fastmodel.h"
#include "MainFrm.h"

#ifndef FM_DEMO
// include for CINT
#include "G__ci.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef FM_DEMO
// needed by CINT, includes part in G__cpp_cintInterface.cxx
extern "C" {
extern void G__setothermain(int othermain);
extern void G__cpp_setupcintInterface();
}
#endif

BEGIN_MESSAGE_MAP(COutputWnd, baseCOutputWnd)
	//{{AFX_MSG_MAP(COutputWnd)
	ON_WM_CREATE()
    ON_WM_NCLBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// global var to store the output window object (to grant access 
// to callback global functions)
COutputWnd *theOutputWnd;

// event for CINT and OutputWnd gets synchronization
HANDLE getsEvent;


// callback functions
void PrintfCallback(char *text)
{
#ifndef FM_DEMO
	theOutputWnd->OutputText(text);
#endif
}

void GetsCallback(char *text)
{
#ifndef FM_DEMO
	// signal we are busy waiting for input
	theOutputWnd->SetWait(TRUE);

	// mark prompt 'free' text in edit window (on which user cannot write)
	theOutputWnd->SetCaptionFree();

	// wait indefinitely until input is typed in by the user
	// (VK_RETURN ends input)
	WaitForSingleObject(getsEvent, INFINITE);

	// no more waiting
	theOutputWnd->SetWait(FALSE);

	theOutputWnd->GetInput(text);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// COutputWnd

COutputWnd::COutputWnd()
{
	m_bEnableWriting = FALSE;
	m_wndChild.m_bWaitingForInput = FALSE;

#ifndef FM_DEMO
	// this function will be called by CINT thread
	// TBC warning: OutputText must be made multi-thread safe?
	G__set_printfcallback(PrintfCallback);
	G__set_errmsgcallback(PrintfCallback);
	G__set_getscallback(GetsCallback);
#endif
}

COutputWnd::~COutputWnd()
{
	// TBC warning: must TERMINATE thread! To do so, set the gets event to unlock
	// and signal thread to exit
}


/////////////////////////////////////////////////////////////////////////////
// COutputWnd overrides

// creation method with custom dock window size
BOOL COutputWnd::Create(LPCTSTR lpszWindowName, CWnd* pParentWnd, 
							   CSize szHorz, CSize szVert, CSize szFloat,
							   UINT nID, DWORD dwStyle)
{
	BOOL status;

    m_szHorz = szHorz;
	m_szVert = szVert;
	m_szFloat = szFloat;
	m_bVisible = TRUE;

	status = baseCOutputWnd::Create(lpszWindowName, pParentWnd, nID, dwStyle);

	return status;
}

void COutputWnd::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
    if (nHitTest == HTCLOSE) {
		// this is used to signal that docking window is now hidden
		m_bVisible = FALSE;
	}

    baseCOutputWnd::OnNcLButtonUp(nHitTest, point);
}

/////////////////////////////////////////////////////////////////////////////
// COutputWnd message handlers

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (baseCOutputWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetSCBStyle(GetSCBStyle() | SCBS_SHOWEDGES | SCBS_SIZECHILD);

	if (!m_wndChild.Create(WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL|
			ES_MULTILINE|ES_WANTRETURN|ES_AUTOVSCROLL|
			ES_AUTOHSCROLL,
		CRect(0,0,0,0), this, OW_CONTROL_IDD))
		return -1;

	m_wndChild.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	if (!m_font.CreatePointFont(OW_FONT_SIZE, OW_FONT_TYPE))
		return -1;

	m_wndChild.SetFont(&m_font);

	return 0;
}

// Operations

void COutputWnd::OutputText(char *text)
{
	// TBC warning: output must be made multithread-safe

	int startSel, endSel, lastpos, lastlinenum, lastlineindex;
	int insertPos, newlastpos, newlastlinenum, newlastlineindex;
	int deltastart, deltaend, gap;
	char *start, *newline, o_text[OW_MAXLINELEN];
	BOOL outputBefore;

	// copy m_bWaitingForInput because it could be modified by CINT thread
	// during function execution (even if this function is multi-thread safe)
	outputBefore = m_wndChild.m_bWaitingForInput;

	// retrieve selection (or simply caret) position
	m_wndChild.GetSel(startSel, endSel);

	// retrieve total text length
	lastpos = m_wndChild.GetWindowTextLength();

	lastlinenum = m_wndChild.GetLineCount();
	lastlineindex = m_wndChild.LineIndex(lastlinenum-1);

	// if waiting for input, text must appear before input line
	if(outputBefore == TRUE) {
		// text insertion point is at the end of previous line 
		// (just after the \r\n); this assumes that every system-generated
		// message ends with \r\n, not to make confusion with prompt!
		insertPos = lastlineindex;
	}
	// if not waiting, text insertion point is simply at the end of edit window
	else
		insertPos = lastpos;

	// if the output string + the text in the output window
	// would exceed the 16000 chars limit, delete the starting
	// part of the output to accomodate the output string
	if( lastpos + strlen(text) > OW_MAXCEDITCHARS ) {
		m_wndChild.SetSel(0, OW_CHARDELETEAMOUNT);
		m_wndChild.ReplaceSel(_T(""));
	}

	m_wndChild.SetSel(insertPos, insertPos);

	//
	// replace all newlines (\n) with carriage ret + newlines (\r\n) 
	//

	start = text;
	while( (newline = strchr(start, '\n')) != NULL)
	{
		// compute line lenght and truncate, if necessary
		gap = newline - start;
		if( gap > OW_MAXLINELEN )
			gap = OW_MAXLINELEN;

		strncpy(o_text, text, gap);
		o_text[gap] = '\0';

		m_wndChild.ReplaceSel(o_text);
		m_wndChild.ReplaceSel(_T("\r\n"));
		start = newline + 1;
	}

	// if last line did not end with newline, output it accordingly, without \r\n
	if( *start != '\0') {
		m_wndChild.ReplaceSel(start);
	}

	// restore caret position

	newlastlinenum = m_wndChild.GetLineCount();
	newlastlineindex = m_wndChild.LineIndex(newlastlinenum-1);

	// if waiting for input (outputBefore == TRUE), must restore selection; 
	// if selection was entirely on last line, set it again
	if(outputBefore == TRUE && endSel >= lastlineindex && startSel >= lastlineindex) {

		deltastart = startSel - lastlineindex;
		deltaend = endSel - lastlineindex;

		m_wndChild.SetSel(newlastlineindex + deltastart, newlastlineindex + deltaend);
	}
	// otherwise put caret on last pos
	else  {
		newlastpos = m_wndChild.GetWindowTextLength();
		m_wndChild.SetSel(newlastpos, newlastpos);
	}
}

void COutputWnd::SetWait(BOOL status)
{
	m_wndChild.m_bWaitingForInput = status;
}

// calculate and store prompt 'free' text (on which user cannot write)
void COutputWnd::SetCaptionFree()
{
	int lastpos, lastlinenum, lastlineindex;

	lastpos = m_wndChild.GetWindowTextLength();
	lastlinenum = m_wndChild.GetLineCount();
	lastlineindex = m_wndChild.LineIndex(lastlinenum-1);
	m_wndChild.m_iPromptLen = lastpos - lastlineindex;
}


void COutputWnd::GetInput(char *input)
{
	strcpy(input, (LPCTSTR)m_wndChild.m_strInputLine);
}

void COutputWnd::ClearPanel()
{
	// select all text in the edit window
	m_wndChild.SetSel(0, -1);
	// and clear it
	m_wndChild.ReplaceSel(_T(""));
}

//////////////////////////////////////////////////////////
// Worker Thread

UINT RunCintThread(LPVOID p)
{
#ifndef FM_DEMO
	COutputWnd *theOutWnd = (COutputWnd *)p;

	// Remark: G__pause() is used instead of G__main(); but G__main automatically
	// called G__init_cint(), so now it must be called manually. 
	// But where? Easy: G__main() does many things but fundamentally calls 
	// G__init_cint() followed by G__pause(). So these two instruction must
	// be kept together. It follows that if G__set_p2fsetup() was to be called 
	// before G__main(), it still must be called before G__init_cint()
	// No hint about G__setothermain(); it works fine anyway. Following this
	// reasonment, G__free_p2fsetup() has to be called after G__scratch_all().

	// calls taken from G__main.cxx created with makecint;
	// but TBC warning: does not work!!

	do {	
		G__set_p2fsetup(G__cpp_setupcintInterface);
		G__setothermain(0);

		// the argument of G__init_cint() is the prompt shown in the I/O window!!
		switch(G__init_cint(">"))
		{
			case G__INIT_CINT_SUCCESS:
				break;
			default:
				;
		}

		while(G__pause() == G__PAUSE_NORMAL); 

		// TBC warning: shuld handle problems in G__pause
		G__scratch_all();
		G__free_p2fsetup();

		theOutputWnd->OutputText("Restarting interpreter..\n");

		//bIsFHRunning = FALSE;
	} while(1);

#endif
	return 0;
}

void COutputWnd::StartCint()
{
#ifndef FM_DEMO
	// TBC warning: could add saving to global var of output wnd address, so the 
	// CINT I/O function callbacks does not lose time searching for the pointer every time

	theOutputWnd = this;

	// create event for gets synchronization
	m_wndChild.m_hndGetsEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	getsEvent = m_wndChild.m_hndGetsEvent;

	// initialization text
	OutputText("FastModel Interactive Interface 1.0\n");
	OutputText("Starting C/C++ interpreter..\n");
	OutputText("Ready\n");

	AfxBeginThread(RunCintThread, this);
#endif

}




