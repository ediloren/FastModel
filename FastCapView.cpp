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


// FastCapView.cpp : implementation file
//

#include "stdafx.h"

#include "fastmodel.h"
#include "FastCapView.h"
#include "ImportFastCap.h"
#include "ImportPatran.h"
#include "FastModelView.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFastCapView

IMPLEMENT_DYNCREATE(CFastCapView, CColorEditView)

BEGIN_MESSAGE_MAP(CFastCapView, CColorEditView)
	//{{AFX_MSG_MAP(CFastCapView)
	ON_COMMAND(ID_FILE_OPEN_FASTCAP, OnImportFastCapFile)
	ON_COMMAND(ID_FILE_SAVE_FASTCAP, OnFileSaveFastCap)
	ON_COMMAND(ID_FILE_SAVE_AS_FASTCAP, OnFileSaveAsFastCap)
	ON_COMMAND(ID_FASTCAP_UPDATE_MODEL, OnFastCapUpdateModel)
	ON_WM_CLOSE()
	ON_WM_CHAR()
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_FILE_SAVE_FASTCAP_ALL, OnFileSaveFastCapAll)
	ON_COMMAND(ID_FILE_CLOSE_FASTCAP, OnFileCloseFastCap)
	ON_COMMAND(ID_FILE_OPEN, OnImportFastCapFile)
	ON_COMMAND(ID_FILE_SAVE, OnFileSaveFastCap)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CFastCapView, CColorEditView)
	//{{AFX_DISPATCH_MAP(CFastCapView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IFastCapView to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {6D9AE487-4BB1-11C6-9276-54E814C10000}
static const IID IID_IFastCapView =
{ 0x6d9ae487, 0x4bb1, 0x11c6, { 0x92, 0x76, 0x54, 0xe8, 0x14, 0xc1, 0x0, 0x0 } };

BEGIN_INTERFACE_MAP(CFastCapView, CColorEditView)
	INTERFACE_PART(CFastCapView, IID_IFastCapView, Dispatch)
END_INTERFACE_MAP()

// cold/hot colormap
#define FCV_GRADIENT_COLORS		5
static const unsigned char gradientTable[FCV_GRADIENT_COLORS][3] = { 
	{0,255,255}, {0,0,255}, {0,0,0}, {255,0,0}, {255,255,0} };
// grey colormap
//#define FCV_GRADIENT_COLORS		2
//static const unsigned char gradientTable[FCV_GRADIENT_COLORS][3] = { 
//	{32,32,32}, {224,224,224} };

// static class variable initialization
// (must be performed at file scope)
CFastCapView *CFastCapView::m_pFastCapMain = NULL;

CFastCapView::CFastCapView()
{
	float tablelen, gradientcolors;
	float step, colorstep[3], color[3];
	unsigned int gradientIndex, base;
	int i, j;

	EnableAutomation();

	m_commentColor = RGB(150,175,80);
	m_commandColor = RGB(255,0,255);
	m_elementColor = RGB(0,0,255);
	m_errorColor = RGB(255,0,0);
	m_defaultColor = RGB(0,0,0);
	InitRTF();

	// initialize file name and status
	m_strPathName = "Unnamed";
	m_isModified = FALSE;
	m_iViewType = CEV_VIEW_FASTCAP;

	// main window
	//
	// if m_pFastCapMain not initialized, this is main window 
	if( m_pFastCapMain == NULL )
	{
		m_pFastCapMain = this;
		isMainView = TRUE;
	}
	// otherwise simply remember this is not
	else {
		isMainView = FALSE;
	}

	// as defalut, this is a FastCap view
	m_cViewType = FCV_ISFASTCAP;

	// if main view, init the color table
	if(isMainView == TRUE) {

		tablelen = FCV_COLOR_TABLE_LEN;
		gradientcolors = FCV_GRADIENT_COLORS;
		step = tablelen / (gradientcolors - 1);

		// init first value of the table
		colorTable[0][0] = gradientTable[0][0];
		colorTable[0][1] = gradientTable[0][1];
		colorTable[0][2] = gradientTable[0][2];

		for(i=0; i<FCV_COLOR_TABLE_LEN; i++) {

			gradientIndex = i/step;
			base = gradientIndex * step;

			for(j=0; j<3; j++) {
				colorstep[j] = (gradientTable[gradientIndex+1][j] - gradientTable[gradientIndex][j]) / step;
				color[j] = gradientTable[gradientIndex][j] + (i-base)*colorstep[j];
				if(color[j] > 255) {
					color[j] = 255;
				}
				else if(color[0] < 0) {
					color[j] = 0;
				}
				colorTable[i][j] = color[j];
			}
// debug
//if(colorTable[i][0] != 224 || colorTable[i][1] != 224 || colorTable[i][2] != 224) {
//i=i;
//}

		}
	}
	
}

// initialization routine for non-main FastCap views
void CFastCapView::InitFastCap()
{
	CFastModelDoc* pDoc = GetDocument();
	ASSERT(pDoc); 

	// initialize view 
	pDoc->addFastCapView(this);

}

CFastCapView::~CFastCapView()
{
	OnClose();
}

void CFastCapView::InitRTF()
{
	char color[30];

	// initialize RTF variables

	strcpy(m_sRTF, "{\\rtf1\\ansi\\ansicpg1252\\deff0\\deftab720");
	strcat(m_sRTF, "{\\fonttbl{\\f0\\fmodern Courier New;}}");

	strcat(m_sRTF, "{\\colortbl"); 
	sprintf(color, "\\red%u\\green%u\\blue%u;", RED(m_commentColor), GREEN(m_commentColor), BLUE(m_commentColor) );
	strcat(m_sRTF, color);
	sprintf(color, "\\red%u\\green%u\\blue%u;", RED(m_commandColor), GREEN(m_commandColor), BLUE(m_commandColor) );
	strcat(m_sRTF, color);
	sprintf(color, "\\red%u\\green%u\\blue%u;", RED(m_elementColor), GREEN(m_elementColor), BLUE(m_elementColor) );
	strcat(m_sRTF, color);
	sprintf(color, "\\red%u\\green%u\\blue%u;", RED(m_errorColor), GREEN(m_errorColor), BLUE(m_errorColor) );
	strcat(m_sRTF, color);
	sprintf(color, "\\red%u\\green%u\\blue%u;", RED(m_defaultColor), GREEN(m_defaultColor), BLUE(m_defaultColor) );
	strcat(m_sRTF, color);
	strcat(m_sRTF, "}");

	strcat(m_sRTF, "\\deflang1040\\pard\\ri10\\plain\\f0\\fs19 ");

	m_sRTFLine = m_sRTF + strlen(m_sRTF);
}

void CFastCapView::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CRichEditView::OnFinalRelease();
}

/////////////////////////////////////////////////////////////////////////////
// CFastCapView drawing

void CFastCapView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CFastCapView diagnostics

#ifdef _DEBUG
void CFastCapView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CFastCapView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFastCapView message handlers

void CFastCapView::OnFileCloseFastCap() 
{
	FileCloseFastCap(); 
}

bool CFastCapView::FileCloseFastCap() 
{
	// if not the main window, must call the main close routine
	if( isMainView == FALSE ) {
		return m_pFastCapMain->FileCloseFastCap();
	}

	CFastModelDoc* pDoc = GetDocument();

	// if main, go on
	
	if(pDoc->UserRemoveAllFCViews() == TRUE) {
		// must delete also the content of the main view

		// delete text in rich edit view and reset to default
		GetRichEditCtrl().SetSel(0,-1);
		GetRichEditCtrl().Clear();
		SetDefaultCharFormat();
		SetDefaultParaFormat();	

		// reset the name of the view
		m_strPathName = "Unnamed";
		// update caption
		((CChildFrame*)GetParent())->OnUpdateFrameTitle(TRUE);

		// remove model
		delete(pDoc->m_pFastCapModel);
		pDoc->m_pFastCapModel = NULL;
		// and update all document's views 
		pDoc->UpdateAllViews( NULL, 0L, NULL );

		return true;
	}
	else
		return false;

}

void CFastCapView::OnImportFastCapFile() 
{
	CImportFastCap importObj;
	FILE *fp;
	CFastModelDoc* pDoc;
	CFastModelApp *pApp;
	CString fileDlgPath;

	// if not the main window, must call the main import routine
	if( isMainView == FALSE ) {
		m_pFastCapMain->OnImportFastCapFile();
		return;
	}

	// if main, go on

	pDoc = GetDocument();
	ASSERT(pDoc); 
	pApp = (CFastModelApp *)AfxGetApp();
	ASSERT(pApp);

	// create the dialog box
	//

	CFileDialog	dlgFile(TRUE, "lst", NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
		"FastCap2 List files (*.lst)|*.lst|FastCap2 Geometry files (*.neu, *.qui, *.txt)|*.neu;*.qui;*.txt|All files (*.*)|*.*|");

	// load currend directory from the Registry
	//

	fileDlgPath = pApp->GetProfileString(_T("FileDialogs"), _T("FastCapView"));
	dlgFile.m_ofn.lpstrInitialDir = fileDlgPath;

	// then open the dialog
	//

    if(dlgFile.DoModal() == IDOK)
	// if the user really wants to import a file..
	{	
		// and if it's ok to discard changes
		if(FileCloseFastCap() == false)
			return;

		m_strPathName = dlgFile.GetPathName();
		// save in the Registry
		pApp->WriteProfileString(_T("FileDialogs"), _T("FastCapView"), m_strPathName);
		// update caption
		((CChildFrame*)GetParent())->OnUpdateFrameTitle(TRUE);

		// read file

		fp = fopen((LPCSTR)m_strPathName, "r");

		if( fp != NULL) {

			// TBC warning: when opening new doc, someone must free memory!!
			// delete(pDoc->m_pFastCapModel); but it should be only the 
			// FastCap model, not FastHenry or FastModel one (so not the
			// whole document)
			//
			pDoc->DeleteContents();
			delete(pDoc->m_pFastCapModel);
		
			pDoc->m_pFastCapModel = new C3DShell;

			if( dlgFile.GetFileExt().CompareNoCase(_T("neu")) == 0 )
				m_cViewType = FCV_ISPATRAN;
			else
				m_cViewType = FCV_ISFASTCAP;
	
			// clear charge densities information
			m_bInputHasCharge = false;

			importFastCap(CEV_COLOR_LINES | CEV_CONVERT_TO_MODEL | CEV_FILE_SOURCE | 
						CEV_GENERATE_ERRORS, pDoc->m_pFastCapModel, fp, 0.0, 0.0, 0.0);
		

			// if the input file contained charge densities information, color the panels
			if(m_bInputHasCharge == true) {
				colorPanelsFromCharges(pDoc->m_pFastCapModel);
			}

			//
			// TBC warning : modify and uncomment to create FastCap model
			//
			// scan all views to find the Model ones, because at import must 
			// reset the viewing point of every Model view to zoom all geometry
			// TCB warning: could be better to derive every view from a single
			// base class, to use a member function to overload to know which
			// view is the one retrieved without using IsKindOf
			POSITION pos = pDoc->GetFirstViewPosition();
			while (pos != NULL)   
			{
				CView* myview = pDoc->GetNextView(pos);
				// if it is a model view, zoom all
				if( myview->IsKindOf(RUNTIME_CLASS(CFastModelView)) ) {
					((CFastModelView*)myview)->m_bInputHasCharge = m_bInputHasCharge;
					((CFastModelView*)myview)->InitializeScene();
					((CFastModelView*)myview)->ZoomAll();
				}   
			}   


			// update all document's views (including this one; if first parameter
			// pSender were 'this', all document's views but this one would be 
			// updated)
			pDoc->UpdateAllViews( NULL, 0L, NULL );

			fclose(fp);
		}
		else
			MessageBox("Cannot open file!", "Error", MB_ICONSTOP);
	}
}

void CFastCapView::colorPanelsFromCharges(C3DShell *shell)
{
	C3DList<C3DShell>::iterator its;
	C3DList<C3DFace>::iterator itf;
	int index;

	// scan every subshell in shell 
	for(its = shell->shellList.begin(); its != shell->shellList.end(); its++) {
		colorPanelsFromCharges(*its);
	}

	// scan every face in shell 
	for(itf = shell->faceList.begin(); itf != shell->faceList.end(); itf++) {
		index = (int)( ( ((*itf)->density - m_fMinCharge) / (m_fMaxCharge - m_fMinCharge) ) * ((float)FCV_COLOR_TABLE_LEN-1) );
		(*itf)->rgb.red = colorTable[index][0];
		(*itf)->rgb.green = colorTable[index][1];
		(*itf)->rgb.blue = colorTable[index][2];
		(*itf)->hasColor = true;

// debug
//if(colorTable[index][0] != 224 || colorTable[index][1] != 224 || colorTable[index][2] != 224) {
//(*itf)->hasColor = true;
//}
	}	 
} 

void CFastCapView::importFastCap(unsigned int operation, C3DShell *model, FILE *fp, 
								 float xtran, float ytran, float ztran, bool hascolor, unsigned long color) 
{

	long lastpos;

	CFastModelDoc* pDoc = GetDocument();
	ASSERT(pDoc);

	// delete text in rich edit view and reset to default
	GetRichEditCtrl().SetSel(0,-1);
	GetRichEditCtrl().Clear();
	SetDefaultCharFormat();
	SetDefaultParaFormat();	

	// inhibit drawing in view while importing
	SetRedraw(FALSE);

	// add a space as last char
	// (must add a char in the end, because an RTF text ending with \par 
	// will NOT insert a newline after the text. However, two consecutive 
	// \par \par DOES add ONE newline (in the END of the edit window). 
	// In all other cases, \par adds a newline. So is possible to add a
	// dummy char in the end, and everything works as one would expect;
	// then will be possible to remove the dummy char).
	lastpos = GetTextLength();
	GetRichEditCtrl().SetSel(lastpos, lastpos);
	GetRichEditCtrl().ReplaceSel(_T(" "));
	// then must reset insert position to one char before
	// the dummy char (which is the old pos)
	GetRichEditCtrl().SetSel(lastpos, lastpos);

	importInView(operation, fp, model, 0, 0, 0, xtran, ytran, ztran, hascolor, color);

	// delete dummy char added 
	lastpos = GetTextLength();
	GetRichEditCtrl().SetSel(lastpos-1, lastpos);
	GetRichEditCtrl().ReplaceSel(_T(""));

	// start off with unmodified
	pDoc->SetModifiedFlag(FALSE);     
	m_isModified = FALSE;

	// go to beginning of text
	GetRichEditCtrl().SetSel(0,0);
	GetRichEditCtrl().ReplaceSel(_T(""));

	// now all is ready, allow drawing in view
	SetRedraw(TRUE);
}

void CFastCapView::OnFileSaveFastCapAll() 
{
	int i;

	CFastModelDoc* pDoc = GetDocument();
	ASSERT(pDoc); 

	// if not the main window, must call the main save all routine
	if( isMainView == FALSE ) {
		m_pFastCapMain->OnFileSaveFastCapAll();
		return;
	}

	// save the main one (this)
	OnFileSaveFastCap();

	// scan all FastCap files and save them all
	for( i=0; i<FMD_MAX_FC_VIEWS; i++) {
		if( pDoc->m_pFastCapView[i] != NULL ) {
			pDoc->m_pFastCapView[i]->OnFileSaveFastCap();
		}
	}
	
	// set everything as unmodified
	pDoc->m_bIsFCModified = FALSE;
	m_isModified = FALSE;
}

void CFastCapView::OnFileSaveFastCap() 
{
	if( m_strPathName == "Unnamed") {
		SaveAsFastCapFile();
	}
	else {
		SaveFastCapFile();
	}
}

void CFastCapView::OnFileSaveAsFastCap() 
{
	SaveAsFastCapFile();
}

void CFastCapView::SaveFastCapFile() 
{
	long lineno, linelen;
	char str[CEV_MAX_LINE_LEN];
	FILE *fp;

	CFastModelDoc* pDoc = GetDocument();
	ASSERT(pDoc);

	// if not the main window, the file was being watched
	if( isMainView == FALSE ) {
		// remove file from watch list (otherwise, the watch thread will think 
		// that the file has been modified, since it is saved and its time stamp changes)
		pDoc->m_clFileMonitor.removeFile((LPCTSTR)m_strPathName);
	}

	CWaitCursor wait;

	// REMARK: must open as binary for writing, otherwise the '\n' are converted
	// to '\r\n', so the correct EOL '\r\n' will become '\r\r\n' in the file!
	fp = fopen((LPCTSTR)m_strPathName, "wb");

	if( fp != NULL) {
		lineno = 0;
		// when last line, linelen == 0
		while(linelen = GetRichEditCtrl().GetLine(lineno++, str, CEV_MAX_LINE_LEN))
		{			
			if(linelen > CEV_MAX_LINE_LEN-1) {
				// TBC warning : error, line too long
			}
			str[linelen] = 0;

			fprintf(fp, "%s", str);
		}

		fclose(fp);
	}
	else {
		MessageBox("Cannot write file!", "Error", MB_ICONSTOP);
	}

	pDoc->SetModifiedFlag(FALSE);     // start off with unmodified
	m_isModified = FALSE;

	AddFileToWatchList();

} 

void CFastCapView::AddFileToWatchList() 
{
	CFastModelDoc* pDoc = GetDocument();
	ASSERT(pDoc);

	// if not the main window, can add file to watch list
	if( isMainView == FALSE ) {
		// add file name to monitor list 
		pDoc->m_clFileMonitor.addFile((LPCTSTR)m_strPathName, FMD_FASTCAP_FILE, pDoc->m_ulFileMonTokCount); 
		// and map token to FastCapView
		m_ulFileMonTok = pDoc->m_ulFileMonTokCount;
		pDoc->m_stdMonFilesMap.insert(std::make_pair(m_ulFileMonTok, (CFastCapView*)this));
		pDoc->m_ulFileMonTokCount++;
	}
}

BOOL CFastCapView::SaveAsFastCapFile() 
{
	CFileDialog	dlgFile(FALSE, "lst", m_strPathName, OFN_OVERWRITEPROMPT,
		"FastCap2 List files (*.lst)|*.lst|FastCap2 Geometry files (*.neu, *.qui, *.txt)|*.neu;*.qui;*.txt|All files (*.*)|*.*|");

    if(dlgFile.DoModal() == IDOK)
	// if the user really wants to export to a file..
	{	
		m_strPathName = dlgFile.GetPathName();
		// update caption
		((CChildFrame*)GetParent())->OnUpdateFrameTitle(TRUE);

		SaveFastCapFile();
		return TRUE;
	}

	return FALSE;
}

// overloaded function to replace virtual one in CColorEditView
int CFastCapView::ColorLines(long baselineno, long startline, long endline)
{
	int deletedChars;

	deletedChars = importInView(CEV_COLOR_LINES, NULL, NULL,
					baselineno, startline, endline, 0.0, 0.0, 0.0);

	return(deletedChars);
}

int CFastCapView::importInView(unsigned int operation, FILE *fp, C3DShell *model, 
							   long baselineno, long startline, long endline,
								float xtran, float ytran, float ztran, bool hascolor, unsigned long color)
{
	int deletedChars;
	CImportFastCap importFastCapObj;
	CImportPatran  importPatranObj;

	// show wait cursor
	((CFastModelApp*)AfxGetApp())->BeginArrowWaitCursor();
	// disable closing FastModel
	((CMainFrame*)(AfxGetApp()->m_pMainWnd))->DisableClose();

	if( m_cViewType == FCV_ISPATRAN ) {
		deletedChars = importPatranObj.import(operation,
						fp, this, model, baselineno, startline, endline,
						xtran, ytran, ztran);
	}
	else {
		deletedChars = importFastCapObj.import(operation,
						fp, this, model, baselineno, startline, endline,
						xtran, ytran, ztran, hascolor, color);
	}

	// end wait cursor
	((CFastModelApp*)AfxGetApp())->EndArrowWaitCursor();
	// re-enable closing FastModel
	((CMainFrame*)(AfxGetApp()->m_pMainWnd))->EnableClose();

	return(deletedChars);
}

// must override or will return the doc type linked with the
// richedit view (RichEditDoc)
inline CFastModelDoc* CFastCapView::GetDocument() const
{
	ASSERT(m_pDocument != NULL);
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFastModelDoc)));
	return (CFastModelDoc*)m_pDocument;
}

void CFastCapView::OnFastCapUpdateModel() 
{
	// if not the main window, must call the main update routine
	if( isMainView == FALSE ) {
		m_pFastCapMain->OnFastCapUpdateModel();
		return;
	}

	CFastModelDoc* pDoc = GetDocument();
	ASSERT(pDoc);

	// show wait cursor
	((CFastModelApp*)AfxGetApp())->BeginArrowWaitCursor();
	// disable closing FastModel
	((CMainFrame*)(AfxGetApp()->m_pMainWnd))->DisableClose();

	// delete FastModel 3D model and create a new one
	delete(pDoc->m_pFastCapModel);
	pDoc->m_pFastCapModel = new C3DShell;

	updateFastCap(pDoc->m_pFastCapModel, 0.0, 0.0, 0.0);

	// scan all views to find the Model ones, because at update must 
	// re-create the display list (but not reset the view point, like for import)
	// TCB warning: could be better to derive every view from a single
	// base class, to use a member function to overload to know which
	// view is the one retrieved without using IsKindOf
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)   
	{
		CView* myview = pDoc->GetNextView(pos);
		// if it is a model view, zoom all
		if( myview->IsKindOf(RUNTIME_CLASS(CFastModelView)) ) {
			((CFastModelView*)myview)->InitializeScene();
		}   
	}   

	// update all document's views (including this one; if first parameter
	// pSender were 'this', all document's views but this one would be 
	// updated)
	pDoc->UpdateAllViews( NULL, 0L, NULL );	

	// end wait cursor
	((CFastModelApp*)AfxGetApp())->EndArrowWaitCursor();
	// re-enable closing FastModel
	((CMainFrame*)(AfxGetApp()->m_pMainWnd))->EnableClose();

}

void CFastCapView::updateFastCap(C3DShell *model, float xtran, float ytran, float ztran, bool hascolor, unsigned long color)
{
	long lastline; 

	// get last line number
	lastline = GetRichEditCtrl().GetLineCount() - 1;

	// clear charge densities information
	m_bInputHasCharge = false;

	importInView(CEV_CONVERT_TO_MODEL | CEV_GENERATE_ERRORS, NULL, model, 
				0, 0, lastline, xtran, ytran, ztran, hascolor, color);

	// if the input file contained charge densities information, color the panels
	if(m_bInputHasCharge == true) {
		colorPanelsFromCharges(model);
	}
}

HMENU CFastCapView::GetContextMenu( WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE* lpchrg )
{
	// create menu object; warning: creating the menu object 
	// does not associate a menu item automatically
	CMenu menu;
	BOOL status;

	CWnd* pWndPopupOwner = this;
	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	// so the menu item has to be loaded into the object
	status = menu.LoadMenu(IDR_POPUP_FASTCAPTYPE);
	VERIFY(status);

	// then the first item of the menu, which is the _POPUP_ item, must be removed
	// and only the content of the submenu (the descending part) is kept
	//
	// so the submenu object is got 
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	// and the first menu item is removed
	menu.RemoveMenu(0, MF_BYPOSITION);

	// then the menu item is detached from the menu object (which will be destroyed)
	// and its handle is returned for showing
	return pPopup->Detach();
}

void CFastCapView::setRTFLine()
{
	// close RTF line
	strcat(m_sRTFLine, "}");
	// insert in view
	SetRTF(m_sRTF);
	// delete old line
	m_sRTFLine[0] = 0;
}

void CFastCapView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// inhibit typing in Patran view
	if( m_cViewType != FCV_ISPATRAN ) {
		CColorEditView::OnChar(nChar, nRepCnt, nFlags);
	}
}

void CFastCapView::OnEditPaste() 
{

	// inhibit typing in Patran view
	if( m_cViewType != FCV_ISPATRAN ) {

		CFastModelDoc* pDoc = GetDocument();
		ASSERT(pDoc); 
		// signal as modified
		pDoc->m_bIsFCModified = TRUE;
		m_isModified = TRUE;

		CColorEditView::OnEditPaste();
	}		
}



void CFastCapView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	
	// inhibit typing in Patran view
	if( m_cViewType != FCV_ISPATRAN ) {

		CFastModelDoc* pDoc = GetDocument();
		ASSERT(pDoc); 
		// signal as modified
		pDoc->m_bIsFCModified = TRUE;
		m_isModified = TRUE;

		CColorEditView::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

void CFastCapView::OnClose() 
{
	// if main window, close all FastCap subwindows
	if( isMainView == TRUE ) {

		CFastModelDoc* pDoc = GetDocument();

		pDoc->RemoveAllFCViews();

		// remove model
		delete(pDoc->m_pFastCapModel);
		pDoc->m_pFastCapModel = NULL;
	}

	CColorEditView::OnClose();
}

void CFastCapView::OnFileNew() 
{
	CFastModelDoc* pDoc = GetDocument();

	// new FastCap file creation is handled by CFastModelDoc
	pDoc->OnFileNewFastCap();
	
}
