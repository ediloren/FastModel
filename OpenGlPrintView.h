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


// OpenGlPrintView.h : interface of the COpenGlPrintView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(OPENGLPRINTVIEW_H)
#define OPENGLPRINTVIEW_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OpenGlView.h"

// OpenGL to EPS converter definitions and variables
//

// Lower for smoother gourad shading
#define EPS_GOURAUD_THRESHOLD 0.1   
// Lower for better smooth lines
#define EPS_SMOOTH_LINE_FACTOR 0.06 
// tokens for glPassThrough in OpenGL feedback mode 
// and status for recognising lines composing polygon contours
#define FMV_EPS_STATUS_NULL			0.0f
#define FMV_EPS_CONTOUR_START		1.0f
#define FMV_EPS_CONTOUR_CONT		2.0f
#define FMV_EPS_CONTOUR_END			3.0f
#define FMV_EPS_POLYGON_START		4.0f
#define FMV_EPS_POLYGON_CONT		5.0f
#define FMV_EPS_POLYGON_END			6.0f
// starting size for OpenGL feedback buffer (to be increased if
// not sufficient)
#define FMV_EPS_FEEDBACK_BUFFER		8152
#define FMV_EPS_MAX_GLFLOAT			(float)3.4E+38 

// OpenGL GL_3D_COLOR feedback vertex format
class CGLFeedback3Dcolor 
{
public:
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat red;
	GLfloat green;
	GLfloat blue;
	GLfloat alpha;
};

class CContour
{
public:
	GLfloat *ptr;
	long num;
};

class CDepthIndex 
{
public:
	GLfloat *ptr;
	GLfloat depth;
};

class CPolyDepthIndex : public CDepthIndex
{
public:
	CContour *contour;
	int	polycount;
};


class COpenGlPrintView : public COpenGlView
{

protected: // create from serialization only
	COpenGlPrintView();
	DECLARE_DYNCREATE(COpenGlPrintView)

// Attributes
public:

// Operations
public:

protected:
	void outputEPS(int doSort, const char *filename);
	void convertFeedbackToEPS(FILE * file, int doSort, GLint size, GLfloat * buffer, char *creator);
	void convSortFeedbackToEPS(FILE *file, GLint size, GLfloat *buffer);
	void convUnsortFeedbackToEPS(FILE * file, GLint size, GLfloat * buffer);
	GLfloat *printPrimitiveToEPS(FILE * file, GLfloat * loc);

	GLfloat			m_pointSize;
	CGLParams		m_clsPrintParams;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenGlPrintView)
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Virtual functions
protected:
	virtual void CreateModelDisplayList(CGLParams params);
	virtual void SetModelViewMatrix();
	virtual void UpdateLighting(CGLParams params); 

// Implementation
public:
	virtual ~COpenGlPrintView();


// Generated message map functions
protected:
	//{{AFX_MSG(COpenGlPrintView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(OPENGLPRINTVIEW_H)
