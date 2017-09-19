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


// lexer.h : a generic lexer class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#define LEXER_MAX_BUF_LEN 4096
#define LEXER_MAX_TOK_LEN 256

// error messages
#define LEXER_ERR_EOF				-1
#define LEXER_ERR_BUFFER_OVERFLOW	-2

// tokens from 258 to 299 and above 8192 are here standardized
// (between 0 and 255 correspond to ascii codes)
// LEXER_NULL is used in token initialization
#define	LEXER_NULL					258
#define	LEXER_NEWLINE				259
#define LEXER_EOF					260

// error codes are composed by the LEXER_ERROR token or-ed with the 
// error code specification
#define LEXER_ERROR					8192
#define LEXER_TOKEN_OVERFLOW		1
#define LEXER_BUFFER_OVERFLOW		2
#define LEXER_UNKNOWN_TOKEN			4

class CLexer 
{

// interface
public:
	void initLexer();
	int moveForward(unsigned int *offset);
	BOOL rewindPosition(unsigned int *position, unsigned int offset);
	inline int getTok()
	{
		int tok;
		unsigned int position;

		position = 0;
		tok = recognizeTok(&position);

		moveForward(&position);

		return (tok);
	}
	inline int readTok(unsigned int *position)
	{
		return(recognizeTok(position));
	}

	char			m_sString[LEXER_MAX_TOK_LEN];
	int				m_iInteger;
	float			m_fFloat;
	unsigned long	m_ulHex;

// implementation
protected:
	virtual int getInputChar();
	virtual int recognizeTok(unsigned int *position);
	// read char from stream at future position 'position',
	// without removing it from buffer and automatically
	// increasing reading position to next char.
	// 'position' is an offset relative to last read char
	// (position = 0 gives the same char getChar() would return) 
	inline int readChar(unsigned int *position)
	{
		unsigned int i;

		// if position is too large, char cannot be read
		// TBC warning: this should be an internal error
		if (*position >= LEXER_MAX_BUF_LEN) {
			return LEXER_ERROR | LEXER_BUFFER_OVERFLOW;
		}
		else {
			// automatically position to next char
			i = *position;
			(*position)++;

			return( readCharFromBuf(i) );
		}
	}
	// get char from stream removing it from buffer and automatically
	// increasing getting position to next char
	inline int CLexer::getChar()
	{
		return (pullCharFromBuf());
	}
	// buffer is circular and fifo, so pulled char is the first
	inline int CLexer::pullCharFromBuf()
	{
		char ret;
		BOOL canRead;

		// if no chars in buffer, must really read it
		if( m_uiLen <= 0 )
			canRead = pushCharToBuf();
		else
			canRead = TRUE;

		// if impossible to add enough chars or pushCharToBuf returned
		// FALSE (should be equivalent) then we have reached EOF
		if( m_uiLen <= 0 || !canRead)
			return LEXER_EOF;

		ret = m_sBuffer[m_uiStart];
		m_uiStart = (m_uiStart + 1) % LEXER_MAX_BUF_LEN;
		m_uiLen--;

		return((int)ret);
	
	}
	// simply reads char  
	inline int CLexer::readCharFromBuf(unsigned int position)
	{
		unsigned int index;
		long i;
		BOOL canRead;

		// if not enough chars in buf, add them (if already enough, the
		// for cycle is entirely skipped)
		canRead = TRUE;
		for( i = position - m_uiLen; i >= 0 && canRead; i--)   
				canRead = pushCharToBuf();

		// if impossible to add enough chars or pushCharToBuf returned
		// FALSE (should be equivalent) then we have reached EOF
		if( position >= m_uiLen || !canRead )
			return LEXER_EOF;

		index = (m_uiStart + position) % LEXER_MAX_BUF_LEN;

		return((int)m_sBuffer[index]);
	}

	// buffer is circular and fifo, so pushed char is the last
	inline BOOL CLexer::pushCharToBuf()
	{
		int readChar;

		// read char from file and store in buffer
		readChar = getInputChar();
		if( readChar != EOF ) {
			m_sBuffer[(m_uiStart + m_uiLen) % LEXER_MAX_BUF_LEN] = (char) readChar;
			m_uiLen++;
			return TRUE;
		}
		else {
			m_sBuffer[(m_uiStart + m_uiLen) % LEXER_MAX_BUF_LEN] = 0;
			// signal EOF
			return FALSE;
		}
	}
	inline BOOL ischar(int code) { return( (code>=0 && code <=255) ? TRUE : FALSE ); };
	// must redefine because original function returns TRUE for code=-2
	inline BOOL isdigit(int code) { return( (code>='0' && code <='9') ? TRUE : FALSE ); };
	// must redefine because original function does not include (?) the '.'
	inline BOOL isgraph(int code) { return( (code>=33 && code <=126) ? TRUE : FALSE ); };

	unsigned int m_uiLen;
	unsigned int m_uiStart;
	unsigned int m_uiSpacePos;
	char m_sBuffer[LEXER_MAX_BUF_LEN];
	char m_sSpaceBuffer[LEXER_MAX_BUF_LEN];

};

#endif
