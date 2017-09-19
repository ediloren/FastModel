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


// fastmodelDoc.h : interface of the CFastModelDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FASTMODELDOC_H__78E7EC5B_3755_11D4_A718_444553540000__INCLUDED_)
#define AFX_FASTMODELDOC_H__78E7EC5B_3755_11D4_A718_444553540000__INCLUDED_


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// include for import
#include "Geometry.h"
#include "FileChangeEvent.h"
#include "FastCapView.h"
#include "FastHenryView.h"

#define FMD_MAX_FC_VIEWS	1000
#define FMD_MAX_PATH_LEN	512
#define FMD_MAX_NAME_LEN	512

#define FMD_FASTCAP_FILE	0
#define FMD_FASTHENRY_FILE	1

// anticipate definitions (cannot include fastmodelDoc.h in FastCapView.h and
// vice-versa: a class will always be defined before the other one)
//class CFastHenryView;
//class CFastCapView;
// for use in CFileChangeMonitor
class CFastModelDoc;

/////////////////////////////////////////////////////
// class CImport3DObj
// this class contains the functions to import an external
// 3D objects description (e.g. from autocad)
class CImport3DObj : public CObject
{

public:
    static void Import3DObj(CArchive *ar, CString strExtension );

private:
    void ImportDXF(CString strPathName);
	void ImportSAT(CString strPathName);

};

class CFileChangeMonitor : public CFileChangeEvent
{
private:
	// override virtual function
	void OnFileAlarm(unsigned int nAlarm, const std::string &sFileName, 
					unsigned int fileCategory, unsigned long fileCode);

public:

};

// costruct a map where the key is a file token and and the data is the file name 
typedef std::map<unsigned long, CFastCapView *> FMDMonitorFiles;

class CFastModelDoc : public COleDocument
{

friend class CFastCapView;

protected: // create from serialization only
	CFastModelDoc();
	DECLARE_DYNCREATE(CFastModelDoc)
	LRESULT OnFileChangeNotify(WPARAM fileCategory, LPARAM fileCode);

// Operations
public:
	BOOL addFastCapView(CFastCapView *view);
	BOOL CloseFastCapView(CFastCapView *view);
	BOOL CloseFastCapView(int viewnumber);
	BOOL existsAnyModel();
	CFastCapView *getFastCapViewFromName(char *name);
	CFastCapView *getFastCapViewFromTreeTok(unsigned long token, int *viewnumber);
	void updateIsFCModified();
	CFastCapView *getFastCapMainView();
	CFastHenryView *getFastHenryMainView();
	CFastCapView *NewFastCapView(char *name, bool filealreadyexist = false);
	BOOL UserRemoveAllFCViews();
	void RemoveAllFCViews();
	bool LoadWindowState();
	void SaveWindowState();

	C3DShell *m_pFastHenryModel;
	C3DShell *m_pFastCapModel;
	int m_iFCViewNum;
	CFastCapView *m_pFastCapView[FMD_MAX_FC_VIEWS];
	BOOL m_bIsFCModified;
	CFileChangeMonitor m_clFileMonitor;
	unsigned long m_ulFileMonTokCount;
	FMDMonitorFiles m_stdMonFilesMap;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFastModelDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

	virtual void UpdateFrameCounts();

// Implementation
public:
	virtual ~CFastModelDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CFastModelDoc)
	afx_msg void OnFileNewFastCap();
	afx_msg void OnRunFastCap();
	afx_msg void OnRunFastHenry();
	afx_msg void OnRunFasterCap();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FASTMODELDOC_H__78E7EC5B_3755_11D4_A718_444553540000__INCLUDED_)
