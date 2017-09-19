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


// OpenGlPrintView.cpp : implementation of the COpenGlPrintView class
//

#include "stdafx.h"

#include <math.h>

#include "OpenGlPrintView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char *gouraudtriangleEPS[] =
{
  "/bd{bind def}bind def /triangle { aload pop   setrgbcolor  aload pop 5 3",
  "roll 4 2 roll 3 2 roll exch moveto lineto lineto closepath fill } bd",
  "/computediff1 { 2 copy sub abs threshold ge {pop pop pop true} { exch 2",
  "index sub abs threshold ge { pop pop true} { sub abs threshold ge } ifelse",
  "} ifelse } bd /computediff3 { 3 copy 0 get 3 1 roll 0 get 3 1 roll 0 get",
  "computediff1 {true} { 3 copy 1 get 3 1 roll 1 get 3 1 roll 1 get",
  "computediff1 {true} { 3 copy 2 get 3 1 roll  2 get 3 1 roll 2 get",
  "computediff1 } ifelse } ifelse } bd /middlecolor { aload pop 4 -1 roll",
  "aload pop 4 -1 roll add 2 div 5 1 roll 3 -1 roll add 2 div 3 1 roll add 2",
  "div 3 1 roll exch 3 array astore } bd /gouraudtriangle { computediff3 { 4",
  "-1 roll aload 7 1 roll 6 -1 roll pop 3 -1 roll pop add 2 div 3 1 roll add",
  "2 div exch 3 -1 roll aload 7 1 roll exch pop 4 -1 roll pop add 2 div 3 1",
  "roll add 2 div exch 3 -1 roll aload 7 1 roll pop 3 -1 roll pop add 2 div 3",
  "1 roll add 2 div exch 7 3 roll 10 -3 roll dup 3 index middlecolor 4 1 roll",
  "2 copy middlecolor 4 1 roll 3 copy pop middlecolor 4 1 roll 13 -1 roll",
  "aload pop 17 index 6 index 15 index 19 index 6 index 17 index 6 array",
  "astore 10 index 10 index 14 index gouraudtriangle 17 index 5 index 17",
  "index 19 index 5 index 19 index 6 array astore 10 index 9 index 13 index",
  "gouraudtriangle 13 index 16 index 5 index 15 index 18 index 5 index 6",
  "array astore 12 index 12 index 9 index gouraudtriangle 17 index 16 index",
  "15 index 19 index 18 index 17 index 6 array astore 10 index 12 index 14",
  "index gouraudtriangle 18 {pop} repeat } { aload pop 5 3 roll aload pop 7 3",
  "roll aload pop 9 3 roll 4 index 6 index 4 index add add 3 div 10 1 roll 7",
  "index 5 index 3 index add add 3 div 10 1 roll 6 index 4 index 2 index add",
  "add 3 div 10 1 roll 9 {pop} repeat 3 array astore triangle } ifelse } bd",
  NULL
};


/////////////////////////////////////////////////////////////////////////////
// COpenGlPrintView

IMPLEMENT_DYNCREATE(COpenGlPrintView, COpenGlView)

BEGIN_MESSAGE_MAP(COpenGlPrintView, COpenGlView)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(COpenGlPrintView)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenGlPrintView construction/destruction

COpenGlPrintView::COpenGlPrintView()
{
	// initalize parameters
	m_clsPrintParams.m_clsColor.Set(0.0f, 0.0f, 0.0f, 1.0f);
	m_clsPrintParams.m_clsBackColor[0].Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_clsPrintParams.m_clsBackColor[1].Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_clsPrintParams.m_clsSolidColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
	m_clsPrintParams.m_clsAmbientLight.Set(0.1f, 0.2f, 0.3f, 1.0f);
	m_clsPrintParams.m_bPrinting = true;
	m_clsPrintParams.m_bPrintingToEPS = false;
	m_clsPrintParams.m_bCanPrintToEPS = true;
}

COpenGlPrintView::~COpenGlPrintView()
{
}

/////////////////////////////////////////////////////////////////////////////
// COpenGlPrintView printing

BOOL COpenGlPrintView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void COpenGlPrintView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void COpenGlPrintView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

// this printing routine uses DIBs
void COpenGlPrintView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	HDC			hMemDC;
	HGLRC		hMemRC;
	BITMAPINFO	bmi;
	LPVOID		pBitmapBits;
	HBITMAP		hDib;
	CSize		szPage, szDIB;

	COGLColors colors;

	// retrieve view screen colors
	colors = m_clsScreenParams.GetColors();
	// modify background colors to be white (printing)
	colors.m_clsBackTopColor = RGB(255, 255, 255);
	colors.m_clsBackBotColor = RGB(255, 255, 255);
	// and set these colors to the print parameters
	m_clsPrintParams.SetColors(colors);

//////////////////////////////////////////////////////////////////////
/// Print part #1: initializing


	// - Determine the DIB size for either printing or print preview

	CRect rcClient;
	GetClientRect(&rcClient); 
	float fClientRatio = float(rcClient.Height())/rcClient.Width();

	// Get page size
	szPage.cx  = pDC->GetDeviceCaps(HORZRES);
	szPage.cy = pDC->GetDeviceCaps(VERTRES);

	if (pInfo->m_bPreview)
	{
		// Use screen resolution for preview
		// TBC warning: should use preview page resolution!!
		szDIB.cx = rcClient.Width() / 2;
		szDIB.cy = rcClient.Height() / 2;
	}
	else  // Printing
	{
		// Use higher resolution for printing.
		// Adjust size according screen's ratio.
		if (szPage.cy > fClientRatio*szPage.cx)
		{
			// View area is wider than Printer area
			szDIB.cx = szPage.cx;
			szDIB.cy = long(fClientRatio*szPage.cx);
		}
		else
		{
			// View area is narrower than Printer area
			szDIB.cx = long(float(szPage.cy)/fClientRatio);
			szDIB.cy = szPage.cy;
		}
		// Reduce the Resolution if the Bitmap size is too big.
		// Ajdust the maximum value, which is depends on printer's memory.
		// Now limit is set to 20 MB. 
		while (szDIB.cx*szDIB.cy > 20e6)   
		{
			szDIB.cx = szDIB.cx/2;
			szDIB.cy = szDIB.cy/2;
		}
	}


	//TRACE("Buffer size: %d x %d = %6.2f MB\n", szDIB.cx, szDIB.cy, szDIB.cx*szDIB.cy*0.000001);
	
	// - Create DIB Section

	memset(&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = szDIB.cx;
	bmi.bmiHeader.biHeight = szDIB.cy;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = szDIB.cx * szDIB.cy * 3;

	HDC	hDC = ::GetDC(m_hWnd);
	hDib = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, &pBitmapBits, NULL, (DWORD)0);
	::ReleaseDC(m_hWnd, hDC);

	// - Create memory DC, and associate it with the DIB

	hMemDC = ::CreateCompatibleDC(NULL);
	if (!hMemDC)
	{
		DeleteObject(hDib);
		hDib = NULL;
		return;
	}
	SelectObject(hMemDC, hDib);

	// - Setup memory DC's pixel format

	if (!bSetupPixelFormat(hMemDC, PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_STEREO_DONTCARE))
	{
		DeleteObject(hDib);
		hDib = NULL;
		DeleteDC(hMemDC);
		hMemDC = NULL;
		return;
	}

	// - Create memory RC

	hMemRC = ::wglCreateContext(hMemDC);
	if (!hMemRC)
	{
		DeleteObject(hDib);
		hDib = NULL;
		DeleteDC(hMemDC);
		hMemDC = NULL;
		return;
	}

	// - Store old DC and RC
	//m_hOldDC = ::wglGetCurrentDC();
	//m_hOldRC = ::wglGetCurrentContext(); 

	// - Make the memory RC current

	MakeCurrentWGL(hMemDC, hMemRC);

	// - Set OpenGL state for memory RC

	// init OpenGL status
	InitOpenGL(m_clsPrintParams);

	// set viewport dimensions
	m_clsPrintParams.m_GLuiViewportCx = szDIB.cx;
	m_clsPrintParams.m_GLuiViewportCy = szDIB.cy;

	// set modelview matrix (rotate & translate)
	SetModelViewMatrix();

	// set projection matrix (ortho or persepctive)
	UpdateCamera(m_clsPrintParams);

	// enable / disable lighting
	UpdateLighting(m_clsPrintParams);

	// - Create display list with the newly created memory RC

	CreateModelDisplayList(m_clsPrintParams);

	// - Render the scene

	RenderScene(m_clsPrintParams);

	// - Release OpenGL

	ReleaseWGL();	

	
// Print2

	// Now the image is in the DIB already. We don't need the memory RC anymore. 
	// We'll copy the image to the DC for printing or print preview.

	// - Release memory RC, and restore the old DC and RC

	::wglDeleteContext(hMemRC);

	// Restore last DC and RC
	//::MakeCurrentWGL(m_hOldDC, m_hOldRC);	

	// - Calculate the target size according to the image size, and orientation of the paper

	float fBmiRatio = float(bmi.bmiHeader.biHeight) / bmi.bmiHeader.biWidth;
	CSize szTarget;  
	if (szPage.cx > szPage.cy)	 // Landscape page
	{
		if(fBmiRatio<1)	  // Landscape image
		{
			szTarget.cx = szPage.cx;
			szTarget.cy = long(fBmiRatio * szPage.cx);
		}
		else			  // Portrait image
		{
			szTarget.cx = long(szPage.cy/fBmiRatio);
			szTarget.cy = szPage.cy;
		}
	}
	else		    // Portrait page
	{
		if(fBmiRatio<1)	  // Landscape image
		{
			szTarget.cx = szPage.cx;
			szTarget.cy = long(fBmiRatio * szPage.cx);
		}
		else			  // Portrait image
		{
			szTarget.cx = long(szPage.cy/fBmiRatio);
			szTarget.cy = szPage.cy;
		}
	}

	CSize szOffset((szPage.cx - szTarget.cx) / 2, (szPage.cy - szTarget.cy) / 2);

	// - Stretch image to fit in the target size

	SetStretchBltMode(pDC->GetSafeHdc(), COLORONCOLOR);
	int nRet = ::StretchDIBits(pDC->GetSafeHdc(),
							  szOffset.cx, szOffset.cy,
							  szTarget.cx, szTarget.cy,
							  0, 0,
							  bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight,
							  (GLubyte*) pBitmapBits,
							  &bmi, DIB_RGB_COLORS, SRCCOPY);

	if(nRet == GDI_ERROR)
		TRACE0("Failed in StretchDIBits()");

	// - Release memory

	DeleteObject(hDib);
	hDib = NULL;
	DeleteDC(hMemDC);	
	hMemDC = NULL;

}

// this is the compare function required and used
// by standard library function 'qsort'
static int compare(const void *a, const void *b)
{
	CDepthIndex *p1 = (CDepthIndex *) a;
	CDepthIndex *p2 = (CDepthIndex *) b;
	GLfloat diff = p2->depth - p1->depth;

	if (diff > 0.0) {
		return 1;
	} else if (diff < 0.0) {
		return -1;
	} else {
		return 0;
	}
}

void COpenGlPrintView::outputEPS(int doSort, const char *filename)
{
	GLfloat *feedbackBuffer;
	GLint ret;
	FILE *file;
	char str[512];
	long size;
	CGLParams params;

	// copy OpenGL screen parameters
	params = m_clsScreenParams;
	// and modify the colors for output to EPS
	// (white background)
	params.m_clsBackColor[0].Set(1.0f, 1.0f, 1.0f, 1.0f);
	params.m_clsBackColor[1].Set(1.0f, 1.0f, 1.0f, 1.0f);
	params.m_bPrintingToEPS = true;

	MakeCurrentWGL(m_pDC->GetSafeHdc(), m_hrc);

	// a negative ret value means that the feedback buffer has overflowed,
	// so increase the buffer length until either ret > 0 or out of memory
	for( feedbackBuffer = NULL, size = FMV_EPS_FEEDBACK_BUFFER, ret = -1;
			ret < 0; size *= 2)		{

		if(feedbackBuffer != NULL)
			free(feedbackBuffer);

		feedbackBuffer = (GLfloat *) calloc(size, sizeof(GLfloat));
		if(feedbackBuffer == NULL)
			break;

		glFeedbackBuffer(size, GL_3D_COLOR, feedbackBuffer);
		glRenderMode(GL_FEEDBACK);

		// Render the scene in feedback mode for printing

		// create the display list with correct printing colors
		CreateModelDisplayList(params);
		// set modelview matrix (rotate & translate)
		SetModelViewMatrix();
		// set projection matrix (ortho or persepctive)
		UpdateCamera(params);
		// render
		RenderScene(params);
		// restore display list with correct colors
		CreateModelDisplayList(m_clsScreenParams);

		ret = glRenderMode(GL_RENDER);
	}

	if(feedbackBuffer == NULL) {
		sprintf(str, "Output to EPS failed\nCould not allocate %.f bytes", (float) size * sizeof(GLfloat));
		AfxMessageBox(str, MB_ICONSTOP);
		return;
	}

	if (filename && ret > 0) {
		file = fopen(filename, "w");
		if (file) {
			convertFeedbackToEPS(file, doSort, ret, feedbackBuffer, "FastModel");
			fclose(file);
		} 
		else {
			sprintf(str, "%s\ncould not be opened for writing", filename);
			AfxMessageBox(str, MB_ICONWARNING);
		}
	} 
	else {
		if(!filename) {
			AfxMessageBox("Internal error: no file name passed to EPS converter", MB_ICONSTOP);
		}
		else {
			AfxMessageBox("Error: nothing to print to EPS", MB_ICONSTOP);
		}
	}

	free(feedbackBuffer);

	ReleaseWGL();
}

void COpenGlPrintView::convertFeedbackToEPS(FILE * file, int doSort, GLint size, GLfloat * buffer, char *creator)
{
	GLfloat clearColor[4], viewport[4];
	GLfloat lineWidth;
	int i;

	// read OpenGL state variables for use in EPS header
	glGetFloatv(GL_VIEWPORT, viewport);
	glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
	glGetFloatv(GL_LINE_WIDTH, &lineWidth);
	glGetFloatv(GL_POINT_SIZE, &m_pointSize);

	// EPS header. */
	fputs("%!PS-Adobe-2.0 EPSF-2.0\n", file);
	fprintf(file, "%%%%Creator: %s\n", creator);
	fprintf(file, "%%%%BoundingBox: %g %g %g %g\n", viewport[0], viewport[1], viewport[2], viewport[3]);
	fputs("%%EndComments\n", file);
	fputs("\n", file);
	fputs("gsave\n", file);
	fputs("\n", file);

	// Output Frederic Delhoume's "gouraudtriangle" PostScript fragment
	fputs("% the gouraudtriangle PostScript fragement below is free\n", file);
	fputs("% written by Frederic Delhoume (delhoume@ilog.fr)\n", file);
	fprintf(file, "/threshold %g def\n", EPS_GOURAUD_THRESHOLD);
	for (i = 0; gouraudtriangleEPS[i]; i++) {
		fprintf(file, "%s\n", gouraudtriangleEPS[i]);
	}

	fprintf(file, "\n%g setlinewidth\n", lineWidth);
	// set clear color
	fprintf(file, "%g %g %g setrgbcolor\n", clearColor[0], clearColor[1], clearColor[2]);
	// fill a rectangle corresponding to visibleportion showed in view
	fprintf(file, "%g %g %g %g rectfill\n\n", viewport[0], viewport[1], viewport[2], viewport[3]);

	if (doSort) {
		convSortFeedbackToEPS(file, size, buffer);
	} else {
		convUnsortFeedbackToEPS(file, size, buffer);
	}

	// EPS trailer
	fputs("grestore\n\n", file);
	fputs("%Add `showpage' to the end of this file to be able to print to a printer.\n", file);
}

void COpenGlPrintView::convSortFeedbackToEPS(FILE *file, GLint size, GLfloat *buffer)
{
	int token;
	GLfloat *loc, *end, *polyptr;
	CGLFeedback3Dcolor *vertex;
	GLfloat depthSum, linedepth, polydepth, pointdepth;
	long nlines, npolygons, npoints, ncontourlines, ncontours, npolygroups;
	long ilines, ipolygons, ipoints, icontourlines, icontours, ipolygroups;
	long contourindex, contournum;
	CDepthIndex *lines, *points;
	CPolyDepthIndex  *polygons;
	CContour *contourlines, *pcontour;
	int nvertices, i;
	GLfloat status;
	BOOL isPolygon;

	end = buffer + size;

	// Count how many primitives there are
	//
	// there are three plus two types of primitives counted:
	// - points, lines, polygons
	// - contours and multiple polygons
	// while the first three are OpenGL elementary primitives, the other two
	// arise when is wanted to highlight the contour of a polygon (e.g. for 
	// hidden lines removal). In this case, passThrough pointers are inserted
	// before and after the polygon drawn as GL_LINE (the contour) and 
	// before and after the same polygon drawn as GL_FILL (the multiple polygon).
	// It must be remembered that Windows implementation of OpenGL renders polygons
	// as triangles, therefore any polygon is divided before rendering; from this
	// the multiple polygons arise.
	nlines = npolygons = npoints = ncontours = ncontourlines = npolygroups = 0;
	status = FMV_EPS_STATUS_NULL;
	loc = buffer;
	while (loc < end) {
		token = (int)(*loc);
		loc++;
		switch (token) {
			case GL_LINE_TOKEN:
			case GL_LINE_RESET_TOKEN:
				if( status == FMV_EPS_CONTOUR_START )
					ncontourlines++;
				else
					nlines++;
				loc += 14;
				break;
			case GL_POLYGON_TOKEN:
				isPolygon = TRUE;
				nvertices = (int)(*loc);
				loc++;
				loc += (7 * nvertices);

				if( status != FMV_EPS_POLYGON_START )
					npolygons++;

				break;
			case GL_POINT_TOKEN:
				loc += 7;
				npoints++;
				break;
			case GL_BITMAP_TOKEN:
			case GL_DRAW_PIXEL_TOKEN:
			case GL_COPY_PIXEL_TOKEN:
				// dummy! And not counts.
				loc += 7;
				break;
			case GL_PASS_THROUGH_TOKEN: 
				status = *loc;  
				if( status == FMV_EPS_CONTOUR_END )
					ncontours++;
				// TBC warning: see below next scan
				else if( status == FMV_EPS_POLYGON_START ) {
					isPolygon = FALSE;
				}
				else if( status == FMV_EPS_POLYGON_END ) {
					npolygroups++;
					// if there really was a polygon (not clipped out)
					if( isPolygon == TRUE) {
						npolygons++;
					}
				}
				else if( status != FMV_EPS_CONTOUR_START && status != FMV_EPS_POLYGON_START) 
					ASSERT(false);	// TBC warning: should issue error (not valid pass-through token)

				loc++;
				break;			
			default:
				;
				// this must be an error; however, pass over
				// TBC warning: should issue a warning, or errors in Output window
				//printf("Unknown token (%d).\n",	token);
		}
	}

	// Allocate an array of pointers that will point back at
	// primitives in the feedback buffer.  There will be one
	// entry per primitive.  This array is also where we keep the
	// primitive's average depth.  There is one entry per
	// primitive  in the feedback buffer.
	lines = (CDepthIndex *) malloc(sizeof(CDepthIndex) * nlines);
	// must add one contour more for termination, see below
	contourlines = (CContour *) malloc(sizeof(CContour) * (ncontourlines + 1));
	polygons = (CPolyDepthIndex *) malloc(sizeof(CPolyDepthIndex) * npolygons);
	points = (CDepthIndex *) malloc(sizeof(CDepthIndex) * npoints);

	if(lines == NULL || contourlines == NULL || polygons == NULL || points == NULL) {
		AfxMessageBox("Output to EPS failed\nCould not allocate enough memory", MB_ICONSTOP);
		return;
	}

	status = FMV_EPS_STATUS_NULL;
	ilines = ipolygons = ipoints = icontours = icontourlines = ipolygroups = 0;
	contourindex = 0;
	loc = buffer;
	while (loc < end) {
		
		token = (int)(*loc);
		switch (token) {
			case GL_LINE_TOKEN:
			case GL_LINE_RESET_TOKEN:
				// if a contour, save it as a contour; lines belonging to the same
				// contour share the same 'num'
				// TBC warning: as lines belonging to the same contour are sequential,
				// could save a pointer only to the first, plus a counter of how many
				// lines belong to the contour (as done for multiple polygons)
				if( status == FMV_EPS_CONTOUR_START ) {
					contourlines[icontourlines].ptr = loc; 
					contourlines[icontourlines].num = icontours;
					icontourlines++;
					loc += 15;
				}
				// otherwise, treat line as a self-standing element
				else {	
					lines[ilines].ptr = loc;  
					loc++;
					vertex = (CGLFeedback3Dcolor *) loc;
					depthSum = vertex[0].z + vertex[1].z;
					lines[ilines].depth = (GLfloat)(depthSum / 2.0);
					loc += 14;
					ilines++;
				}
				break;
			case GL_POLYGON_TOKEN:
				// if a multiple polygon, save as a polygon but signal 1) it has a contour,
				// linking the contour to the polygon 2) calculate total depth, intended
				// as the mean of the mean depth of every triangle 3) how many polygons
				// compose the polygon
				// TBC warning: could use a different array for polygons and for multiple 
				// polygons, to save memory not used in the array for normal polygons,
				// like it is done for contours
				if( status == FMV_EPS_POLYGON_START || status == FMV_EPS_POLYGON_CONT) {
					isPolygon = TRUE;
					if( status == FMV_EPS_POLYGON_START ) {
						status = FMV_EPS_POLYGON_CONT;
						polygons[ipolygons].ptr = loc;  
						polygons[ipolygons].depth = 0.0f;
						polygons[ipolygons].polycount = 0;
						// search for corresponding contour
						for( ; contourlines[contourindex].num != ipolygroups && contourindex < ncontourlines; contourindex++);
						if(contourindex >= ncontourlines)
							polygons[ipolygons].contour = NULL; //TBC warning: should be an internal error
						else
							polygons[ipolygons].contour = &(contourlines[contourindex]);
					}
					loc++;
					nvertices = (int)(*loc);
					loc++;
					vertex = (CGLFeedback3Dcolor *) loc;
					depthSum = vertex[0].z;
					for (i = 1; i < nvertices; i++) {
						depthSum += vertex[i].z;
					}
					// this average is partial and will be averaged once more 
					// at passThrough token FMV_EPS_POLYGON_END (see below)
					polygons[ipolygons].depth += depthSum / nvertices;
					loc += (7 * nvertices);
					// increase the count of how many polygons compose the multiple one
					(polygons[ipolygons].polycount)++;
				}
				// otherwise, treat like a normal polygon
				else {
					polygons[ipolygons].ptr = loc;  
					polygons[ipolygons].contour = NULL;
					polygons[ipolygons].polycount = 1;
					loc++;
					nvertices = (int)(*loc);
					loc++;
					vertex = (CGLFeedback3Dcolor *) loc;
					depthSum = vertex[0].z;
					for (i = 1; i < nvertices; i++) {
						depthSum += vertex[i].z;
					}
					polygons[ipolygons].depth = depthSum / nvertices;
					loc += (7 * nvertices);
					ipolygons++;				
				}
				break;
			case GL_POINT_TOKEN:
				points[ipoints].ptr = loc; 
				loc++;
				vertex = (CGLFeedback3Dcolor *) loc;
				points[ipoints].depth = vertex[0].z;
				loc += 7;
				ipoints++;
				break;
			case GL_BITMAP_TOKEN:
			case GL_DRAW_PIXEL_TOKEN:
			case GL_COPY_PIXEL_TOKEN:
				// dummy! And not counts.
				loc += 8;
				break;
			case GL_PASS_THROUGH_TOKEN:
				loc++;
				status = *loc;  
				if( status == FMV_EPS_CONTOUR_END )
					icontours++;
				// set pointer to NULL so, if polygon is not rendered due to 
				// clipping (the two START and END token group nothing between)
				// the conversion to EPS routine won't try to access invalid
				// pointers
				// TBC warning: using a different array for polygons and multiple
				// polygons should solve the problem automatically, as done
				// for contours: the index variable is incremented only if
				// a valid POLYGON token is encountered (see above for contours)
				else if( status == FMV_EPS_POLYGON_START ) {
					isPolygon = FALSE;
				}
				else if( status == FMV_EPS_POLYGON_END ) {
					ipolygroups++;
					// if there really was a polygon (not clipped out)
					if( isPolygon == TRUE ) {
						// perform total average for multiple polygon 
						polygons[ipolygons].depth /= polygons[ipolygons].polycount;
						ipolygons++;
					}
				}
				else if( status != FMV_EPS_CONTOUR_START && status != FMV_EPS_POLYGON_START) 
					ASSERT(false);	// TBC warning: should issue error (not valid pass-through token)
				loc++;
				break;			
			default:
				;
				// this must be an error; however, pass over
				// TBC warning: should issue a warning, or errors in Output window
				//printf("Unknown token (%d).\n",	token);
		}
	}
	ASSERT(nlines == ilines);
	ASSERT(ncontourlines == icontourlines);
	ASSERT(npolygons == ipolygons);
	ASSERT(npolygroups == ipolygroups);
	ASSERT(npoints == ipoints);
	ASSERT(ncontours == icontours);

	// sort the primitives back to front using average depth
	//
	// contours are linked to multiple polygons, so don't need sorting
	qsort(lines, nlines, sizeof(CDepthIndex), compare);
	qsort(points, npoints, sizeof(CDepthIndex), compare);
	qsort(polygons, npolygons, sizeof(CPolyDepthIndex), compare);

	// sorting by a primitives average depth
	// doesn't allow to disambiguate some cases like self
	// intersecting polygons.  Handling these cases would require
	// breaking up the primitives 

	// output primitives to Encapsulated PostScript file in back to front order
	//
	//
	// set initial depths
	//
	// if there aren't lines, set depth to minimum (last output)
	// and set termination condition (ilines >= nlines) to true.
	// The depth setting to FMV_EPS_MAX_GLFLOAT is a trick to 
	// avoid searching over the end of the arrays: setting depth 
	// to maximum it is certain that the other arrays will be
	// scanned first. When everythig has been scanned, the for cycle
	// exits anyway (ilines > nlines, etc.) but the routine will not
	// have ever tried to access lines[ilines] with ilines >= nlines.
	if(nlines > 0) {
		linedepth = lines[0].depth;
		ilines = 0;
	}
	else {
		linedepth = FMV_EPS_MAX_GLFLOAT;
		ilines = 1;
	}
	if(npoints > 0) {
		pointdepth = points[0].depth;
		ipoints = 0;
	}
	else {
		pointdepth = FMV_EPS_MAX_GLFLOAT;
		ipoints = 1;
	}
	if(npolygons > 0) {
		polydepth = polygons[0].depth;
		ipolygons = 0;
	}
	else {
		polydepth = FMV_EPS_MAX_GLFLOAT;
		ipolygons = 1;
	}
	// termination condition for contour searching: add a last contour with a serial
	// number bigger that all the others, but no polygon will ever point to it
	contourlines[icontourlines].ptr = NULL; 
	contourlines[icontourlines].num = icontours;
	// loop until all lines, points and polygons have been scanned
	// of course, having separated points, lines and polygons, should ordinate
	// them while outputting
	for (icontourlines = 0; ilines < nlines || ipoints < npoints || ipolygons < npolygons; ) {

		// line is the farthest
		if(linedepth < pointdepth && linedepth < polydepth) {
			(void) printPrimitiveToEPS(file, lines[ilines].ptr);

			ilines++;
			if(ilines < nlines)
				linedepth = lines[ilines].depth;
			else
				linedepth = FMV_EPS_MAX_GLFLOAT;
		}

		// point is the farthest
		if(pointdepth < linedepth && pointdepth < polydepth) {
			(void) printPrimitiveToEPS(file, points[ipoints].ptr);

			ipoints++;
			if(ipoints < npoints)
				pointdepth = points[ipoints].depth;
			else
				pointdepth = FMV_EPS_MAX_GLFLOAT;
		}

		// polygon is the farthest
		if(polydepth < linedepth && polydepth < pointdepth) {
			// render the polygon, either single or multiple
			for( i=0, polyptr = polygons[ipolygons].ptr; i<polygons[ipolygons].polycount; i++) {
				(void) printPrimitiveToEPS(file, polyptr);
				nvertices = (int)(*(polyptr+1));
				polyptr += 2 + nvertices * 7;
			}

			// render the contour, if any
			pcontour = polygons[ipolygons].contour;
			if(pcontour != NULL) {
				for( pcontour = polygons[ipolygons].contour, contournum = (polygons[ipolygons].contour)->num; pcontour->num == contournum; pcontour++)
					(void) printPrimitiveToEPS(file, pcontour->ptr);
			}

			ipolygons++;
			if(ipolygons < npolygons)
				polydepth = polygons[ipolygons].depth;
			else
				polydepth = FMV_EPS_MAX_GLFLOAT;
		}

	}
	
	free(lines);
	free(contourlines);
	free(polygons);
	free(points);
}

void COpenGlPrintView::convUnsortFeedbackToEPS(FILE * file, GLint size, GLfloat * buffer)
{
	GLfloat *loc, *end;

	loc = buffer;
	end = buffer + size;
	while (loc < end) {
		loc = printPrimitiveToEPS(file, loc);
	}
}

GLfloat *COpenGlPrintView::printPrimitiveToEPS(FILE * file, GLfloat * loc)
{
	int token;
	int nvertices, i;
	GLfloat red, green, blue;
	int smooth;
	GLfloat dx, dy, dr, dg, db, absR, absG, absB, colormax;
	int steps;
	CGLFeedback3Dcolor *vertex;
	GLfloat xstep, ystep, rstep, gstep, bstep;
	GLfloat xnext, ynext, rnext, gnext, bnext, distance;

	token = (int)(*loc);
	loc++;
	switch (token) {
		case GL_LINE_RESET_TOKEN:
		case GL_LINE_TOKEN:
			vertex = (CGLFeedback3Dcolor *) loc;

			dr = vertex[1].red - vertex[0].red;
			dg = vertex[1].green - vertex[0].green;
			db = vertex[1].blue - vertex[0].blue;

			if (dr != 0 || dg != 0 || db != 0) {
				// Smooth shaded line
				dx = vertex[1].x - vertex[0].x;
				dy = vertex[1].y - vertex[0].y;
				distance = (GLfloat)(sqrt(dx * dx + dy * dy));

				absR = (GLfloat)(fabs(dr));
				absG = (GLfloat)(fabs(dg));
				absB = (GLfloat)(fabs(db));

				colormax = max(absR, max(absG, absB));
				steps = (int)(max(1.0, colormax * distance * EPS_SMOOTH_LINE_FACTOR));

				xstep = dx / steps;
				ystep = dy / steps;

				rstep = dr / steps;
				gstep = dg / steps;
				bstep = db / steps;

				xnext = vertex[0].x;
				ynext = vertex[0].y;
				rnext = vertex[0].red;
				gnext = vertex[0].green;
				bnext = vertex[0].blue;

				// Back up half a step; the end points have to be 
				// exactly the their endpoint colors
				xnext -= (GLfloat)(xstep / 2.0);
				ynext -= (GLfloat)(ystep / 2.0);
				rnext -= (GLfloat)(rstep / 2.0);
				gnext -= (GLfloat)(gstep / 2.0);
				bnext -= (GLfloat)(bstep / 2.0);
			} else {
				// Single color line
				steps = 0;
			}

			fprintf(file, "%g %g %g setrgbcolor\n",
			vertex[0].red, vertex[0].green, vertex[0].blue);
			fprintf(file, "%g %g moveto\n", vertex[0].x, vertex[0].y);

			for (i = 0; i < steps; i++) {
				xnext += xstep;
				ynext += ystep;
				rnext += rstep;
				gnext += gstep;
				bnext += bstep;
				fprintf(file, "%g %g lineto stroke\n", xnext, ynext);
				fprintf(file, "%g %g %g setrgbcolor\n", rnext, gnext, bnext);
				fprintf(file, "%g %g moveto\n", xnext, ynext);
			}
			fprintf(file, "%g %g lineto stroke\n", vertex[1].x, vertex[1].y);

			loc += 14;

			break;

		case GL_POLYGON_TOKEN:
			nvertices = (int)(*loc);
			loc++;

			vertex = (CGLFeedback3Dcolor *) loc;

			if (nvertices > 0) {
				red = vertex[0].red;
				green = vertex[0].green;
				blue = vertex[0].blue;
				smooth = 0;
				for (i = 1; i < nvertices; i++) {
					if (red != vertex[i].red || green != vertex[i].green || blue != vertex[i].blue) {
						smooth = 1;
						break;
					}
				}
				if (smooth) {
					// Smooth shaded polygon; varying colors at vertices
					int triOffset;

					// Break polygon into "nvertices-2" triangle fans. */
					for (i = 0; i < nvertices - 2; i++) {
						triOffset = i * 7;
						fprintf(file, "[%g %g %g %g %g %g]",
							vertex[0].x, vertex[i + 1].x, vertex[i + 2].x,
							vertex[0].y, vertex[i + 1].y, vertex[i + 2].y);
						fprintf(file, " [%g %g %g] [%g %g %g] [%g %g %g] gouraudtriangle\n",
							vertex[0].red, vertex[0].green, vertex[0].blue,
							vertex[i + 1].red, vertex[i + 1].green, vertex[i + 1].blue,
							vertex[i + 2].red, vertex[i + 2].green, vertex[i + 2].blue);
					}
				} else {
					// Flat shaded polygon; all vertex colors the same
					fprintf(file, "newpath\n");
					fprintf(file, "%g %g %g setrgbcolor\n", red, green, blue);

					// Draw a filled triangle
					fprintf(file, "%g %g moveto\n", vertex[0].x, vertex[0].y);
					for (i = 1; i < nvertices; i++) {
						fprintf(file, "%g %g lineto\n", vertex[i].x, vertex[i].y);
					}
					fprintf(file, "closepath fill\n\n");
				}
			}
			loc += nvertices * 7;  

			break;

		case GL_POINT_TOKEN:
			vertex = (CGLFeedback3Dcolor *) loc;
			fprintf(file, "%g %g %g setrgbcolor\n", vertex[0].red, vertex[0].green, vertex[0].blue);
			fprintf(file, "%g %g %g 0 360 arc fill\n\n", vertex[0].x, vertex[0].y, m_pointSize / 2.0);
			loc += 7;           
			break;

 		case GL_BITMAP_TOKEN:
		case GL_DRAW_PIXEL_TOKEN:
		case GL_COPY_PIXEL_TOKEN:
			// dummy! And not counts.
			loc += 7;
			break;
		case GL_PASS_THROUGH_TOKEN:
			// dummy! And not counts.
			loc += 1;
			break;			
		default:
			// this must be an error; however, pass over
			// TBC warning: should issue a warning, or errors in Output window
			//printf("Unknown token (%d).\n",	token);
			;
	}

	return loc;
}

// virtual function
void COpenGlPrintView::CreateModelDisplayList(CGLParams params)
{
}

// virtual function
void COpenGlPrintView::SetModelViewMatrix() 
{
}

// virtual function
void COpenGlPrintView::UpdateLighting(CGLParams params) 
{
}
