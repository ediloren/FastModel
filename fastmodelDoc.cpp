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


// fastmodelDoc.cpp : implementation of the CFastModelDoc class
//

#include "stdafx.h"

#include "fastmodel.h"

#include "fastmodelDoc.h"
#include "CntrItem.h"

// must include view definitions for usage in override
// of ::UpdateFrameCounts()
#include "FastHenryView.h"
#include "FastCapView.h"
#include "FastModelView.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFastModelDoc
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CFastModelDoc, COleDocument)

BEGIN_MESSAGE_MAP(CFastModelDoc, COleDocument)
	//{{AFX_MSG_MAP(CFastModelDoc)
	ON_COMMAND(ID_FILE_NEW_FASTCAP, OnFileNewFastCap)
	ON_COMMAND(ID_RUN_FASTCAP, OnRunFastCap)
	ON_COMMAND(ID_RUN_FASTHENRY, OnRunFastHenry)
	ON_COMMAND(ID_RUN_FASTERCAP, OnRunFasterCap)
	//}}AFX_MSG_MAP
	// Enable default OLE container implementation
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, COleDocument::OnUpdatePasteMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, COleDocument::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, COleDocument::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT, COleDocument::OnEditConvert)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, COleDocument::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, COleDocument::OnEditLinks)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_FIRST, COleDocument::OnUpdateObjectVerbMenu)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFastModelDoc construction/destruction

CFastModelDoc::CFastModelDoc()
{
	int i;

	// Use OLE compound files
	EnableCompoundFile();

	m_pFastHenryModel = NULL;
	m_pFastCapModel = NULL;

	// token count starts from 1
	m_ulFileMonTokCount = 1;

	// FastCap view inits
	m_iFCViewNum = 0;
	for( i=0; i<FMD_MAX_FC_VIEWS; i++)
		m_pFastCapView[i] = NULL;
	m_bIsFCModified = FALSE;

	// start watch modified files thread
	m_clFileMonitor.startWatch();

	// TODO: add one-time construction code here

}

CFastModelDoc::~CFastModelDoc()
{
	// stop watching modified files thread
	m_clFileMonitor.stopWatch();

}

// if no state info in the registry, return 'false'
bool CFastModelDoc::LoadWindowState()
{
	int numOfFMViews, i;
	CFastModelApp *pApp;
	CMainFrame *pMainFrame;
	CString regEntry;
	POSITION pos;
	CView* pView;
	CFrameWnd* pFrame;

	// get a pointer to the main frame
	pApp = (CFastModelApp *)AfxGetApp();
	ASSERT(pApp);
	pMainFrame = pApp->GetMainFrame();
	ASSERT(pMainFrame);

	// retrieve from the registry the number of open FastModel View
	numOfFMViews = pApp->GetProfileInt( _T("State"), _T("FastModelViewNumber"), 0);

	// if registry corrupted, return 'false'
	if( numOfFMViews <= 0 )
		return false;

	// create Fast Model views
	// (remark: the first one has already been created)
	for(i=0; i<numOfFMViews-1; i++) {
		pMainFrame->CreateNewModelWindow();
	}

	// for every view, load view state from the registry
	i = 0;
	pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		// get view
		pView = GetNextView(pos);
		// get its frame window
		pFrame = pView->GetParentFrame();

		// if FastHenry window
		if( pFrame == pMainFrame->GetFastHenryFrame() ) {
			((CFastHenryView*)pView)->LoadWindowState(_T("State\\FastHenryView"));
		}
		// if FastCap
		else if( pFrame == pMainFrame->GetFastCapFrame() ) {
			((CFastCapView*)pView)->LoadWindowState(_T("State\\FastCapView"));
		}
		// else it is a FastModel view
		else if( pView->IsKindOf( RUNTIME_CLASS(CFastModelView) ) ) {
			regEntry.Format("State\\FastModelView%d", i+1);
			((CFastModelView*)pView)->LoadWindowState(regEntry);
			i++;
		}
	}   

	// verify that there are as many ModelViews as created
	ASSERT( i == numOfFMViews);

	return true;
}

void CFastModelDoc::SaveWindowState()
{
	int numOfFMViews;
	CFastModelApp *pApp;
	CMainFrame *pMainFrame;
	CString regEntry;
	POSITION pos;
	CView* pView;
	CFrameWnd* pFrame;

	// get a pointer to the main frame
	pApp = (CFastModelApp *)AfxGetApp();
	ASSERT(pApp);
	pMainFrame = pApp->GetMainFrame();
	ASSERT(pMainFrame);

	// for every view, save view state in the registry and
	// count the number of FastModel views
	numOfFMViews = 0;
	pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		// get view
		pView = GetNextView(pos);
		// get its frame window
		pFrame = pView->GetParentFrame();

		// if main FastHenry window
		if( pFrame == pMainFrame->GetFastHenryFrame() ) {
			((CFastHenryView*)pView)->SaveWindowState(_T("State\\FastHenryView"));
		}
		// if main FastCap window
		else if( pFrame == pMainFrame->GetFastCapFrame() ) {
			((CFastCapView*)pView)->SaveWindowState(_T("State\\FastCapView"));
		}
		else if( pView->IsKindOf( RUNTIME_CLASS(CFastModelView) ) ) {
			regEntry.Format("State\\FastModelView%d", numOfFMViews+1);
			((CFastModelView*)pView)->SaveWindowState(regEntry);
			numOfFMViews++;
		}
	}   

	// save in the registry the number of open FastModel View
	pApp->WriteProfileInt( _T("State"), _T("FastModelViewNumber"), numOfFMViews);
}

/////////////////////////////////////////////////////////////////////////////
// FastCapView-related functions
/////////////////////////////////////////////////////////////////////////////

BOOL CFastModelDoc::addFastCapView(CFastCapView *view)
{
	return FALSE;
}

BOOL CFastModelDoc::CloseFastCapView(CFastCapView *view)
{
	int i;
	CBrowserWnd	*browserwnd;

	browserwnd = &(((CMainFrame *)AfxGetMainWnd())->m_wndBrowser);

	for( i=0; i<FMD_MAX_FC_VIEWS; i++) {
		if( m_pFastCapView[i] == view ) {

			// remove file name to project browser tree window
			browserwnd->removeFastCapFile(m_pFastCapView[i]->m_ulTreeToken);

			// remove file name from watch list
			m_clFileMonitor.removeFile((LPCTSTR)m_pFastCapView[i]->m_strPathName);
			m_stdMonFilesMap.erase(m_pFastCapView[i]->m_ulFileMonTok);
	
			// close window 
			m_pFastCapView[i]->GetParentFrame()->PostMessage(WM_CLOSE);

			m_pFastCapView[i] = NULL;

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CFastModelDoc::CloseFastCapView(int viewnumber)
{
	CBrowserWnd	*browserwnd;

	browserwnd = &(((CMainFrame *)AfxGetMainWnd())->m_wndBrowser);

	if( viewnumber >= 0 && viewnumber < FMD_MAX_FC_VIEWS && m_pFastCapView[viewnumber] != NULL) {

		// remove file name to project browser tree window
		browserwnd->removeFastCapFile(m_pFastCapView[viewnumber]->m_ulTreeToken);

		// remove file name from watch list
		m_clFileMonitor.removeFile((LPCTSTR)m_pFastCapView[viewnumber]->m_strPathName);
		m_stdMonFilesMap.erase(m_pFastCapView[viewnumber]->m_ulFileMonTok);
		
		// close window 
		m_pFastCapView[viewnumber]->GetParentFrame()->PostMessage(WM_CLOSE);

		m_pFastCapView[viewnumber] = NULL;

		return TRUE;
	}

	return FALSE;
}

CFastCapView *CFastModelDoc::getFastCapViewFromName(char *name)
{
	int i;
	char path[FMD_MAX_PATH_LEN];

	for( i=0; i<FMD_MAX_FC_VIEWS; i++) {
		if( m_pFastCapView[i] != NULL ) {
			// should have saved path name always as a full path, so no ambiguity;
			// only, use case-insensitive comparison to reflect the MS file system
			_fullpath( path, name, FMD_MAX_PATH_LEN );
			if( (m_pFastCapView[i]->m_strPathName).CompareNoCase(path)  == 0 )
				return m_pFastCapView[i];
		}
	}

	return NULL;
}

CFastCapView *CFastModelDoc::getFastCapViewFromTreeTok(unsigned long token, int *viewnumber)
{
	int i;

	for( i=0; i<FMD_MAX_FC_VIEWS; i++) {
		if( m_pFastCapView[i] != NULL ) {
			if( m_pFastCapView[i]->m_ulTreeToken  == token ) {
				*viewnumber = i;
				return m_pFastCapView[i];
			}
		}
	}

	*viewnumber = -1;
	return NULL;
}

// scans all FastCap views (including the main one) and
// if all are unmodified, updates the global flag 
// (m_bIsFCModified) accordingly
void CFastModelDoc::updateIsFCModified()
{
	CFastCapView *fcv;
	BOOL modified;
	int i;

	fcv = getFastCapMainView();

	fcv->m_isModified ? modified=TRUE : modified = FALSE;

	for( i=0; i<FMD_MAX_FC_VIEWS && modified == FALSE; i++) {
		if( m_pFastCapView[i] != NULL ) {
			if(m_pFastCapView[i]->m_isModified)
				modified=TRUE;
		}
	}

	m_bIsFCModified = modified;
}

CFastCapView *CFastModelDoc::getFastCapMainView()
{
	POSITION pos = GetFirstViewPosition();
	CView *view;

	view = GetNextView(pos);
	while( view != NULL) {
		if( view->IsKindOf(RUNTIME_CLASS(CFastCapView)) )
			return ((CFastCapView *)view)->m_pFastCapMain;

		view = GetNextView(pos);
	}

	return NULL;
}

CFastHenryView *CFastModelDoc::getFastHenryMainView()
{
	POSITION pos = GetFirstViewPosition();
	CView *view;

	view = GetNextView(pos);
	while( view != NULL) {
		if( view->IsKindOf(RUNTIME_CLASS(CFastHenryView)) )
			return ((CFastHenryView *)view)->m_pFastHenryMain;

		view = GetNextView(pos);
	}

	return NULL;
}

CFastCapView *CFastModelDoc::NewFastCapView(char *name, bool filealreadyexist)
{
	int i;
	CChildFrame *fastCapFrame;
	CFastCapView *fastCapView;
	char path[FMD_MAX_PATH_LEN];
	CRect rect;

	for( i=0; i<FMD_MAX_FC_VIEWS; i++) {
		if( m_pFastCapView[i] == NULL ) {

			// create new FastCap view
			fastCapFrame = (CChildFrame *) ((CFastModelApp *)AfxGetApp())->m_pFastCapViewTemplate->CreateNewFrame(this, NULL);
			((CFastModelApp *)AfxGetApp())->m_pFastCapViewTemplate->InitialUpdateFrame(fastCapFrame, this);

			fastCapView = (CFastCapView *)fastCapFrame->GetActiveView();
			ASSERT(fastCapView);

			// initialization as secondary view
			
			// creates a path (from a file name, or relative file name, 
			// or absoulte path, see _makepath, _fullpath, _spiltpath)
			_fullpath( path, name, FMD_MAX_PATH_LEN );
			fastCapView->m_strPathName = path;
			m_pFastCapView[i] = fastCapView;

			// add file name to project browser tree window
			fastCapView->m_ulTreeToken = ((CMainFrame *)AfxGetApp()->m_pMainWnd)->m_wndBrowser.addFastCapFile(path);

			// If not already existing, also SaveAs file (the file is created,
			// added to watch list, etc.)
			// Otherwise, simply add to watch list
			if( filealreadyexist == false)
				fastCapView->SaveFastCapFile();
			else
				fastCapView->AddFileToWatchList();

			// update frame caption
			((CChildFrame*)(fastCapView->GetParent()))->OnUpdateFrameTitle(TRUE);

			return fastCapView;
		}
	}

	AfxMessageBox("Internal error:\nMaximum number of FastCap2 views reached!", MB_ICONERROR);
	return NULL;
}

BOOL CFastModelDoc::existsAnyModel()
{
	if( m_pFastHenryModel != NULL || m_pFastCapModel != NULL )
		return(TRUE);
	else
		return(FALSE);
}

// returns FALSE if user doesn't want to discard changes
BOOL CFastModelDoc::UserRemoveAllFCViews() 
{

	// the doc will be lost on import -> ask user yes/no
	updateIsFCModified();
	if (m_bIsFCModified)
		if( AfxMessageBox("The current project has unsaved changes\nAre you sure you want to discard changes?", MB_OKCANCEL|MB_ICONEXCLAMATION) == IDCANCEL )
			return FALSE;

	RemoveAllFCViews();

	return TRUE;
}

void CFastModelDoc::RemoveAllFCViews() 
{
	CBrowserWnd	*browserwnd;
	int i;

	// close all views (except the main, this one)

	browserwnd = &(((CMainFrame *)AfxGetMainWnd())->m_wndBrowser);

	m_clFileMonitor.stopWatch();

	for( i=0; i<FMD_MAX_FC_VIEWS && m_pFastCapView[i] != NULL; i++) {
		// remove file name from project browser tree window
		browserwnd->removeFastCapFile(m_pFastCapView[i]->m_ulTreeToken);
		// remove file name from watch list
		m_clFileMonitor.removeFile((LPCTSTR)m_pFastCapView[i]->m_strPathName);
		m_stdMonFilesMap.erase(m_pFastCapView[i]->m_ulFileMonTok);
		// delete view
		m_pFastCapView[i]->GetParentFrame()->PostMessage(WM_CLOSE);
		m_pFastCapView[i]= NULL;
	}

 	m_clFileMonitor.startWatch();

}

/////////////////////////////////////////////////////////////////////////////
// CFastModelDoc message handlers

void CFastModelDoc::OnFileNewFastCap() 
{
	CFastCapView *fastcapview;

	// TBC warning: should be a 'new' dialog box, not a Open one

	CFileDialog	dlgFile(FALSE, "qui", NULL, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,
		"FastCap2 Geometry files (*.neu, *.qui, *.txt)|*.neu;*.qui;*.txt|All files (*.*)|*.*|");

	// modify title 
	dlgFile.m_ofn.lpstrTitle = _T("New File Save As");

    if(dlgFile.DoModal() == IDOK)
	// if the user really wants to create a new file..
	{	
		
		m_strPathName = dlgFile.GetPathName();

		fastcapview = NewFastCapView((char *)(LPCTSTR)m_strPathName);
	}	
}

// process working thread notification of file change
LRESULT CFastModelDoc::OnFileChangeNotify(WPARAM FileAlarmAndCategory, LPARAM fileCode)
{
	int res;
	FMDMonitorFiles::iterator it;
	char message[FMD_MAX_NAME_LEN];
	char filename[_MAX_FNAME];
	char extension[_MAX_EXT];
	FILE *fp;
	unsigned int fileCategory, alarm;

	// get alarm type and file category
	fileCategory = LOWORD(FileAlarmAndCategory);
	alarm = HIWORD(FileAlarmAndCategory);

	if( fileCategory == FMD_FASTCAP_FILE ) {

		it = m_stdMonFilesMap.find(fileCode);
		if( it == m_stdMonFilesMap.end() ) {
			AfxMessageBox("Internal error in FileChangeNotify", MB_ICONSTOP);
			return 0;
		}

		_splitpath(it->second->m_strPathName, NULL, NULL, filename, extension);

		// if file has been deleted
		if(alarm == FCE_REMOVED) {
			sprintf(message, "The file %s%s has been deleted outside FastModel\nDo you want close it?", filename, extension);
			res = AfxMessageBox(message, MB_OKCANCEL|MB_ICONEXCLAMATION);
	
			if( res == IDCANCEL ) {
				// save it again
				it->second->SaveFastCapFile();
			}
			else {
				// close file
				CloseFastCapView(it->second);
			}
				
		}
		// otherwise (modified or created, but the latter should never happen)
		else {
			sprintf(message, "The file %s%s has been changed outside FastModel\nDo you want to reload the model?", filename, extension);
			res = AfxMessageBox(message, MB_OKCANCEL|MB_ICONEXCLAMATION);
			if( res == IDCANCEL )
				return 0;

			fp = fopen((LPCSTR)(it->second->m_strPathName), "r");

			if( fp != NULL) {
				it->second->importFastCap(CEV_COLOR_LINES | CEV_FILE_SOURCE | 
							CEV_GENERATE_ERRORS, NULL, fp, 0.0, 0.0, 0.0);
				fclose(fp);
			}
			else {
				sprintf(message, "Cannot open file %s%s!", filename, extension);
				res = AfxMessageBox(message, MB_ICONSTOP);
			}

			it->second->OnFastCapUpdateModel();
		}
	}

	// update all document's views 
	UpdateAllViews( NULL, 0L, NULL );

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CFastModelDoc overrides

BOOL CFastModelDoc::OnNewDocument()
{
	if (!COleDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

 // the overridden function assumed 1 doc per frame;
 // therefore, a customization was needed
void CFastModelDoc::UpdateFrameCounts()
{
	// walk all frames of views (mark and sweep approach)
	// and set them as unknown
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		ASSERT_VALID(pView);
		ASSERT(::IsWindow(pView->m_hWnd));

		if (pView->IsWindowVisible())   // Do not count invisible windows.
		{
			CFrameWnd* pFrame = pView->GetParentFrame();
			if (pFrame != NULL)
				pFrame->m_nWindow = -1;     // unknown
		}
	}

	// now do it again counting the unique ones
	int nFrames = 0;
	pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		ASSERT_VALID(pView);
		ASSERT(::IsWindow(pView->m_hWnd));
		if (pView->IsWindowVisible())   // Do not count invisible windows.
		{
			CFrameWnd* pFrame = pView->GetParentFrame();
			// Enrico - added test on class type
			if (pFrame != NULL && pFrame->m_nWindow == -1 
				&& !pView->IsKindOf(RUNTIME_CLASS(CFastHenryView))
				&& !pView->IsKindOf(RUNTIME_CLASS(CFastCapView)) )
			{
				ASSERT_VALID(pFrame);
				// not yet counted (give it a 1 based number)
				pFrame->m_nWindow = ++nFrames;
			}
		}
	}

	// lastly walk the frames and update titles (assume same order)
	// go through frames updating the appropriate one
	int iFrame = 1;
	pos = GetFirstViewPosition();
	while (pos != NULL)
	{
		CView* pView = GetNextView(pos);
		ASSERT_VALID(pView);
		ASSERT(::IsWindow(pView->m_hWnd));
		if (pView->IsWindowVisible())   // Do not count invisible windows.
		{
			CFrameWnd* pFrame = pView->GetParentFrame();
			// Enrico - see above
			if (pFrame != NULL && pFrame->m_nWindow == iFrame 
				&& !pView->IsKindOf(RUNTIME_CLASS(CFastHenryView))
				&& !pView->IsKindOf(RUNTIME_CLASS(CFastCapView)) )

			{
				ASSERT_VALID(pFrame);
				if (nFrames == 1)
					pFrame->m_nWindow = 0;      // the only one of its kind
				pFrame->OnUpdateFrameTitle(TRUE);
				iFrame++;
			}
		}
	}
	ASSERT(iFrame == nFrames + 1);
}


/////////////////////////////////////////////////////////////////////////////
// CFastModelDoc serialization

void CFastModelDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}

	// Calling the base class COleDocument enables serialization
	//  of the container document's COleClientItem objects.
	COleDocument::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CFastModelDoc diagnostics

#ifdef _DEBUG
void CFastModelDoc::AssertValid() const
{
	COleDocument::AssertValid();
}

void CFastModelDoc::Dump(CDumpContext& dc) const
{
	COleDocument::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CFileChangeMonitor
/////////////////////////////////////////////////////////////////////////////

void CFileChangeMonitor::OnFileAlarm(unsigned int nAlarm, const std::string &sFileName, unsigned int fileCategory, unsigned long fileCode)
{
	WPARAM fileAlrmAndCategory;

	static const UINT FMD_FILE_CHANGE_NOTIFY = RegisterWindowMessage(_T("FMD_FILE_CHANGE_NOTIFY-FastModel-Enrico_Di_Lorenzo"));

	// send notification as message

	// build wparam with fileCategory as low word and nAlarm as high word
	fileAlrmAndCategory = MAKEWPARAM(fileCategory, nAlarm);
	((CMainFrame *)AfxGetMainWnd())->PostMessage(FMD_FILE_CHANGE_NOTIFY, fileAlrmAndCategory, fileCode);
}



/////////////////////////////////////////////////////////////////////////////
// CImport3DObj
/////////////////////////////////////////////////////////////////////////////

void CImport3DObj::Import3DObj(CArchive *ar, CString strExtension)
{
	CString str;
	
	while(ar->ReadString(str)) // read all rows
	{
		AfxMessageBox(str);
	}
	

}

void CImport3DObj::ImportDXF(CString strPathName)
{

}

void CImport3DObj::ImportSAT(CString strPathName)
{

}

void CFastModelDoc::OnRunFastCap() 
{
	CFastCapView *fc = getFastCapMainView();
	CFastModelApp *myapp = (CFastModelApp *)AfxGetApp();

	// if never saved before, 
	if( fc->m_strPathName == "Unnamed") {
		AfxMessageBox("You must save your FastCap2 input file\nbefore launching FastCap2", MB_ICONWARNING);
		if (fc->SaveAsFastCapFile() == FALSE)
			return;
	}

	// save all before launching the solver, in order to have the simulation
	// to be performed on the updated files
	fc->OnFileSaveFastCapAll();

	myapp->m_clControlSolvers.RunFastCap(fc->m_strPathName);
}

void CFastModelDoc::OnRunFasterCap() 
{
	CFastCapView *fc = getFastCapMainView();
	CFastModelApp *myapp = (CFastModelApp *)AfxGetApp();

	// if never saved before, 
	if( fc->m_strPathName == "Unnamed") {
		AfxMessageBox("You must save your FastCap2 input file\nbefore launching FasterCap", MB_ICONWARNING);
		if (fc->SaveAsFastCapFile() == FALSE)
			return;
	}

	// save all before launching the solver, in order to have the simulation
	// to be performed on the updated files
	fc->OnFileSaveFastCapAll();

	myapp->m_clControlSolvers.RunFasterCap(fc->m_strPathName);
}

void CFastModelDoc::OnRunFastHenry() 
{
	CFastHenryView *fh = getFastHenryMainView();
	CFastModelApp *myapp = (CFastModelApp *)AfxGetApp();

	// if never saved before, 
 	if( fh->m_strPathName == "Unnamed") {
		AfxMessageBox("You must save your FastHenry2 input file\nbefore launching FastHenry2", MB_ICONWARNING);
		if (fh->SaveAsFastHenryFile() == FALSE)
			return;
	}

	// save all before launching the solver, in order to have the simulation
	// to be performed on the updated files
	fh->OnFileSaveFasthenry();

	myapp->m_clControlSolvers.RunFastHenry(fh->m_strPathName);
}
