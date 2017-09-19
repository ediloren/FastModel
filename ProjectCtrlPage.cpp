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


// ProjectCtrlPage.cpp : implementation file
//

#include "stdafx.h"
#include "fastmodel.h"
#include "ProjectCtrlPage.h"
#include "FastCapView.h"
#include "FastModelDoc.h"
#include "ChildFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectCtrlPage property page

IMPLEMENT_DYNCREATE(CProjectCtrlPage, CPropertyPage)

CProjectCtrlPage::CProjectCtrlPage() : CPropertyPage(CProjectCtrlPage::IDD)
{
	//{{AFX_DATA_INIT(CProjectCtrlPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CProjectCtrlPage::~CProjectCtrlPage()
{
	// upon exit, must remove the tree view image list
	// allocated in OnInitDialog()
	delete m_pImageList;
}

void CProjectCtrlPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectCtrlPage)
	DDX_Control(pDX, IDC_TREE_PROJECT, m_projecttree);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProjectCtrlPage, CPropertyPage)
	//{{AFX_MSG_MAP(CProjectCtrlPage)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE_PROJECT, OnKeydownTreeProject)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_PROJECT, OnDblclkTreeProject)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectCtrlPage message handlers

void CProjectCtrlPage::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyPage::OnSize(nType, cx, cy);
	
	if(::IsWindow(m_projecttree) )
		m_projecttree.SetWindowPos(NULL, 0,0,cx,cy, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);	
}

BOOL CProjectCtrlPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	HTREEITEM		proj_item;
	TVINSERTSTRUCT  curTreeItem;

	m_pImageList = new CImageList();
	m_pImageList->Create(16, 16, ILC_MASK, 8, 0);

	HICON hiconproj = AfxGetApp()->LoadIcon( IDI_PROJECT );
	HICON hiconfh = AfxGetApp()->LoadIcon( IDR_FASTHENRYTYPE );
	HICON hiconfc = AfxGetApp()->LoadIcon( IDR_FASTCAPTYPE );
	HICON hiconchild = AfxGetApp()->LoadIcon( IDI_TEXT_FILE );

		// selected?
	m_pImageList->Add(hiconproj);
	// non selected?
	m_pImageList->Add(hiconproj);
	// selected?
	m_pImageList->Add(hiconfh);
	// non selected?
	m_pImageList->Add(hiconfh);
	// selected?
	m_pImageList->Add(hiconfc);
	// non selected?
	m_pImageList->Add(hiconfc);
	// selected?
	m_pImageList->Add(hiconchild);
	// non selected?
	m_pImageList->Add(hiconchild);

	m_projecttree.SetImageList(m_pImageList, TVSIL_NORMAL);

	curTreeItem.hParent = NULL;
	curTreeItem.hInsertAfter = TVI_SORT;
	// lParam = 0 means cannot be deleted (see OnKeydownTreeProject function)
	curTreeItem.item.lParam = 0;
	curTreeItem.item.iImage = 0;
	curTreeItem.item.iSelectedImage = 1;
	curTreeItem.item.pszText = _T("Project");
	curTreeItem.item.mask = TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_TEXT | TVIF_PARAM;
	proj_item = m_projecttree.InsertItem(&curTreeItem);

	curTreeItem.hParent = proj_item;
	curTreeItem.hInsertAfter = TVI_SORT;
	// lParam = 0 means cannot be deleted (see OnKeydownTreeProject function)
	curTreeItem.item.lParam = 0;
	curTreeItem.item.iImage = 2;
	curTreeItem.item.iSelectedImage = 3;
	curTreeItem.item.pszText = _T("FastHenry2");
	curTreeItem.item.mask = TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_TEXT | TVIF_PARAM;
	m_fhitem = m_projecttree.InsertItem(&curTreeItem);
	
	curTreeItem.hParent = proj_item;
	curTreeItem.hInsertAfter = TVI_SORT;
	// lParam = 0 means cannot be deleted (see OnKeydownTreeProject function)
	curTreeItem.item.lParam = 0;
	curTreeItem.item.iImage = 4;
	curTreeItem.item.iSelectedImage = 5;
	curTreeItem.item.pszText = _T("FastCap2");
	curTreeItem.item.mask = TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_TEXT | TVIF_PARAM;
	m_fcitem = m_projecttree.InsertItem(&curTreeItem);

	// expand root node
	m_projecttree.Expand(proj_item, TVE_EXPAND); 
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CProjectCtrlPage::OnKeydownTreeProject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNMTVKEYDOWN ptvkd = (LPNMTVKEYDOWN) pNMHDR;
	CFastCapView *fcv;
	CFastModelDoc *fastmodeldoc;
	int viewnum;

	if(ptvkd->wVKey == VK_DELETE) {
		HTREEITEM hItem = m_projecttree.GetSelectedItem( );
		DWORD data = m_projecttree.GetItemData(hItem);
		// in this case, data is eraseable
		if (data != 0) {
			fastmodeldoc = ((CFastModelApp *)AfxGetApp())->GetFastModelDoc();
			fcv = fastmodeldoc->getFastCapViewFromTreeTok(data, &viewnum);
			if(fcv == NULL)
				AfxMessageBox("Internal error: cannot delete item", MB_ICONSTOP);
			else {
				// the view will be closed -> if modified, ask user yes/no
				if (fcv->m_isModified == TRUE)
					if( MessageBox("The chosen file has unsaved changes\nAre you sure you want to discard changes?", "Warning", MB_OKCANCEL|MB_ICONEXCLAMATION) == IDCANCEL )
						return;

				// ok close view
				fastmodeldoc->CloseFastCapView(viewnum);

				// delete item 
				m_projecttree.DeleteItem( hItem );
			}
		}
	}
	
	*pResult = 0;
}

void CProjectCtrlPage::OnDblclkTreeProject(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CColorEditView *cev;
	CFastModelDoc *fastmodeldoc;
	int viewnum;

	HTREEITEM hItem = m_projecttree.GetSelectedItem( );
	DWORD data = m_projecttree.GetItemData(hItem);

	fastmodeldoc = ((CFastModelApp *)AfxGetApp())->GetFastModelDoc();

	if (data != 0) {
		cev = (CColorEditView *)fastmodeldoc->getFastCapViewFromTreeTok(data, &viewnum);
	}
	else {
		if( (m_projecttree.GetItemText(hItem)).Compare("FastCap2") == 0 )
			cev = (CColorEditView *)fastmodeldoc->getFastCapMainView();
		else if( (m_projecttree.GetItemText(hItem)).Compare("FastHenry2") == 0 )
			cev = (CColorEditView *)fastmodeldoc->getFastHenryMainView();
		else
			cev = NULL;
	}

	if(cev == NULL) {
		// not necessary an error. Simply, the user did not double-clicked on 
		// a recognized leaf. For instance, the used clicked on the root leaf.
		// AfxMessageBox("Internal error: cannot show view", MB_ICONSTOP);
	}
	else {
		// show view
		CChildFrame *pFrame = (CChildFrame *)(cev->GetParent());
		pFrame->ShowWindow(SW_SHOWNORMAL);
		pFrame->SetWindowPos( &wndTop, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	*pResult = 0;
}


