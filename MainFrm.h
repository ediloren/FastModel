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


// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__78E7EC57_3755_11D4_A718_444553540000__INCLUDED_)
#define AFX_MAINFRM_H__78E7EC57_3755_11D4_A718_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "outputWnd.h"
#include "browserWnd.h"
#include "ChildFrm.h"

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	void CreateMDIWindows();
	CChildFrame *CreateNewModelWindow();
	CChildFrame *GetFastHenryFrame();
	CChildFrame *GetFastCapFrame();
		
	COutputWnd m_wndOutput;
	CBrowserWnd m_wndBrowser;
	CChildFrame *m_pFastHenryFrame;
	CChildFrame *m_pFastCapFrame;

// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	int m_nDatePaneNo;
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	BOOL		m_bCanClose;

public:
	inline void DisableClose() { m_bCanClose = FALSE; }
	inline void EnableClose() { m_bCanClose = TRUE; }

// Generated message map functions
protected:
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewOutput();
	afx_msg void OnUpdateViewOutput(CCmdUI* pCmdUI);
	afx_msg void OnViewFastHenryView();
	afx_msg void OnUpdateViewFastHenryView(CCmdUI* pCmdUI);
	afx_msg void OnViewFastCapView();
	afx_msg void OnUpdateViewFastCapView(CCmdUI* pCmdUI);
	afx_msg void OnViewProject();
	afx_msg void OnUpdateViewProject(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnHelp();
	afx_msg void OnModelNewWindow();
	afx_msg void OnViewClearOutputWin();
	//}}AFX_MSG
	afx_msg LRESULT OnFileChangeNotify(WPARAM fileCategory, LPARAM fileCode);
	void OnUpdateRowAndColumn(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()
private:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__78E7EC57_3755_11D4_A718_444553540000__INCLUDED_)
