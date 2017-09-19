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


// OGLViewProps.cpp : implementation file
//

#include "stdafx.h"
#include "fastmodel.h"
#include "OGLViewProps.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// size of the bitmap to be shown in button
#define OGL_BITMAP_SIZE		14


/////////////////////////////////////////////////////////////////////////////
// COGLViewProps dialog


COGLViewProps::COGLViewProps(CWnd* pParent /*=NULL*/)
	: CDialog(COGLViewProps::IDD, pParent)
{
	//{{AFX_DATA_INIT(COGLViewProps)
	//}}AFX_DATA_INIT
}


void COGLViewProps::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COGLViewProps)
	DDX_Control(pDX, IDC_BUTTON_WIREFRAME_COLOR, m_ctrlWireColor);
	DDX_Control(pDX, IDC_BUTTON_SOLID_COLOR, m_ctrlSolidColor);
	DDX_Control(pDX, IDC_BUTTON_BACK_TOP_COLOR, m_ctrlBackTopColor);
	DDX_Control(pDX, IDC_BUTTON_BACK_BOT_COLOR, m_ctrlBackBotColor);
	DDX_Control(pDX, IDC_BUTTON_AMBIENT_LIGHT_COLOR, m_ctrlAmbientLightColor);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COGLViewProps, CDialog)
	//{{AFX_MSG_MAP(COGLViewProps)
	ON_BN_CLICKED(IDC_BUTTON_WIREFRAME_COLOR, OnButtonWireframeColor)
	ON_BN_CLICKED(IDC_BUTTON_SOLID_COLOR, OnButtonSolidColor)
	ON_BN_CLICKED(IDC_BUTTON_AMBIENT_LIGHT_COLOR, OnButtonAmbientLightColor)
	ON_BN_CLICKED(IDC_BUTTON_BACK_TOP_COLOR, OnButtonBackTopColor)
	ON_BN_CLICKED(IDC_BUTTON_BACK_BOT_COLOR, OnButtonBackBotColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COGLViewProps message handlers

BOOL COGLViewProps::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetButtonBitmap(&m_ctrlWireColor, &m_clsBitmapWireColor, m_clsColors.m_clsWireColor);
	SetButtonBitmap(&m_ctrlSolidColor, &m_clsBitmapSolidColor, m_clsColors.m_clsSolidColor);
	SetButtonBitmap(&m_ctrlBackTopColor, &m_clsBitmapBackTopColor, m_clsColors.m_clsBackTopColor);
	SetButtonBitmap(&m_ctrlBackBotColor, &m_clsBitmapBackBotColor, m_clsColors.m_clsBackBotColor);
	SetButtonBitmap(&m_ctrlAmbientLightColor, &m_clsBitmapAmbLightColor, m_clsColors.m_clsAmbientLightColor);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COGLViewProps::SetButtonBitmap(CButton *button, CBitmap *bitmap, COLORREF color)
{
	CBitmap *oldbitmap;

	// use CClientDC so calls to GetDC() and ReleaseDC() are automatically
	// handled (common DCs are limited in number under Windows and must 
	// be released)
	CClientDC dc(this);
	// construct temporary memory DC to use for paint operations
	CDC dcMem;
	// and create it compatible with the Window DC
	if(!dcMem.CreateCompatibleDC(&dc))
		ASSERT(false);

	// if a bitmap already exists, delete it
	if(bitmap->GetSafeHandle() != NULL)
		bitmap->DeleteObject();

	// create a bitmap compatible with current DC
	// (note that address of 'dc' gives a correct result,
	// while address of 'dcMem' produces a monochromatic bitmap)
	if(!bitmap->CreateCompatibleBitmap(&dc, OGL_BITMAP_SIZE, OGL_BITMAP_SIZE))
		ASSERT(false);

	//
	// initialize button color
	//

	// select bitmap into memory DC
	oldbitmap = dcMem.SelectObject(bitmap);

	// draw
	dcMem.FillSolidRect(0, 0, OGL_BITMAP_SIZE, OGL_BITMAP_SIZE, color );

	// release the bitmap
	dcMem.SelectObject(oldbitmap);

	// and set it as image in the button
	button->SetBitmap(*bitmap);
}

void COGLViewProps::OnButtonWireframeColor() 
{
	CColorDialog color;

	// set initial color in CColorDialog 
	color.m_cc.Flags |= CC_RGBINIT;
	color.m_cc.rgbResult = m_clsColors.m_clsWireColor;

	color.DoModal();

	m_clsColors.m_clsWireColor = color.GetColor();

	SetButtonBitmap(&m_ctrlWireColor, &m_clsBitmapWireColor, m_clsColors.m_clsWireColor);
}

void COGLViewProps::OnButtonSolidColor() 
{
	CColorDialog color;

	// set initial color in CColorDialog 
	color.m_cc.Flags |= CC_RGBINIT;
	color.m_cc.rgbResult = m_clsColors.m_clsSolidColor;

	color.DoModal();

	m_clsColors.m_clsSolidColor = color.GetColor();

	SetButtonBitmap(&m_ctrlSolidColor, &m_clsBitmapSolidColor, m_clsColors.m_clsSolidColor);
}

void COGLViewProps::OnButtonAmbientLightColor() 
{
	CColorDialog color;

	// set initial color in CColorDialog 
	color.m_cc.Flags |= CC_RGBINIT;
	color.m_cc.rgbResult = m_clsColors.m_clsAmbientLightColor;

	color.DoModal();

	m_clsColors.m_clsAmbientLightColor = color.GetColor();

	SetButtonBitmap(&m_ctrlAmbientLightColor, &m_clsBitmapAmbLightColor, m_clsColors.m_clsAmbientLightColor);
}

void COGLViewProps::OnButtonBackTopColor() 
{
	CColorDialog color;

	// set initial color in CColorDialog 
	color.m_cc.Flags |= CC_RGBINIT;
	color.m_cc.rgbResult = m_clsColors.m_clsBackTopColor;

	color.DoModal();

	m_clsColors.m_clsBackTopColor = color.GetColor();

	SetButtonBitmap(&m_ctrlBackTopColor, &m_clsBitmapBackTopColor, m_clsColors.m_clsBackTopColor);
}

void COGLViewProps::OnButtonBackBotColor() 
{
	CColorDialog color;

	// set initial color in CColorDialog 
	color.m_cc.Flags |= CC_RGBINIT;
	color.m_cc.rgbResult = m_clsColors.m_clsBackBotColor;

	color.DoModal();

	m_clsColors.m_clsBackBotColor = color.GetColor();

	SetButtonBitmap(&m_ctrlBackBotColor, &m_clsBitmapBackBotColor, m_clsColors.m_clsBackBotColor);
}

void COGLViewProps::InitColors(COGLColors colors)
{
	m_clsColors = colors;
}


COGLColors COGLViewProps::GetColors()
{
	return m_clsColors;
}
