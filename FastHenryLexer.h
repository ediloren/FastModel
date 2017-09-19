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


// FastHenryLexer.h : a FastHenry lexer class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef FASTHENRY_LEXER_H
#define FASTHENRY_LEXER_H

#include <stdio.h>

#include "lexer.h"

// tokens from 258 to 299 and above 8156 are reserved 
// for standardization (see lexer.h)
#define	FHL_COMMENT		300
#define	FHL_DOT_UNI		301
#define	FHL_DOT_EXT		302
#define	FHL_DOT_FRE		303
#define	FHL_DOT_EQU		304
#define	FHL_DOT_DEF		305
#define	FHL_DOT_END		306
#define	FHL_NODE		307
#define	FHL_SEGMENT		308
#define	FHL_GROUND		309
#define	FHL_FLOAT		310
#define	FHL_SIGMA		311
#define	FHL_RHO			312
#define	FHL_NHINC		313
#define FHL_NWINC		314
#define FHL_WX			315
#define FHL_WY			316
#define FHL_WZ			317	
#define FHL_RW			318
#define FHL_RH			319
#define FHL_TEXT		320
#define FHL_NEWLINE		321
#define FHL_CONTINUE	322
#define FHL_THICK		323
#define FHL_GNDVERTEX	324
#define FHL_HOLE		325


class CFastHenryLexer : public CLexer 
{

// interface
public:

// implementation
protected:
	int recognizeTok(unsigned int *position);

};

#endif
