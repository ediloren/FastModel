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


// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "fastmodel.h"

#include "MainFrm.h"
#include "resource.h"
#include "FastHenryView.h"
#include "FastCapView.h"
#include "FastModelView.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

static const UINT FMD_FILE_CHANGE_NOTIFY = RegisterWindowMessage(_T("FMD_FILE_CHANGE_NOTIFY-FastModel-Enrico_Di_Lorenzo"));

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_OUTPUT, OnViewOutput)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUT, OnUpdateViewOutput)
	ON_COMMAND(ID_VIEW_FASTHENRY_VIEW, OnViewFastHenryView)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FASTHENRY_VIEW, OnUpdateViewFastHenryView)
	ON_COMMAND(ID_VIEW_FASTCAP_VIEW, OnViewFastCapView)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FASTCAP_VIEW, OnUpdateViewFastCapView)
	ON_COMMAND(ID_VIEW_PROJECT, OnViewProject)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROJECT, OnUpdateViewProject)
	ON_WM_CLOSE()
	ON_COMMAND(ID_HELP, OnHelp)
	ON_COMMAND(ID_MODEL_NEW_WINDOW, OnModelNewWindow)
	ON_COMMAND(ID_VIEW_CLEAR_OUTPUT_WIN, OnViewClearOutputWin)
	//}}AFX_MSG_MAP
	// Global help commands

	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ROW_AND_COLUMN, OnUpdateRowAndColumn)

	ON_REGISTERED_MESSAGE(FMD_FILE_CHANGE_NOTIFY, OnFileChangeNotify)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_ROW_AND_COLUMN,  // row and column indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bCanClose = TRUE;
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CSize szHorz, szVert, szFloat;
	CRect clientRect;
	LONG width, height;

	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

// if MFC version allows direct call of CToolBar::CreateEx
#if _MFC_VER > 0x0421

	// must use CreateEx to have a flat-looking toolbar. 
	// Notes:
	// - CBSR_TOP style MUST be defined here, at creation, in order not to have some small
	//   repaint problems at the toolbar left and right extremes when moving the toolbar
	// - A standard 6-pixel border is used at left/right, a 3-pixel border at up/down
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP, CRect(6, 3, 6, 3)) )
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
#else
	if (!m_wndToolBar.Create(this) )
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_wndToolBar.ModifyStyle(0, m_wndToolBar.GetStyle() | TBSTYLE_FLAT); 
#endif //_MFC_VER > 0x0421
	if ( !(m_wndToolBar.LoadToolBar(IDR_MAINFRAME)) )
	{
		TRACE0("Failed to load toolbar\n");
		return -1;      // fail to load toolbar
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

// for dockable toolbar gripper, not defined in MFC <= 0x421
#ifndef CBRS_GRIPPER
#define CBRS_GRIPPER 0x400000L
#endif
	// modify control bar style to have tool tips, resizeable toolbar and gripper
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | CBRS_GRIPPER);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// dockable output window
	//
	// create with default dimensions, that is 
	// equal 1/5 of frame client window space
	
	// retrieve dimensions of client area of main frame window
	GetClientRect(clientRect);
	width = clientRect.right - clientRect.left;
	height = clientRect.bottom - clientRect.top;

	szHorz.cx = szHorz.cy = szFloat.cx = width / 5;
	szVert.cx = szVert.cy = szFloat.cy = height / 5;

	if (!m_wndOutput.Create(_T("Output"), this, szHorz, szVert, szFloat, 123)) {
		TRACE0("Failed to create output wnd\n");
		return -1;
		// fail to create
	}

	m_wndOutput.SetBarStyle(m_wndOutput.GetBarStyle() |
		   CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);

	DockControlBar(&m_wndOutput, AFX_IDW_DOCKBAR_BOTTOM);


	// dockable project browser window

	// calculate dimensions
	szHorz.cx = szHorz.cy = szFloat.cx = width / 5;
	szVert.cx = szVert.cy = szFloat.cy = height / 5;

	if (!m_wndBrowser.Create(_T("Browser"), this, szHorz, szVert, szFloat, 126))
	{
		TRACE0("Failed to create browser wnd\n");
		return -1;		// fail to create
	}

	m_wndBrowser.SetBarStyle(m_wndBrowser.GetBarStyle() |
		   CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	m_wndBrowser.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);

	DockControlBar(&m_wndBrowser, AFX_IDW_DOCKBAR_LEFT);


	// valid for all docking windows
	#ifdef _SCB_REPLACE_MINIFRAME
		m_pFloatingFrameClass = RUNTIME_CLASS(CSCBMiniDockFrameWnd);
	#endif //_SCB_REPLACE_MINIFRAME

	// retrieve bar state (dockin, floating, etc.) from the registry
	// Remark: if bars are removed of ID changed, the call to LoadBarState()
	// crashes the app. The reason is that the ID of the bar is saved on
	// the registry; when LoadBarState() is called, it tries to retrieve the
	// Window object with that ID, which should have already been created
	// by MainFrm.cpp. If it doesn't exists, app crashes. 
	// (this behaviour does not happen the first time when a bar is added,
	// because no ID info is in the registry. It only affects *removed* bars
	// or bars which changed *ID*, see also article on docking windows.
	CString sProfile(_T("BarState"));
	m_wndOutput.LoadState(sProfile);
	m_wndBrowser.LoadState(sProfile);
	LoadBarState(sProfile);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	// change the style to maximized to inform main frame
	// we have shown the main window maximized (see fastmodel.cpp)
	cs.style |= WS_MAXIMIZE;

	return CMDIFrameWnd::PreCreateWindow(cs);
}


void CMainFrame::CreateMDIWindows()
{
	CMDIChildWnd* pActiveFastModelFrame;
	CWnd *pMainFrameClientWnd;
	CFastModelDoc* pDoc;
	CRect clientRect;
	LONG width, height;
	CFastModelApp *pApp;

	// modified from CMDIFrameWnd::OnWindowNew()
	//

	// remark: an active window must already exist!
	pActiveFastModelFrame = MDIGetActive();
	// get its parent (the Main Frame client window)
	pMainFrameClientWnd = pActiveFastModelFrame->GetParent();

	// the previous one is the bottom-up approach;
	// could also retrieve the Main Frame client window 
	// and the child frame window from the top, just like this:
	//
	// get first child of main frame: is the client window
	//CWnd *pMainFrameClientWnd = pApp->m_pMDIMainFrameWnd->GetTopWindow();
	//ASSERT(pMainFrameClientWnd);
	// get first child of main client: is the (model) view child frame window
	//pActiveFastModelFrame = (CFrameWnd*)pMainFrameClientWnd->GetTopWindow();
	//ASSERT(pFastModelFrame);

	// get a pointer to the current document
	if (pActiveFastModelFrame == NULL ||
	  (pDoc = (CFastModelDoc*)pActiveFastModelFrame->GetActiveDocument()) == NULL)
	{
		TRACE0("Warning: No active document for CreateMDIWindows command.\n");
		AfxMessageBox(AFX_IDP_COMMAND_FAILURE);
		return;     // command failed
	}

	// and get the pointer to the app
	pApp = (CFastModelApp*)AfxGetApp();

	// create FastCap view
	m_pFastCapFrame = (CChildFrame*)pApp->m_pFastCapViewTemplate->CreateNewFrame(pDoc, NULL);
	pApp->m_pFastCapViewTemplate->InitialUpdateFrame(m_pFastCapFrame, pDoc);

	// create FastHenry view
	m_pFastHenryFrame = (CChildFrame*)pApp->m_pFastHenryViewTemplate->CreateNewFrame(pDoc, NULL);
	pApp->m_pFastHenryViewTemplate->InitialUpdateFrame(m_pFastHenryFrame, pDoc);

	// Tile the MDI children windows within the MDI frame window.

	ASSERT_KINDOF(CMDIChildWnd, m_pFastCapFrame);
	ASSERT_KINDOF(CMDIChildWnd, m_pFastHenryFrame);

	// if the windows state was not correctly loaded (e.g. first launch,
	// registry corrupted, etc.), perform default initialization
	if(!pDoc->LoadWindowState()) {
		//
		// size child windows to default positions
		//
		
		// retrieve dimensions of client area of main frame window
		//
		// (the commented following line  was the old way;
		// but the ClientRect thus retrieved
		// did not consider toolbars. To get free client area,
		// considering toolbars, must ask the client window directly.
		// To have this calculation done when toolbars toggle on/off,
		// the MFC call CFrameWnd::RecalcLayout() )
		// pApp->m_pMDIMainFrameWnd->GetClientRect(clientRect);
		pMainFrameClientWnd->GetWindowRect(clientRect);
		width = clientRect.right - clientRect.left;
		height = clientRect.bottom - clientRect.top;
		
		// the model view should occupy 1/2 of client area,
		// the fasthenry view and the fastcap view the remaining 1/4
		//pFastModelFrame->MoveWindow(0, 0, width, height * 2 / 3);
		//m_pFastHenryFrame->MoveWindow(0, height * 2 / 3, width, height / 3);
		pActiveFastModelFrame->MoveWindow(0, 0, width, height * 1 / 2);
		m_pFastHenryFrame->MoveWindow(0, height * 1 / 2, width, height / 4);
		m_pFastCapFrame->MoveWindow(0, height * 3 / 4, width, height / 4);
		
		// old command; but here is required a more complex 
		// view windows positioning
		//m_pMDIMainFrameWnd->MDITile(MDITILE_HORIZONTAL);
	}
}

void CMainFrame::OnModelNewWindow() 
{
	CreateNewModelWindow();
}

CChildFrame *CMainFrame::CreateNewModelWindow() 
{
	CFastModelApp* pApp;
	CFastModelDoc* pDoc;
	CChildFrame* modelFrame;

	pApp = (CFastModelApp*)AfxGetApp();
	ASSERT(pApp);
	pDoc = pApp->GetFastModelDoc();
	ASSERT(pDoc);

	// create new FastModel view
	modelFrame = (CChildFrame*)pApp->m_pModelViewTemplate->CreateNewFrame(pDoc, NULL);
	ASSERT(modelFrame);

	pApp->m_pModelViewTemplate->InitialUpdateFrame(modelFrame, pDoc);

	return modelFrame;
}

CChildFrame *CMainFrame::GetFastHenryFrame() 
{
	return m_pFastHenryFrame;
}

CChildFrame *CMainFrame::GetFastCapFrame() 
{
	return m_pFastCapFrame;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnContextMenu(CWnd*, CPoint point)
{

	// CG: This block was added by the Pop-up Menu component

	if (point.x == -1 && point.y == -1){
		//keystroke invocation
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);

		point = rect.TopLeft();
		point.Offset(5, 5);
	}

// TBC warning: should present menu only if outside any window (on background)
//	else if (ChildWindowFromPoint(point) == this) {

		CMenu menu;
		VERIFY(menu.LoadMenu(CG_IDR_POPUP_MAIN_FRAME));

		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		CWnd* pWndPopupOwner = this;

		while (pWndPopupOwner->GetStyle() & WS_CHILD)
			pWndPopupOwner = pWndPopupOwner->GetParent();

		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
			pWndPopupOwner);
//	}
}

void CMainFrame::OnViewOutput() 
{
	if (m_wndOutput.m_bVisible) {
		ShowControlBar(&m_wndOutput, FALSE, FALSE);
		m_wndOutput.m_bVisible = FALSE;
	}
	else {
		ShowControlBar(&m_wndOutput, TRUE, FALSE);
		m_wndOutput.m_bVisible = TRUE;
	}
}

void CMainFrame::OnUpdateViewOutput(CCmdUI* pCmdUI) 
{
	( m_wndOutput.m_bVisible == TRUE ) ? pCmdUI->SetCheck(TRUE) : pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnViewProject() 
{
	if (m_wndBrowser.m_bVisible) {
		ShowControlBar(&m_wndBrowser, FALSE, FALSE);
		m_wndBrowser.m_bVisible = FALSE;
	}
	else {
		ShowControlBar(&m_wndBrowser, TRUE, FALSE);
		m_wndBrowser.m_bVisible = TRUE;
	}
}

void CMainFrame::OnUpdateViewProject(CCmdUI* pCmdUI) 
{
	( m_wndBrowser.m_bVisible == TRUE ) ? pCmdUI->SetCheck(TRUE) : pCmdUI->SetCheck(FALSE);	
}

void CMainFrame::OnViewFastHenryView() 
{
	ASSERT(m_pFastHenryFrame);

	if (m_pFastHenryFrame->IsWindowVisible()) {
		m_pFastHenryFrame->ShowWindow(SW_HIDE);   
	}
	else {
		m_pFastHenryFrame->ShowWindow(SW_SHOW);   
	}
}

void CMainFrame::OnUpdateViewFastHenryView(CCmdUI* pCmdUI) 
{
	ASSERT(m_pFastHenryFrame);

	( m_pFastHenryFrame->IsWindowVisible() == TRUE ) ? pCmdUI->SetCheck(TRUE) : pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnViewFastCapView() 
{
	ASSERT(m_pFastCapFrame);

	if (m_pFastCapFrame->IsWindowVisible()) {
		m_pFastCapFrame->ShowWindow(SW_HIDE);   
	}
	else {
		m_pFastCapFrame->ShowWindow(SW_SHOW);   
	}
}

void CMainFrame::OnUpdateViewFastCapView(CCmdUI* pCmdUI) 
{
	ASSERT(m_pFastCapFrame);

	( m_pFastCapFrame->IsWindowVisible() == TRUE ) ? pCmdUI->SetCheck(TRUE) : pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnUpdateRowAndColumn(CCmdUI *pCmdUI) 
{
	CView *currView;
	CMDIChildWnd *child;
	int index;
	BOOL ismax;
	char strRowAndColumn[128];
	long col, row, startpos, endpos;
	CPoint pos;

	index = m_wndStatusBar.CommandToIndex( ID_INDICATOR_ROW_AND_COLUMN );

	child = MDIGetActive(&ismax);
	if( child ) {
		currView = child->GetActiveView(); 
		if( currView ) {
			if( currView->IsKindOf(RUNTIME_CLASS(CFastHenryView)) ||
				currView->IsKindOf(RUNTIME_CLASS(CFastCapView))) {

				row = ((CRichEditView*)currView)->GetRichEditCtrl().LineFromChar(-1);
				((CRichEditView*)currView)->GetRichEditCtrl().GetSel(startpos, endpos);
				col = endpos - ((CRichEditView*)currView)->GetRichEditCtrl().LineIndex(-1);
				sprintf(strRowAndColumn, "Row: %d  Col: %d", row, col ); 
				//pCmdUI->SetText( strPage );
				m_wndStatusBar.SetPaneText( index, strRowAndColumn, TRUE );
			}
			else if( currView->IsKindOf(RUNTIME_CLASS(CFastModelView)) ) {
				pos = ((CFastModelView*)currView)->m_clsPos;
				sprintf(strRowAndColumn, "X: %d  Y: %d", pos.x, pos.y); 
				m_wndStatusBar.SetPaneText( index, strRowAndColumn, TRUE );
			}
			else {
				sprintf(strRowAndColumn, ""); 
				m_wndStatusBar.SetPaneText( index, strRowAndColumn, TRUE );
			}
			pCmdUI->Enable(TRUE);
		}
		else 
			pCmdUI->Enable(FALSE);
	}
	else 
		pCmdUI->Enable(FALSE);

}

void CMainFrame::OnClose() 
{
	CFastModelDoc *pDoc;
	CDocTemplate *pTempl;
	POSITION pos;
	BOOL modified;
	CFastHenryView *fhv;
	CMDIChildWnd *pChild;
	CFastModelApp *pApp;
	
	// closing is inhibited because of an importing / updating operation
	if(m_bCanClose == FALSE) {
		MessageBox(_T("Cannot close FastModel!\nPlease wait for end of current operation first"), 
					_T("Warning"), MB_ICONWARNING);
		return;
	}

	modified = FALSE;

	pChild = MDIGetActive();

	if(pChild != NULL) {
		pDoc = (CFastModelDoc*)(pChild->GetActiveDocument());
		pTempl = pDoc->GetDocTemplate();
	
		pos = pTempl->GetFirstDocPosition();
		do {
			pDoc = (CFastModelDoc*) pTempl->GetNextDoc(pos);
			if(pDoc != NULL) {
				pDoc->updateIsFCModified();
				if(pDoc->m_bIsFCModified) {
					modified = TRUE;
					break;
				}
				fhv = pDoc->getFastHenryMainView();
				if(fhv->m_isModified) {
					modified = TRUE;
					break;
				}
			}
		} while( pos != NULL);

		// the doc will be lost on import -> ask user yes/no
		if (modified == TRUE)
			if( MessageBox("The project has unsaved changes\nAre you sure you want to discard changes?", "Warning", MB_OKCANCEL|MB_ICONEXCLAMATION) == IDCANCEL )
				return;
	}
	
	// close FastCap, FasterCap and FastHenry consoles
	pApp = (CFastModelApp*)AfxGetApp();
	pApp->m_clControlSolvers.CloseFH();
	pApp->m_clControlSolvers.CloseFC();
	pApp->m_clControlSolvers.CloseFRC();

	// save bar status
    CString sProfile(_T("BarState"));
	m_wndOutput.SaveState(sProfile);
	m_wndBrowser.SaveState(sProfile);
    SaveBarState(sProfile);

	// save windows state 
	pDoc = pApp->GetFastModelDoc();
	ASSERT(pDoc);
	pDoc->SaveWindowState();

	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnHelp() 
{
	CString path;

	// get install directory path
	path = ((CFastModelApp*)AfxGetApp())->m_strAppPath;
	// and form full application path name
	path += _T("\\fastmodel.chm::/WelcometoFastModel.htm");


	HtmlHelp(NULL, path, HH_DISPLAY_TOPIC, 0);
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame pseudo message pump
// 
// As Windows does not route standard window messages through a standard sequence
// of target objects, as it does for command messages, any messages which should
// reach a class implementation will be sent (arbitrary) to the Main Window (this);
// then the message handler will simply retrieve the class target object and call
// the member implementation function, passing on its message parameters
/////////////////////////////////////////////////////////////////////////////


afx_msg LRESULT CMainFrame::OnFileChangeNotify(WPARAM fileCategory, LPARAM fileCode)
{
	CFastModelDoc *pDoc;
	CChildFrame *pChildFrm;

	pChildFrm = (CChildFrame*)MDIGetActive();
	pDoc = (CFastModelDoc*)pChildFrm->GetActiveDocument();

	// check if there is an active doc!
	if( pDoc )
		return(pDoc->OnFileChangeNotify(fileCategory, fileCode));
	else
		return 0;
}

void CMainFrame::OnViewClearOutputWin() 
{
	// clear output window
	m_wndOutput.ClearPanel();
}
