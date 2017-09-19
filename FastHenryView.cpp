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


// FastHenryView.cpp : implementation file
//

#include "stdafx.h"
#include "fastmodel.h"
#include "FastHenryView.h"
#include "FastModelView.h"
#include "ChildFrm.h"

// include for importing FastHenry data
#include "ImportFastHenry.h"
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFastHenryView

IMPLEMENT_DYNCREATE(CFastHenryView, CRichEditView)

BEGIN_MESSAGE_MAP(CFastHenryView, CColorEditView)
	//{{AFX_MSG_MAP(CFastHenryView)
	ON_COMMAND(ID_FILE_OPEN_FASTHENRY, OnImportFastHenryFile)
	ON_COMMAND(ID_FILE_SAVE_FASTHENRY, OnFileSaveFasthenry)
	ON_COMMAND(ID_FASTHENRY_UPDATE_MODEL, OnFastHenryUpdateModel)
	ON_COMMAND(ID_FILE_SAVE_AS_FASTHENRY, OnFileSaveAsFasthenry)
	ON_WM_CLOSE()
	ON_COMMAND(ID_FILE_CLOSE_FASTHENRY, OnFileCloseFastHenry)
	ON_COMMAND(ID_FILE_OPEN, OnImportFastHenryFile)
	ON_COMMAND(ID_FILE_SAVE, OnFileSaveFasthenry)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CFastHenryView, CColorEditView)
	//{{AFX_DISPATCH_MAP(CFastHenryView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IFastHenryView to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {6D9AE485-4BB1-11C6-9276-54E814C10000}
static const IID IID_IFastHenryView =
{ 0x6d9ae485, 0x4bb1, 0x11c6, { 0x92, 0x76, 0x54, 0xe8, 0x14, 0xc1, 0x0, 0x0 } };

BEGIN_INTERFACE_MAP(CFastHenryView, CColorEditView)
	INTERFACE_PART(CFastHenryView, IID_IFastHenryView, Dispatch)
END_INTERFACE_MAP()

// static class variable initialization
// (must be performed at file scope)
CFastHenryView *CFastHenryView::m_pFastHenryMain = NULL;

CFastHenryView::CFastHenryView()
{
	EnableAutomation();

	m_commentColor = RGB(150,175,80);
	m_dotColor = RGB(255,0,255);
	m_elementColor = RGB(0,0,255);
	m_errorColor = RGB(255,0,0);
	m_defaultColor = RGB(0,0,0);
	InitRTF();

	// initialize file name and status
	m_strPathName = "Unnamed";
	m_isModified = FALSE;
	m_iViewType = CEV_VIEW_FASTHENRY;

	// main window
	//
	// if m_pFastHenryMain not initialized, this is main window 
	if( m_pFastHenryMain == NULL )
	{
		m_pFastHenryMain = this;
		isMainView = TRUE;
	}
	// otherwise simply remember this is not
	else {
		isMainView = FALSE;
	}
}

void CFastHenryView::InitRTF()
{
	char color[30];

	// initialize RTF variables

	strcpy(m_sRTF, "{\\rtf1\\ansi\\ansicpg1252\\deff0\\deftab720");
	strcat(m_sRTF, "{\\fonttbl{\\f0\\fmodern Courier New;}}");

	strcat(m_sRTF, "{\\colortbl"); 
	sprintf(color, "\\red%u\\green%u\\blue%u;", RED(m_commentColor), GREEN(m_commentColor), BLUE(m_commentColor) );
	strcat(m_sRTF, color);
	sprintf(color, "\\red%u\\green%u\\blue%u;", RED(m_dotColor), GREEN(m_dotColor), BLUE(m_dotColor) );
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

CFastHenryView::~CFastHenryView()
{
	OnClose();
}

void CFastHenryView::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CRichEditView::OnFinalRelease();
}


/////////////////////////////////////////////////////////////////////////////
// CFastHenryView drawing

void CFastHenryView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CFastHenryView diagnostics

#ifdef _DEBUG
void CFastHenryView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CFastHenryView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFastHenryView overrides


// this function is overridden to support continuing lines coloring when directly typing
// into view (looks below and above current block of text until no more continuing
// lines are found, then colors all the new, super block)
void CFastHenryView::ColorAndReplace(long baselineno, long startline, long endline)
{

	char str[CEV_MAX_LINE_LEN];
	long startlinenum, endlinenum, lastline;
	int numchars;
	BOOL cont;

	// verify whether startlines points to a line that starts with '+' (this means
	// that the line is continuing from the previous one) or if it is blank 
	// (all spaces, tabs and newlines); in the latter case, should check 
	// downwards to see if next line starts with '+' and so on.

	// positions on first non-blank line from the top (if any)
	startlinenum = startline - 1;
	do {
		startlinenum++;
		// remark: GetLine() does NOT terminate line with NULL char
		numchars = GetRichEditCtrl().GetLine(startlinenum, str, CEV_MAX_LINE_LEN - 3);
		str[numchars] = '\0';
		cont = IsBlank(str);		
	} while (cont == TRUE && startlinenum < endline);
	// then check upward for continuing lines
	cont = IsContinuing(str) || IsBlank(str) || IsComment(str);
	while(cont == TRUE && startlinenum > 0) {
		startlinenum--;
		// remark: GetLine() does NOT terminate line with NULL char
		numchars = GetRichEditCtrl().GetLine(startlinenum, str, CEV_MAX_LINE_LEN - 3);
		str[numchars] = '\0';
		cont = IsContinuing(str) || IsBlank(str) || IsComment(str);
	}
	// at this point, startlinenum should be the first upward non-blank, non-continuing line
	// (otherwise, there is a syntax error in the input, e.g. a '+' on the first line)

	// check downward for continuing lines
	//
	// remark: GetLineCount() returns # of lines, but GetLine() uses a zero-index linenum, so
	// must subtract 1
	lastline = GetRichEditCtrl().GetLineCount() - 1; 
	endlinenum=endline;
	// if not already last line
	if(endlinenum < lastline) {
		do {
			endlinenum++;
			// remark: GetLine() does NOT terminate line with NULL char
			numchars = GetRichEditCtrl().GetLine(endlinenum, str, CEV_MAX_LINE_LEN - 3);
			str[numchars] = '\0';
			cont = IsContinuing(str) || IsBlank(str) || IsComment(str);
		} while (cont == TRUE && endlinenum < lastline);
	}
	// at this point, endlinenum should be the first downward non-blank, non-continuing line;
	// but this must not be counted, except in case the cycle ended because of 'last line' read
	if(endlinenum < lastline)
		endlinenum--;

	CColorEditView::ColorAndReplace(startlinenum, startlinenum, endlinenum);
}
	
BOOL CFastHenryView::IsContinuing(char *str)
{
	int i;
	char c;

	for(i=0; i<CEV_MAX_LINE_LEN; i++) {
		c = str[i];
		if( c == ' ' || c == '\t')
			continue;
		if( c == '+' )
			return TRUE;
		else
			// any other char (including '\0', '\r', '\n' !) will terminate cycle
			return FALSE;
	}

	// line too long, not ending with '\0'; but anyway a '+' is not found
	return FALSE;
}

BOOL CFastHenryView::IsBlank(char *str)
{
	int i;
	char c;

	c = str[0];
	for(i=0; i<CEV_MAX_LINE_LEN; i++) {
		c = str[i];
		if( c == ' ' || c == '\t' || c == '\r' || c == '\n')
			continue;
		else if( c == '\0' )
			return TRUE;
		else
			// any other char will terminate cycle
			return FALSE;
	}

	// line too long, not ending with '\0'; but anyway blank until now
	return TRUE;
}

BOOL CFastHenryView::IsComment(char *str)
{
	int i;
	char c;

	for(i=0; i<CEV_MAX_LINE_LEN; i++) {
		c = str[i];
		if( c == ' ' || c == '\t')
			continue;
		if( c == '*' )
			return TRUE;
		else
			// any other char (including '\0', '\r', '\n' !) will terminate cycle
			return FALSE;
	}

	// line too long, not ending with '\0'; but anyway a '*' is not found
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CFastHenryView message handlers

void CFastHenryView::OnFileCloseFastHenry() 
{
	// the doc will be lost -> ask user yes/no
	if (m_isModified)
		if( MessageBox("The current project has unsaved changes\nAre you sure you want to discard changes?", "Warning", MB_OKCANCEL|MB_ICONEXCLAMATION) == IDCANCEL )
			return;

	// delete text in rich edit view
	GetRichEditCtrl().SetSel(0,-1);
	GetRichEditCtrl().Clear();
	SetDefaultCharFormat();
	SetDefaultParaFormat();	

	// reset the name of the view
	m_strPathName = "Unnamed";
	// update caption
	((CChildFrame*)GetParent())->OnUpdateFrameTitle(TRUE);

	CFastModelDoc* pDoc = GetDocument();
	ASSERT(pDoc);

	// remove model
	delete(pDoc->m_pFastHenryModel);
	pDoc->m_pFastHenryModel = NULL;
	// and update all document's views 
	pDoc->UpdateAllViews( NULL, 0L, NULL );
	
}

void CFastHenryView::OnImportFastHenryFile() 
{

	CImportFastHenry importObj;
	FILE *fp;
	long lastpos;
	CFastModelDoc* pDoc;
	CFastModelApp *pApp;
	CString fileDlgPath;

	pDoc = GetDocument();
	ASSERT(pDoc);
	pApp = (CFastModelApp *)AfxGetApp();
	ASSERT(pApp);

	// create the dialog box
	//

	CFileDialog	dlgFile(TRUE, "inp", NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
		"FastHenry2 files (*.inp)|*.inp|All files (*.*)|*.*|");

	// load currend directory from the Registry
	//

	fileDlgPath = pApp->GetProfileString(_T("FileDialogs"), _T("FastHenryView"));
	dlgFile.m_ofn.lpstrInitialDir = fileDlgPath;

	// then open the dialog
	//

    if(dlgFile.DoModal() == IDOK)
	// if the user really wants to import a file..
	{	
		
		// the doc will be lost on import -> ask user yes/no
		if (m_isModified)
			if( MessageBox("The current project has unsaved changes\nAre you sure you want to discard changes?", "Warning", MB_OKCANCEL|MB_ICONEXCLAMATION) == IDCANCEL )
				return;

		m_strPathName = dlgFile.GetPathName();
		// save in the Registry
		pApp->WriteProfileString(_T("FileDialogs"), _T("FastHenryView"), m_strPathName);
		// update caption
		((CChildFrame*)GetParent())->OnUpdateFrameTitle(TRUE);

		// read file

		fp = fopen((LPCSTR)m_strPathName, "r");

		if( fp != NULL) {

			// TBC warning: when opening new doc, someone must free memory!!
			// delete(pDoc->m_pFastHenryModel); but it should be only the 
			// FastHenry model, not FastCap or FastModel one (so not the
			// whole document)
			//
			// this func does nothing: must be overridden
			pDoc->DeleteContents();
			delete(pDoc->m_pFastHenryModel);

			// delete text in rich edit view
			GetRichEditCtrl().SetSel(0,-1);
			GetRichEditCtrl().Clear();
			SetDefaultCharFormat();
			SetDefaultParaFormat();	

			pDoc->SetModifiedFlag();  // dirty during de-serialize
	
			// show wait cursor
			((CFastModelApp*)AfxGetApp())->BeginArrowWaitCursor();
			// disable closing FastModel
			((CMainFrame*)(AfxGetApp()->m_pMainWnd))->DisableClose();

			pDoc->m_pFastHenryModel = new C3DShell;
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

			importObj.import(IICV_COLOR_LINES | IICV_CONVERT_TO_MODEL	|
						IICV_FILE_SOURCE | IICV_GENERATE_ERRORS,
						fp, this, pDoc->m_pFastHenryModel, 0, 0, 0, 0.0, 0.0, 0.0);

			// delete dummy char added 
			lastpos = GetTextLength();
			GetRichEditCtrl().SetSel(lastpos-1, lastpos);
			GetRichEditCtrl().ReplaceSel(_T(""));

			pDoc->SetModifiedFlag(FALSE);     // start off with unmodified
			m_isModified = FALSE;

			// go to beginning of text
			GetRichEditCtrl().SetSel(0,0);
			GetRichEditCtrl().ReplaceSel(_T(""));

			// now all is ready, allow drawing in view
			SetRedraw(TRUE);

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
					// inductance models do not have charge densities information
					((CFastModelView*)myview)->m_bInputHasCharge = false;
					((CFastModelView*)myview)->InitializeScene();
					((CFastModelView*)myview)->ZoomAll();
				}   
			}   

			// update all document's views (including this one; if first parameter
			// pSender were 'this', all document's views but this one would be 
			// updated)
			pDoc->UpdateAllViews( NULL, 0L, NULL );

			fclose(fp);

			// end wait cursor
			((CFastModelApp*)AfxGetApp())->EndArrowWaitCursor();
			// re-enable closing FastModel
			((CMainFrame*)(AfxGetApp()->m_pMainWnd))->EnableClose();

		}
		else
			MessageBox("Cannot open file!", "Error", MB_ICONSTOP);
	}
}

void CFastHenryView::OnFileSaveFasthenry() 
{
	if( m_strPathName == "Unnamed") {
		SaveAsFastHenryFile();
	}
	else {
		SaveFastHenryFile();
	}
}

void CFastHenryView::OnFileSaveAsFasthenry() 
{
	SaveAsFastHenryFile();
}

void CFastHenryView::SaveFastHenryFile() 
{
	long lineno, linelen;
	char str[CEV_MAX_LINE_LEN];

	CFastModelDoc* pDoc = GetDocument();
	ASSERT(pDoc);

	// save file. Section copied from MFC CDocument::OnSaveDocument
	CFileException fe;
	CFile* pFile = pDoc->GetFile(m_strPathName,
		CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive, &fe);

	if (pFile == NULL)
	{
		pDoc->ReportSaveLoadException(m_strPathName, &fe,
			TRUE, AFX_IDP_INVALID_FILENAME);
		return;
	}

	CArchive saveArchive(pFile, CArchive::store | CArchive::bNoFlushOnDelete);
	saveArchive.m_pDocument = pDoc;
	saveArchive.m_bForceFlat = FALSE;
	TRY
	{
		CWaitCursor wait;

		// Serialize(saveArchive);  this should save!

		lineno = 0;
		// when last line, linelen == 0
		while(linelen = GetRichEditCtrl().GetLine(lineno++, str, CEV_MAX_LINE_LEN))
		{			
			if(linelen > CEV_MAX_LINE_LEN-1) {
				// TBC warning : error, line too long
			}
			str[linelen] = 0;

			saveArchive.WriteString(str);
		}

		saveArchive.Close();
		pDoc->ReleaseFile(pFile, FALSE);
	}
	CATCH_ALL(e)
	{
		pDoc->ReleaseFile(pFile, TRUE);

		TRY
		{
			pDoc->ReportSaveLoadException(m_strPathName, e,
				FALSE, AFX_IDP_FAILED_TO_SAVE_DOC);
		}
		END_TRY
		DELETE_EXCEPTION(e);
		return;
	}
	END_CATCH_ALL

	pDoc->SetModifiedFlag(FALSE);     // start off with unmodified
	m_isModified = FALSE;
} 

BOOL CFastHenryView::SaveAsFastHenryFile() 
{
	CFileDialog	dlgFile(FALSE, "inp", m_strPathName, OFN_OVERWRITEPROMPT,
		"FastHenry2 files (*.inp)|*.inp|All files (*.*)|*.*|");

    if(dlgFile.DoModal() == IDOK)
	// if the user really wants to export to a file..
	{	
		m_strPathName = dlgFile.GetPathName();
		// update caption
		((CChildFrame*)GetParent())->OnUpdateFrameTitle(TRUE);

		m_isModified = FALSE;
		SaveFastHenryFile();
		return TRUE;
	}

	return FALSE;
}

int CFastHenryView::ColorLines(long baselineno, long startline, long endline)
{
	int deletedChars;
	CImportFastHenry importObj;

	deletedChars = importObj.import(IICV_COLOR_LINES,
					NULL, this, NULL, baselineno, startline, endline,
					0.0, 0.0, 0.0);

	return(deletedChars);
}

// must override or will return the doc type linked with the
// richedit view (RichEditDoc)
inline CFastModelDoc* CFastHenryView::GetDocument() const
{
	ASSERT(m_pDocument != NULL);
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFastModelDoc)));
	return (CFastModelDoc*)m_pDocument;
}

void CFastHenryView::OnFastHenryUpdateModel() 
{
	CImportFastHenry importObj;
	long lastline;

	CFastModelDoc* pDoc = GetDocument();
	ASSERT(pDoc);

	// show wait cursor
	((CFastModelApp*)AfxGetApp())->BeginArrowWaitCursor();
	// disable closing FastModel
	((CMainFrame*)(AfxGetApp()->m_pMainWnd))->DisableClose();

	// delete FastModel 3D model and create a new one
	delete(pDoc->m_pFastHenryModel);
	pDoc->m_pFastHenryModel = new C3DShell;

	// get last line number
	lastline = GetRichEditCtrl().GetLineCount() - 1;

	importObj.import(IICV_CONVERT_TO_MODEL | IICV_GENERATE_ERRORS,
					NULL, this, pDoc->m_pFastHenryModel, 0, 0, lastline,
					0.0, 0.0, 0.0);

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

HMENU CFastHenryView::GetContextMenu( WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE* lpchrg )
{
	// create menu object; warning: creating the menu object 
	// does not associate a menu item automatically
	CMenu menu;
	BOOL status;

	CWnd* pWndPopupOwner = this;
	while (pWndPopupOwner->GetStyle() & WS_CHILD)
		pWndPopupOwner = pWndPopupOwner->GetParent();

	// so the menu item has to be loaded into the object
	status = menu.LoadMenu(IDR_POPUP_FASTHENRYTYPE);
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

void CFastHenryView::OnClose() 
{
	CFastModelDoc* pDoc = GetDocument();

	// remove model
	delete(pDoc->m_pFastHenryModel);
	pDoc->m_pFastHenryModel = NULL;

	CColorEditView::OnClose();
}

