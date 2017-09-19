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


// IntroDialog.cpp : implementation file
//

#include "stdafx.h"


#include "fastmodel.h"
#include "IntroDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CIntroDialog dialog


CIntroDialog::CIntroDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CIntroDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIntroDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CIntroDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIntroDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIntroDialog, CDialog)
	//{{AFX_MSG_MAP(CIntroDialog)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIntroDialog message handlers

BOOL CIntroDialog::OnInitDialog() 
{

	CStdioFile fl;
	CString textline, path;

	CDialog::OnInitDialog();
	
	CEdit *pEdit = (CEdit*) GetDlgItem( IDC_LICENSE );
	
	// get install directory path
	path = ((CFastModelApp*)AfxGetApp())->m_strBasePath;
	// and form full license path name
	path += _T("\\LICENCE.txt");

	if (!fl.Open( (LPCTSTR)path, CFile::modeRead | CFile::typeText )) {
		AfxMessageBox("Cannot open license file!", MB_ICONSTOP);
		pEdit->ReplaceSel("Cannot open license file!\r\nCheck installation\r\n");
		pEdit->SetSel(0,0);
	}
	else {

		while (fl.ReadString(textline)) 
		{
			pEdit->ReplaceSel(textline+"\r\n");
		}
	
		pEdit->SetSel(0,0);

		fl.Close();
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CIntroDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	pDC->SetTextColor(RGB(0,0,0));
	
	// TODO: Return a different brush if the default is not desired
	return hbr;
}
