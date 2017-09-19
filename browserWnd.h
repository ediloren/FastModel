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


// browserWnd.h : header file
//

#ifndef BROWSERWND_H
#define BROWSERWND_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "browserSheet.h"

/////////////////////////////////////////////////////////////////////////////
// CBrowserWnd window

#ifndef baseCBrowserWnd
#define baseCBrowserWnd CSizingControlBarG
#endif

#define BW_MAX_STR_LEN		256

class CBrowserWnd : public baseCBrowserWnd
{
// Construction
public:
	CBrowserWnd();

	//void CBrowserWnd::SomeOperation(char *text);

// Operations
public:
	unsigned long addFastCapFile(char *filename);
	BOOL removeFastCapFile(unsigned long token);

	BOOL m_bVisible; 

protected:

	CBrowserSheet 	m_clsBrowserSheet; 
	unsigned long	m_ulIndex;

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputWnd)
	//}}AFX_VIRTUAL

BOOL Create(LPCTSTR lpszWindowName, CWnd* pParentWnd,
		CSize szHorz, CSize szVert , CSize szFloat,
        UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP);



// Implementation
public:
	virtual ~CBrowserWnd();

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

#endif // !defined(__BROWSERWND_H__)
