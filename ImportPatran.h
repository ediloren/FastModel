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


// ImportPatran.h : Patran import interface
//
/////////////////////////////////////////////////////////////////////////////

#ifndef IMPORT_PATRAN_H
#define IMPORT_PATRAN_H

#include <stdio.h>

#include "ColorEditView.h"
#include "PatranLexer.h"
#include "ImportInColorView.h"

// these are the codes for setColor
//
// PATRAN neutral file has no comments; the token is then
// used to signal skipped parts of input file
#define IP_COMMENT		1
#define IP_CONTROL		2
#define IP_ELEMENT		3
#define IP_ERROR		4
#define IP_DEFAULT		5
#define IP_TEXT		6

// defines for 'read' routines answers
#define IP_GO			0
#define IP_NONGO		1

#define MAX_TEXT_LEN	1024

// import PATRAN classes
class CPatranNode 
{
public:
	long m_lId;
	float m_fX, m_fY, m_fZ;
};

class CPatranElement
{
public:
	long m_lId;
	char m_cNodenum;
	long m_lNodes[4];
};

class CPatranCFEG
{
public:
	long m_lID, m_lNELS, m_lLPH, m_lLPH_ID;
	long m_lLSHAPE, m_lNODES, m_lICONF, m_lNDIM;
	CPatranCFEG *m_pNext, *m_pPrev;
	long *m_pElemList;
};

class CImportPatran :	public CImportInColorView, public CPatranLexer
{
public:
	CImportPatran();

protected:
	char readFloat(float *result);
	char readLong(long *result);
	char readText(char *result, int maxchar);
	char readTextOrNumber(char *result, int maxchar);

	float 	m_fXtran;
	float 	m_fYtran;
	float 	m_fZtran;
	C3DShell *m_pCurrShell;
	char m_cReadRes;

// CImportInColorView overrides
public:
	// overloaded import function
	int import(unsigned int operation, FILE *fp, CColorEditView *colorEditView,
		C3DShell *model, long baselineno, long startline, long endline,
		float xtran, float ytran, float ztran);
							 
protected:
	void setColor(unsigned int colorIndex);
	char processLine();
	void convertPatran2Model();
	void convertPatchToFaces(long nels, long *elemList);

	CPatranNode *m_pNodes;
	long m_lNodeIndex;
	CPatranElement *m_pElements;
	long m_lElementIndex;
	CPatranCFEG *m_pCFEGstart;
	CPatranCFEG *m_pCFEGcurrent;

// CLexer overrides
	inline int getInputChar();
	inline int getTok();
	inline int  readTok(unsigned int *position);
	inline int  moveForward(unsigned int *position);
};

#endif
