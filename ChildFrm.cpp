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


// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "fastmodel.h"

#include "ChildFrm.h"

// must include FastHenry and FastCap views definitions for use
// in override of function ::OnUpdateFrameTitle
#include "FastHenryView.h"
#include "FastCapView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
	
}

CChildFrame::~CChildFrame()
{
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame overrides

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CMDIChildWnd::PreCreateWindow(cs);
}

// This function has been overridden to prefix FastHenry or FastCap name
// to the corresponding view frame caption
void CChildFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	char tmp[CF_MAX_STR_LEN], fname[CF_MAX_STR_LEN], ext[CF_MAX_STR_LEN];

	// update our parent window first
	GetMDIFrame()->OnUpdateFrameTitle(bAddToTitle);

	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave child window alone!

	CDocument* pDocument = GetActiveDocument();
	if (bAddToTitle)
	{
		TCHAR szText[256+_MAX_PATH];


		// check if active view is one of FastHenry or FastCap view;
		// if so, prefix view name

		lstrcpy(szText, _T(""));

		CView *pView = (CView *) GetActiveView();

		// must check for NULL because IsKindOf ASSERTs the class, therefore
		// throwing an exception if found NULL
		if( pView != NULL)
			if ( pView->IsKindOf(RUNTIME_CLASS(CFastHenryView)) ) {
				lstrcpy(szText, _T("FastHenry2 - "));

				// split path into components and assemble text to insert in caption
				_splitpath((LPCSTR)(((CFastHenryView*)pView)->m_strPathName), tmp, tmp, fname, ext);
				strcat(fname,ext);

				wsprintf(szText + lstrlen(szText), _T("%s"), fname);
			}

			else if ( pView->IsKindOf(RUNTIME_CLASS(CFastCapView)) ) {
				lstrcpy(szText, _T("FastCap2 - "));

				// split path into components and assemble text to insert in caption
				_splitpath((LPCSTR)(((CFastCapView*)pView)->m_strPathName), tmp, tmp, fname, ext);
				strcat(fname,ext);

				wsprintf(szText + lstrlen(szText), _T("%s"), fname);
			}

			else if (pDocument == NULL)
				wsprintf(szText + lstrlen(szText), _T("%s"), m_strTitle);
			else
				wsprintf(szText + lstrlen(szText), _T("%s"), pDocument->GetTitle());

		if (m_nWindow > 0)
			wsprintf(szText + lstrlen(szText), _T(":%d"), m_nWindow);

		// set title if changed, but don't remove completely
		SetWindowText(szText);
	}
}

// retrieve window state from the registry
bool CChildFrame::LoadWindowState(CString regSection)
{
	CString pos;
	int x1, y1, x2, y2, num, isvisible;
	CWinApp *pApp;

	pApp = AfxGetApp();
	ASSERT(pApp);

	// get window position from the registry
	pos = pApp->GetProfileString( regSection, _T("Position"), _T("0,0,100,100") );

	// extract position from the string
	num = sscanf( (LPCTSTR)pos, "%d,%d,%d,%d", &x1, &y1, &x2, &y2);

	// only if good position information, move window
	if(num == 4) {
		// apply new position
		MoveWindow(x1, y1, x2-x1, y2-y1, FALSE);
	}

	// get window visibility state from the registry
	isvisible = pApp->GetProfileInt( regSection, _T("IsVisible"), 1 );

	// hide or show window according to visibility
	if(isvisible == 1) {
		ShowWindow(SW_SHOW);   
	}
	else {
		ShowWindow(SW_HIDE);   
	}

	return true;
}

// save window state in the registry
void CChildFrame::SaveWindowState(CString regSection)
{
	CString pos;
	CWinApp *pApp;
	CRect cRect;

	pApp = AfxGetApp();
	ASSERT(pApp);

	// get child frame position
	GetWindowRect(cRect);

	// convert to client coordinates, using the parent frame coordinate map
	// (that is, the client area of the main frame window)
	GetParent()->ScreenToClient(cRect);

	// build position string
	pos.Format("%d,%d,%d,%d", cRect.left, cRect.top, cRect.right, cRect.bottom);

	// write window position in the registry
	pApp->WriteProfileString( regSection, _T("Position"), pos );

	// save visibility state
	if( IsWindowVisible() == TRUE) {
		pApp->WriteProfileInt( regSection, _T("IsVisible"), 1 );
	}
	else {
		pApp->WriteProfileInt( regSection, _T("IsVisible"), 0 );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

void CChildFrame::OnClose() 
{
	if( GetTopWindow()->IsKindOf(RUNTIME_CLASS(CFastHenryView)) ||
		GetTopWindow()->IsKindOf(RUNTIME_CLASS(CFastCapView))) {
		// if is FastHenry or FastCap View frame, instead of closing, hide
		ShowWindow(SW_HIDE);   
	}
	else
		CMDIChildWnd::OnClose();
}

// used to force closure
void CChildFrame::CloseWindow() 
{
		CMDIChildWnd::OnClose();
}
