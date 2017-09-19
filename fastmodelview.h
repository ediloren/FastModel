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


// fastmodelView.h : interface of the CFastModelView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FASTMODELVIEW_H__78E7EC5D_3755_11D4_A718_444553540000__INCLUDED_)
#define AFX_FASTMODELVIEW_H__78E7EC5D_3755_11D4_A718_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OpenGlPrintView.h"
#include "Geometry.h"

#define FMV_ZOOM			1
#define FMV_ROTATE_NEAR		2
#define FMV_ROTATE_FAR		4
#define FMV_TRANSLATE		8

#define FMV_KEY_SHIFT		1
#define FMV_KEY_CONTROL		2

#define PI					3.1415926535897932384626433832795

// this is the 'speed' at which we zoom in/out in response
// to mouse movements
#define FMV_DEPTH_SPEED		100.0
// this is the translation speed 
#define FMV_TRANSL_SPEED	100.0
// this is the maximum allowed difference between max and min
// bounding box coordinate when calculating translation speed
#define FMV_MAX_DIFF_BTW_MAX_AND_MIN	1000.0

// projection drawing modes
#define FMV_PERSPECTIVE		1
#define FMV_ORTHOGRAPHIC	2


// allowed drawing modes
#define FMV_WIREFRAME		1
#define FMV_HIDDENLINES		2
#define FMV_SOLID			4
// one if drawing polygon contours with glPolygonMode GL_LINE,
// zero if drawing filled polygons with glPolygonMode GL_FILL
#define FMV_EPS_CONTOUR		128


class CFastModelView : public COpenGlPrintView
{

protected: // create from serialization only
	CFastModelView();
	DECLARE_DYNCREATE(CFastModelView)

// Attributes
public:

// Operations
public:
	void InitializeScene();
	void ZoomAll();
	void RotateFar(float angle, float x, float y, float z);
	void RotateNear(float angle, float x, float y, float z);
	bool LoadWindowState(CString regSection);
	void SaveWindowState(CString regSection);

	CPoint			m_clsPos;
	bool			m_bInputHasCharge;

protected:
	C3DBBox ComputeGlobalBBox();
	void computeBBoxParams();
	void CreateModelDisplayList(CGLParams params);
	void RenderScene(CGLParams params);
	void RenderModel(CGLParams params);
	void RenderAxes(CGLParams params);
	void UpdateCamera(CGLParams params);
	void DrawWireframeModel(C3DShell *model, CGLParams params);
	void DrawHiddenlinesModel(C3DShell *model, CGLParams params);
	void DrawSolidModel(C3DShell *model, CGLParams params);
	void DrawWireframeShell(C3DShell *shell);
	void DrawWireframeShellFace(C3DFace *face);
	void DrawWireframeShellLoop(C3DLoop *loop);
	void DrawWireframeShellEdgeUse(C3DEdgeUse *edgeuse);
	void DrawPatchShell(C3DShell *shell, CGLParams params, bool opaque = true);
	void DrawPatchShellFace(C3DFace *face, CGLParams params, bool opaque = true);
	void UpdateModel();
	void UpdateLighting(CGLParams params);
	void SetStatus();
	void SetModelViewMatrix(); 
	void OnColorsUpdate();

	CFastModelDoc* GetDocument();

	char			m_status, m_cViewProjection;
	CPoint			m_ptPrev, m_ptZoom;
	unsigned int	m_uiKeyPressed;
	float			m_fZoomCParam;
	float			m_fZoomBParam;
	float			m_fTranslParam;
	float			m_fRadius;
	GLdouble		m_daMVMatrix[16], m_daAxesMVMatrix[16];
	int				m_iDrawMode;
	C3DBBox			m_clsModelBBox;
	float			m_fScale;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFastModelView)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFastModelView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	//{{AFX_MSG(CFastModelView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnViewDisplayWireframe();
	afx_msg void OnViewDisplayHiddenlines();
	afx_msg void OnUpdateViewDisplayHiddenlines(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewDisplayWireframe(CCmdUI* pCmdUI);
	afx_msg void OnViewZoomAll();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnFilePrintToEps();
	afx_msg void OnViewDisplay2D();
	afx_msg void OnUpdateViewDisplay2D(CCmdUI* pCmdUI);
	afx_msg void OnViewDisplay3D();
	afx_msg void OnUpdateViewDisplay3D(CCmdUI* pCmdUI);
	afx_msg void OnViewAutosetXY();
	afx_msg void OnViewAutosetYZ();
	afx_msg void OnViewAutosetZX();
	afx_msg void OnViewDisplaySolid();
	afx_msg void OnUpdateViewDisplaySolid(CCmdUI* pCmdUI);
	afx_msg void OnViewOriginTranslate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in fastmodelView.cpp
inline CFastModelDoc* CFastModelView::GetDocument()
   { return (CFastModelDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FASTMODELVIEW_H__78E7EC5D_3755_11D4_A718_444553540000__INCLUDED_)
