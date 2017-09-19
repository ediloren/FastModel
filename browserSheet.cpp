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


// AllControlsSheet.cpp : implementation file


#include "stdafx.h"
#include "browserSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CBrowserSheet

IMPLEMENT_DYNAMIC(CBrowserSheet, CPropertySheet)

CBrowserSheet::CBrowserSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
	AddControlPages();
}

CBrowserSheet::CBrowserSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddControlPages();
}

CBrowserSheet::CBrowserSheet() : CPropertySheet()
{
	AddControlPages();
}

CBrowserSheet::~CBrowserSheet()
{
}

void CBrowserSheet::AddControlPages()
{
	// if you want icon, e.g. for dragging...
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//m_psh.dwFlags |= PSP_USEHICON;
	//m_psh.hIcon = m_hIcon;

	m_psh.hIcon = NULL;
	m_psh.dwFlags |= PSH_NOAPPLYNOW;    // Lose the Apply Now button
	m_psh.dwFlags &= ~PSH_HASHELP;  // Lose the Help button

	AddPage(&m_projectctrlpage);

}

void CBrowserSheet::AddTabIcons()
{
	TCITEM tabctrlitem;

	// get underlying tab control
	CTabCtrl* tabctrl = GetTabControl();
	// create the image list from the bitmap IDB_CONTROLTABS.
	// every image is 16x16 pixels and the list can grow up to 12 tabs,
	// the color RGB(0,0,0) is used to generate the mask for transparent drawing
	m_clsTabImageList.Create(IDB_CONTROLTABS, 16, 12, RGB(0,0,0) );
	// set the background color as transparent: the mask is used to determine
	// which area is background
	m_clsTabImageList.SetBkColor(CLR_NONE);
	// set the image list in the tab control
	tabctrl->SetImageList(&m_clsTabImageList);
	// set the images to the tabs
	// 
	// set mask to image
	tabctrlitem.mask = TCIF_IMAGE;
	// select image #0 from the image list
	tabctrlitem.iImage = 0;
	// set it to tab #0
	tabctrl->SetItem(0, &tabctrlitem);

	/* TBC WARNING: not working
	// create tooltips 
	//
	RECT bbrect;
	// create it
	m_clsTabToolTips.Create(this);
	// get the bounding rectangle for the tab #0  
	tabctrl->GetItemRect(0, &bbrect);
	// associate tooltip with a tool in the window
	m_clsTabToolTips.AddTool(this, IDS_CTRLWIN_TIP_PROJECT, &bbrect, IDD_DIALOG_PROJECT);
	// set the tool info
	//m_clsTabToolTips.SetToolInfo( LPTOOLINFO lpToolInfo );
	// set tooltips to tabs
	tabctrl->SetToolTips(&m_clsTabToolTips);
	*/

}

BEGIN_MESSAGE_MAP(CBrowserSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CBrowserSheet)
	ON_WM_QUERYDRAGICON()
	ON_WM_SYSCOMMAND()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowserSheet message handlers

BOOL CBrowserSheet::OnInitDialog()
{

	CPropertySheet::OnInitDialog();

	// if you want to modify the TabControl styles of the CPropertySheet,
	// this is the correct point
	//GetTabControl()->ModifyStyle(0, TCS_OWNERDRAWFIXED);
	
	return TRUE;

}

HCURSOR CBrowserSheet::OnQueryDragIcon()
{
	//(HCURSOR) m_hIcon;
	return NULL; 
}

void CBrowserSheet::OnSysCommand(UINT nID, LPARAM lParam)
{

	// left from previous implementation (see CAllControlsSheet in the
	// resizable dialogs project)
	CPropertySheet::OnSysCommand(nID, lParam);

}

void CBrowserSheet::OnSize(UINT nType, int cx, int cy) 
{
	CRect rect;
	int ncx;

	CPropertySheet::OnSize(nType, cx, cy);
	
	// the tab control is the tabs plus the frame; the CPropertySheet is a 
	// tab control plus CPropertyPages
	CTabCtrl* pTab = GetTabControl();
	if(pTab) {

		// The following test is a patch to a MS bug in the comctrl32.dll
		// which causes Windows to crash when the property sheet has 
		// only one page AND the underlying tab control is resized to
		// the same width of the tab item itself.
		// Therefore the patch forbids the tab to become smaller than 
		// the tab item, by getting the horizontal size of the tab control
		// and adding a fixed number of pixels

		// get first tab item bounding rectangle
		pTab->GetItemRect(0, rect);

		if(cx < rect.Width() + 30)
			ncx = rect.Width() + 30;
		else
			ncx = cx;

		// resize the tab control (buttons plus frame)
		pTab->MoveWindow(0,0,ncx-10,cy-10);

		// get the area occupied by the control and derive from that the client
		// area, i.e. the area for the property pages.
		// Remark: when tabs are stacked, also the origin of the property pages
		// area is modified. So it is necessary to change both size and origin
		// of property pages.
		pTab->GetWindowRect(rect);
		pTab->AdjustRect(FALSE, rect);
		// simply converts to screen coordinates (relative to desktop) to 
		// client ones (relative to upper left window corner)
		ScreenToClient(rect);

		// at the beginning, OnSize is called before creating the property page,
		// so must check for existence
		if( ::IsWindow(m_projectctrlpage) ) 
			m_projectctrlpage.MoveWindow(rect.left, rect.top, rect.Width(), rect.Height());

	}

	
		

}

