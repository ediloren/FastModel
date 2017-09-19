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


// OpenGLView.cpp : implementation file
//

#include "stdafx.h"
#include "fastmodel.h"
#include "OpenGLView.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// default color values
#define OGV_DEF_WIRECOLOR			0.5f, 0.5f, 0.5f, 1.0f
#define OGV_DEF_BACKTOPCOLOR		0.0f, 0.0f, 1.0f, 1.0f
#define OGV_DEF_BACKBOTCOLOR		0.0f, 0.0f, 0.2f, 1.0f
#define OGV_DEF_SOLIDCOLOR			1.0f, 1.0f, 1.0f, 1.0f
#define OGV_DEF_AMBIENTLIGHTCOLOR	0.1f, 0.2f, 0.3f, 1.0f


void CGLParams::SetColors(COGLColors colors)
{
	m_clsColor = colors.m_clsWireColor;
	m_clsBackColor[0] = colors.m_clsBackTopColor;
	m_clsBackColor[1] = colors.m_clsBackBotColor;
	m_clsSolidColor = colors.m_clsSolidColor;
	m_clsAmbientLight = colors.m_clsAmbientLightColor;
}

COGLColors CGLParams::GetColors()
{
	COGLColors colors;

	colors.m_clsWireColor = m_clsColor.GetColorref();
	colors.m_clsBackTopColor = m_clsBackColor[0].GetColorref();
	colors.m_clsBackBotColor = m_clsBackColor[1].GetColorref();
	colors.m_clsSolidColor = m_clsSolidColor.GetColorref();
	colors.m_clsAmbientLightColor = m_clsAmbientLight.GetColorref();

	return colors;
}

// there follow stuff for OpenGL / window palette definition 
unsigned char threeto8[8] = 
{
	0, 0111>>1, 0222>>1, 0333>>1, 0444>>1, 0555>>1, 0666>>1, 0377
};

unsigned char twoto8[4] = 
{
	0, 0x55, 0xaa, 0xff
};

unsigned char oneto8[2] = 
{
	0, 255
};

static int defaultOverride[13] = 
{
	0, 3, 24, 27, 64, 67, 88, 173, 181, 236, 247, 164, 91
};

static PALETTEENTRY defaultPalEntry[20] = 
{
	{ 0,   0,   0,    0 },
	{ 0x80,0,   0,    0 },
	{ 0,   0x80,0,    0 },
	{ 0x80,0x80,0,    0 },
	{ 0,   0,   0x80, 0 },
	{ 0x80,0,   0x80, 0 },
	{ 0,   0x80,0x80, 0 },
	{ 0xC0,0xC0,0xC0, 0 },

	{ 192, 220, 192,  0 },
	{ 166, 202, 240,  0 },
	{ 255, 251, 240,  0 },
	{ 160, 160, 164,  0 },

	{ 0x80,0x80,0x80, 0 },
	{ 0xFF,0,   0,    0 },
	{ 0,   0xFF,0,    0 },
	{ 0xFF,0xFF,0,    0 },
	{ 0,   0,   0xFF, 0 },
	{ 0xFF,0,   0xFF, 0 },
	{ 0,   0xFF,0xFF, 0 },
	{ 0xFF,0xFF,0xFF, 0 }
};


/////////////////////////////////////////////////////////////////////////////
// CopenGlView

IMPLEMENT_DYNCREATE(COpenGlView, CView)

BEGIN_MESSAGE_MAP(COpenGlView, CView)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(COpenGlView)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_VIEW_PROPERTIES, OnViewProperties)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenGlView construction/destruction

// also constructs lock object used to access OpenGL functions
COpenGlView::COpenGlView() : m_clsGLLock(&m_clsGLMutex)
{
	m_pDC = NULL;
	m_pOldPalette = NULL;

	// initalize parameters
	m_clsScreenParams.m_clsColor.Set(OGV_DEF_WIRECOLOR);
	m_clsScreenParams.m_clsBackColor[0].Set(OGV_DEF_BACKTOPCOLOR);
	m_clsScreenParams.m_clsBackColor[1].Set(OGV_DEF_BACKBOTCOLOR);
	m_clsScreenParams.m_clsSolidColor.Set(OGV_DEF_SOLIDCOLOR);
	m_clsScreenParams.m_clsAmbientLight.Set(OGV_DEF_AMBIENTLIGHTCOLOR);
	m_clsScreenParams.m_bPrinting = false;
	m_clsScreenParams.m_bPrintingToEPS = false;
	m_clsScreenParams.m_bCanPrintToEPS = true;
}

COpenGlView::~COpenGlView()
{
}

BOOL COpenGlView::PreCreateWindow(CREATESTRUCT& cs)
{

	// these styles are requested by OpenGL
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	// TBC warning: check if is needed to register another class
	// these styles are meant for a use of this class in a MDI application
	//
	// this is also one way to change the cursor: the class cursor is permanently changed,
	// but this requires the registration of another class, which is substituted
	// in the lpszClass member of the CREATESTRUCT of this window
	//
	cs.lpszClass = AfxRegisterWndClass(CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
					AfxGetApp()->LoadStandardCursor(IDC_CROSS),
					(HBRUSH)(COLOR_WINDOW+1));
	if (cs.lpszClass == NULL)
		return FALSE;

	return CView::PreCreateWindow(cs);

}

bool COpenGlView::LoadWindowState(CString regSection)
{
	LoadStateColor(regSection, _T("WireColor"), &(m_clsScreenParams.m_clsColor));
	LoadStateColor(regSection, _T("BackTopColor"), &(m_clsScreenParams.m_clsBackColor[0]));
	LoadStateColor(regSection, _T("BackBottomColor"), &(m_clsScreenParams.m_clsBackColor[1]));
	LoadStateColor(regSection, _T("SolidColor"), &(m_clsScreenParams.m_clsSolidColor));
	LoadStateColor(regSection, _T("AmbientLightColor"), &(m_clsScreenParams.m_clsAmbientLight));

	return ((CChildFrame*)GetParentFrame())->LoadWindowState(regSection);
}

void COpenGlView::LoadStateColor(CString regSection, CString colorName, CGLColor4f *colorParam)
{
	CString colorStr;
	CGLColor4f color;
	int num;
	CWinApp *pApp;

	pApp = AfxGetApp();
	ASSERT(pApp);

	// get window color from the registry
	colorStr = pApp->GetProfileString( regSection, colorName, _T("") );

	// extract color from the string
	num = sscanf( (LPCTSTR)colorStr, "%f,%f,%f,%f", &color.red, &color.green, &color.blue, &color.alpha);

	// if correct color information, set color; otherwise leave default color
	if(num == 4)
		*colorParam = color;
}

void COpenGlView::SaveWindowState(CString regSection)
{
	SaveStateColor(regSection, _T("WireColor"), m_clsScreenParams.m_clsColor);
	SaveStateColor(regSection, _T("BackTopColor"), m_clsScreenParams.m_clsBackColor[0]);
	SaveStateColor(regSection, _T("BackBottomColor"), m_clsScreenParams.m_clsBackColor[1]);
	SaveStateColor(regSection, _T("SolidColor"), m_clsScreenParams.m_clsSolidColor);
	SaveStateColor(regSection, _T("AmbientLightColor"), m_clsScreenParams.m_clsAmbientLight);

	((CChildFrame*)GetParentFrame())->SaveWindowState(regSection);
}

void COpenGlView::SaveStateColor(CString regSection, CString colorName, CGLColor4f colorParam)
{
	CString colorStr;
	CWinApp *pApp;

	pApp = AfxGetApp();
	ASSERT(pApp);

	// build color string
	colorStr.Format("%f,%f,%f,%f", colorParam.red, colorParam.green, colorParam.blue, colorParam.alpha);

	// write window position in the registry
	pApp->WriteProfileString( regSection, colorName, colorStr );
}

// multi-thread safe access to OpenGL functions
BOOL COpenGlView::MakeCurrentWGL(HDC  hdc, HGLRC  hglrc)
{
	m_clsGLLock.Lock();

	return(wglMakeCurrent(hdc, hglrc));
}

// multi-thread safe access to OpenGL functions
BOOL COpenGlView::ReleaseWGL()
{
	BOOL sts;
	
	sts = wglMakeCurrent(NULL, NULL);

	m_clsGLLock.Unlock();

	return sts;
}

int COpenGlView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	Init();	// initialize OpenGL
	
	return 0;
}

void COpenGlView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
}

void COpenGlView::OnDestroy()
{

	// OpenGL destroy activities

	HGLRC	hrc;

	hrc = ::wglGetCurrentContext();

	// make sure the context is released
    ::wglMakeCurrent(NULL,  NULL);

    if (hrc)
        ::wglDeleteContext(hrc);

    if (m_pOldPalette)
        m_pDC->SelectPalette(m_pOldPalette, FALSE);

    if (m_pDC)
        delete m_pDC;

   CView::OnDestroy();
}

void COpenGlView::Init()
{

    PIXELFORMATDESCRIPTOR pfd;
    int         n;
	BOOL isWglCurrent;

    m_pDC = new CClientDC(this);

    ASSERT(m_pDC != NULL);

	// flags mean support window, support OpenGL, have front and back buffers
    if (!bSetupPixelFormat(m_pDC->GetSafeHdc(), PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER)) 
        return;
	
    n = ::GetPixelFormat(m_pDC->GetSafeHdc());
    ::DescribePixelFormat(m_pDC->GetSafeHdc(), n, sizeof(pfd), &pfd);

    CreateRGBPalette();

    m_hrc = wglCreateContext(m_pDC->GetSafeHdc());

	isWglCurrent = MakeCurrentWGL(m_pDC->GetSafeHdc(), m_hrc);
	ASSERT(isWglCurrent);

	InitOpenGL(m_clsScreenParams);

	ReleaseWGL();  

	// initialize the scene
	InitializeScene();
	ZoomAll();
}

void COpenGlView::InitOpenGL(CGLParams &params)
{
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST); 

	// display list for model visualization
	params.m_GLuiModelDispList = glGenLists(1);
	if (params.m_GLuiModelDispList == 0) {
		// TBC warning: throw warning
		AfxMessageBox("Internal error: cannot assign display list", MB_ICONSTOP);
	}

	// display list for axes visualization
	params.m_GLuiAxesDispList = glGenLists(1);
	if (params.m_GLuiAxesDispList == 0) {
		// TBC warning: throw warning
		AfxMessageBox("Internal error: cannot assign display list", MB_ICONSTOP);
	}
	InitAxesDisplayList(params);

	// This flag enables lighting of both faces of model; the effect
	// is achieved by inverting the normal when the face is viewed
	// on the opposite side. Remark: this is true also for faces 
	// whose normal is opposite w.r.t. OpenGL face orientation (CCW),
	// therefore if normal is inverted when face is viewed on opposite
	// side, the normal points always in the wrong direction -> the 
	// face is never lighted 
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_LIGHT0);
}

void COpenGlView::InitAxesDisplayList(CGLParams params)
{
	glNewList(params.m_GLuiAxesDispList, GL_COMPILE);

	glBegin(GL_LINES);
		// light red x axis arrow
		glColor3f(1.f,0.5f,.5f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(1.0f,0.0f,0.0f);
		// x letter & arrowhead
		glVertex3f(1.1f,0.1f,0.0f);
		glVertex3f(1.3f,-0.1f,0.0f);
		glVertex3f(1.3f,0.1f,0.0f);
		glVertex3f(1.1f,-0.1f,0.0f);
		glVertex3f(1.0f,0.0f,0.0f);
		glVertex3f(0.9f,0.1f,0.0f);
		glVertex3f(1.0f,0.0f,0.0f);
		glVertex3f(0.9f,-0.1f,0.0f);

		// light green y axis arrow
		glColor3f(.5f,1.f,0.5f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		// y letter & arrowhead
		glVertex3f(-0.1f,1.3f,0.0f);
		glVertex3f(0.f,1.2f,0.0f);
		glVertex3f(0.1f,1.3f,0.0f);
		glVertex3f(0.f,1.2f,0.0f);
		glVertex3f(0.f,1.2f,0.0f);
		glVertex3f(0.f,1.1f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(0.1f,0.9f,0.0f);
		glVertex3f(0.0f,1.0f,0.0f);
		glVertex3f(-0.1f,0.9f,0.0f);

		// light blue z axis arrow
		glColor3f(.5f,.5f,1.f);
		glVertex3f(0.0f,0.0f,0.0f);
		glVertex3f(0.0f,0.0f,1.0f);
		// z letter & arrowhead
		glVertex3f(0.0f,-0.1f,1.3f);
		glVertex3f(0.0f,0.1f,1.3f);
		glVertex3f(0.0f,0.1f,1.3f);
		glVertex3f(0.0f,-0.1f,1.1f);
		glVertex3f(0.0f,-0.1f,1.1f);
		glVertex3f(0.0f,0.1f,1.1f);
		glVertex3f(0.0f,0.0f,1.0f);
		glVertex3f(0.0f,0.1f,0.9f);
		glVertex3f(0.0f,0.0f,1.0f);
		glVertex3f(0.0f,-0.1f,0.9f);
	glEnd();

	glEndList();
}

BOOL COpenGlView::bSetupPixelFormat(HDC hDC, DWORD dwFlags)
{
	PIXELFORMATDESCRIPTOR pixelDesc;
	
	pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pixelDesc.nVersion = 1;
	
	pixelDesc.dwFlags = dwFlags;
	pixelDesc.iPixelType = PFD_TYPE_RGBA;
	pixelDesc.cColorBits = 24;
	pixelDesc.cRedBits = 8;
	pixelDesc.cRedShift = 16;
	pixelDesc.cGreenBits = 8;
	pixelDesc.cGreenShift = 8;
	pixelDesc.cBlueBits = 8;
	pixelDesc.cBlueShift = 0;
	pixelDesc.cAlphaBits = 0;
	pixelDesc.cAlphaShift = 0;
	pixelDesc.cAccumBits = 64;
	pixelDesc.cAccumRedBits = 16;
	pixelDesc.cAccumGreenBits = 16;
	pixelDesc.cAccumBlueBits = 16;
	pixelDesc.cAccumAlphaBits = 0;
	pixelDesc.cDepthBits = 32;
	pixelDesc.cStencilBits = 8;
	pixelDesc.cAuxBuffers = 0;
	pixelDesc.iLayerType = PFD_MAIN_PLANE;
	pixelDesc.bReserved = 0;
	pixelDesc.dwLayerMask = 0;
	pixelDesc.dwVisibleMask = 0;
	pixelDesc.dwDamageMask = 0;
	
	int nPixelIndex = ::ChoosePixelFormat(hDC, &pixelDesc);
	if (nPixelIndex == 0) // Choose default
	{
		nPixelIndex = 1;
		if (::DescribePixelFormat(hDC, nPixelIndex, 
			sizeof(PIXELFORMATDESCRIPTOR), &pixelDesc) == 0)
			return FALSE;
	}

	if (!::SetPixelFormat(hDC, nPixelIndex, &pixelDesc)) {
		MessageBox("SetPixelFormat failed");
        return FALSE;
	}

	return TRUE;
}

unsigned char COpenGlView::ComponentFromIndex(int i, UINT nbits, UINT shift)
{
    unsigned char val;

    val = (unsigned char) (i >> shift);
    switch (nbits) 
	{

    case 1:
        val &= 0x1;
        return oneto8[val];
    case 2:
        val &= 0x3;
        return twoto8[val];
    case 3:
        val &= 0x7;
        return threeto8[val];

    default:
        return 0;
    }
}

void COpenGlView::CreateRGBPalette()
{

    PIXELFORMATDESCRIPTOR pfd;
    LOGPALETTE *pPal;
    int n, i;
 
    n = ::GetPixelFormat(m_pDC->GetSafeHdc());
    ::DescribePixelFormat(m_pDC->GetSafeHdc(), n, sizeof(pfd), &pfd);

    if (pfd.dwFlags & PFD_NEED_PALETTE)
    {
        n = 1 << pfd.cColorBits;
        pPal = (PLOGPALETTE) new char[sizeof(LOGPALETTE) + n * sizeof(PALETTEENTRY)];

        ASSERT(pPal != NULL);

        pPal->palVersion = 0x300;
        pPal->palNumEntries = n;
        for (i=0; i<n; i++)
        {
            pPal->palPalEntry[i].peRed =
                    ComponentFromIndex(i, pfd.cRedBits, pfd.cRedShift);
            pPal->palPalEntry[i].peGreen =
                    ComponentFromIndex(i, pfd.cGreenBits, pfd.cGreenShift);
            pPal->palPalEntry[i].peBlue =
                    ComponentFromIndex(i, pfd.cBlueBits, pfd.cBlueShift);
            pPal->palPalEntry[i].peFlags = 0;
        }

        //fix up the palette to include the default GDI palette 
        if ((pfd.cColorBits == 8)                           &&
            (pfd.cRedBits   == 3) && (pfd.cRedShift   == 0) &&
            (pfd.cGreenBits == 3) && (pfd.cGreenShift == 3) &&
            (pfd.cBlueBits  == 2) && (pfd.cBlueShift  == 6)
           )
        {
			for (i = 1 ; i <= 12 ; i++)
                pPal->palPalEntry[defaultOverride[i]] = defaultPalEntry[i];
        }

        m_cPalette.CreatePalette(pPal);
        delete pPal;

        m_pOldPalette = m_pDC->SelectPalette(&m_cPalette, FALSE);
        m_pDC->RealizePalette();
    }

}

/////////////////////////////////////////////////////////////////////////////
// COpenGlView drawing

void COpenGlView::OnDraw(CDC* pDC)
{
	BOOL isWglCurrent = MakeCurrentWGL(m_pDC->GetSafeHdc(), m_hrc);
	ASSERT( isWglCurrent );

	RenderScene(m_clsScreenParams);	
	
	// swap buffers to show new image, if required
	// (this is not required when printing: no doule buffer to swap)
	SwapBuffers(m_pDC->GetSafeHdc());

	ReleaseWGL();
}

/////////////////////////////////////////////////////////////////////////////
// COpenGlView message handlers

void COpenGlView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// update model window
	Invalidate(FALSE);
}

void COpenGlView::OnSize(UINT nType, int cx, int cy)
{

	BOOL isWglCurrent;

	CView::OnSize(nType, cx, cy);

	// when called with a nonzero window:
	if ( 0 < cx && 0 < cy )
	{

		// update the rect and the aspect ratio

		// specify the target DeviceContext of the subsequent OGL calls
		isWglCurrent = MakeCurrentWGL(m_pDC->GetSafeHdc(), m_hrc);
		ASSERT(isWglCurrent);

		// set correspondence between window and OGL viewport
		glViewport(0,0,cx,cy);
		// and store values, needed to know which is the visible area
		// (used by functions like ZoomAll )
		m_clsScreenParams.m_GLuiViewportCx = cx;
		m_clsScreenParams.m_GLuiViewportCy = cy;

		// update the camera
		UpdateCamera(m_clsScreenParams);

		// free the target DeviceContext (window)
		ReleaseWGL();

		// force redraw
		Invalidate(TRUE);
	}

}

void COpenGlView::OnViewProperties() 
{
	COGLViewProps propsDialog;
	COGLColors colors;

	// init dialog colors
	colors = m_clsScreenParams.GetColors();
	propsDialog.InitColors(colors);

	// show dialog
	if( propsDialog.DoModal() == IDOK) {

		// retrieve colors
		colors = propsDialog.GetColors();
		m_clsScreenParams.SetColors(colors);
		
		// call virtual notification function
		OnColorsUpdate();
		
		// force redraw
		Invalidate(TRUE);
	}
}

void COpenGlView::ClearScreen(CGLParams params)
{
	// if top and bottom colors are equal, no gradient background 
	// must be drawn
	if( params.m_clsBackColor[0] == params.m_clsBackColor[1]) {
		glClearColor(params.m_clsBackColor[0].red, params.m_clsBackColor[0].green,
					params.m_clsBackColor[0].blue, params.m_clsBackColor[0].alpha);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	// otherwise draw a gradient background
	else {
		
		// save on the stack all those parameters which will be modified
		//glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
		glPushAttrib(GL_ALL_ATTRIB_BITS); 

		// disable all performance-reducing options
		// which are not needed to paint a background
		glDisable(GL_LIGHTING);	
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_COLOR_MATERIAL);
		
		// set projection mode to orthogonal
		
		glMatrixMode(GL_PROJECTION);
		glPushMatrix(); 
		glLoadIdentity();
		// set orthographic matrix to screen dimensions
		glOrtho(0.0f, params.m_GLuiViewportCx, params.m_GLuiViewportCy, 0.0f, -1.0f, 1.0f);

		// draw a gradient fill background, using Gourad shading

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix(); 
			glLoadIdentity();

			glBegin(GL_QUADS);
				glColor4f(params.m_clsBackColor[0].red, params.m_clsBackColor[0].green,
						params.m_clsBackColor[0].blue, params.m_clsBackColor[0].alpha);
				glVertex3i(0, 0, 0);
			
				glColor4f(params.m_clsBackColor[0].red, params.m_clsBackColor[0].green,
						params.m_clsBackColor[0].blue, params.m_clsBackColor[0].alpha);
				glVertex3i(params.m_GLuiViewportCx, 0, 0);
		
				glColor4f(params.m_clsBackColor[1].red, params.m_clsBackColor[1].green,
						params.m_clsBackColor[1].blue, params.m_clsBackColor[1].alpha);
				glVertex3i(params.m_GLuiViewportCx, params.m_GLuiViewportCy, 0);
			
				glColor4f(params.m_clsBackColor[1].red, params.m_clsBackColor[1].green,
						params.m_clsBackColor[1].blue, params.m_clsBackColor[1].alpha);
				glVertex3i(0, params.m_GLuiViewportCy, 0);
			glEnd();

		glPopMatrix();
		
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		// restore all OpenGL attributes saved on the stack
		glPopAttrib();

		// only the z-buffer is left to be cleared
        glClear(GL_DEPTH_BUFFER_BIT);
	}
}

BOOL COpenGlView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

// virtual function
void COpenGlView::OnColorsUpdate()
{

}

// virtual function
void COpenGlView::UpdateCamera(CGLParams params)
{
}

// virtual function
void COpenGlView::RenderScene(CGLParams params)
{
}

// virtual function
void COpenGlView::ZoomAll()
{
}

// virtual function
void COpenGlView::InitializeScene()
{
}

/////////////////////////////////////////////////////////////////////////////
// COpenGlView diagnostics

#ifdef _DEBUG
void COpenGlView::AssertValid() const
{
	CView::AssertValid();
}

void COpenGlView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}


#endif //_DEBUG

