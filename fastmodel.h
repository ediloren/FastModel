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


// fastmodel.h : main header file for the FASTMODEL application
//

#if !defined(AFX_FASTMODEL_H__78E7EC53_3755_11D4_A718_444553540000__INCLUDED_)
#define AFX_FASTMODEL_H__78E7EC53_3755_11D4_A718_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "fastmodelDoc.h"
#include "ControlSolvers.h"

#define FMA_WAIT_CURSOR				0
#define FMA_APPSTARTING_CURSOR		0

#define DELETE_EXCEPTION(e) do { e->Delete(); } while (0)

/////////////////////////////////////////////////////////////////////////////
// CFastModelApp:
// See fastmodel.cpp for the implementation of this class
//

class CFastModelApp : public CWinApp
{
public:
	CFastModelApp();
	CFastModelDoc *GetFastModelDoc();
	CMainFrame *GetMainFrame();

	CMainFrame* m_pMDIMainFrameWnd;
	CMultiDocTemplate *m_pModelViewTemplate;
	CMultiDocTemplate *m_pFastHenryViewTemplate;
	CMultiDocTemplate *m_pFastCapViewTemplate;
	CControlSolvers m_clControlSolvers;
	CString m_strAppPath, m_strBasePath;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFastModelApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
private:
	void ErrorProjMessage();
	void DoArrowWaitCursor(int nCode);

	HCURSOR m_hcurAppstarting;
	HCURSOR m_hcurArrowWaitCursorRestore;
	int m_nArrowWaitCursorCount;

public:

	inline void BeginArrowWaitCursor() { DoArrowWaitCursor(1); }
	inline void EndArrowWaitCursor()	{ DoArrowWaitCursor(-1); }
	inline void RestoreArrowWaitCursor()	{ DoArrowWaitCursor(0); }

public:
	//{{AFX_MSG(CFastModelApp)
	afx_msg void OnAppAbout();
	afx_msg BOOL OnImportFile();
	afx_msg void OnFileNew();
	afx_msg void OnFileCloseProj();
	afx_msg void OnFileNewProj();
	afx_msg void OnFileOpenProj();
	afx_msg void OnFileSaveAsProj();
	afx_msg void OnFileSaveProj();
	afx_msg void OnAppExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FASTMODEL_H__78E7EC53_3755_11D4_A718_444553540000__INCLUDED_)
