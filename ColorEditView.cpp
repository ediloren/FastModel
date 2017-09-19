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


// ColorEditView.cpp : implementation file
//

#include "stdafx.h"
#include "fastmodel.h"
#include "ColorEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorEditView

IMPLEMENT_DYNCREATE(CColorEditView, CRichEditView)

BEGIN_MESSAGE_MAP(CColorEditView, CRichEditView)
	//{{AFX_MSG_MAP(CColorEditView)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CColorEditView::CColorEditView()
{
	// initialize file name and status
	m_strPathName = "";
	m_isModified = FALSE;
	m_iViewType = CEV_VIEW_COLOREDIT;

	InitRTF();
}

CColorEditView::~CColorEditView()
{
}

bool CColorEditView::LoadWindowState(CString regSection)
{
	return ((CChildFrame*)GetParentFrame())->LoadWindowState(regSection);
}

void CColorEditView::SaveWindowState(CString regSection)
{
	((CChildFrame*)GetParentFrame())->SaveWindowState(regSection);
}

void CColorEditView::InitRTF()
{
	char color[30];

	m_commentColor = RGB(150,175,80);
	m_commandColor = RGB(255,0,255);
	m_elementColor = RGB(0,0,255);
	m_errorColor = RGB(255,0,0);
	m_defaultColor = RGB(0,0,0);

	// initialize RTF variables

	strcpy(m_sRTF, "{\\rtf1\\ansi\\ansicpg1252\\deff0\\deftab720");
	strcat(m_sRTF, "{\\fonttbl{\\f0\\fmodern Courier New;}}");

	strcat(m_sRTF, "{\\colortbl"); 
	sprintf(color, "\\red%u\\green%u\\blue%u;", RED(m_commentColor), GREEN(m_commentColor), BLUE(m_commentColor) );
	strcat(m_sRTF, color);
	sprintf(color, "\\red%u\\green%u\\blue%u;", RED(m_commandColor), GREEN(m_commandColor), BLUE(m_commandColor) );
	strcat(m_sRTF, color);
	sprintf(color, "\\red%u\\green%u\\blue%u;", RED(m_elementColor), GREEN(m_elementColor), BLUE(m_elementColor) );
	strcat(m_sRTF, color);
	sprintf(color, "\\red%u\\green%u\\blue%u;", RED(m_errorColor), GREEN(m_errorColor), BLUE(m_errorColor) );
	strcat(m_sRTF, color);
	sprintf(color, "\\red%u\\green%u\\blue%u;", RED(m_defaultColor), GREEN(m_defaultColor), BLUE(m_defaultColor) );
	strcat(m_sRTF, color);
	strcat(m_sRTF, "}");

	strcat(m_sRTF, "\\deflang1040\\pard\\ri10\\plain\\f0\\fs19 ");

	m_sRTFLine = m_sRTF + strlen(m_sRTF);
}

void CColorEditView::setRTFLine()
{
	// close RTF line
	strcat(m_sRTFLine, "}");
	// insert in view
	SetRTF(m_sRTF);
	// delete old line
	m_sRTFLine[0] = '\0';
}

void CColorEditView::SetDefaultCharFormat()
{
	CHARFORMAT cf;
	
	cf = GetCharFormatSelection();

	// reset all possible char formats to the only ones desired
	cf.dwMask = CFM_BOLD | CFM_CHARSET | CFM_COLOR | CFM_FACE | CFM_ITALIC
		| CFM_OFFSET | CFM_PROTECTED | CFM_STRIKEOUT | CFM_UNDERLINE;
	cf.dwEffects = 0;
	cf.yOffset = 0;
	cf.crTextColor = RGB(0, 0, 0); // color black
	cf.bCharSet = ANSI_CHARSET;
	cf.bPitchAndFamily = DEFAULT_PITCH | FF_MODERN;
	strcpy(cf.szFaceName, "Courier New");

	// set the char format on current selection
	SetCharFormat(cf);
	// set the char format as default for the view (any new text will 
	// have this format)
	GetRichEditCtrl().SetDefaultCharFormat(cf);
}

void CColorEditView::SetDefaultParaFormat()
{
	PARAFORMAT pf;

	pf = GetParaFormatSelection();
	
	pf.dwMask = PFM_ALIGNMENT | PFM_NUMBERING |  PFM_OFFSET | PFM_OFFSETINDENT
		| PFM_RIGHTINDENT | PFM_STARTINDENT |  PFM_TABSTOPS;

	pf.wNumbering = 0;
	pf.dxStartIndent = 0;
	pf.dxRightIndent = 10;
	pf.dxOffset = 0;
	pf.wAlignment = PFA_LEFT;
	pf.cTabCount = 0;
	pf.cTabCount = 0;

	SetParaFormat(pf);
}

CString CColorEditView::GetRTF()
{
	// return the RTF string of the text in the control.
	
	// stream out here.
	EDITSTREAM es;
	es.dwError = 0;

	// set the callback
	es.pfnCallback = CBStreamOut;		

	CString sRTF = "";

	// so sRTF receives the string
	es.dwCookie = (DWORD) &sRTF;	

	// call CRichEditCtrl::StreamOut to get the string.	
	GetRichEditCtrl().StreamOut(SF_RTF | SFF_SELECTION, es);			

	return sRTF;

}

void CColorEditView::SetRTF(CString sRTF)
{
	// put the RTF string sRTF into the rich edit control.

	// read the text in
	EDITSTREAM es;
	es.dwError = 0;
	es.pfnCallback = CBStreamIn;
	es.dwCookie = (DWORD) &sRTF;

	// insert
	GetRichEditCtrl().StreamIn(SF_RTF | SFF_SELECTION, es);	
	
}

// This callback function is used to stream an RTF string 
// into the rich edit control
DWORD CALLBACK CColorEditView::CBStreamIn(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	
	CString *pstr = (CString *) dwCookie;

	if (pstr->GetLength() < cb)
	{
		*pcb = pstr->GetLength();
		memcpy(pbBuff, (LPCSTR) *pstr, *pcb);
		pstr->Empty();
	}
	else
	{
		*pcb = cb;
		memcpy(pbBuff, (LPCSTR) *pstr, *pcb);
		*pstr = pstr->Right(pstr->GetLength() - cb);
	}

	return 0;
}

// This callback function is used to stream an RTF string 
// out of the rich edit control
DWORD CALLBACK CColorEditView::CBStreamOut(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	// the address of the string is in psEntry
	CString *psEntry = (CString*) dwCookie;
	
	CString tmpEntry = "";
	tmpEntry = (CString) pbBuff;

	// And write it!!!
	*psEntry += tmpEntry.Left(cb);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CColorEditView overrides

BOOL CColorEditView::CanPaste() const
{
	// only text is admissible
	return (CountClipboardFormats() != 0) &&
		(IsClipboardFormatAvailable(CF_TEXT) );
}

void CColorEditView::OnInitialUpdate() 
{
	long len;

	CRichEditView::OnInitialUpdate();
	
	// set the desired char and paragraph formats
	GetRichEditCtrl().SetSel(0,-1);
	SetDefaultCharFormat();
	SetDefaultParaFormat();	
	len = GetTextLength();
	GetRichEditCtrl().SetSel(len,len);

	// this option are needed to set auto horizontal scroll (no word wrap
	// at end of line)
	GetRichEditCtrl().SetOptions(ECOOP_SET, ECO_AUTOVSCROLL | ECO_AUTOHSCROLL);
	m_nWordWrap = WrapNone;
	WrapChanged();

	m_isModified = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CColorEditView message handlers

void CColorEditView::OnEditPaste()
{
	long startlineno, endlineno;

	ASSERT_VALID(this);

	// inhibit drawing in view
	SetRedraw(FALSE);

	// get current line
	startlineno = GetRichEditCtrl().LineFromChar(-1);

	// paste the text in the view (but there won't be a redraw,
	// because SetRedraw is FALSE)
	// remark: lines too long will be trimmed by the import functions
	GetRichEditCtrl().PasteSpecial(CF_TEXT);

	// get new current line
	endlineno = GetRichEditCtrl().LineFromChar(-1);

	ColorAndReplace(startlineno, startlineno, endlineno);
}

// Note: OnChar is called by TranslateMessage on a WM_KEYDOWN, WM_KEYUP event;
// the difference in respect to OnKeyDown is that in the latter case, only the 
// VIRTUAL key code of the pressed key is given (e.g. VK_SHIFT, VK_DEL, VK_A, etc.)
// while in the former is given the CHARACTER code (e.g. 'A' for a VK_SHIFT+VK_A
// group of events)
void CColorEditView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	long linelen, startlineno, endlineno;

	// inhibit drawing in view
	SetRedraw(FALSE);

	// get current line
	startlineno = GetRichEditCtrl().LineFromChar(-1);

	// write the new char in the view (but there won't be a redraw,
	// because SetRedraw is FALSE)
	CRichEditView::OnChar(nChar, nRepCnt, nFlags);

	// if the typed char makes current line longer than the maximum allowed,
	// undo operation and return: that is, inhibit further typing
	linelen = GetRichEditCtrl().LineLength(-1);
	// max len is CEV_MAX_LINE_LEN minus 1 char for the terminating
	// zero and 2 chars for the \r and \n
	if( GetRichEditCtrl().LineLength(-1) >= CEV_MAX_LINE_LEN - 3) {
		GetRichEditCtrl().Undo();

		// TBC warning: must inform that line is too long

		// allow drawing in view
		SetRedraw(TRUE);
		// and force redraw
		Invalidate();
		return;
	}

	// get new current line
	endlineno = GetRichEditCtrl().LineFromChar(-1);

	ColorAndReplace(startlineno, startlineno, endlineno);
}

void CColorEditView::ColorAndReplace(long baselineno, long startline, long endline)
{
	long lastpos, firstchar, startcharpos, endcharpos;
	int deletedChars;
	CHARRANGE cr;

	// retrieve start and end text position
	startcharpos = GetRichEditCtrl().LineIndex(startline); 
	endcharpos = GetRichEditCtrl().LineIndex(endline + 1);

	// get current caret position and first visible char
	GetRichEditCtrl().GetSel(cr);
	firstchar = GetRichEditCtrl().LineIndex(GetRichEditCtrl().GetFirstVisibleLine());
	
	if( endcharpos == -1) {
		endcharpos = GetTextLength() + 2;
	
	}

	// add a newline and a space as last char:
	// - must add newline; otherwise in case of 'return' key pressed, 
	//   import routing will add chars on the last line without
	//   a newline, therefore reading also what it itself has written
	// - must add a char in the end, because an RTF text ending with \par 
	//   will NOT insert a newline after the text. However, two consecutive 
	//   \par \par DOES add ONE newline (in the END of the edit window). 
	//   In all other cases, \par adds a newline. So is possible to add a
	//   dummy char in the end, and everything works as one would expect;
	//   then will be possible to remove the dummy char).
	lastpos = GetTextLength();
	GetRichEditCtrl().SetSel(lastpos, lastpos);
	GetRichEditCtrl().ReplaceSel(_T("\r\n "));

	// set the selection at end position (beginning of next line) 
	GetRichEditCtrl().SetSel(endcharpos, endcharpos);

	// insert new colored lines after the end position
	deletedChars = ColorLines(baselineno, startline, endline);

	// delete old lines
	GetRichEditCtrl().SetSel(startcharpos, endcharpos);
	GetRichEditCtrl().ReplaceSel(_T(""));

	// delete added dummy chars ("\r\n ")
	lastpos = GetTextLength();
	GetRichEditCtrl().SetSel(lastpos-3, lastpos);
	GetRichEditCtrl().ReplaceSel(_T(""));

	// set caret position to the original one, minus the eventually deleted
	// chars (because line too long)
	// Remark: the ReplaceSel(_T("")) are needed because, as stated in the
	// online docs, SetSel() does not scroll the contents of the CRichEditCtrl
	// so that the caret is visible; so a call to ReplaceSel forces scrolling
	cr.cpMin -= deletedChars;
	cr.cpMax -= deletedChars;
	GetRichEditCtrl().SetSel(firstchar, firstchar);
	GetRichEditCtrl().ReplaceSel(_T(""));
	GetRichEditCtrl().SetSel(cr.cpMax, cr.cpMax);
	GetRichEditCtrl().ReplaceSel(_T(""));

	// allow drawing in view
	SetRedraw(TRUE);
	// and force redraw
	Invalidate();

	// signal that the text has been modified
	m_isModified = TRUE;
}

// OnChar is not sufficient to handle all keys, because deals only with
// chars and not, for instance, arrow up, down, etc. and in particular
// the DEL key: when the user deletes a char, the line must be coloured
// accordingly 
void CColorEditView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// must consider DEL key pressure
	if( nChar == VK_DELETE) 
		OnChar(nChar, nRepCnt, nFlags);
	else
		CRichEditView::OnKeyDown(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
// CColorEditView virtual functions

int CColorEditView::ColorLines(long baselineno, long startline, long endline)
{
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CColorEditView diagnostics

#ifdef _DEBUG
void CColorEditView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CColorEditView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

#endif //_DEBUG



