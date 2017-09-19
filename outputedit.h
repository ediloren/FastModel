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


// OutputEdit.h : header file
//

#if !defined(AFX_OUTPUTEDIT_H__37DB5F37_D58E_4941_B058_3C1DB2A75D80__INCLUDED_)
#define AFX_OUTPUTEDIT_H__37DB5F37_D58E_4941_B058_3C1DB2A75D80__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CCmdHistory 
{
public:
	inline CCmdHistory() {m_strCommand = _T("");m_pNext = NULL;m_pPrev = NULL;}

	CString m_strCommand;
	CCmdHistory *m_pNext;
	CCmdHistory *m_pPrev;

};

/////////////////////////////////////////////////////////////////////////////
// COutputEdit window

class COutputEdit : public CEdit
{
// Construction
public:
	COutputEdit();

	BOOL m_bWaitingForInput;
	int m_iPromptLen;
	CString m_strInputLine;
	HANDLE m_hndGetsEvent;

// Attributes
public:

// Operations
public:

protected:
	CCmdHistory *m_pLastCmd, *m_pCurrentCmd, *m_pNewCmd;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COutputEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(COutputEdit)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OUTPUTEDIT_H__37DB5F37_D58E_4941_B058_3C1DB2A75D80__INCLUDED_)
