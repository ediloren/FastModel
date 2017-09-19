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


// ColorEditView.h : header file
//

#if !defined(AFX_COLOREDITVIEW_H__6707BB43_88D1_11D5_9282_F4E514C10000__INCLUDED_)
#define AFX_COLOREDITVIEW_H__6707BB43_88D1_11D5_9282_F4E514C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// include for the CRichEditView class
#include <afxrich.h>

// max line lenght
#define CEV_MAX_LINE_LEN		512

// CEV_RTF_LINE_LEN is the max len of a single RTF line. Is is calculated
// assuming that the max len of a line is 1000 chars; then, 6 chars
// are needed to assign a color, so 6000 chars. 1000 chars are reserved for 
// RTF prologue (font, color tables, etc.)
# define CEV_RTF_LINE_LEN	7000

// define types of import operations
#define CEV_COLOR_LINES			1
#define CEV_CONVERT_TO_MODEL	2
#define CEV_FILE_SOURCE			4
#define CEV_GENERATE_ERRORS		8

#define CEV_VIEW_COLOREDIT		0
#define CEV_VIEW_FASTCAP		1
#define CEV_VIEW_FASTHENRY		2

// macros to extract single colors from RGB encoded in LONG
#define RED(r) (BYTE) (r)
#define GREEN(g) (BYTE) (g >> 8) 
#define BLUE(b) (BYTE) (b >> 16) 

/////////////////////////////////////////////////////////////////////////////
// CColorEditView view

class CColorEditView : public CRichEditView
{
protected: // create from serialization only
	CColorEditView();
	DECLARE_DYNCREATE(CColorEditView)

// Attributes
public:

// Operations
public:
	bool LoadWindowState(CString regSection);
	void SaveWindowState(CString regSection);
	virtual int ColorLines(long baselineno, long startline, long endline);

	char *m_sRTFLine;
	void setRTFLine();
	CString m_strPathName;
	// used to distinguish between FastHenry and FastCap views (and possibliy
	// also others?)
	int m_iViewType;
	BOOL m_isModified;

protected:
	void SetDefaultCharFormat();
	void SetDefaultParaFormat();
	virtual void ColorAndReplace(long baselineno, long startline, long endline);
	virtual void InitRTF();
	void SetRTF(CString sRTF);
	CString GetRTF();
	static DWORD CALLBACK CBStreamIn(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);
	static DWORD CALLBACK CBStreamOut(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG* pcb);

	char m_sRTF[CEV_RTF_LINE_LEN];
	unsigned long m_commentColor, m_commandColor, m_elementColor;
	unsigned long m_errorColor, m_defaultColor;

// Overrides
public:
	// override to select what can be pasted in the view
	BOOL CanPaste() const;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorEditView)
	public:
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CColorEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// Generated message map functions
	//{{AFX_MSG(CColorEditView)
	afx_msg void OnEditPaste();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLOREDITVIEW_H__6707BB43_88D1_11D5_9282_F4E514C10000__INCLUDED_)
