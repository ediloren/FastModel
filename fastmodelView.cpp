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


// fastmodelView.cpp : implementation of the CFastModelView class
//

#include "stdafx.h"

#include <math.h>

#include "fastmodel.h"

#include "fastmodelDoc.h"
#include "fastmodelView.h"

// scaling of the viewport used to show axes
// (i.e. the axes are shown in a 'subwindow' large
// 1 / FMV_AXES_SCALE of the model viewport
#define FMV_AXES_SCALE		4
// constant used in field of view calculations
#define FMV_TAN45_BY2		0.82842712474619
// model scale factor
#define FMV_MAX_MODEL_DIM	100.0
// rotation unit step for 'rotate near' 
#define FMV_STEP_ROTATE_NEAR	10.0

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CFastModelView

IMPLEMENT_DYNCREATE(CFastModelView, COpenGlPrintView)

BEGIN_MESSAGE_MAP(CFastModelView, COpenGlPrintView)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CFastModelView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_VIEW_DISPLAY_WIREFRAME, OnViewDisplayWireframe)
	ON_COMMAND(ID_VIEW_DISPLAY_HIDDENLINES, OnViewDisplayHiddenlines)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DISPLAY_HIDDENLINES, OnUpdateViewDisplayHiddenlines)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DISPLAY_WIREFRAME, OnUpdateViewDisplayWireframe)
	ON_COMMAND(ID_VIEW_ZOOM_ALL, OnViewZoomAll)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(ID_FILE_PRINT_TO_EPS, OnFilePrintToEps)
	ON_COMMAND(ID_VIEW_DISPLAY_2D, OnViewDisplay2D)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DISPLAY_2D, OnUpdateViewDisplay2D)
	ON_COMMAND(ID_VIEW_DISPLAY_3D, OnViewDisplay3D)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DISPLAY_3D, OnUpdateViewDisplay3D)
	ON_COMMAND(ID_VIEW_AUTOSET_XY, OnViewAutosetXY)
	ON_COMMAND(ID_VIEW_AUTOSET_YZ, OnViewAutosetYZ)
	ON_COMMAND(ID_VIEW_AUTOSET_ZX, OnViewAutosetZX)
	ON_COMMAND(ID_VIEW_DISPLAY_SOLID, OnViewDisplaySolid)
	ON_UPDATE_COMMAND_UI(ID_VIEW_DISPLAY_SOLID, OnUpdateViewDisplaySolid)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_VIEW_ORIGIN_TRANSLATE, OnViewOriginTranslate)
	//}}AFX_MSG_MAP
	// Standard printing commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFastModelView construction/destruction

CFastModelView::CFastModelView()
{
	m_ptPrev = CPoint(0,0);

	m_iDrawMode = FMV_WIREFRAME;
	m_cViewProjection = FMV_PERSPECTIVE;

	m_uiKeyPressed = 0;
	m_status = FMV_TRANSLATE;

	m_fZoomBParam = 1.0;
	m_fZoomCParam = 1.0;

	m_clsPos = CPoint(0, 0);
}

CFastModelView::~CFastModelView()
{
}

bool CFastModelView::LoadWindowState(CString regSection)
{
	CWinApp *pApp;

	pApp = AfxGetApp();
	ASSERT(pApp);

	// get view draw mode from the registry
	m_iDrawMode = pApp->GetProfileInt( regSection, _T("DrawMode"), FMV_WIREFRAME );
	// get view projection type from the registry
	m_cViewProjection = pApp->GetProfileInt( regSection, _T("ViewProjection"), FMV_PERSPECTIVE );

	return COpenGlPrintView::LoadWindowState(regSection);
}

void CFastModelView::SaveWindowState(CString regSection)
{
	CWinApp *pApp;

	pApp = AfxGetApp();
	ASSERT(pApp);

	// write view draw mode in the registry
	pApp->WriteProfileInt( regSection, _T("DrawMode"), m_iDrawMode );
	// write view projection type from the registry
	pApp->WriteProfileInt( regSection, _T("ViewProjection"), m_cViewProjection );

	COpenGlPrintView::SaveWindowState(regSection);
}

void CFastModelView::RenderScene(CGLParams params)
{

	// clear screen
	ClearScreen(params);

	glPushMatrix();

		glLoadIdentity();

		// set light position to be coincident with viewpoint
		GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glPopMatrix();
	
	// render the model
	RenderModel(params);

	// render cartesian axes
	RenderAxes(params);
}

void CFastModelView::RenderAxes(CGLParams params)
{
	int minCoord;

	minCoord = min(params.m_GLuiViewportCx, params.m_GLuiViewportCy);

	// draw the axes in the lower left corner
	glPushAttrib(GL_VIEWPORT_BIT);
	glPushAttrib(GL_ENABLE_BIT);

	glViewport(0, 0, minCoord / FMV_AXES_SCALE, minCoord / FMV_AXES_SCALE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glPushMatrix();
		glLoadIdentity();

		// translate into view (the perspective view clips at 0.1,
		// therefore objects at the origin are not visible)
		glTranslatef(0.0f, 0.0f, -4.0f);

		// apply rotation
		glMultMatrixd(m_daAxesMVMatrix);

		// define the projection type
		glMatrixMode(GL_PROJECTION);

		glPushMatrix();

			glLoadIdentity();

			if( m_cViewProjection == FMV_PERSPECTIVE) {
				// warning: cannot set near plane to 0.0f, otherwise
				// the z-buffer does not work correctly any more;
				// aspect ratio is 1.0f because wiewport is square
				gluPerspective(45.0, 1.0f, 0.1f, 10000.0f);
			}
			else {
				glOrtho(-FMV_MAX_AXES_DIM, FMV_MAX_AXES_DIM,
					-FMV_MAX_AXES_DIM, FMV_MAX_AXES_DIM, 0.0, 10000.0);
			}

			glCallList(params.m_GLuiAxesDispList);

		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);

	glPopMatrix();

	glPopAttrib();
	glPopAttrib();
}

void CFastModelView::RenderModel(CGLParams params)
{
	CFastModelDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	glLoadIdentity();
	glMultMatrixd(m_daMVMatrix);

	if( pDoc->existsAnyModel() == TRUE) {

		glCallList(params.m_GLuiModelDispList);

		glFinish();
	}
}

// public function to initialize the scene (for istance, after importing
// a new FastHenry file) 
void CFastModelView::InitializeScene()
{
	// compute zoom parameters
	if (GetDocument()->existsAnyModel() == TRUE) {
		computeBBoxParams();		
	}

	UpdateModel();
}

void CFastModelView::computeBBoxParams()
{
	float min, max;
	C3DVector span;

	m_clsModelBBox = ComputeGlobalBBox();
	
	span = m_clsModelBBox.max_point - m_clsModelBBox.min_point;
	
	min = min(span.x, span.y);
	min = min(min, span.z);
	max = max(span.x, span.y);
	max = max(max, span.z);

	// scale parameter
	// all models are scaled so that the max dimension is FMV_MAX_MODEL_DIM,
	// therefore they are not unesthetically clipped by perspective view 
	// near and far clip planes when zooming
	m_fScale = fabs(FMV_MAX_MODEL_DIM / max);

	// scale max and min values
	max *= m_fScale;
	min *= m_fScale;

	// quadratic zoom
	// the formula used is c + b * x ^ 2
	// c and b coefficients are found by imposing for x = FMV_DEPTH_SPEED => zoom = max,
	// for x = 1.0 => zoom = min / FMV_DEPTH_SPEED ;
	// the formulas for finding b and c are approximated
	m_fZoomBParam = max / (FMV_DEPTH_SPEED * FMV_DEPTH_SPEED); 
	m_fZoomCParam = min / FMV_DEPTH_SPEED - m_fZoomBParam;
	
	// translation speed
	//
	// this if is needed in case of planar (or quasi planar) structures,
	// where min approaches 0
	if( max > FMV_MAX_DIFF_BTW_MAX_AND_MIN * min)
		m_fTranslParam = max / FMV_MAX_DIFF_BTW_MAX_AND_MIN;
	else
		m_fTranslParam = min / FMV_TRANSL_SPEED;
}		

C3DBBox CFastModelView::ComputeGlobalBBox()
{
	C3DBBox gbbox;

	CFastModelDoc *pDoc = GetDocument();

	if(pDoc->m_pFastHenryModel != NULL) {
		gbbox += pDoc->m_pFastHenryModel->bbox;
	}

	if(pDoc->m_pFastCapModel != NULL) {
		gbbox += pDoc->m_pFastCapModel->bbox;
	}

	return gbbox;
}

void CFastModelView::ZoomAll()
{
	C3DVector span, middle;
	float base, height, dist, yxRatioViewport;
	BOOL isWglCurrent;

	if (GetDocument()->existsAnyModel() == TRUE) {

		span = (m_clsModelBBox.max_point - m_clsModelBBox.min_point) * m_fScale;

		middle = (m_clsModelBBox.max_point + m_clsModelBBox.min_point) * m_fScale / 2.0;

		yxRatioViewport = (float) m_clsScreenParams.m_GLuiViewportCy / m_clsScreenParams.m_GLuiViewportCx;

		if( span.x * yxRatioViewport > span.y ) {
			// in this case is possible to show model using x bbox as base 
			// and y calculated from viewport ratio
			base = span.x;
			height= span.x * yxRatioViewport;
		}
		else {
			// in this other case is possible to show model using y bbox as height
			// and x calculated from viewport ratio
			height = span.y;
			base = span.y / yxRatioViewport;
		}

		// enlarge a 5% to show model a bit inside the window (not cutting edges)
		height += height * 5.0 / 100.0;
		// math trigonometric functions use radians
		// this is (height / 2) / tan((45.0 / 2) * PI / 180)
		dist = (height / 2) / 0.4142136;


		// update camera
		isWglCurrent = MakeCurrentWGL(m_pDC->GetSafeHdc(), m_hrc);
		ASSERT( isWglCurrent );

		// initialize the modelview matrix
		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();

		// store axes modelview matrix
		glGetDoublev(GL_MODELVIEW_MATRIX, m_daAxesMVMatrix);

		// init the translate (zoom) and rotation position
		glTranslatef(-middle.x, -middle.y, -(middle.z + dist));

		// store the distance from current center of view 
		m_fRadius = -(middle.z + dist);

		// store the modelview matrix
		glGetDoublev(GL_MODELVIEW_MATRIX, m_daMVMatrix);
					
		UpdateCamera(m_clsScreenParams);
					
		ReleaseWGL();

		// update model window
		Invalidate(FALSE);
	}
}

void CFastModelView::CreateModelDisplayList(CGLParams params)
{
	CFastModelDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	if( pDoc->existsAnyModel() == TRUE) {

		glNewList(params.m_GLuiModelDispList, GL_COMPILE);

		if( m_iDrawMode & FMV_WIREFRAME ) {

			if( pDoc->m_pFastHenryModel != NULL)
				DrawWireframeModel(pDoc->m_pFastHenryModel, params);
			if( pDoc->m_pFastCapModel != NULL)
				DrawWireframeModel(pDoc->m_pFastCapModel, params);

		}
		else if( m_iDrawMode & FMV_HIDDENLINES ) {

			if( pDoc->m_pFastHenryModel != NULL)
				DrawHiddenlinesModel(pDoc->m_pFastHenryModel, params);
			if( pDoc->m_pFastCapModel != NULL)
				DrawHiddenlinesModel(pDoc->m_pFastCapModel, params);

		}
		else if( m_iDrawMode & FMV_SOLID ) {

			if( pDoc->m_pFastHenryModel != NULL)
				DrawSolidModel(pDoc->m_pFastHenryModel, params);
			if( pDoc->m_pFastCapModel != NULL)
				DrawSolidModel(pDoc->m_pFastCapModel, params);

		}

		glEndList();

	}

}

void CFastModelView::DrawWireframeModel(C3DShell *model, CGLParams params)
{
	glColor4f(params.m_clsColor.red, params.m_clsColor.green,
		params.m_clsColor.blue, params.m_clsColor.alpha);
	DrawWireframeShell(model);
}

void CFastModelView::DrawHiddenlinesModel(C3DShell *model, CGLParams params)
{
	//
	// first pass, draw only polygon interiors, filled with 
	// background color, slightly z-offset
	//
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);
	// preserve background color by inhibiting drawing to color buffers
	// (drawing to other buffers still occur, in particular to z-buffer,
	// necessary for hidden line removal trick)
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	
	DrawPatchShell(model, params);
	
	glDisable(GL_POLYGON_OFFSET_FILL);
	// restore drawing to color buffers
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	
	//
	// second pass, draw only contours
	//
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	m_iDrawMode |= FMV_EPS_CONTOUR;
	DrawPatchShell(model, params);
	m_iDrawMode &= ~FMV_EPS_CONTOUR;
}

void CFastModelView::DrawSolidModel(C3DShell *model, CGLParams params)
{
	// material color for lighting
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	
	glColor4f(params.m_clsSolidColor.red, params.m_clsSolidColor.green,
		params.m_clsSolidColor.blue, params.m_clsSolidColor.alpha);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	//
	// first pass, draw only fully opaque primitives
	//

	DrawPatchShell(model, params, true);

	//
	// second pass, draw partially opaque primitives
	// remark: this is a 'hack' that will not work correctly
	// for overlapping partially opaque patches.
	// Partially opaque primitives should be rendered in back-to-front order;
	// alternatively, an 'Order Independent Transparency' approach should be used,
	// see for instance the 'Order Independent Transparency' presentation by A. Rigazzi 
	//

	glEnable (GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	DrawPatchShell(model, params, false);

	glDisable(GL_BLEND);
}

void CFastModelView::DrawWireframeShell(C3DShell *shell)
{
	C3DList<C3DShell>::iterator its;
	C3DList<C3DFace>::iterator itf;
	C3DList<C3DLoop>::iterator itl;
	C3DList<C3DEdgeUse>::iterator iteu;

	// scan every subshell in shell 
	for(its = shell->shellList.begin(); its != shell->shellList.end(); its++)
		DrawWireframeShell(*its);

	// scan every face in shell 
	for(itf = shell->faceList.begin(); itf != shell->faceList.end(); itf++)
		DrawWireframeShellFace(*itf);

	// scan every loop in shell 
	for(itl = shell->loopList.begin(); itl != shell->loopList.end(); itl++)
		DrawWireframeShellLoop(*itl);

	// scan every edgeuse in shell 
	for(iteu = shell->edgeUseList.begin(); iteu != shell->edgeUseList.end(); iteu++)
		DrawWireframeShellEdgeUse(*iteu);
}

void CFastModelView::DrawWireframeShellFace(C3DFace *face)
{
	C3DList<C3DLoop>::iterator itl;

	// scan every loop in face 
	for(itl = face->loopList.begin(); itl != face->loopList.end(); itl++)
		DrawWireframeShellLoop(*itl);
}

void CFastModelView::DrawWireframeShellLoop(C3DLoop *loop)
{
	C3DList<C3DEdgeUse>::iterator iteu;

	// scan every edgeuse in loop 
	for(iteu = loop->edgeUseList.begin(); iteu != loop->edgeUseList.end(); iteu++)
		DrawWireframeShellEdgeUse(*iteu);
}

void CFastModelView::DrawWireframeShellEdgeUse(C3DEdgeUse *edgeuse)
{
	C3DVector *vertex1, *vertex2;

	glBegin(GL_LINES);

	vertex1 = &(edgeuse->pEdge->pVertexUse1->pVertex->pos);
	vertex2 = &(edgeuse->pEdge->pVertexUse2->pVertex->pos);

	glVertex3f(vertex1->x * m_fScale, vertex1->y * m_fScale, vertex1->z * m_fScale);
	glVertex3f(vertex2->x * m_fScale, vertex2->y * m_fScale, vertex2->z * m_fScale);

	glEnd();
}

void CFastModelView::DrawPatchShell(C3DShell *shell, CGLParams params, bool opaque)
{
	C3DList<C3DShell>::iterator its;
	C3DList<C3DFace>::iterator itf;
	C3DList<C3DLoop>::iterator itl;
	C3DList<C3DEdgeUse>::iterator iteu;

	// scan every subshell in shell 
	for(its = shell->shellList.begin(); its != shell->shellList.end(); its++)
		DrawPatchShell(*its, params, opaque);

	// scan every face in shell 
	for(itf = shell->faceList.begin(); itf != shell->faceList.end(); itf++)
		DrawPatchShellFace(*itf, params, opaque);

	// scan every loop in shell 
	for(itl = shell->loopList.begin(); itl != shell->loopList.end(); itl++)
		// loops are not faces: draw as wireframe
		DrawWireframeShellLoop(*itl);

	// scan every edgeuse in shell 
	for(iteu = shell->edgeUseList.begin(); iteu != shell->edgeUseList.end(); iteu++)
		DrawWireframeShellEdgeUse(*iteu);
}

void CFastModelView::DrawPatchShellFace(C3DFace *face, CGLParams params, bool opaque)
{
	C3DList<C3DLoop>::iterator itl;
	C3DList<C3DEdgeUse>::iterator iteu;
	C3DVector *vertex, *normal;


	if( m_iDrawMode & FMV_HIDDENLINES ) {
		glColor4f(params.m_clsColor.red, params.m_clsColor.green,
			params.m_clsColor.blue, params.m_clsColor.alpha);
	}
	else {
		
		// set face color, or default color
		if(face->hasColor == false) {
			glColor4f(params.m_clsSolidColor.red, params.m_clsSolidColor.green,
				params.m_clsSolidColor.blue, params.m_clsSolidColor.alpha);
		}
		else {
			glColor4ub(face->rgb.red, face->rgb.green, face->rgb.blue, face->rgb.alpha);
			
			// debug
			//if(face->rgb.red != 224 || face->rgb.green != 224 || face->rgb.blue != 224)
			//glNormal3f(normal->x, normal->y, normal->z);
			
		}
	}
	
	if( (opaque == true && face->rgb.alpha == 255) ||
		(opaque == false && face->rgb.alpha != 255) ||
		m_iDrawMode & FMV_HIDDENLINES) {

		// set face normal
		normal = &(face->plane.m_vecNormal);
		glNormal3f(normal->x, normal->y, normal->z);
		
		// scan every loop in face 
		for(itl = face->loopList.begin(); itl != face->loopList.end(); itl++) {
			
			// signal it is a polygon start, for exporting
			// to EPS routines
			if(params.m_bPrintingToEPS)
				if( m_iDrawMode & FMV_EPS_CONTOUR )
					glPassThrough(FMV_EPS_CONTOUR_START);
				else
					glPassThrough(FMV_EPS_POLYGON_START);
				
				glBegin(GL_POLYGON);
				
				for(iteu = itl->edgeUseList.begin(); iteu != itl->edgeUseList.end(); iteu++) {
					// Must choose vertexes in correct order; edgeuses are 
					// ordered in a loop, but can have two different orientations
					// w.r.t. the pointed edge, so must select always the same vertex
					// (or always the opposite one). GetFirstVertexUse() guarantees
					// to get always the first vertex
					vertex = &(iteu->GetFirstVertexUse()->pVertex->pos);
					
					glVertex3f(vertex->x * m_fScale, vertex->y * m_fScale, vertex->z * m_fScale);
				}
				
				glEnd();
				
				// signal it is a polygon end, for exporting
				// to EPS routines
				if(params.m_bPrintingToEPS)
					if( m_iDrawMode & FMV_EPS_CONTOUR )
						glPassThrough(FMV_EPS_CONTOUR_END);
					else
						glPassThrough(FMV_EPS_POLYGON_END);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFastModelView message handlers


void CFastModelView::UpdateCamera(CGLParams params) 
{
	float hx, hy;

	glPushMatrix();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		if( m_cViewProjection == FMV_PERSPECTIVE) {
			// warning: cannot set near plane to 0.0f, otherwise
			// the z-buffer does not work correctly any more
			gluPerspective(45.0, (GLdouble) params.m_GLuiViewportCx / params.m_GLuiViewportCy,
							0.1f, 10000.0f);
		}
		else {
			// Field of view is      hy
			//                    ---------
			//                    \   |   /
			//                     \  |z /
			//                      \ | /
			//                        \/
			//                       45 degrees
			// 
			// Therefore, tan(45) = hy / (2*z) -> hy = z * tan(45) * 2
			hy = -m_fRadius * FMV_TAN45_BY2;
			hx = params.m_GLuiViewportCx * hy / params.m_GLuiViewportCy;
			glOrtho(-hx/2.0, hx/2.0, -hy/2.0, hy/2.0, 0.0, 10000.0);
		}
		glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void CFastModelView::OnViewZoomAll() 
{
	ZoomAll();
}

void CFastModelView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// If the view changes the viewport origin and mapping mode,
	// It's necessary to convert the point from device coordinates
	// to logical coordinates, such as are stored in the document:
	//CClientDC dc(this);
	//OnPrepareDC(&dc);
	//dc.DPtoLP(&point);

	// Serves as the anchor point for the
	// action to the next point, as the user drags the
	// mouse; the action depends on current key 
	// pressed (zoom, rotate, etc.)
	m_ptZoom = point;
	m_ptPrev = point;   

	// Capture the mouse until button up.
	SetCapture();       
}

void CFastModelView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// If this window (view) didn't capture the mouse,
	// then the user isn't drawing in this window.
	if (GetCapture() != this)
		return; 
				
	m_ptPrev = CPoint(0,0);

	// Release the mouse capture established at
	// the beginning of the mouse drag.
	ReleaseCapture();   
}

void CFastModelView::OnMouseMove(UINT nFlags, CPoint point) 
{
	C3DVector angle;
	C3DVector coord;
	GLfloat zoomRadius, zoomRadius1, zoomRadius2;
	float dist1, dist2;
	BOOL isWglCurrent;

	// If this window (view) didn't capture the mouse,
	// then the user isn't using in this window.
	if (GetCapture() == this) {	

		CFastModelDoc* pDoc = GetDocument();

		ASSERT(pDoc);
		//CClientDC dc(this);

		// If the view changes the viewport origin and mapping mode,
		// It's necessary to convert the point from device coordinates
		// to logical coordinates, such as are stored in the document:
		//
		// set up mapping mode and viewport origin
		//OnPrepareDC(&dc);  
		//dc.DPtoLP(&point);	

		isWglCurrent = MakeCurrentWGL(m_pDC->GetSafeHdc(), m_hrc);
		ASSERT( isWglCurrent );

		// must move the view in function of user's current position;
		// to do so, is necessary to invert the order of multiplication
		// in the matrix, in order to apply current movement as last one.
		// So the modelview matrix is retrieved, stored and then multiplied
		// for the current rototranslation
		glLoadIdentity();

		if( (nFlags & MK_LBUTTON) && m_status == FMV_ROTATE_NEAR) {

			angle.y = (point.x - m_ptPrev.x) / FMV_STEP_ROTATE_NEAR;
			angle.x = (point.y - m_ptPrev.y) / FMV_STEP_ROTATE_NEAR;

	        glRotatef(angle.x, 1.0f, 0.0f, 0.0f);
		    glRotatef(angle.y, 0.0f, 1.0f, 0.0f);

			// update axes modelview matrix
			glPushMatrix();
				glMultMatrixd(m_daAxesMVMatrix);
				glGetDoublev(GL_MODELVIEW_MATRIX, m_daAxesMVMatrix);
			glPopMatrix();

		}
		else if( (nFlags & MK_LBUTTON) && m_status == FMV_ROTATE_FAR) {

			angle.y = point.x - m_ptPrev.x;
			angle.x = point.y - m_ptPrev.y;

			// update axes modelview matrix
			glPushMatrix();
				glRotatef(angle.x, 1.0f, 0.0f, 0.0f);
				glRotatef(angle.y, 0.0f, 1.0f, 0.0f);
				glMultMatrixd(m_daAxesMVMatrix);
				glGetDoublev(GL_MODELVIEW_MATRIX, m_daAxesMVMatrix);
			glPopMatrix();

			glTranslatef(0.0f, 0.0f, m_fRadius);
		    glRotatef(angle.x, 1.0f, 0.0f, 0.0f);
			glRotatef(angle.y, 0.0f, 1.0f, 0.0f);
			glTranslatef(0.0f, 0.0f, -m_fRadius);

		}
		else if( (nFlags & MK_LBUTTON) && m_status == FMV_TRANSLATE) {

			coord.x = point.x - m_ptPrev.x;
			coord.y = m_ptPrev.y - point.y;
		    glTranslatef(coord.x * m_fTranslParam, coord.y * m_fTranslParam, 0.0f);
		}
		else if( (nFlags & MK_LBUTTON) && m_status == FMV_ZOOM) {

			//zoomRadius = (point.y - m_ptPrev.y) / FMV_DEPTH_SPEED;
			dist1 = (m_ptPrev.y - m_ptZoom.y); 
			dist2 = (point.y - m_ptZoom.y); 

			if( dist1 != 0.0 && dist2 != 0.0 ) {

				// quadratic zoom
				zoomRadius1 = ( dist1 / fabs(dist1) ) * ( m_fZoomCParam + m_fZoomBParam * dist1 * dist1 );
				zoomRadius2 = ( dist2 / fabs(dist2) ) * ( m_fZoomCParam + m_fZoomBParam * dist2 * dist2 );
				zoomRadius = zoomRadius2 - zoomRadius1;
				glTranslatef(0.0f, 0.0f, zoomRadius);
				m_fRadius += zoomRadius;

				// this is required for zooming: a z-translation is not visible in 2D,
				// so in that case the viewing volume must be adjusted
				if(	m_cViewProjection == FMV_ORTHOGRAPHIC) {
								
					UpdateCamera(m_clsScreenParams);
				}
			}
		}

		// update model modelview matrix
		glMultMatrixd(m_daMVMatrix);
		glGetDoublev(GL_MODELVIEW_MATRIX, m_daMVMatrix);

		ReleaseWGL();

		m_ptPrev = point;

		// could use any of the three; but:
		// - invalidate, with FALSE for background erasing (OpenGL takes care
		//   of that) is enough fast and seem not to have 'memory' of intermediate
		//   states with the result of model still beign moved when the cursor
		//   is stopped
		// - RedrawWindow with those flags should bypass the message chain and
		//   send the WM_PAINT directly to the window; the RDW_NOINTERNALPAINT flag,
		//   moreover, should suppress any pending WM_PAINT messages (no memory effect)
		// - UpdateAllViews forces all views will be repainted => takes more time 
		Invalidate(FALSE);
		//RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_NOINTERNALPAINT );
		//pDoc->UpdateAllViews(this, 0L, NULL);
	}
}

// modify modelview matrix
void CFastModelView::SetModelViewMatrix() 
{
	// update model modelview matrix
	glLoadIdentity();
	glMultMatrixd(m_daMVMatrix);
}

void CFastModelView::SetStatus() 
{
	if( (m_uiKeyPressed & FMV_KEY_SHIFT) && (m_uiKeyPressed & FMV_KEY_CONTROL) ) {
		m_status = FMV_ROTATE_NEAR;
	}
	else
	if (m_uiKeyPressed & FMV_KEY_SHIFT) {
		m_status = FMV_ROTATE_FAR;
	}
	else if (m_uiKeyPressed & FMV_KEY_CONTROL) {
		m_status = FMV_ZOOM;
	}
	else {
		m_status = FMV_TRANSLATE;
	}

}

BOOL CFastModelView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// this is another way to change the cursor: the class cursor is not 
	// permanently changed, simply the cursor shape is set to the desired one
	// any time the cursor is going to be re-painted

	HCURSOR mycursor;

	// if the pointer is in 'this' window, change its shape;
	// otherwise, let other windows care
	if(pWnd == this) {
		if(m_status == FMV_ROTATE_NEAR) {
			mycursor = AfxGetApp()->LoadCursor(IDC_NEAR_ROTATE);
		}
		else if(m_status == FMV_ZOOM) {
			mycursor = AfxGetApp()->LoadCursor(IDC_ZOOM);
		}
		else if(m_status == FMV_ROTATE_FAR) {
			mycursor = AfxGetApp()->LoadCursor(IDC_FAR_ROTATE);
		}
		else
			mycursor = ::LoadCursor(NULL,IDC_CROSS);
		
		::SetCursor(mycursor);
		
		return(1);
	}
	
	return(0);

	//return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CFastModelView::OnViewDisplayWireframe() 
{
	m_iDrawMode = FMV_WIREFRAME;

	UpdateModel();

	// update model window
	Invalidate(FALSE);
}

void CFastModelView::OnUpdateViewDisplayWireframe(CCmdUI* pCmdUI) 
{
	( m_iDrawMode == FMV_WIREFRAME ) ? pCmdUI->SetRadio(TRUE) : pCmdUI->SetRadio(FALSE);
	
}

void CFastModelView::OnViewDisplayHiddenlines() 
{
	m_iDrawMode = FMV_HIDDENLINES;

	UpdateModel();

	// update model window
	Invalidate(FALSE);
}

void CFastModelView::OnUpdateViewDisplayHiddenlines(CCmdUI* pCmdUI) 
{
	( m_iDrawMode == FMV_HIDDENLINES ) ? pCmdUI->SetRadio(TRUE) : pCmdUI->SetRadio(FALSE);
	
}

void CFastModelView::OnViewDisplaySolid() 
{
	m_iDrawMode = FMV_SOLID;

	UpdateModel();

	// update model window
	Invalidate(FALSE);	
}

void CFastModelView::OnUpdateViewDisplaySolid(CCmdUI* pCmdUI) 
{
	( m_iDrawMode == FMV_SOLID ) ? pCmdUI->SetRadio(TRUE) : pCmdUI->SetRadio(FALSE);
	
}

void CFastModelView::UpdateModel()
{
	BOOL isWglCurrent = MakeCurrentWGL(m_pDC->GetSafeHdc(), m_hrc);
	ASSERT( isWglCurrent );

	CreateModelDisplayList(m_clsScreenParams);

	// light
	UpdateLighting(m_clsScreenParams);

	ReleaseWGL();
}

void CFastModelView::UpdateLighting(CGLParams params) 
{
	// set light parameters
	GLfloat lmodel_ambient[] = { params.m_clsAmbientLight.red, params.m_clsAmbientLight.green,
								params.m_clsAmbientLight.blue, params.m_clsAmbientLight.alpha };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	if(m_iDrawMode == FMV_SOLID) {
		// light
		//

		// if there is no charge density information, can use the lighting model
		// (with shadows). Otherwise, if loading a charge density information,
		// must disable lighting, so we can see the real densitiy information
		if(m_bInputHasCharge == false) {
			glEnable(GL_LIGHTING);
			glEnable(GL_COLOR_MATERIAL);
		}
		else {
			glDisable(GL_LIGHTING);
			glDisable(GL_COLOR_MATERIAL);
		}
	}
	else {
		// disable light
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);
	}
}

void CFastModelView::OnViewDisplay2D() 
{
	m_cViewProjection = FMV_ORTHOGRAPHIC;

	OnSize(SIZE_RESTORED, m_clsScreenParams.m_GLuiViewportCx, m_clsScreenParams.m_GLuiViewportCy);
}

void CFastModelView::OnUpdateViewDisplay2D(CCmdUI* pCmdUI) 
{
	( m_cViewProjection == FMV_ORTHOGRAPHIC ) ? pCmdUI->SetRadio(TRUE) : pCmdUI->SetRadio(FALSE);
	
}

void CFastModelView::OnViewAutosetXY() 
{
	BOOL isWglCurrent;

	isWglCurrent = MakeCurrentWGL(m_pDC->GetSafeHdc(), m_hrc);
	ASSERT( isWglCurrent );

	glLoadIdentity();
	glGetDoublev(GL_MODELVIEW_MATRIX, m_daAxesMVMatrix);

	glTranslatef(0.0f, 0.0f, m_fRadius);
	glGetDoublev(GL_MODELVIEW_MATRIX, m_daMVMatrix);

	ReleaseWGL();

	// update model window
	Invalidate(FALSE);
}

void CFastModelView::OnViewAutosetYZ() 
{
	BOOL isWglCurrent;

	isWglCurrent = MakeCurrentWGL(m_pDC->GetSafeHdc(), m_hrc);
	ASSERT( isWglCurrent );

	glLoadIdentity();

	glRotatef(90.0, 0.0, 1.0, 0.0); 
	glGetDoublev(GL_MODELVIEW_MATRIX, m_daAxesMVMatrix);

	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, m_fRadius);
	glRotatef(90.0, 0.0, 1.0, 0.0); 
	glGetDoublev(GL_MODELVIEW_MATRIX, m_daMVMatrix);

	ReleaseWGL();

	// update model window
	Invalidate(FALSE);
}

void CFastModelView::OnViewAutosetZX() 
{
	BOOL isWglCurrent;

	isWglCurrent = MakeCurrentWGL(m_pDC->GetSafeHdc(), m_hrc);
	ASSERT( isWglCurrent );

	glLoadIdentity();
	glRotatef(90.0, 1.0, 0.0, 0.0); 
	glGetDoublev(GL_MODELVIEW_MATRIX, m_daAxesMVMatrix);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, m_fRadius);
	glRotatef(90.0, 1.0, 0.0, 0.0); 
	glGetDoublev(GL_MODELVIEW_MATRIX, m_daMVMatrix);

	ReleaseWGL();

	// update model window
	Invalidate(FALSE);
}

void CFastModelView::OnViewDisplay3D() 
{
	m_cViewProjection = FMV_PERSPECTIVE;

	OnSize(SIZE_RESTORED, m_clsScreenParams.m_GLuiViewportCx, m_clsScreenParams.m_GLuiViewportCy);
}

void CFastModelView::OnUpdateViewDisplay3D(CCmdUI* pCmdUI) 
{
	( m_cViewProjection == FMV_PERSPECTIVE ) ? pCmdUI->SetRadio(TRUE) : pCmdUI->SetRadio(FALSE);
	
}

void CFastModelView::OnContextMenu(CWnd*, CPoint point)
{

	// CG: This block was added by the Pop-up Menu component
	{
		if (point.x == -1 && point.y == -1){
			//keystroke invocation
			CRect rect;
			GetClientRect(rect);
			ClientToScreen(rect);

			point = rect.TopLeft();
			point.Offset(5, 5);
		}

		CMenu menu;
		VERIFY(menu.LoadMenu(CG_IDR_POPUP_FAST_MODEL_VIEW));

		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);
		CWnd* pWndPopupOwner = this;

		while (pWndPopupOwner->GetStyle() & WS_CHILD)
			pWndPopupOwner = pWndPopupOwner->GetParent();

		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,
			pWndPopupOwner);
	}
}

void CFastModelView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{

	if( nChar == VK_SHIFT) {
		m_uiKeyPressed |= FMV_KEY_SHIFT;
	}
	else if( nChar == VK_CONTROL) {
		m_uiKeyPressed |= FMV_KEY_CONTROL;
	}

	SetStatus();

	// change cursor shape (otherwise it will be changed
	// only when cursor movers)
	OnSetCursor(this, NULL, NULL);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CFastModelView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{

	if( nChar == VK_SHIFT) {
		m_uiKeyPressed &= ~FMV_KEY_SHIFT;
	}
	else if( nChar == VK_CONTROL) {
		m_uiKeyPressed &= ~FMV_KEY_CONTROL;
	}

	SetStatus();

	// change cursor shape (otherwise it will be changed
	// only when cursor movers)
	OnSetCursor(this, NULL, NULL);

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

// Called when view colors are changed
// (i.e. by the user via the preferences)
// Must update the display list of the model, otherwise
// it doesn't change color 
void CFastModelView::OnColorsUpdate()
{
	UpdateModel();
}

/////////////////////////////////////////////////////////////////////////////
// CFastModelView auxiliary function

void CFastModelView::RotateFar(float angle, float x, float y, float z) 
{
	// TBC warning: should be made multi-thread safe: remember that this routine
	// could be called by CINT while maybe a redraw by fastmodel is going on
	// Maybe a mutex could be useful?

	BOOL isWglCurrent;

	isWglCurrent = MakeCurrentWGL(m_pDC->GetSafeHdc(), m_hrc);
	ASSERT( isWglCurrent );

	glLoadIdentity();

    glRotatef(angle, x, y, z);

	// TBC warning should rotate axes
	glMultMatrixd(m_daMVMatrix);
	glGetDoublev(GL_MODELVIEW_MATRIX, m_daMVMatrix);

	ReleaseWGL();

	// see OnMouseMove 
	Invalidate(FALSE);
}

void CFastModelView::RotateNear(float angle, float x, float y, float z) 
{
	// TBC warning: should be made multi-thread safe: remember that this routine
	// could be called by CINT while maybe a redraw by fastmodel is going on
	// Maybe a mutex could be useful?

	BOOL isWglCurrent;

	isWglCurrent = MakeCurrentWGL(m_pDC->GetSafeHdc(), m_hrc);
	ASSERT( isWglCurrent );

	glTranslatef(0.0f, 0.0f, m_fRadius);
    glRotatef(angle, x, y, z);
	glTranslatef(0.0f, 0.0f, -m_fRadius);

	// TBC warning should rotate axes
	glMultMatrixd(m_daMVMatrix);
	glGetDoublev(GL_MODELVIEW_MATRIX, m_daMVMatrix);

	ReleaseWGL();


	// see OnMouseMove 
	Invalidate(FALSE);
}

// this printing routine uses the feedback mode to output geometry
// to an EPS file 
void CFastModelView::OnFilePrintToEps() 
{
	CString pathName;

	CFileDialog	dlgFile(FALSE, "eps", NULL, OFN_OVERWRITEPROMPT,
		"Encapsulated PostScript files (*.eps)|*.eps|All files (*.*)|*.*|");

    if(dlgFile.DoModal() == IDOK)
	// if the user really wants to export to a file..
	{	
		pathName = dlgFile.GetPathName();
		outputEPS(1, (LPCSTR) pathName);
	}

}

/////////////////////////////////////////////////////////////////////////////
// CFastModelView diagnostics

#ifdef _DEBUG
void CFastModelView::AssertValid() const
{
	CView::AssertValid();
}

void CFastModelView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFastModelDoc* CFastModelView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFastModelDoc)));
	return (CFastModelDoc*)m_pDocument;
}
#endif //_DEBUG


void CFastModelView::OnViewOriginTranslate() 
{
	// TODO: Add your command handler code here
	
}

