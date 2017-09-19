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


// CFastHenryLexer, a FastHenry lexer class - Enrico Di Lorenzo 2001/12/27
//

#include "stdafx.h"

#include "FastHenryLexer.h"

// remark: all string comparison are case-insensitive, because FastHenry is case-insensitive
int CFastHenryLexer::recognizeTok(unsigned int *position)
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
		// end of file
		//
		if( c==LEXER_EOF ) {
			m_sString[0] = '\0';
			return LEXER_EOF;
		}

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
		// comment token
		//
		if( c=='*' ) {
			m_sString[0] = (char)c;

			i = 0;
			do {
				i++;
				ctmp = readChar(position);
				m_sString[i] = (char)ctmp; 
			} while (ctmp != '\n' && ischar(ctmp) && i < LEXER_MAX_TOK_LEN-1);

			if( i >= LEXER_MAX_TOK_LEN ) 
				return LEXER_ERROR | LEXER_TOKEN_OVERFLOW;
			else {
				m_sString[i] = '\0';

				// if error 
				if( ctmp == LEXER_BUFFER_OVERFLOW )
					return LEXER_ERROR | LEXER_BUFFER_OVERFLOW;

				// rewind one position, because read one char too much
				rewindPosition(position, 1);

				return FHL_COMMENT;
			}
		}

		//
		// all commands starting with '.'
		//
		if( c=='.' ) {
			m_sString[0] = (char)c;

			// read the first three significative chars
			m_sString[1] = readChar(position);
			m_sString[2] = readChar(position);
			m_sString[3] = readChar(position);
			m_sString[4] = '\0';

			if(_stricmp(m_sString, ".uni") == 0) {
				i = 3;
				do {
					i++;
					ctmp = readChar(position);
					m_sString[i] = (char)ctmp; 
				} while (ctmp != '\n' && ctmp != '\r' && ctmp != '\t' && ctmp != ' ' &&
						ischar(ctmp) && i < LEXER_MAX_TOK_LEN-1);

				if( i >= LEXER_MAX_TOK_LEN ) 
					return LEXER_ERROR | LEXER_TOKEN_OVERFLOW;
				else {
					m_sString[i] = '\0';

					// if error 
					if( ctmp == LEXER_BUFFER_OVERFLOW )
						return LEXER_ERROR | LEXER_BUFFER_OVERFLOW;

					// rewind one position, because read one char too much
					rewindPosition(position, 1);

					return FHL_DOT_UNI;
				}
			}
			else if(_stricmp(m_sString, ".ext") == 0) {
				i = 3;
				do {
					i++;
					ctmp = readChar(position);
					m_sString[i] = (char)ctmp; 
				} while (ctmp != '\n' && ctmp != '\r' && ctmp != '\t' && ctmp != ' ' &&
						ischar(ctmp) && i < LEXER_MAX_TOK_LEN-1);

				if( i >= LEXER_MAX_TOK_LEN ) 
					return LEXER_ERROR | LEXER_TOKEN_OVERFLOW;
				else {
					m_sString[i] = '\0';

					// if error 
					if( ctmp == LEXER_BUFFER_OVERFLOW )
						return LEXER_ERROR | LEXER_BUFFER_OVERFLOW;

					// rewind one position, because read one char too much
					rewindPosition(position, 1);

					return FHL_DOT_EXT;
				}
			}
			else if(_stricmp(m_sString, ".fre") == 0) {
				i = 3;
				do {
					i++;
					ctmp = readChar(position);
					m_sString[i] = (char)ctmp; 
				} while (ctmp != '\n' && ctmp != '\r' && ctmp != '\t' && ctmp != ' ' &&
						ischar(ctmp) && i < LEXER_MAX_TOK_LEN-1);

				if( i >= LEXER_MAX_TOK_LEN ) 
					return LEXER_ERROR | LEXER_TOKEN_OVERFLOW;
				else {
					m_sString[i] = '\0';

					// if error 
					if( ctmp == LEXER_BUFFER_OVERFLOW )
						return LEXER_ERROR | LEXER_BUFFER_OVERFLOW;

					// rewind one position, because read one char too much
					rewindPosition(position, 1);

					return FHL_DOT_FRE;
				}
			}
			else if(_stricmp(m_sString, ".equ") == 0) {
				i = 3;
				do {
					i++;
					ctmp = readChar(position);
					m_sString[i] = (char)ctmp; 
				} while (ctmp != '\n' && ctmp != '\r' && ctmp != '\t' && ctmp != ' ' &&
						ischar(ctmp) && i < LEXER_MAX_TOK_LEN-1);

				if( i >= LEXER_MAX_TOK_LEN ) 
					return LEXER_ERROR | LEXER_TOKEN_OVERFLOW;
				else {
					m_sString[i] = '\0';

					// if error 
					if( ctmp == LEXER_BUFFER_OVERFLOW )
						return LEXER_ERROR | LEXER_BUFFER_OVERFLOW;

					// rewind one position, because read one char too much
					rewindPosition(position, 1);

					return FHL_DOT_EQU;
				}
			}
			else if(_stricmp(m_sString, ".def") == 0) {
				i = 3;
				do {
					i++;
					ctmp = readChar(position);
					m_sString[i] = (char)ctmp; 
				} while (ctmp != '\n' && ctmp != '\r' && ctmp != '\t' && ctmp != ' ' &&
						ischar(ctmp) && i < LEXER_MAX_TOK_LEN-1);

				if( i >= LEXER_MAX_TOK_LEN ) 
					return LEXER_ERROR | LEXER_TOKEN_OVERFLOW;
				else {
					m_sString[i] = '\0';

					// if error 
					if( ctmp == LEXER_BUFFER_OVERFLOW )
						return LEXER_ERROR | LEXER_BUFFER_OVERFLOW;

					// rewind one position, because read one char too much
					rewindPosition(position, 1);

					return FHL_DOT_DEF;
				}
			}
			else if(_stricmp(m_sString, ".end") == 0) {
				i = 3;
				do {
					i++;
					ctmp = readChar(position);
					m_sString[i] = (char)ctmp; 
				} while (ctmp != '\n' && ctmp != '\r' && ctmp != '\t' && ctmp != ' ' &&
						ischar(ctmp) && i < LEXER_MAX_TOK_LEN-1);

				if( i >= LEXER_MAX_TOK_LEN ) 
					return LEXER_ERROR | LEXER_TOKEN_OVERFLOW;
				else {
					m_sString[i] = '\0';

					// if error 
					if( ctmp == LEXER_BUFFER_OVERFLOW )
						return LEXER_ERROR | LEXER_BUFFER_OVERFLOW;

					// rewind one position, because read one char too much
					rewindPosition(position, 1);

					return FHL_DOT_END;
				}
			}
			else {
					// rewind three positions, because read three chars too much
					rewindPosition(position, 3);
			}
		}
	
		//
		// all commands starting with 'n'
		//
		if( c=='n' || c=='N' ) {
			m_sString[0] = (char)c;

			// the following two must stay here because of possible confusion 
			// with node names (begins with 'n') 

			// read the first four significative chars
			m_sString[1] = readChar(position);
			m_sString[2] = readChar(position);
			m_sString[3] = readChar(position);
			m_sString[4] = readChar(position);
			m_sString[5] = '\0';

			if(_stricmp(m_sString, "nhinc") == 0) {
					return FHL_NHINC;
			}
			else if(_stricmp(m_sString, "nwinc") == 0) {
					return FHL_NWINC;
			}
			// otherwise MUST be a node name!
			else {
				// rewind the four read positions
				rewindPosition(position, 4);

				i = 0;
				do {
					i++;
					ctmp = readChar(position);
					m_sString[i] = (char)ctmp; 
				} while (ctmp != '\n' && ctmp != '\r' && ctmp != '\t' && ctmp != ' ' && ctmp != '=' &&
						ischar(ctmp) && i < LEXER_MAX_TOK_LEN-1);

				if( i >= LEXER_MAX_TOK_LEN ) 
					return LEXER_ERROR | LEXER_TOKEN_OVERFLOW;
				else {
					m_sString[i] = '\0';
				}

				// if error 
				if( ctmp == LEXER_BUFFER_OVERFLOW )
					return LEXER_ERROR | LEXER_BUFFER_OVERFLOW;

				// rewind one position, because read one char too much
				rewindPosition(position, 1);

				return FHL_NODE;
			}
		}	

		//
		// segment identifiers 
		//
		if( c=='e' || c=='E' ) {
			m_sString[0] = (char)c;

			i = 0;
			do {
				i++;
				ctmp = readChar(position);
				m_sString[i] = (char)ctmp; 
			} while (ctmp != '\n' && ctmp != '\r' && ctmp != '\t' && ctmp != ' ' && 
					ischar(ctmp) && i < LEXER_MAX_TOK_LEN-1);

			if( i >= LEXER_MAX_TOK_LEN ) 
				return LEXER_ERROR | LEXER_TOKEN_OVERFLOW;
			else {
				m_sString[i] = '\0';
			}

			// if error 
			if( ctmp == LEXER_BUFFER_OVERFLOW )
				return LEXER_ERROR | LEXER_BUFFER_OVERFLOW;

			// rewind one position, because read one char too much
			rewindPosition(position, 1);

			return FHL_SEGMENT;
		}	

		//
		// ground identifiers 
		//
		if( c=='g' || c=='G' ) {
			m_sString[0] = (char)c;

			i = 0;
			do {
				i++;
				ctmp = readChar(position);
				m_sString[i] = (char)ctmp; 
			} while (ctmp != '\n' && ctmp != '\r' && ctmp != '\t' && ctmp != ' ' && 
					ischar(ctmp) && i < LEXER_MAX_TOK_LEN-1);

			if( i >= LEXER_MAX_TOK_LEN ) 
				return LEXER_ERROR | LEXER_TOKEN_OVERFLOW;
			else {
				m_sString[i] = '\0';
			}

			// if error 
			if( ctmp == LEXER_BUFFER_OVERFLOW )
				return LEXER_ERROR | LEXER_BUFFER_OVERFLOW;

			// rewind one position, because read one char too much
			rewindPosition(position, 1);

			return FHL_GROUND;
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
							state = 8;

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

					case 8:
						ctmp = readChar(position);
						m_sString[i++] = (char) ctmp;

						if( isdigit(ctmp) )
							state = 4;
						else if ( ctmp == 'e' || ctmp == 'E' )
							state = 5;
						else if( ctmp == LEXER_BUFFER_OVERFLOW)
							state = 1002;
						else
							// in this case, there was only a SINGLE dot;
							// this is NOT considered a number
							state = 1000;

						break;
				}
			}

			// if correct ending state, token has been recognized
			if ( state == 1001 ) {
				// rewind one position, because read one char too much  
				rewindPosition(position, 1);
				m_sString[i-1] = '\0';
				sscanf(m_sString, "%f", &m_fFloat);
				return FHL_FLOAT;
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
		// 'continue' directive 
		//
		if( c=='+' ) {
			m_sString[0] = (char)c;
			m_sString[1] = '\0';

			return FHL_CONTINUE;
		}	

		//
		// all other keywords
		//
		m_sString[0] = (char)c;
		m_sString[1] = readChar(position);
		m_sString[2] = readChar(position);
		m_sString[3] = readChar(position);
		m_sString[4] = readChar(position);
		m_sString[5] = '\0';

		// five letter tokens
		if(_stricmp(m_sString, "sigma") == 0) {
			return FHL_SIGMA;
		}
		else if(_stricmp(m_sString, "thick") == 0) {
			return FHL_THICK;
		}

		// four letter tokens
		rewindPosition(position, 1);
		m_sString[4] = '\0';
		if(_stricmp(m_sString, "hole") == 0) {
			return FHL_HOLE;
		}

		// three letter tokens
		rewindPosition(position, 1);
		m_sString[3] = '\0';
		if(_stricmp(m_sString, "rho") == 0) {
			return FHL_RHO;
		}

		// two letter tokens
		rewindPosition(position, 1);
		m_sString[2] = '\0';
		if(_stricmp(m_sString, "wx") == 0) {
			return FHL_WX;
		}
		else if(_stricmp(m_sString, "wy") == 0) {
			return FHL_WY;
		}
		else if(_stricmp(m_sString, "wz") == 0) {
			return FHL_WZ;
		}
		else if(_stricmp(m_sString, "rw") == 0) {
			return FHL_RW;
		}
		else if(_stricmp(m_sString, "rh") == 0) {
			return FHL_RH;
		}
		else if( (c == 'x' || c == 'y' || c == 'z' || c == 'X' || c == 'Y' || c == 'Z') &&
				(m_sString[1] == '1' || m_sString[1] == '2' || m_sString[1] == '3') ) {
			return FHL_GNDVERTEX;
		}

		// one letter tokens
		rewindPosition(position, 1);
		m_sString[1] = '\0';
		// characters
		if(c == 'x' || c == 'y' || c == 'z' || c == 'w' || c == 'h' || c == '=' || 
			c == 'X' || c == 'Y' || c == 'Z' || c == 'W' || c == 'H') {
			return(tolower(c));
		}
		// graph symbols
		if(c == '(' || c == ')' || c == ',') {
			return(c);
		}

		// 
		// text
		//
		// uses state diagram representing the following flex statement:
		// [a-zA-Z_][a-zA-Z0-9_]*
		//
		// FHL_TEXT is used for all 'slow' reading of different keywords
		// REMARK: text beginning with 'e', 'g', 'n' cannot be read here
		// because is interpreted as a segment, ground plane or node respectively!
		
		// if c is any kind of alphabetic character but space and escape ones
		if( isalpha(c) || c=='_' ) {
			m_sString[0] = (char)c;

			i = 0;
			do {
				i++;
				ctmp = readChar(position);
				m_sString[i] = (char)ctmp;
			} while ( (isalpha(ctmp) || isdigit(ctmp) || ctmp=='_' || ctmp=='.') && i < LEXER_MAX_TOK_LEN);
	
			if( i >= LEXER_MAX_TOK_LEN ) 
				return LEXER_ERROR;
			else if( ctmp == LEXER_BUFFER_OVERFLOW )
				return LEXER_ERROR | LEXER_BUFFER_OVERFLOW;
			else {
				// rewind one position, because read one char too much 
				rewindPosition(position, 1);
				m_sString[i] = '\0';
				return FHL_TEXT;
			}
		}

		else {
			// 
			// any other char
			//
			m_sString[0] = (char)c;
			m_sString[1] = '\0';
			return LEXER_ERROR | LEXER_UNKNOWN_TOKEN;
		}

	// repeat until an exit (return) condition is met; anyway, this statement
	// should be never reached
	// (exit is guaranteed by EOF condition)
	} while (1);
	
}
