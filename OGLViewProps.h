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


#if !defined(AFX_OGLVIEWPROPS_H__BFE255A9_5B8D_4E1E_8345_1176377D297C__INCLUDED_)
#define AFX_OGLVIEWPROPS_H__BFE255A9_5B8D_4E1E_8345_1176377D297C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OGLViewProps.h : header file
//

#include "resource.h"

class COGLColors
{
public:
	COLORREF m_clsWireColor;
	COLORREF m_clsSolidColor;
	COLORREF m_clsBackTopColor;
	COLORREF m_clsBackBotColor;
	COLORREF m_clsAmbientLightColor;
};

/////////////////////////////////////////////////////////////////////////////
// COGLViewProps dialog

class COGLViewProps : public CDialog
{
// Construction
public:
	COGLViewProps(CWnd* pParent = NULL);   // standard constructor
	COGLColors GetColors();
	void InitColors(COGLColors colors);

// Dialog Data
	//{{AFX_DATA(COGLViewProps)
	enum { IDD = IDD_OGLVIEWPROPS };
	CButton	m_ctrlWireColor;
	CButton	m_ctrlSolidColor;
	CButton	m_ctrlBackTopColor;
	CButton	m_ctrlBackBotColor;
	CButton	m_ctrlAmbientLightColor;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COGLViewProps)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void SetButtonBitmap(CButton *button, CBitmap *bitmap, COLORREF color);

	COGLColors m_clsColors;

	CBitmap m_clsBitmapWireColor;
	CBitmap m_clsBitmapSolidColor;
	CBitmap m_clsBitmapBackTopColor;
	CBitmap m_clsBitmapBackBotColor;
	CBitmap m_clsBitmapAmbLightColor;

	// Generated message map functions
	//{{AFX_MSG(COGLViewProps)
	afx_msg void OnButtonWireframeColor();
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonSolidColor();
	afx_msg void OnButtonAmbientLightColor();
	afx_msg void OnButtonBackTopColor();
	afx_msg void OnButtonBackBotColor();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OGLVIEWPROPS_H__BFE255A9_5B8D_4E1E_8345_1176377D297C__INCLUDED_)
