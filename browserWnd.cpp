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


// browserWnd.cpp : implementation file
//

#include "stdafx.h"
#include "browserWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP(CBrowserWnd, baseCBrowserWnd)
	//{{AFX_MSG_MAP(CBrowserWnd)
	ON_WM_CREATE()
    ON_WM_NCLBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBrowserWnd

CBrowserWnd::CBrowserWnd()
{
	m_ulIndex = 1;
}

CBrowserWnd::~CBrowserWnd()
{
}


/////////////////////////////////////////////////////////////////////////////
// CBrowserWnd overrides

// creation method with custom dock window size
BOOL CBrowserWnd::Create(LPCTSTR lpszWindowName, CWnd* pParentWnd, 
							   CSize szHorz, CSize szVert, CSize szFloat,
							   UINT nID, DWORD dwStyle)
{
    m_szHorz = szHorz;
	m_szVert = szVert;
	m_szFloat = szFloat;
	m_bVisible = TRUE;
    return baseCBrowserWnd::Create(lpszWindowName, pParentWnd, nID, dwStyle);
}

void CBrowserWnd::OnNcLButtonUp(UINT nHitTest, CPoint point)
{
    if (nHitTest == HTCLOSE) {
		// this is used to signal that docking window is now hidden
		m_bVisible = FALSE;
	}

    baseCBrowserWnd::OnNcLButtonUp(nHitTest, point);
}

/////////////////////////////////////////////////////////////////////////////
// CBrowserWnd message handlers

int CBrowserWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{

	if (baseCBrowserWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetSCBStyle(GetSCBStyle() | SCBS_SHOWEDGES | SCBS_SIZECHILD);

	m_clsBrowserSheet.Create(this, WS_CHILD | WS_VISIBLE);
	m_clsBrowserSheet.AddTabIcons();
	// may be redundant with WS_VISIBLE
	// m_clsBrowserSheet.ShowWindow(SW_SHOW); 

	return 0;
}

// filename is a full path filename
// the function returns a token linkedto the item (so it is 
// possible to recognize two different views associated to
// files with same name but in different directories, which
// otherwise would have the same text name in the CTreeCtrl)
unsigned long CBrowserWnd::addFastCapFile(char *filename)
{
	TVINSERTSTRUCT  curTreeItem;
	CProjectCtrlPage *page = &(m_clsBrowserSheet.m_projectctrlpage);
	char tmp[BW_MAX_STR_LEN]; 
	char fname[BW_MAX_STR_LEN], ext[BW_MAX_STR_LEN];

	// split path into components and assemble text to insert 
	// in CTreeCtrl item
	_splitpath(filename, tmp, tmp, fname, ext);
	strcat(fname,ext);

	curTreeItem.hParent = page->m_fcitem;
	// lParam != 0 means can be deleted (see OnKeydownTreeProject function);
	curTreeItem.item.lParam = m_ulIndex;
	curTreeItem.item.iImage = 6;
	curTreeItem.item.iSelectedImage = 7;
	curTreeItem.item.pszText = fname;
	curTreeItem.item.mask = TVIF_SELECTEDIMAGE | TVIF_IMAGE | TVIF_TEXT | TVIF_PARAM;
	page->m_projecttree.InsertItem(&curTreeItem);

	//invalidate client area to force redraw
	page->m_projecttree.Invalidate(TRUE);

	return(m_ulIndex++);
}

BOOL CBrowserWnd::removeFastCapFile(unsigned long token)
{
	HTREEITEM item;
	CString text;
	CProjectCtrlPage *page = &(m_clsBrowserSheet.m_projectctrlpage);
	CTreeCtrl *tree = &(page->m_projecttree);

	// find item with given filename
	item = tree->GetChildItem(page->m_fcitem);
	while( item != NULL && tree->GetItemData(item) != token) {
		item =  tree->GetNextSiblingItem(item);
	}

	// operation not successful
	if( item == NULL)
		return FALSE;

	// operation successful
	tree->DeleteItem(item);

	//invalidate client area to force redraw
	tree->Invalidate(TRUE);

	return TRUE;
}


