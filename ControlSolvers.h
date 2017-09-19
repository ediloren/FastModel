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


#if !defined(AFX_CONTROLSOLVERS_H__5B0F8E76_B9E4_4987_A00F_508F050224A9__INCLUDED_)
#define AFX_CONTROLSOLVERS_H__5B0F8E76_B9E4_4987_A00F_508F050224A9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ControlSolvers.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CControlSolvers view

class CControlSolvers 
{
public:
	void StartFastHenry();
	void RunFastHenry(CString pathName);
	void CloseFH();
	void StartFastCap();
	void RunFastCap(CString pathName);
	void CloseFC();
	void StartFasterCap();
	void RunFasterCap(CString pathName);
	void CloseFRC();


protected:
	void FindFastHenryHWND();
	void FindFastCapHWND();
	void FindFasterCapHWND();
	
	DWORD m_dwFastCapProcessId; 
    DWORD m_dwFastCapThreadId;
	BOOL m_bIsFastCap;
	DWORD m_dwFasterCapProcessId; 
    DWORD m_dwFasterCapThreadId;
	BOOL m_bIsFasterCap;
	DWORD m_dwFastHenryProcessId; 
    DWORD m_dwFastHenryThreadId;
	BOOL m_bIsFastHenry;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTROLSOLVERS_H__5B0F8E76_B9E4_4987_A00F_508F050224A9__INCLUDED_)
