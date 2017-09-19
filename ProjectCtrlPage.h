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


#if !defined(AFX_PROJECTCTRLPAGE_H__CCDB4863_9FCE_11D5_9282_F4E514C10000__INCLUDED_)
#define AFX_PROJECTCTRLPAGE_H__CCDB4863_9FCE_11D5_9282_F4E514C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ProjectCtrlPage.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CProjectCtrlPage dialog

class CProjectCtrlPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CProjectCtrlPage)

// Construction
public:
	CProjectCtrlPage();
	~CProjectCtrlPage();

	HTREEITEM		m_fcitem, m_fhitem;

// Dialog Data
	//{{AFX_DATA(CProjectCtrlPage)
	enum { IDD = IDD_DIALOG_PROJECT };
	CTreeCtrl	m_projecttree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CProjectCtrlPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CImageList      *m_pImageList;
	
	// Generated message map functions
	//{{AFX_MSG(CProjectCtrlPage)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnKeydownTreeProject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTreeProject(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROJECTCTRLPAGE_H__CCDB4863_9FCE_11D5_9282_F4E514C10000__INCLUDED_)
