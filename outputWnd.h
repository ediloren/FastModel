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


#ifndef OUTPUTWND_H
#define OUTPUTWND_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dockWnd.h : header file
//

#include "outputEdit.h"

#define OW_MAXCEDITCHARS	16000
#define OW_CHARDELETEAMOUNT	4000
#define OW_MAXLINELEN		1024
#define OW_FONT_TYPE		"Courier New"
#define OW_FONT_SIZE		100
#define OW_CONTROL_IDD		124


/////////////////////////////////////////////////////////////////////////////
// COutputWnd window

#ifndef baseCOutputWnd
#define baseCOutputWnd CSizingControlBarG
#endif

class COutputWnd : public baseCOutputWnd
{
// Construction
public:
	COutputWnd();
	void OutputText(char *text);
	void StartCint();
	inline void SetWait(BOOL status);
	void SetCaptionFree();
	void GetInput(char *input);
	void ClearPanel();

// Attributes
public:
	BOOL m_bVisible; 

	// member variable for interaction with CINT 
	volatile BOOL m_bEnableWriting;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputWnd)
	//}}AFX_VIRTUAL

BOOL Create(LPCTSTR lpszWindowName, CWnd* pParentWnd,
		CSize szHorz, CSize szVert , CSize szFloat,
        UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP);

private:

// Implementation
public:
	virtual ~COutputWnd();

protected:
	COutputEdit		m_wndChild;
	CFont			m_font;


	// Generated message map functions
protected:
	//{{AFX_MSG(COutputWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(__OUTPUTWND_H__)
