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


// fastmodel.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "fastmodel.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "fastmodelDoc.h"
#include "fastmodelView.h"
#include "FastHenryView.h"
#include "FastCapView.h"
#include "ErrorHandler.h"
#include "IntroDialog.h"
#include <locale.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFastModelApp

BEGIN_MESSAGE_MAP(CFastModelApp, CWinApp)
	//{{AFX_MSG_MAP(CFastModelApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_IMPORT_FILE, OnImportFile)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_CLOSE_PROJ, OnFileCloseProj)
	ON_COMMAND(ID_FILE_NEW_PROJ, OnFileNewProj)
	ON_COMMAND(ID_FILE_OPEN_PROJ, OnFileOpenProj)
	ON_COMMAND(ID_FILE_SAVE_AS_PROJ, OnFileSaveAsProj)
	ON_COMMAND(ID_FILE_SAVE_PROJ, OnFileSaveProj)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	//ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFastModelApp construction

CFastModelApp::CFastModelApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFastModelApp object

CFastModelApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFastModelApp initialization

BOOL CFastModelApp::InitInstance()
{
	HKEY rkey;
	char app_path[_MAX_PATH];
	unsigned long len;
	LONG ret;
	CString strFHSamplePath, strFCSamplePath;
	CIntroDialog intro;

	try {
		// CG: The following block was inserted by 'Status Bar' component.
		//{
		//Set up date and time defaults so they're the same as system defaults
		//	setlocale(LC_ALL, "");
		//}
		
		// Initialize OLE libraries
		if (!AfxOleInit())
		{
			AfxMessageBox(IDP_OLE_INIT_FAILED);
			return FALSE;
		}
		
		AfxEnableControlContainer();
		
		// Standard initialization
		// If you are not using these features and wish to reduce the size
		//  of your final executable, you should remove from the following
		//  the specific initialization routines you do not need.
		
#ifdef _AFXDLL
		Enable3dControls();			// Call this when using MFC in a shared DLL
#else
		Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
		
		// Change the registry key under which our settings are stored.
		// You should modify this string to be something appropriate
		// such as the name of your company or organization.
		SetRegistryKey(_T("FastFieldSolvers"));
		
		LoadStdProfileSettings();  // Load standard INI file options (including MRU)
		
		// Load application settings from registry
		//
		
		// Load global (LOCAL_MACHINE) FastFieldSolvers variables from the registry
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\FastFieldSolvers\\Settings"), 0,
			KEY_READ, &rkey) != ERROR_SUCCESS) {
			AfxMessageBox("Cannot find FastFieldSolvers 'Settings' key in Registry\nPlease try installing the software again", MB_ICONSTOP);
		}
		else {
			// Get FastFieldSolvers bundle installation path
			len = _MAX_PATH;
			if ( RegQueryValueEx( rkey, _T("Path"), NULL, NULL, (unsigned char *)app_path, &len) != ERROR_SUCCESS ) {
				AfxMessageBox("Cannot find FastFieldSolvers path in Registry\nPlease try installing the software again", MB_ICONSTOP);
			}
			else {
				m_strBasePath = app_path;
			}
		}
		RegCloseKey(rkey);

		// Load global (LOCAL_MACHINE) FastModel variables from the registry
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\FastFieldSolvers\\FastModel\\Settings"), 0,
			KEY_READ, &rkey) != ERROR_SUCCESS) {
			AfxMessageBox("Cannot find FastModel 'Settings' key in Registry\nPlease try installing the software again", MB_ICONSTOP);
		}
		else {
			// Get FastModel installation path
			len = _MAX_PATH;
			if ( RegQueryValueEx( rkey, _T("Path"), NULL, NULL, (unsigned char *)app_path, &len) != ERROR_SUCCESS ) {
				AfxMessageBox("Cannot find FastModel path in Registry\nPlease try installing the software again", MB_ICONSTOP);
			}
			else {
				m_strAppPath = app_path;
			}
		}
		RegCloseKey(rkey);
		
		// check if no FCView folder is already present in the registry
		strFCSamplePath = GetProfileString(_T("FileDialogs"), _T("FastCapView"));
		if(strFCSamplePath.IsEmpty() == TRUE) {
			// if so, get it from the installation defaults
			strFCSamplePath = _T("");
			// Load global (LOCAL_MACHINE) FasterCap variables from the registry
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\FastFieldSolvers\\FasterCap\\Settings"), 0,
				KEY_READ, &rkey) != ERROR_SUCCESS) {
				AfxMessageBox("Cannot find FasterCap 'Settings' key in Registy\nPlease try installing FasterCap again", MB_ICONSTOP);
			}
			else {
				// sample path
				len = _MAX_PATH;
				ret = RegQueryValueEx( rkey, _T("SamplePath"), NULL, NULL, (unsigned char *)app_path, &len);
				if ( ret == ERROR_SUCCESS ) {
					strFCSamplePath = app_path;
				}
			}
			RegCloseKey(rkey);
			// and store the sample path in the Registry for persistency (used in OnImportFastCapView() )
			WriteProfileString(_T("FileDialogs"), _T("FastCapView"), strFCSamplePath);
		}
		
		// check if no FHView folder is already present in the registry
		strFHSamplePath = GetProfileString(_T("FileDialogs"), _T("FastHenryView"));
		if(strFHSamplePath.IsEmpty() == TRUE) {
			// Load global (LOCAL_MACHINE) FastHenry2 variables from the registry
			if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Software\\FastFieldSolvers\\FastHenry2\\Settings"), 0,
				KEY_READ, &rkey) != ERROR_SUCCESS) {
				AfxMessageBox("Cannot find FastHenry2 'Settings' key in Registy\nPlease try installing FastHenry2 again", MB_ICONSTOP);
			}
			else {
				// sample path
				len = _MAX_PATH;
				ret = RegQueryValueEx( rkey, _T("SamplePath"), NULL, NULL, (unsigned char *)app_path, &len);
				if ( ret == ERROR_SUCCESS ) {
					strFHSamplePath = app_path;
				}
			}
			RegCloseKey(rkey);
			// and store the sample path in the Registry for persistency (used in OnImportFastHenryView() )
			WriteProfileString(_T("FileDialogs"), _T("FastHenryView"), strFHSamplePath);
		}


		// Register the application's document templates.  Document templates
		// serve as the connection between documents, frame windows and views.
		//
		// We create three doc template objects to orchestrate the creation of three
		// distinct MDI children-hosted views of the document: (1) the model
		// view, (2) the FastHenry view and (3) the FastCap view.
		//
		// We register all the doc templates with the CWinApp object, by calling
		// AddDocTemplate.  However, if we were to do this and nothing else, then
		// the framework would mistakenly believe that the application supports
		// two document types.  The framework would display a File New dialog
		// that lists three document types, both which would be "Model".
		// We avoid this problem by removing the third string from
		// the document template for the FastXxxx frame/view (the strings are
		// in the resources under 'strings').
		// Finding no GetDocString(CDocTemplate::fileNewName) for the
		// other document template, CWinApp concludes that there is only
		// one document type supported by the application (the "Model"
		// document type specified in the first document template) and
		// therefore does not represent the user with a File New dialog.
		
		m_pModelViewTemplate = new CMultiDocTemplate(
			IDR_FASTMODELTYPE,
			RUNTIME_CLASS(CFastModelDoc),
			RUNTIME_CLASS(CChildFrame), // custom MDI child frame
			RUNTIME_CLASS(CFastModelView));
		m_pModelViewTemplate->SetContainerInfo(IDR_FASTMODELTYPE_CNTR_IP);
		AddDocTemplate(m_pModelViewTemplate);
		
		m_pFastHenryViewTemplate = new CMultiDocTemplate(
			IDR_FASTHENRYTYPE,
			RUNTIME_CLASS(CFastModelDoc),
			RUNTIME_CLASS(CChildFrame), // custom MDI child frame
			RUNTIME_CLASS(CFastHenryView));
		m_pFastHenryViewTemplate->SetContainerInfo(IDR_FASTHENRYTYPE_CNTR_IP);
		AddDocTemplate(m_pFastHenryViewTemplate);
		
		m_pFastCapViewTemplate = new CMultiDocTemplate(
			IDR_FASTCAPTYPE,
			RUNTIME_CLASS(CFastModelDoc),
			RUNTIME_CLASS(CChildFrame), // custom MDI child frame
			RUNTIME_CLASS(CFastCapView));
		m_pFastCapViewTemplate->SetContainerInfo(IDR_FASTCAPTYPE_CNTR_IP);
		AddDocTemplate(m_pFastCapViewTemplate); 
		
		// create main MDI Frame window
		CMainFrame* pMainFrame = new CMainFrame;
		if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
			return FALSE;
		m_pMainWnd = pMainFrame;
		m_pMDIMainFrameWnd = pMainFrame;
		
		// Enable drag/drop open
		//m_pMainWnd->DragAcceptFiles(); // TBC Warning: commented out, but must be reenabled
		
		// Registry-related file extensions initialization
		//
		
		// Enable DDE Execute open
		//EnableShellOpen();			// TBC Warning: commented out, but must be reenabled
		//RegisterShellFileTypes(TRUE);	// TBC Warning: commented out, but must be reenabled
		
		// load standard arrow wait cursor
		m_hcurAppstarting = ::LoadCursor(NULL, IDC_APPSTARTING);
		ASSERT(m_hcurAppstarting != NULL);
		
		// Parse command line for standard shell commands, DDE, file open
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);
		
		// Dispatch commands specified on the command line
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
		
		// The main window has been initialized, so show 
		// (maximized) and update it.
		pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
		pMainFrame->UpdateWindow();
		
		// link the error handler to the main frame, so it is possible
		// to output text to the output window
		CErrorHandler errHndl;
		errHndl.SetMainFrame(pMainFrame);
		
		// start CINT interpreter
		(pMainFrame->m_wndOutput).StartCint();
		
		// start FastHenry, FastCap and FasterCap remote consoles
		m_clControlSolvers.StartFastHenry();
		m_clControlSolvers.StartFastCap();
		m_clControlSolvers.StartFasterCap();
	}
	catch( CException *e) {
		e->ReportError();
		e->Delete();
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnShowLicense();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_BN_CLICKED(IDC_SHOW_LICENSE, OnShowLicense)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CFastModelApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CFastModelApp commands


// Used for importing files in other formats (e.g. autocad, SAT)
BOOL CFastModelApp::OnImportFile() 
{

	AfxMessageBox("Sorry, FastModel will support importing\nof other 3D file formats in the future!", MB_ICONEXCLAMATION | MB_OK);

/*	CFileDialog	dlgFile(TRUE, NULL, NULL, OFN_OVERWRITEPROMPT,
		"Autocad DXF files (*.dxf)|*.dxf|SAT files (*.sat)|*.sat|All files (*.*)|*.*|");

    if(dlgFile.DoModal() == IDOK)
	{	// if the user really wants to import a file..

 		CMDIChildWnd* ActiveChildWnd;
		BOOL is_maximized;
		CDocument* current_doc;
		C3DList *imported_obj;
		
		// find out the active child window; if no one, create new one and activate
		if( !(ActiveChildWnd = m_pMDIMainFrameWnd->MDIGetActive(&is_maximized)) )
		{
			CWinApp::OnFileNew();
			if( !(ActiveChildWnd = m_pMDIMainFrameWnd->MDIGetActive(&is_maximized)) )
				AfxMessageBox("Error: cannot open new child window");
		}			
		
		// get the document associated
		current_doc = ActiveChildWnd->GetActiveDocument();
		
		// the doc will be lost on import -> ask user yes/no
		if (current_doc->IsModified())
			;//MessageBox("The current project has unsaved changes\nAre you sure you want to discard changes?", "Warning", MB_ICONEXCLAMATION);
		
		// read file. Section copied from MFC CDocument::OnOpenDocument
		CFileException fe;
		CFile* pFile = current_doc->GetFile(dlgFile.GetPathName(),
			CFile::modeRead|CFile::shareDenyWrite, &fe);
		if (pFile == NULL)
		{
			current_doc->ReportSaveLoadException(dlgFile.GetPathName(), &fe,
				FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
				return FALSE;
		}

		current_doc->DeleteContents();
		current_doc->SetModifiedFlag();  // dirty during de-serialize

		CArchive loadArchive(pFile, CArchive::load | CArchive::bNoFlushOnDelete);
		loadArchive.m_pDocument = current_doc;
		loadArchive.m_bForceFlat = FALSE;
		TRY
		{
			CWaitCursor wait;
			if (pFile->GetLength() != 0)
				// *current_doc::Serialize(loadArchive);  this should import!
				imported_obj = CImport3DObj::Import3DObj(&loadArchive, dlgFile.GetFileExt());
			loadArchive.Close();
			current_doc->ReleaseFile(pFile, FALSE);
		}
		CATCH_ALL(e)
		{
			current_doc->ReleaseFile(pFile, TRUE);
			current_doc->DeleteContents();   // remove failed contents

			TRY
			{
				current_doc->ReportSaveLoadException(dlgFile.GetPathName(), e,
					FALSE, AFX_IDP_FAILED_TO_OPEN_DOC);
			}
			END_TRY
			DELETE_EXCEPTION(e);
			return FALSE;
		}
		END_CATCH_ALL

		current_doc->SetModifiedFlag(FALSE);     // start off with unmodified
	}

	return TRUE;
*/
	return FALSE;
}

void CFastModelApp::OnFileNew() 
{

	// the original CWinApp::OnFileNew does almost nothing but calling
	// CDocManager::OnFileNew. This in turn checks if there is more than
	// one document type (in that case prompts user for the required type)
	// and then calls CDocTemplate::OpenDocumentFile(NULL); the NULL parameter
	// means that the document must be created and not opened
	//
	// here the passage is skipped and CDocTemplate::OpenDocumentFile
	// is called directly

	// create a new document (if passed NULL as argument);
	// the function returns a pointer to the doc
	m_pModelViewTemplate->OpenDocumentFile(NULL);

	// and create the views
	m_pMDIMainFrameWnd->CreateMDIWindows();
}

// Warning: this function is called also by the interpreter thread.
// Should it modify some internal structure / data, should be made
// thread-safe
CFastModelDoc *CFastModelApp::GetFastModelDoc() 
{
	POSITION pos;
	CFastModelDoc* fastmodeldoc;
	
	pos = m_pModelViewTemplate->GetFirstDocPosition();
	fastmodeldoc = (CFastModelDoc*) m_pModelViewTemplate->GetNextDoc(pos); 

	// only for debug
	if( pos != NULL)
		ASSERT("More than one document type!");

	return fastmodeldoc;

}

CMainFrame *CFastModelApp::GetMainFrame() 
{
	return m_pMDIMainFrameWnd;
}

void CFastModelApp::OnFileCloseProj() 
{
	// TODO: Add your command handler code here
	ErrorProjMessage();
	
}

void CFastModelApp::OnFileNewProj() 
{
	// TODO: Add your command handler code here
	ErrorProjMessage();
	
}

void CFastModelApp::OnFileOpenProj() 
{
	// TODO: Add your command handler code here
	ErrorProjMessage();
	
}

void CFastModelApp::OnFileSaveAsProj() 
{
	// TODO: Add your command handler code here
	ErrorProjMessage();
	
}

void CFastModelApp::OnFileSaveProj() 
{
	// TODO: Add your command handler code here
	ErrorProjMessage();
	
}

void CFastModelApp::ErrorProjMessage() 
{
	AfxMessageBox("Sorry, FastModel will support\nFastModel projects in the future!", MB_ICONEXCLAMATION | MB_OK);
}

void CFastModelApp::OnAppExit() 
{
	AfxMessageBox("Just closing!");

	CWinApp::OnAppExit();
	
}

// customized version of DoWaitCursor, used to display a different
// wait cursors, the one with the arrow and the hourglass.
// The code is copied from the MCF source code and modified.
// TBC warning: can have side effects if used together with
// a standard wait cursor, i.e. a wait cursor changes in a
// arrow wait cursor and vice-versa
void CFastModelApp::DoArrowWaitCursor(int nCode) 
{
	HCURSOR hcurPrev;

	// 0 => restore, 1=> begin, -1=> end
	ASSERT(nCode == 0 || nCode == 1 || nCode == -1);
	ASSERT(m_hcurAppstarting != NULL);

	m_nArrowWaitCursorCount += nCode;
	if (m_nArrowWaitCursorCount > 0)
	{
		hcurPrev = ::SetCursor(m_hcurAppstarting);

		if (nCode > 0 && m_nArrowWaitCursorCount == 1)
			m_hcurArrowWaitCursorRestore = hcurPrev;
	}
	else
	{
		// turn everything off

		// prevent underflow
		m_nArrowWaitCursorCount = 0;     
		::SetCursor(m_hcurArrowWaitCursorRestore);
	}
}


void CAboutDlg::OnShowLicense() 
{
	CIntroDialog intro;

	intro.DoModal();	
}
