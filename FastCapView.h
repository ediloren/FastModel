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


// FastCapView.h : header file
//

#if !defined(AFX_FASTCAPVIEW_H__6D9AE488_4BB1_11C6_9276_54E814C10000__INCLUDED_)
#define AFX_FASTCAPVIEW_H__6D9AE488_4BB1_11C6_9276_54E814C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// include for the CColorEditView class
#include "ColorEditView.h"
#include "ImportFastCap.h"

#define FCV_ISFASTCAP	0
#define FCV_ISPATRAN	1

#define FCV_COLOR_TABLE_LEN		256

// anticipate definitions (cannot include fastmodelDoc.h in FastCapView.h and
// vice-versa: a class will always be defined before the other one)
class CFastModelDoc;

class CFastCapView : public CColorEditView
{

// the CImportFastCap class is friend because needs strong link with the 
// edit view it imports to (e.g. to color lines)
	friend class CImportFastCap;
// CFastModelDoc needs access to private members such as OnFileSaveFastCapAll() etc.
	friend class CFastModelDoc;


protected:
	// protected constructor used by dynamic creation
	CFastCapView();     
	DECLARE_DYNCREATE(CFastCapView)

// Operations
public:
	void InitFastCap();
	CFastModelDoc* GetDocument() const;
	int ColorLines(long baselineno, long startline, long endline);
	int importInView(unsigned int operation, FILE *fp, C3DShell *model, 
							   long baselineno, long startline, long endline,
								float xtran, float ytran, float ztran, bool hascolor = false, unsigned long color = 0);
	void colorPanelsFromCharges(C3DShell *model);
	void importFastCap(unsigned int operation, C3DShell *model, FILE *fp, 
						float xtran, float ytran, float ztran, bool hascolor = false, unsigned long color = 0);
	void updateFastCap(C3DShell *model, float xtran, float ytran, float ztran, bool hascolor = false, unsigned long color = 0);
	BOOL SaveAsFastCapFile();
	void AddFileToWatchList();

	static CFastCapView *m_pFastCapMain;
	unsigned long m_ulTreeToken;
	unsigned long m_ulFileMonTok;
	char m_cViewType;
	bool m_bInputHasCharge;
	float m_fMinCharge, m_fMaxCharge;

// Implementation
protected:
	void setRTFLine();
	void InitRTF();
	void SaveFastCapFile();
	bool FileCloseFastCap();

	BOOL isMainView;
	unsigned char colorTable[FCV_COLOR_TABLE_LEN][3];

protected:
	virtual ~CFastCapView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFastCapView)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

	HMENU GetContextMenu( WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE* lpchrg );


	// Generated message map functions
protected:
	//{{AFX_MSG(CFastCapView)
	afx_msg void OnImportFastCapFile();
	afx_msg void OnFileSaveFastCap();
	afx_msg void OnFileSaveAsFastCap();
	afx_msg void OnFastCapUpdateModel();
	afx_msg void OnClose();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditPaste();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnFileSaveFastCapAll();
	afx_msg void OnFileCloseFastCap();
	afx_msg void OnFileNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CFastCapView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FASTCAPVIEW_H__6D9AE488_4BB1_11C6_9276_54E814C10000__INCLUDED_)
