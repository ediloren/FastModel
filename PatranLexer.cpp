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


// CPatranLexer, a Patran lexer class - Enrico Di Lorenzo 2001/09/17
//

#include "stdafx.h"

#include "PatranLexer.h"

int CPatranLexer::recognizeTok(unsigned int *position)
{
	unsigned int start_pos, first_pos, i;
	int c, ctmp;
	int state;

	// initialize the space buffer (keeps the number of spaces 
	// before a token: spaces are not returned as tokens but skipped)
	m_uiSpacePos = 0;
	m_sSpaceBuffer[m_uiSpacePos] = '\0';

	do {
		start_pos = *position;

		c = readChar(position);

		first_pos = *position;

		// 
		// space or tab (must be first, as most frequent)
		//
		if( c == ' ' || c == '\t' ) {
			m_sSpaceBuffer[m_uiSpacePos] = (char)c;
			m_uiSpacePos++;
			m_sSpaceBuffer[m_uiSpacePos] = '\0';
			// does not return anything! Spaces are skipped by the lexer,
			// so continue to next (non-space) token.
			continue;
		}

		// 
		// newline
		//
		if( c=='\r' ) {
			m_sString[0] = (char)c;
			ctmp = readChar(position);
			m_sString[1] = (char) ctmp;

			// if not a windows newline, rewind position and continue searching
			if( ctmp != '\n' ) {
				*position = first_pos;
				if( ctmp == LEXER_BUFFER_OVERFLOW )
					return LEXER_ERROR | LEXER_BUFFER_OVERFLOW;
			}
			else {
				m_sString[2] = '\0';
				return LEXER_NEWLINE;
			}
		}
		else if( c=='\n' ) {
			m_sString[0] = (char)c;
			m_sString[1] = '\0';
			return LEXER_NEWLINE;
		}

		// 
		// float value
		//
		// uses state diagram representing the following flex statement:
		// ([+-]?[0-9]+)?("."[0-9]*)?(e[+-]?[0-9]+)? 
		// with the slight modification that a number cannot begin with 'e'
		// and cannot end with a 'e' followed by anything but a number
		// 
		if( c=='+' || c=='-' || c=='.' || isdigit(c) ) {
			i = 0;
			state = 1;

			while (state < 1000 && i < LEXER_MAX_TOK_LEN-1) {
				switch (state) {
					case 1:
						m_sString[i++] = (char)c;
					
						if( c=='+' || c=='-' )
							state = 2;
						else if( isdigit(c) ) 
							state = 3;
						else if( c=='.' )
							state = 4;

						break;

					case 2:
						ctmp = readChar(position);
						m_sString[i++] = (char) ctmp;

						if( isdigit(ctmp) )
							state = 3;
						else if( ctmp == LEXER_BUFFER_OVERFLOW )
							state = 1002;
						else
							state = 1000;

						break;

					case 3:
						ctmp = readChar(position);
						m_sString[i++] = (char) ctmp;

						if( isdigit(ctmp) )
							state = 3;
						else if ( ctmp == '.' )
							state = 4;
						else if( ctmp == LEXER_BUFFER_OVERFLOW)
							state = 1002;
						else
							state = 1001;

						break;

					case 4:
						ctmp = readChar(position);
						m_sString[i++] = (char) ctmp;

						if( isdigit(ctmp) )
							state = 4;
						else if ( ctmp == 'e' || ctmp == 'E' )
							state = 5;
						else if( ctmp == LEXER_BUFFER_OVERFLOW)
							state = 1002;
						else
							state = 1001;

						break;

					case 5:
						ctmp = readChar(position);
						m_sString[i++] = (char) ctmp;

						if( ctmp=='+' || ctmp=='-' )
							state = 6;
						else if( isdigit(ctmp) )
							state = 7;
						else if( ctmp == LEXER_BUFFER_OVERFLOW)
							state = 1002;
						else
							state = 1000;

						break;

					case 6:
						ctmp = readChar(position);
						m_sString[i++] = (char) ctmp;

						if( isdigit(ctmp) )
							state = 7;
						else if( ctmp == LEXER_BUFFER_OVERFLOW)
							state = 1002;
						else
							state = 1000;

						break;

					case 7:
						ctmp = readChar(position);
						m_sString[i++] = (char) ctmp;

						if( isdigit(ctmp) )
							state = 7;
						else if( ctmp == LEXER_BUFFER_OVERFLOW)
							state = 1002;
						else
							state = 1001;

						break;
				}
			}

			// if correct ending state, token has been recognized
			if ( state == 1001 ) {
				// rewind one position, because read one char too much  
				rewindPosition(position, 1);
				m_sString[i-1] = '\0';
				sscanf(m_sString, "%f", &m_fFloat);
				return FCL_FLOAT;
			}

			// if the token was not recognized, rewind position and continue searching
			// for tokens
			else if ( state == 1000 )
				*position = first_pos;
			// internal buffer overflow error
			else if ( state == 1002 )
				return LEXER_ERROR | LEXER_BUFFER_OVERFLOW;
			// in any other case, generic error
			else 
				return LEXER_ERROR;
		}

		// 
		// name or number
		//
		// uses state diagram representing the following flex statement:
		// [^ ]+
		//
		//
		// if c is any kind of character but space and escape ones
		if( isgraph(c) != 0 ) {
			m_sString[0] = (char)c;

			i = 0;
			do {
				i++;
				ctmp = readChar(position);
				m_sString[i] = (char)ctmp;
			} while (isgraph(ctmp) && ischar(ctmp) && i < LEXER_MAX_TOK_LEN);
	
			if( i >= LEXER_MAX_TOK_LEN ) 
				return LEXER_ERROR;
			else if( ctmp == LEXER_BUFFER_OVERFLOW )
				return LEXER_ERROR | LEXER_BUFFER_OVERFLOW;
			else {
				// rewind one position, because read one char too much 
				rewindPosition(position, 1);
				m_sString[i] = '\0';
				return FCL_TEXT;
			}
		}

		// 
		// end of file
		//
		if( c==LEXER_EOF ) {
			m_sString[0] = '\0';
			return LEXER_EOF;
		}

		else {
			// 
			// any other char
			//
			m_sString[0] = (char)c;
			m_sString[1] = '\0';
			return LEXER_ERROR;
		}

	// repeat until an exit (return) condition is met; anyway, this statement
	// should be never reached
	// (exit is guaranteed by EOF condition)
	} while (1);
	
}


