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


// CLexer generic lexer class - Enrico Di Lorenzo 2001/07/15
//

#include "stdafx.h"

#include "lexer.h"

void CLexer::initLexer()
{
	// initialize private vars
	m_uiLen = m_uiStart = 0;
}

// move forward the get position of 'offset' chars; offset is then reset to 0
int CLexer::moveForward(unsigned int *offset)
{
	unsigned int i;
	int c;

	c = 1;

	for( i=0; i<*offset && c!=0; i++)
		c = pullCharFromBuf();

	*offset = 0;

	// c it is not interesting except in case of premature exit,
	// in which case c = 0
	return c;
}

// rewind position of 'offset' chars
BOOL CLexer::rewindPosition(unsigned int *position, unsigned int offset)
{
	if( *position < offset ) {
		// TBC warinig: should be an internal error: cannot rewind
		// in the past over *position
		return FALSE;
	}

	*position -= offset;
	return TRUE;
}

// virtual functions to be redefined in derived classes

int CLexer::getInputChar()
{
	return(0);
}

int CLexer::recognizeTok(unsigned int *position)
{
	return 0;
}



