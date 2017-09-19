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


// PatranLexer.h : a FastCap lexer class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef PATRAN_LEXER_H
#define PATRAN_LEXER_H

#include <stdio.h>

#include "lexer.h"

// tokens from 258 to 299 and above 8156 are reserved 
// for standardization (see lexer.h)
#define FCL_FLOAT				301
#define FCL_TEXT				302
#define FCL_SPACE				303

class CPatranLexer : public CLexer 
{

// interface
public:

// implementation
protected:
	int recognizeTok(unsigned int *position);

};

#endif
