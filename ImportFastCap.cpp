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


// CImportFastCap import class - Enrico Di Lorenzo 2001/08/05
//

#include "stdafx.h"

#include "ImportFastCap.h"
#include "fastmodelDoc.h"
#include "FastCapView.h"
#include "fastmodel.h"

#define IFC_LASTERROR 58
#define IFC_LASTWARNING ?
#define IFC_LASTINTERNALERROR ????

CImportFastCap::CImportFastCap()
{
	strcpy(m_sErrorMsg, "IMPFASTCAP2"); 
}

// override standard lexer getInputChar function
inline int CImportFastCap::getInputChar()
{ 
	char readChar;
	long startcharpos, endcharpos;
	int linelen, numchars;
	static char str[IICV_MAX_LINE_LEN];

	// read from file
	if(m_uiOperation & IICV_FILE_SOURCE) {
		return(fgetc(m_fp)); 
	}
	// read from view
	else {
		// if the input buffer is empty (whole previous line has been read)
		if (m_iInputBufPos == 0) {
			// if finished reading lines, signal 0 chars read and exit
			if(m_lViewLineNum > m_lEndLine) {
				return(EOF);
			}
			// determinate start and end positions
			startcharpos = m_pColorEditView->GetRichEditCtrl().LineIndex(m_lViewLineNum); 
			endcharpos = m_pColorEditView->GetRichEditCtrl().LineIndex(m_lViewLineNum + 1);
			if( endcharpos == -1) 
				endcharpos = m_pColorEditView->GetTextLength();

			if( startcharpos == -1) {
				genError(1008, "internal error");
				return(EOF); 
			}

			// retrieve the content of the current line
			numchars = m_pColorEditView->GetRichEditCtrl().GetLine(m_lViewLineNum, str, IICV_MAX_LINE_LEN - 1);

			// trim line if too long
			//
			// max len is IICV_MAX_LINE_LEN minus 1 char for the terminating
			// zero; the 2 chars for the \r and \n are already included
			if(numchars >= IICV_MAX_LINE_LEN-1) {
				str[numchars-2] = '\r';
				str[numchars-1] = '\n';
				// retrieve the real length of the line (note: this not 
				// includes terminating \r and \n)
				linelen = endcharpos - startcharpos;
				// the caret position must be shifted back of the number
				// of chars deleted
				m_iCutChars += (linelen - numchars);

				if (m_uiOperation & IICV_COLOR_LINES) {
					genWarning(1, "line too long has been trimmed");
				}
			}
			str[numchars] = '\0';
		}

		// current char to be returned
		readChar = str[m_iInputBufPos];
		m_iInputBufPos++;

		// if next char is end of line, must increment line number and signal
		// to read a new line
		if (str[m_iInputBufPos] == '\0') {
			m_iInputBufPos = 0;
			m_lViewLineNum++;
		}

		return (readChar);
	}
} 

// override standard lexer getTok function
// must override to 1) copy text read to m_sImportText variable 
inline int CImportFastCap::getTok()
{
	CString cleanUp;

	m_iTok = CLexer::getTok();

	// concatenate any text read
	if ( (m_uiOperation & IICV_COLOR_LINES) ) {
		// spaces before token
		strcat(m_sImportText, m_sSpaceBuffer);
		// the token
		//
		// remark: if m_sString contains any character that is reserved in RTF,
		// must escape it
		cleanUp = m_sString;
		cleanUp.Replace("\\", "\\\\");
		strcat(m_sImportText, (LPCTSTR)cleanUp);
	}

	return m_iTok;
}

// override to allow CImportInColorView to access CFastCapLexer functions;
// see getTok for details; to be used together with moveForward()
inline int  CImportFastCap::readTok(unsigned int *position)
{

	m_iTok = CLexer::readTok(position);

	return m_iTok;
}

// override to allow CImportInColorView to access CFastCapLexer functions;
// see getTok for details; does not skip spaces to allow syncNewLine proper
// function (syncNewLine reads until newline found, then backs to last-1 token; 
// if spaces are skipped, last-1 token is not correct if line ends with spaces)
// however, this function is not used here.
inline int  CImportFastCap::moveForward(unsigned int *position)
{
	CString cleanUp;

	// concatenate any text read, but only up to 'position'
	// (the buffers could contain more chars)
	if ( (m_uiOperation & IICV_COLOR_LINES) ) {
		// spaces before token
		cleanUp = m_sSpaceBuffer;
		// the token
		cleanUp += m_sString;
		// cut to position
		cleanUp = cleanUp.Left(*position);
		// if m_sString contains any character that is reserved in RTF,
		// must escape it
		cleanUp.Replace("\\", "\\\\");
		// and finally copy it in the imported text
		strcat(m_sImportText, (LPCTSTR)cleanUp);
	}

	return(CLexer::moveForward(position));
}

void CImportFastCap::setColor(unsigned int colorIndex)
{
	if ( colorIndex == IFC_COMMENT )
		strcat(m_pColorEditView->m_sRTFLine, IICV_COMMENT_COLOR);
	else if ( colorIndex == IFC_CONTROL )
		strcat(m_pColorEditView->m_sRTFLine, IICV_CONTROL_COLOR);
	else if ( colorIndex == IFC_ELEMENT )
		strcat(m_pColorEditView->m_sRTFLine, IICV_ELEMENT_COLOR);
	else if ( (colorIndex == IFC_ERROR) )
		strcat(m_pColorEditView->m_sRTFLine, IICV_ERROR_COLOR);
	else
		strcat(m_pColorEditView->m_sRTFLine, IICV_DEFAULT_COLOR);
}

char CImportFastCap::readFloat(float *result)
{
	if(m_cReadRes == IFC_GO) {
		getTok();

		if( m_iTok!=FCL_FLOAT ) {
			genError(1, "bad float data");
			return(IFC_NONGO);
		}

		*result = m_fFloat;

		return(IFC_GO);
	}
	else
		return(IFC_NONGO);
}

char CImportFastCap::readFloatOrHex(float *result, unsigned long *hex)
{
	*result = 0.0;
	*hex = 0;

	if(m_cReadRes == IFC_GO) {
		getTok();

		if( m_iTok == FCL_FLOAT ) {
			*result = m_fFloat;
		}
		else if( m_iTok == FCL_HEX ) {
			*hex = m_ulHex;
		}
		else if( m_iTok != LEXER_NEWLINE) {
			genError(1, "bad float or hex data");
			return(IFC_NONGO);
		}

		return(IFC_GO);
	}
	else {
		return(IFC_NONGO);
	}
}

char CImportFastCap::readHex(unsigned long *hex)
{
	*hex = 0;

	if(m_cReadRes == IFC_GO) {
		getTok();

		if( m_iTok == FCL_HEX ) {
			*hex = m_ulHex;
		}
		else if( m_iTok != LEXER_NEWLINE) {
			genError(1, "bad hex data");
			return(IFC_NONGO);
		}

		return(IFC_GO);
	}
	else {
		return(IFC_NONGO);
	}
}


char CImportFastCap::readText(char *result, int maxchar)
{
	if(m_cReadRes == IFC_GO) {
		getTok();

		if( m_iTok!=FCL_TEXT ) {
			genError(4, "bad text data '%s'", m_sString);
			return(IFC_NONGO);
		}

		strncpy(result, m_sString, maxchar);

		return(IFC_GO);
	}
	else
		return(IFC_NONGO);
}

char CImportFastCap::readTextOrNumber(char *result, int maxchar)
{
	if(m_cReadRes == IFC_GO) {
		getTok();

		if( m_iTok!=FCL_TEXT && m_iTok!=FCL_FLOAT) {
			genError(5, "bad text or number data '%s'", m_sString);
			return(IFC_NONGO);
		}

		strncpy(result, m_sString, maxchar);

		return(IFC_GO);
	}
	else
		return(IFC_NONGO);
}

char CImportFastCap::readPlusOrMinus(char *result)
{

	if(m_cReadRes == IFC_GO) {
		getTok();

		if( (m_iTok!=FCL_TEXT && m_iTok != LEXER_NEWLINE) ||
			(m_iTok==FCL_TEXT && strcmp(m_sString, "+") != 0 && strcmp(m_sString, "-") != 0) ) {
			genError(15, "expecting '+' or '-' instead of '%s'", m_sString);
			return(IFC_NONGO);
		}

		*result = m_sString[0];

		return(IFC_GO);
	}
	else
		return(IFC_NONGO);
}

char CImportFastCap::readPerm(float *resultRe, float *resultIm)
{
	unsigned int position;

	if(m_cReadRes == IFC_GO) {
		getTok();

		if( m_iTok!=FCL_FLOAT) {
			genError(56, "expecting permittivity value instead of '%s'", m_sString);
			return(IFC_NONGO);
		}

		*resultRe = m_fFloat;

		// prepare for not having an imaginary part (it is optional)
		*resultIm = 0.0;
		// therefore, must use readTok() and not getTok(), not to advance the parser
		// and be able to roll back if the imaginary part is not present
		position = 0;
		m_iTok = readTok(&position);

		if( m_iTok==FCL_TEXT ) {
			if( (m_sString[0] == '+' || m_sString[0] == '-') && m_sString[1] == 'j' ) {
				// skip '+j' or '-j'
				position = 2;
				moveForward(&position);
				// get the imaginary value
				getTok();
				if( m_iTok!=FCL_FLOAT) {
					genError(57, "expecting imaginary part of permittivity value instead of '%s'", m_sString);
					return(IFC_NONGO);
				}
				*resultIm = m_fFloat;
			}
		}

		return(IFC_GO);
	}
	else
		return(IFC_NONGO);
}

/*
char CImportFastCap::readChargeDensity(float *result)
{

	if(m_cReadRes == IFC_GO) {
		getTok();

		if( (m_iTok!=FCL_FLOAT && m_iTok != LEXER_NEWLINE) ) {
			genError(16, "expecting 'float' data type instead of '%s'", m_sString);
			return(IFC_NONGO);
		}

		// if no charge density information, just return zero
		if( m_iTok == FCL_FLOAT ) {
			*result = m_fFloat;
		}
		else {
			*result = 0.0;
		}

		return(IFC_GO);
	}
	else
		return(IFC_NONGO);
}
*/

char CImportFastCap::processLine()
{
	char ret, command;
	char text[LEXER_MAX_TOK_LEN], ext[LEXER_MAX_TOK_LEN];
	char tmp[LEXER_MAX_TOK_LEN];
	C3DVector pointGeo[4];
	C3DShell *subShell;
	float x, y, z, outpermRe, outpermIm, inpermRe, inpermIm, xref, yref, zref, sigma;
	int i;
	CFastModelDoc *fastmodeldoc;
	CFastCapView *fastcapview, *fastcapmainview;
	FILE *fp;
	BOOL readfromfile;
	bool filealreadyexist, hascolor;
	unsigned long color;

	// handles EOF and NEWLINE
	ret = CImportInColorView::processLine();

	if( ret != IICV_CONTINUE ) 
		return ret;

	fastmodeldoc = ((CFastModelApp *)AfxGetApp())->GetFastModelDoc();
	fastcapmainview = fastmodeldoc->getFastCapMainView();

	switch (m_iTok)
	{

		case FCL_COMMENT :

			if (m_uiOperation & IICV_COLOR_LINES) {
				setColor(IFC_COMMENT);
			}

			syncNewLine(IICV_SYNC_NO_ERROR);

			if (m_uiOperation & IICV_COLOR_LINES) {
				copyToRTF();
			}

			return(IICV_OK);


		case FCL_FLOAT :

			// if first line and '0' is correct: title line, 
			// must be treated as comment
			if(m_lBaseLN+m_lLineNum == 0 && strcmp(m_sString, "0")==0 ) {
				if (m_uiOperation & IICV_COLOR_LINES) {
					setColor(IFC_COMMENT);
				}

				syncNewLine(IICV_SYNC_NO_ERROR);

				if (m_uiOperation & IICV_COLOR_LINES) {
					copyToRTF();
				}

				return(IICV_OK);
			}
			else {
				genError(21, "bad line, beginning with float number");
				return(IICV_ERROR);
			}

		case FCL_TEXT :

			command = tolower(m_sString[0]);

			if( (m_sString[1] != '\0') || (command != 'g' && command != 'c' &&
				command != 'd' && command != 'b' && command != 'q' &&
				command != 't' && command != 'n' ) ) {
				genError(20, "bad input data '%s'", m_sString);
				return(IICV_ERROR);
			}


			if (m_uiOperation & IICV_COLOR_LINES) {
				setColor(IFC_CONTROL);
				copyToRTF();
			}
			
			m_cReadRes = IFC_GO;

			switch(command) {

			case 'g':

				m_cReadRes = readTextOrNumber(text, LEXER_MAX_TOK_LEN);

				if ( m_cReadRes != IFC_GO )
					return(IICV_ERROR);	

				if( m_uiOperation & IICV_COLOR_LINES ) {
					setColor(IFC_ELEMENT);
					copyToRTF();
				}

				return(IICV_OK);

			case 'q':

				m_cReadRes = readTextOrNumber(text, LEXER_MAX_TOK_LEN);

				if ( m_cReadRes != IFC_GO )
					return(IICV_ERROR);	

				if( m_uiOperation & IICV_COLOR_LINES ) {
					setColor(IFC_ELEMENT);
					copyToRTF();
				}

				for(i=0; i<4 && m_cReadRes == IFC_GO; i++) {
					m_cReadRes |= readFloat(&x);
					m_cReadRes |= readFloat(&y);
					m_cReadRes |= readFloat(&z);
					pointGeo[i].pos(x + m_fXtran, y + m_fYtran, z + m_fZtran);
				}

				hascolor = false;

				m_cReadRes |= readFloatOrHex(&sigma, &color);

				// if the optional charge parameter was there
				if( m_iTok == FCL_FLOAT ) {
					if(fastcapmainview->m_bInputHasCharge == false) {
						fastcapmainview->m_bInputHasCharge = true;
						fastcapmainview->m_fMinCharge = sigma;
						fastcapmainview->m_fMaxCharge = sigma;
					}
					else {
						if(sigma > fastcapmainview->m_fMaxCharge) {
							fastcapmainview->m_fMaxCharge = sigma;
						}
						if(sigma < fastcapmainview->m_fMinCharge) {
							fastcapmainview->m_fMinCharge = sigma;
						}
					}
				}
				else if(m_iTok == FCL_HEX ) {
					hascolor = true;
				}

				// if the current panel has not its own color, but there is
				// a global color for the conductor or dielectric interface, use it
				if(hascolor == false) {
					if(m_bHascolor == true) {
						hascolor = true;
						color = m_ulColor;
					}
				}

				if( m_cReadRes == IFC_GO) {
					if (m_uiOperation & IICV_COLOR_LINES) {
						setColor(IFC_TEXT);
						copyToRTF();
					}

					if (m_uiOperation & IICV_CONVERT_TO_MODEL) {
 						m_pCurrShell->MakeFace( pointGeo, 4, sigma, hascolor, color );
					}

					return(IICV_OK);
				}
				else
					return(IICV_ERROR);

			case 't':

				m_cReadRes = readTextOrNumber(text, LEXER_MAX_TOK_LEN);

				if ( m_cReadRes != IFC_GO )
					return(IICV_ERROR);	

				if( m_uiOperation & IICV_COLOR_LINES ) {
					setColor(IFC_ELEMENT);
					copyToRTF();
				}

				for(i=0; i<3 && m_cReadRes == IFC_GO; i++) {
					m_cReadRes |= readFloat(&x);
					m_cReadRes |= readFloat(&y);
					m_cReadRes |= readFloat(&z);
					pointGeo[i].pos(x + m_fXtran, y + m_fYtran, z + m_fZtran);
				}
				
				hascolor = false;

				m_cReadRes |= readFloatOrHex(&sigma, &color);

				// if the optional charge parameter was there
				if( m_iTok == FCL_FLOAT ) {
					if(fastcapmainview->m_bInputHasCharge == false) {
						fastcapmainview->m_bInputHasCharge = true;
						fastcapmainview->m_fMinCharge = sigma;
						fastcapmainview->m_fMaxCharge = sigma;
					}
					else {
						if(sigma > fastcapmainview->m_fMaxCharge) {
							fastcapmainview->m_fMaxCharge = sigma;
						}
						if(sigma < fastcapmainview->m_fMinCharge) {
							fastcapmainview->m_fMinCharge = sigma;
						}
					}
				}
				else if(m_iTok == FCL_HEX ) {
					hascolor = true;
				}

				// if the current panel has not its own color, but there is
				// a global color for the conductor or dielectric interface, use it
				if(hascolor == false) {
					if(m_bHascolor == true) {
						hascolor = true;
						color = m_ulColor;
					}
				}

				if( m_cReadRes == IFC_GO) {
					if (m_uiOperation & IICV_COLOR_LINES) {
						setColor(IFC_TEXT);
						copyToRTF();
					}

					if (m_uiOperation & IICV_CONVERT_TO_MODEL) {
						m_pCurrShell->MakeFace( pointGeo, 3, sigma, hascolor, color );
					}

					return(IICV_OK);
				}
				else
					return(IICV_ERROR);

			case 'c':

				m_cReadRes = readTextOrNumber(text, LEXER_MAX_TOK_LEN);

				if ( m_cReadRes != IFC_GO )
					return(IICV_ERROR);	

				if( m_uiOperation & IICV_COLOR_LINES ) {
					setColor(IFC_ELEMENT);
					copyToRTF();
				}

				m_cReadRes |= readPerm(&outpermRe, &outpermIm);
				m_cReadRes |= readFloat(&x);
				m_cReadRes |= readFloat(&y);
				m_cReadRes |= readFloat(&z);
				
				hascolor = false;
				
				// recognize optional parameters
				if(m_cReadRes == IFC_GO) {

					getTok();
					
					if( m_iTok==FCL_TEXT) {
						if( strcmp(m_sString, "+") != 0 ) {
							genError(15, "expecting '+' instead of '%s'", m_sString);
							m_cReadRes |= IFC_NONGO;
						}
						else {
							getTok();
						}
					}
					
					if( m_iTok == FCL_HEX ) {
						color = m_ulHex;
						hascolor = true;
					}
					else if( m_iTok != LEXER_NEWLINE ) {
						genError(15, "expecting hex color information instead of '%s'", m_sString);
						m_cReadRes |= IFC_NONGO;
					}
				}


				// if the current panel has not its own color, but there is
				// a global color for the conductor or dielectric interface, use it
				if(hascolor == false) {
					if(m_bHascolor == true) {
						hascolor = true;
						color = m_ulColor;
					}
				}

				if( m_cReadRes == IFC_GO) {
					if (m_uiOperation & IICV_COLOR_LINES) {
						setColor(IFC_TEXT);
						copyToRTF();
					}

					if (m_uiOperation & IICV_CONVERT_TO_MODEL) {

						// retrieve the view, if already imported
						fastcapview = fastmodeldoc->getFastCapViewFromName(text);

						// verify if file exists, only to warn the user in case the 
						// file should not exist, because in this case NewFastCapView() 
						// here below will create the file, so it won't be possible to 
						// realize later if it was available or not
						fp = fopen(text, "r");
						if( fp == NULL) {
							genWarning(30, "file '%s' does not exist", text);
							filealreadyexist = false;
						}
						else {
							filealreadyexist = true;
							fclose(fp);
						}

						// if view does not exists, create
						if( fastcapview == NULL) { 
							fastcapview = fastmodeldoc->NewFastCapView(text, filealreadyexist);
							// minimize
							fastcapview->GetParent()->ShowWindow(SW_MINIMIZE);
							readfromfile = TRUE;
						}
						else
							readfromfile = FALSE;

						// extract extension from file name and set view type
						//
						_splitpath(text, tmp, tmp, tmp, ext);
						if( _stricmp(ext, IFC_PATRAN_EXTENSION) == 0 )
							fastcapview->m_cViewType = FCV_ISPATRAN;
						// anyway, FCV_ISFASTCAP is the default
						else
							fastcapview->m_cViewType = FCV_ISFASTCAP;

						if( m_uiOperation & IICV_FILE_SOURCE || readfromfile == TRUE)
						{
							fp = fopen(text, "r");
							if( fp != NULL) {

								subShell = new C3DShell(m_pCurrShell);

								fastcapview->importFastCap(CEV_COLOR_LINES | CEV_CONVERT_TO_MODEL | 
									CEV_FILE_SOURCE | CEV_GENERATE_ERRORS, 
									subShell, fp, x + m_fXtran, y + m_fYtran, z + m_fZtran, hascolor, color);
								fclose(fp);
							}
							else
								genError(30, "cannot open file '%s'", text);
						}
						// otherwise, view already existed with meaningful content,
						// so must only update model (do not color input text nor read from file)
						else {
							fastcapview->updateFastCap(m_pShell, x + m_fXtran, y + m_fYtran, z + m_fZtran, hascolor, color);
						}

					}

					return(IICV_OK);
				}
				else
					return(IICV_ERROR);

			case 'd':
			case 'b':

				m_cReadRes = readTextOrNumber(text, LEXER_MAX_TOK_LEN);

				if ( m_cReadRes != IFC_GO )
					return(IICV_ERROR);	

				if( m_uiOperation & IICV_COLOR_LINES ) {
					setColor(IFC_ELEMENT);
					copyToRTF();
				}

				m_cReadRes |= readPerm(&outpermRe, &outpermIm);
				m_cReadRes |= readPerm(&inpermRe, &inpermIm);
				m_cReadRes |= readFloat(&x);
				m_cReadRes |= readFloat(&y);
				m_cReadRes |= readFloat(&z);
				m_cReadRes |= readFloat(&xref);
				m_cReadRes |= readFloat(&yref);
				m_cReadRes |= readFloat(&zref);
				
				hascolor = false;
				
				// recognize optional parameters
				if(m_cReadRes == IFC_GO) {

					getTok();
					
					if( m_iTok==FCL_TEXT) {
						if( strcmp(m_sString, "-") != 0 ) {
							genError(15, "expecting '-' instead of '%s'", m_sString);
							m_cReadRes |= IFC_NONGO;
						}
						else {
							getTok();
						}
					}
					
					if( m_iTok == FCL_HEX ) {
						color = m_ulHex;
						hascolor = true;
					}
					else if( m_iTok != LEXER_NEWLINE ) {
						genError(15, "expecting hex color information instead of '%s'", m_sString);
						m_cReadRes |= IFC_NONGO;
					}
				}

				// if the current panel has not its own color, but there is
				// a global color for the conductor or dielectric interface, use it
				if(hascolor == false) {
					if(m_bHascolor == true) {
						hascolor = true;
						color = m_ulColor;
					}
				}

				if( m_cReadRes == IFC_GO) {
					if (m_uiOperation & IICV_COLOR_LINES) {
						setColor(IFC_TEXT);
						copyToRTF();
					}

					if (m_uiOperation & IICV_CONVERT_TO_MODEL) {

						// retrieve the view, if already imported
						fastcapview = fastmodeldoc->getFastCapViewFromName(text);

						// verify if file exists, only to warn the user in case the 
						// file should not exist, because in this case NewFastCapView() 
						// here below will create the file, so it won't be possible to 
						// realize later if it was available or not
						fp = fopen(text, "r");
						if( fp == NULL) {
							genWarning(30, "file '%s' does not exist", text);
							filealreadyexist = false;
						}
						else {
							filealreadyexist = true;
							fclose(fp);
						}

						// if view does not exists, create
						if( fastcapview == NULL) { 
							fastcapview = fastmodeldoc->NewFastCapView(text, filealreadyexist);
							// minimize
							fastcapview->GetParent()->ShowWindow(SW_MINIMIZE);
							readfromfile = TRUE;
						}
						else
							readfromfile = FALSE;

						// extract extension from file name and set view type
						//
						_splitpath(text, tmp, tmp, tmp, ext);
						if( _stricmp(ext, IFC_PATRAN_EXTENSION) == 0 )
							fastcapview->m_cViewType = FCV_ISPATRAN;
						// anyway, FCV_ISFASTCAP is the default
						else
							fastcapview->m_cViewType = FCV_ISFASTCAP;

						if( m_uiOperation & IICV_FILE_SOURCE || readfromfile == TRUE)
						{
							fp = fopen(text, "r");
							if( fp != NULL) {

								subShell = new C3DShell(m_pCurrShell);

								fastcapview->importFastCap(CEV_COLOR_LINES | CEV_CONVERT_TO_MODEL | 
									CEV_FILE_SOURCE | CEV_GENERATE_ERRORS,
									subShell, fp, x + m_fXtran, y + m_fYtran, z + m_fZtran, hascolor, color);
								fclose(fp);
							}
							else
								genError(30, "cannot open file '%s'", text);
						}
						// otherwise, view already existed with meaningful content,
						// so must only update model (do not color nor read from file)
						else {
							fastcapview->updateFastCap(m_pShell, x + m_fXtran, y + m_fYtran, z + m_fZtran, hascolor, color);
						}

					}

					return(IICV_OK);
				}
				else
					return(IICV_ERROR);


			case 'n':

				m_cReadRes = readTextOrNumber(text, LEXER_MAX_TOK_LEN);
				m_cReadRes |= readTextOrNumber(text, LEXER_MAX_TOK_LEN);

				if ( m_cReadRes != IFC_GO )
					return(IICV_ERROR);	

				if( m_uiOperation & IICV_COLOR_LINES ) {
					setColor(IFC_ELEMENT);
					copyToRTF();
				}

				return(IICV_OK);


			default:
				;
			}


		default:
			
			// handles lexer error; cannot be in 'case' statement, because code errors
			// are formed or-ing the LEXER_ERROR value with the error code
			if( m_iTok & LEXER_ERROR) {

				if ( (m_uiOperation & IICV_GENERATE_ERRORS) ) {
					if( m_iTok & LEXER_TOKEN_OVERFLOW)
						genError(10, "token too long: '%s'", m_sString);
					else if( m_iTok & LEXER_BUFFER_OVERFLOW)
						genError(1002, "internal error");
				}

				if ( (m_uiOperation & IICV_COLOR_LINES) ) {
					setColor(LEXER_ERROR);
					copyToRTF();
				}
				return(IICV_ERROR);
			}

			// if not a lexer error, there is an unknown keyword
			if (m_uiOperation & IICV_CONVERT_TO_MODEL) {
				genWarning(2, "unknown keyword '%s', skipped", m_sString);
				return(IICV_ERROR);
			}

			// any other case
			return(IICV_OK);
	
	}
	
	genError(1003, "internal error");
	return(IICV_ERROR);
}

// main import function (overridden)
int CImportFastCap::import(unsigned int operation, FILE *fp,
							  CColorEditView *colorEditView, C3DShell *model,
							  long baselineno, long startline, long endline,
							  float xtran, float ytran, float ztran, bool hascolor, unsigned long color)
{
	char answer;

	// call base function
	if( CImportInColorView::import(operation, fp, colorEditView, model, 
		baselineno, startline, endline) == IICV_ERROR )
		return 0;

	// tests to see if speed is more affected by conversion to model
	// or by coloring of lines in CRichEditView
	//
	//m_uiOperation &= ~IICV_CONVERT_TO_MODEL;
	//m_uiOperation &= ~IICV_COLOR_LINES;


	if (m_uiOperation & IICV_CONVERT_TO_MODEL) {
		// init current shell into which import FastCap data
		m_pCurrShell = m_pShell;
	}


	// store translation
	m_fXtran = xtran;
	m_fYtran = ytran;
	m_fZtran = ztran;

	// store color
	m_bHascolor = hascolor;
	m_ulColor = color;

	// initialize lexer
	initLexer();

	// process rest of file, line by line
	//

	getTok();
	while( m_iTok != LEXER_EOF ) {
	
		answer = processLine();

		if ( answer == IICV_ERROR ) {
			// if error, resync the input stream to next line 
			syncNewLine(IICV_SYNC_WITH_ERROR);
			if (m_uiOperation & IICV_COLOR_LINES) {
				setColor(IFC_ERROR);
				copyToRTF();
			}
		}
		else if ( answer == IICV_NEWLINE ) {
			if (m_uiOperation & IICV_COLOR_LINES) {
				// set colored line in view
				m_pColorEditView->setRTFLine();
			}
			// prepare for next line
			getTok();		
		}
		else {
			// go to newline
			syncNewLine(IICV_SYNC_NO_ERROR);
			
			if (m_uiOperation & IICV_COLOR_LINES) {

				// copy what eventually remains to string to be inserted in view,
				// in error color (not recognized!)							
				setColor(IFC_ERROR);
				copyToRTF();

			}
		}

	}

	// must set what eventually remains of the line; this is important
	// in case the input ends with a line without carriage return
	if (m_uiOperation & IICV_COLOR_LINES) {
		// set colored line in view
		m_pColorEditView->setRTFLine();
	}

	// number of cut chars (in case of line too long)
	return(m_iCutChars);
}




