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


#if !defined(AFX_OPENGLVIEW_H__E3E6D100_9C56_4C30_B261_DDA3CC64C429__INCLUDED_)
#define AFX_OPENGLVIEW_H__E3E6D100_9C56_4C30_B261_DDA3CC64C429__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenGLView.h : header file
//

#include <afxmt.h>

// includes for OpenGL
#include "gl\gl.h"
#include "gl\glu.h"

#include "OGLViewProps.h"

// bbox dimension containing cartesian axes drawing
#define FMV_MAX_AXES_DIM	2.0f

// OpenGL glColor4f format
class CGLColor4f
{
public:
	GLfloat red;
	GLfloat green;
	GLfloat blue;
	GLfloat alpha;

	CGLColor4f() : 	red(1.0f), green(1.0f), blue(1.0f), alpha(1.0f) {}


	CGLColor4f(GLfloat redc, GLfloat greenc, GLfloat bluec, GLfloat alphac) : 
		red(redc), green(greenc), blue(bluec), alpha(alphac) {}

	void Set(GLfloat redc, GLfloat greenc, GLfloat bluec, GLfloat alphac) 
	{
		red = redc;
		green = greenc;
		blue = bluec;
		alpha = alphac;
	}

	COLORREF GetColorref() 
	{
		COLORREF color;

		color = RGB(red * 255, green * 255, blue * 255);
		
		return color;
	}

	bool operator==(const CGLColor4f &color)
	{
		return (red == color.red && green == color.green && blue == color.blue && alpha == color.alpha);
	}

	void operator=(COLORREF &color)
	{
		red = ((float)GetRValue(color)) / 255;
		green = ((float)GetGValue(color)) / 255;
		blue = ((float)GetBValue(color)) / 255;
		alpha = 1.0f;
	}

};

class CGLParams
{
public:
	CGLColor4f m_clsColor, m_clsBackColor[2], m_clsSolidColor, m_clsAmbientLight;
	GLuint m_GLuiModelDispList, m_GLuiAxesDispList;
	GLuint m_GLuiViewportCx, m_GLuiViewportCy;
	bool m_bPrinting, m_bCanPrintToEPS, m_bPrintingToEPS;

	void SetColors(COGLColors colors);
	COGLColors GetColors();
};

class COpenGlView : public CView
{

protected: // create from serialization only
	COpenGlView();
	DECLARE_DYNCREATE(COpenGlView)

// Attributes
public:

// Operations
public:
	bool LoadWindowState(CString regSection);
	void SaveWindowState(CString regSection);

protected:
	void Init();
	void InitOpenGL(CGLParams &params);
	void InitAxesDisplayList(CGLParams params);
	void ClearScreen(CGLParams params);
	void CreateRGBPalette();
	BOOL bSetupPixelFormat(HDC hDC, DWORD dwFlags);
	unsigned char ComponentFromIndex(int i, UINT nbits, UINT shift);
	BOOL MakeCurrentWGL(HDC  hdc, HGLRC  hglrc);
	BOOL ReleaseWGL();
	void LoadStateColor(CString regSection, CString colorName, CGLColor4f *colorParam);
	void SaveStateColor(CString regSection, CString colorName, CGLColor4f colorParam);

	CPalette		m_cPalette;
    CPalette		*m_pOldPalette;
	CClientDC		*m_pDC;
	HGLRC			m_hrc;
	CMutex			m_clsGLMutex;
	CSingleLock		m_clsGLLock;
	CGLParams		m_clsScreenParams;

// Virtual functions
protected:
	virtual void UpdateCamera(CGLParams params);
	virtual void RenderScene(CGLParams params);
	virtual void InitializeScene();
	virtual void ZoomAll();
	virtual void OnColorsUpdate();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenGlView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COpenGlView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(COpenGlView)
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnViewProperties();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENGLVIEW_H__E3E6D100_9C56_4C30_B261_DDA3CC64C429__INCLUDED_)
