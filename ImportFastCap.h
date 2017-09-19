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


// ImportFastCap.h : FastCap import interface
//
/////////////////////////////////////////////////////////////////////////////

#ifndef IMPORT_FASTCAP_H
#define IMPORT_FASTCAP_H

#include <stdio.h>

#include "ColorEditView.h"
#include "FastCapLexer.h"
#include "ImportInColorView.h"

// these are the codes for setColor
#define IFC_COMMENT		1
#define IFC_CONTROL		2
#define IFC_ELEMENT		3
#define IFC_ERROR		4
#define IFC_DEFAULT		5
#define IFC_TEXT		6

// defines for 'read' routines answers
#define IFC_GO			0
#define IFC_NONGO		1

#define IFC_PATRAN_EXTENSION ".neu"

class CImportFastCap :	public CImportInColorView, public CFastCapLexer
{
public:
	CImportFastCap();

protected:
	char readFloat(float *result);
	char readFloatOrHex(float *result, unsigned long *hex);
	char readHex(unsigned long *hex);
	char readText(char *result, int maxchar);
	char readTextOrNumber(char *result, int maxchar);
	char readPlusOrMinus(char *result);
	char readPerm(float *resultRe, float *resultIm);
//	char readChargeDensity(float *result);

	float 	m_fXtran;
	float 	m_fYtran;
	float 	m_fZtran;
	C3DShell *m_pCurrShell;
	char m_cReadRes;
	bool m_bHascolor;
	unsigned long m_ulColor;

// CImportInColorView overrides
public:
	// overloaded import function
	int import(unsigned int operation, FILE *fp, CColorEditView *colorEditView,
		C3DShell *model, long baselineno, long startline, long endline,
		float xtran, float ytran, float ztran, bool hascolor = false, unsigned long color = 0);
							 
protected:
	void setColor(unsigned int colorIndex);
	char processLine();

// CLexer overrides
	inline int getInputChar();
	inline int getTok();
	inline int readTok(unsigned int *position);
	inline int moveForward(unsigned int *position);
};

#endif
