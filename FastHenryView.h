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


// FastHenryView.h : header file
//

#if !defined(AFX_FASTHENRYVIEW_H__6D9AE486_4BB1_11C6_9276_54E814C10000__INCLUDED_)
#define AFX_FASTHENRYVIEW_H__6D9AE486_4BB1_11C6_9276_54E814C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// include for the CColorEditView class
#include "ColorEditView.h"

// anticipate definitions (cannot include fastmodelDoc.h in FastCapView.h and
// vice-versa: a class will always be defined before the other one)
class CFastModelDoc;

// function needed to control remote FastHenry window
static BOOL CALLBACK EnumWindowsProc(
		HWND hwnd,      // handle to parent window
		LPARAM lParam   // application-defined value
		);

/////////////////////////////////////////////////////////////////////////////
// CFastHenryView view

class CFastHenryView : public CColorEditView
{

// the CImportFastHenry class is friend because needs strong link with the 
// edit view it imports to (e.g. to color lines)
	friend class CImportFastHenry;
// CFastModelDoc needs access to private members such as OnFileSaveFasthenry() etc.
	friend class CFastModelDoc;

protected:
	CFastHenryView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CFastHenryView)

// Attributes
public:
	static CFastHenryView *m_pFastHenryMain;

protected:
	unsigned long m_commentColor, m_dotColor, m_elementColor;
	unsigned long m_errorColor, m_defaultColor;
	BOOL isMainView;

// Operations
public:
	CFastModelDoc* GetDocument() const;
	int ColorLines(long baselineno, long startline, long endline);
	BOOL SaveAsFastHenryFile();

protected:
	void SaveFastHenryFile();

protected:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFastHenryView)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

	HMENU GetContextMenu( WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE* lpchrg );
	void InitRTF();
	void ColorAndReplace(long baselineno, long startline, long endline);
	BOOL IsContinuing(char *str);
	BOOL IsBlank(char *str);
	BOOL IsComment(char *str);

// Implementation
protected:
	virtual ~CFastHenryView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


	// Generated message map functions
protected:
	//{{AFX_MSG(CFastHenryView)
	afx_msg void OnImportFastHenryFile();
	afx_msg void OnFileSaveFasthenry();
	afx_msg void OnFastHenryUpdateModel();
	afx_msg void OnFileSaveAsFasthenry();
	afx_msg void OnClose();
	afx_msg void OnFileCloseFastHenry();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CFastHenryView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FASTHENRYVIEW_H__6D9AE486_4BB1_11C6_9276_54E814C10000__INCLUDED_)
