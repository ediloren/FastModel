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


// CImportFastHenry import class - Enrico Di Lorenzo 2001/12/27
//

#include "stdafx.h"

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "ImportFastHenry.h"
#include "fastmodelDoc.h"
#include "FastCapView.h"
#include "fastmodel.h"

#define IFH_LASTERROR 55
#define IFH_LASTWARNING 6
#define IFH_LASTINTERNALERROR 1012

#define SCALE 1.0  


CImportFastHenry::CImportFastHenry()
{
	strcpy(m_sErrorMsg, "IMPFASTHENRY2"); 
}

// override standard lexer getInputChar function
inline int CImportFastHenry::getInputChar()
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
// 2) manage continuing lines
inline int CImportFastHenry::getTok()
{
	unsigned int position, tokensBetween, i;
	CString cleanUp;

	position = 0;

	m_iTok = CLexer::readTok(&position);

	if(m_iTok == LEXER_NEWLINE || m_iTok == FHL_COMMENT) {

		tokensBetween = 0;

		do {
			// count the number of tokens before a '+' (if any)
			tokensBetween++;

			m_iTok = CLexer::readTok(&position);
			if(m_iTok == FHL_CONTINUE) {

				// these are continuing lines: so collate them and don't
				// let the import routine realize that the commands continue
				// on different lines

				// rewind position and read all newlines and comments
				position = 0;
				for(i=0; i<tokensBetween; i++) {
					m_iTok = CLexer::readTok(&position);

					if(m_iTok == LEXER_NEWLINE) {
						// don't forget to increase line counter: this newline won't be
						// processed by standard processLine() in CImportInColorView
						m_lLineNum++;

						if ( (m_uiOperation & IICV_COLOR_LINES) ) {
							// spaces before newline
							strcat(m_sImportText, m_sSpaceBuffer);
							// the newline
							strcat(m_sImportText, "\\par ");
						}
					}
					// otherwise, MUST be a comment 
					else if(m_iTok == FHL_COMMENT) {
						if ( (m_uiOperation & IICV_COLOR_LINES) ) {
							// spaces before newline
							strcat(m_sImportText, m_sSpaceBuffer);
							// the comment color
							// this is a trick, but legal, otherwise no one will color it,
							// and if element definitions were left unfinished from previous
							// lines, they'd give a wrong color to the comment line
							// Note that the {} preserve, after the comment, the color of preceding token 
							strcat(m_sImportText, "{");
							strcat(m_sImportText, IICV_COMMENT_COLOR);
							// the token
							strcat(m_sImportText, m_sString);
							strcat(m_sImportText, "}");
						}
					}
					// otherwise, there is an internal error
					else {
						genError(1011, "internal error");
					}
				}
				// reset counter of newlines and comments (because after the '+' there could be,
				// again, an arbitrary number of newlines and comments)
				tokensBetween = 0;

				// read the '+'
				m_iTok = CLexer::readTok(&position);
	
				if ( (m_uiOperation & IICV_COLOR_LINES) ) {
					// spaces before token
					strcat(m_sImportText, m_sSpaceBuffer);
					// the '+' color
					// this is a trick, but legal, otherwise no one will color it,
					// and the '+' would assume the color of previous line.
					// Note that the {} preserve, after the '+', the color of preceding token 
					strcat(m_sImportText, "{");
					strcat(m_sImportText, IICV_DEFAULT_COLOR);
					// the token
					strcat(m_sImportText, m_sString);
					strcat(m_sImportText, "}");
				}
	
				// move to last position
				CLexer::moveForward(&position);

				// read the new token
				m_iTok = CLexer::readTok(&position);
			}
			else if(m_iTok != LEXER_NEWLINE && m_iTok != FHL_COMMENT) {
				// rewind position and read again the newline or comment
				// (it is necessary, to read also the possible spaces before the token)
				position = 0;
				m_iTok = CLexer::readTok(&position);
				// exit do-while cycle
				break;
			}

		} while (m_iTok == LEXER_NEWLINE || m_iTok == FHL_COMMENT);
	}

	// move to next position
	CLexer::moveForward(&position);

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

// override to allow CImportInColorView to access CFastHenryLexer functions
inline int  CImportFastHenry::moveForward(unsigned int *position)
{
	return(CLexer::moveForward(position));
}

void CImportFastHenry::setColor(unsigned int colorIndex)
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

char CImportFastHenry::readFloat(float *result)
{
	if(m_cReadRes == IFC_GO) {
		getTok();

		if( m_iTok!=FHL_FLOAT ) {
			genError(1, "bad float data");
			return(IFH_NONGO);
		}

		*result = m_fFloat;

		return(IFH_GO);
	}
	else
		return(IFC_NONGO);
}

char CImportFastHenry::readText(char *result, int maxchar)
{
	if(m_cReadRes == IFC_GO) {
		getTok();

		if( m_iTok!=FHL_TEXT ) {
			genError(2, "bad text data '%s'", m_sString);
			return(IFH_NONGO);
		}

		strncpy(result, m_sString, maxchar);

		return(IFH_GO);
	}
	else
		return(IFC_NONGO);
}

char CImportFastHenry::readNode(char *result, int maxchar, char errOnNewLine)
{
	if(m_cReadRes == IFC_GO) {
		getTok();

		if( (m_iTok==LEXER_NEWLINE)&&(!errOnNewLine) ) 
			return(IFH_GO);

		if( m_iTok!=FHL_NODE ) {
			genError(3, "bad node data");
			return(IFH_NONGO);
		}

		strncpy(result, m_sString, maxchar);

		return(IFH_GO);
	}
	else
		return(IFC_NONGO);
}

char CImportFastHenry::readDef(int *defTok, float *value)
{
	if(m_cReadRes == IFC_GO) {

		getTok();
		// save the token read for returning to the caller
		*defTok = m_iTok;

		// these are all the valid 'def' tokens
		if( m_iTok == 'x' || m_iTok == 'y' || m_iTok == 'z' || m_iTok == 'h' ||
			m_iTok == 'w' || m_iTok == FHL_RHO || m_iTok == FHL_SIGMA ||
			m_iTok == FHL_NHINC || m_iTok == FHL_NWINC || m_iTok == FHL_RW || 
			m_iTok == FHL_RH || m_iTok == FHL_WX || m_iTok == FHL_WY || m_iTok == FHL_WZ) {

			getTok();
			if( m_iTok == '=')
				return(readFloat(value));
			else {
				genError(4, "no '=' following parameter value definition");
				return(IFH_NONGO);
			}
		}
		// but as 'def' tokens are optinal, it is legal to have a NEWLINE or EOF
		else if ( m_iTok == LEXER_NEWLINE || m_iTok == LEXER_EOF ) {
			return(IFH_GO);
		}
		// but otherwise this is an error
		else {
			genError(5, "bad parameter data '%s'", m_sString);
			return(IFH_NONGO);
		}
	}
	else
		return(IFC_NONGO);
}

char CImportFastHenry::readGParam(char *result, int *defTok, float *value)
{
	if(m_cReadRes == IFC_GO) {

		getTok();
		*defTok = m_iTok;

		if( m_iTok == FHL_GNDVERTEX || m_iTok == FHL_THICK || 
			m_iTok == FHL_SIGMA || m_iTok == FHL_RHO || 
			m_iTok == FHL_NHINC || m_iTok == FHL_RH || 
			(m_iTok == FHL_TEXT && (_stricmp(m_sString, "seg1") == 0 ||
									_stricmp(m_sString, "seg2") == 0 ||
									_stricmp(m_sString, "segwid1") == 0 ||
									_stricmp(m_sString, "segwid2") == 0 ||
									_stricmp(m_sString, "relx") == 0 ||
									_stricmp(m_sString, "rely") == 0 ||
									_stricmp(m_sString, "relz") == 0 ))
			) {

			if( strlen(m_sString) > IFH_MAX_NAME_LEN) 
				genError(1010, "internal error");

			strncpy(result, m_sString, IFH_MAX_NAME_LEN);
			getTok();
			if( m_iTok == '=')
				return(readFloat(value));
			else {
				genError(23, "no '=' following '%s' value definition", result);
				return(IFH_NONGO);
			}
		}
		else if ( m_iTok == FHL_NODE || m_iTok == FHL_HOLE ||
					(m_iTok == FHL_TEXT &&	(_stricmp(m_sString, "file") == 0 ||
											_stricmp(m_sString, "contact") == 0 ))
					
					) {

			if( strlen(m_sString) > IFH_MAX_NAME_LEN)
				genError(1010, "internal error");

			strncpy(result, m_sString, IFH_MAX_NAME_LEN);
			return(IFH_GO);
		}
		// but as 'Gparam' tokens are optional, it is legal to have a NEWLINE or EOF
		else if ( m_iTok == LEXER_NEWLINE || m_iTok == LEXER_EOF ) {
			return(IFH_GO);
		}
		else {
			genError(24, "unknown ground parameter data '%s'", m_sString);
			return(IFH_NONGO);
		}
	}
	else
		return(IFC_NONGO);
}

char CImportFastHenry::readGndValues(double *vals, int valuesnum, char *err)
{
	int i;
	float tmp;
	char ret;

	ret = m_cReadRes;

	if(m_cReadRes == IFC_GO) {

		*err = 0;

		getTok();

		if( m_iTok == '(' ) {

			for(i=0; i<valuesnum; i++) {

				m_cReadRes = readFloat(&tmp);
				vals[i] = (double)tmp;

				if(m_cReadRes != IFC_GO) {
					*err = 1;
					ret = IFC_NONGO;
					break;
				}

				getTok();
				// any value but the last one
				if( i < valuesnum-1) {
					if(m_iTok != ',') {
						genError(42, "expecting comma separating float values");
						*err = 1;
						ret = IFC_NONGO;
						break;
					}
				}
				else {
					if(m_iTok != ')') {
						genError(43, "expecting ')' closing float values group");
						*err = 1;
						ret = IFC_NONGO;
						break;
					}
				}
			}
		}
		else {
			genError(44, "expecting '(' before float values group");
			*err = 1;
			ret = IFC_NONGO;
		}
	}

	return ret;
}

// this is a special function used to read a node definition inside a GND plane;
// according to FastHenry, nodes inside a GND plane statement can miss one coordinate,
// which will default to zero; moreover, the coordinates must be written without spaces
// i.e. (-2,4) and not (-2, 4)
char CImportFastHenry::readGndNode(double *vals, char *err)
{
	int i;
	char ret;

	ret = m_cReadRes;

	if(m_cReadRes == IFC_GO) {

		// default coordinate values
		vals[0] = 0;
		vals[1] = 0;
		vals[2] = 0;

		*err = 0;

		getTok();

		if( m_iTok == '(' ) {

			for(i=0; i<3; i++) {

				getTok();

				if( m_iTok == FHL_FLOAT ) {
					vals[i] = m_fFloat;
					getTok();
				}

				if( m_iTok == ')' )
					break;
				else if( m_iTok != ',' ) {
					genError(53, "expecting comma separating float values");
					*err = 1;
					ret = IFC_NONGO;
					break;
				}
			}
		}
		else {
			genError(54, "expecting '(' before float values group");
			*err = 1;
			ret = IFC_NONGO;
		}
	}

	return ret;
}

char CImportFastHenry::processLine()
{
	char ret, skip, err;
	char unitname[IFH_MAX_NAME_LEN];
	char name[2][IFH_MAX_NAME_LEN]; 
	GROUPS *group1, *group2, *group, *prev_group; 
	NODES *node, *node1, *node2; 
	int defTok;
	float value, nodex, nodey, nodez;
	char xread, yread, zread;
	int hread, wread, wxread, wyread, wzread;
	static char segname[IFH_MAX_NAME_LEN], node1name[IFH_MAX_NAME_LEN], node2name[IFH_MAX_NAME_LEN]; 
	PARAM widthdir;   
	float width, height;  /*width and height to cross section */
	double norm, dx, dy;

	// handles EOF and NEWLINE
	ret = CImportInColorView::processLine();

	if( ret != IICV_CONTINUE ) 
		return ret;

	m_cReadRes = IFC_GO;

	switch (m_iTok)
	{

		case FHL_COMMENT :

			if (m_uiOperation & IICV_COLOR_LINES) {
				setColor(IFH_COMMENT);
			}

			syncNewLine(IICV_SYNC_NO_ERROR);

			if (m_uiOperation & IICV_COLOR_LINES) {
				copyToRTF();
			}

			return(IICV_OK);
		
		case FHL_DOT_UNI :

			if (m_uiOperation & IICV_COLOR_LINES) {
				setColor(IFH_CONTROL);
				copyToRTF();
			}

			m_cReadRes = readText(unitname, IFH_MAX_NAME_LEN);

			if (m_cReadRes == IFH_GO) {
				if (m_uiOperation & IICV_CONVERT_TO_MODEL) {
				    m_indsys.units = (char *)MemAlloc(strlen(unitname)+1,sizeof(char));
					strcpy(m_indsys.units,unitname);
				}

				if (m_uiOperation & IICV_COLOR_LINES) {
					setColor(IFH_TEXT);
					copyToRTF();
				}

				return(IICV_OK);
			}
			else {
				return(IICV_ERROR);
			}

		case FHL_DOT_EXT :

			if (m_uiOperation & IICV_COLOR_LINES) {
				setColor(IFH_CONTROL);
				copyToRTF();
			}

			// read the nodes which will be external
			m_cReadRes = readNode(name[0], IFH_MAX_NAME_LEN, 1);
			m_cReadRes |= readNode(name[1], IFH_MAX_NAME_LEN, 1);

			if (m_cReadRes == IFH_GO) {

				if (m_uiOperation & IICV_COLOR_LINES) {
					setColor(IFH_ELEMENT);
					copyToRTF();
				}

				// get optional port name, if any
				getTok();

				if (m_uiOperation & IICV_COLOR_LINES) {
					setColor(IFH_TEXT);
					copyToRTF();
				}


				if (m_uiOperation & IICV_CONVERT_TO_MODEL) {

					group1 = get_nodegroup_from_name(name[0], &node);
					if(group1 == NULL){
						genError(6, "unknown node name '%s'",name[0]);
						return(IICV_ERROR);
					}
 
					group2 = get_nodegroup_from_name(name[1], &node);
					if(group2 == NULL){
						genError(7, "unknown node name '%s'",name[1]);
						return(IICV_ERROR);
					}
 
					group1->name1 = (char *)MemAlloc(strlen(name[0])+1,sizeof(char));   
					strcpy(group1->name1, name[0]); 
					group2->name2 = (char *)MemAlloc(strlen(name[1])+1,sizeof(char));   
					strcpy(group2->name2, name[1]); 
				}

				return(IICV_OK);
			}
			else {
				return(IICV_ERROR);
			}


		case FHL_DOT_FRE	:

			if (m_uiOperation & IICV_COLOR_LINES) {
				setColor(IFH_CONTROL);
				copyToRTF();
			}

			// TBC warning: should deal with the .freq case;
			// for the moment, just color in IFH_TEXT color
			if (m_uiOperation & IICV_COLOR_LINES) {
				setColor(IFH_TEXT);
			}

			syncNewLine(IICV_SYNC_NO_ERROR);

			if (m_uiOperation & IICV_COLOR_LINES) {
				copyToRTF();
			}

			return(IICV_OK);

		case FHL_DOT_EQU	:

			if (m_uiOperation & IICV_COLOR_LINES) {
				setColor(IFH_CONTROL);
				copyToRTF();
			}

			skip = 1;
			while( ((m_cReadRes = readNode(name[0], IFH_MAX_NAME_LEN, 0)) == 0) && (m_iTok != LEXER_NEWLINE) ) {
 
				if (m_uiOperation & IICV_COLOR_LINES) {
					setColor(IFH_ELEMENT);
					copyToRTF();
				}

				if (m_uiOperation & IICV_CONVERT_TO_MODEL) {
					group = get_nodegroup_from_name(name[0], &node);
					// if node wasn't defined, it is a pseudo-node (see FastHenry code about .equiv)
					if(group == NULL) {
						// add node as pseudo-node
						group = makegroup(name[0], 0, 0, 0, true);
					}
				}

				// skip first node in the list (cannot merge with previous node group,
				// if it is the first)
				if(skip) 
					skip = 0; 
				else {
					if (m_uiOperation & IICV_CONVERT_TO_MODEL) {
						merge_nodegroups(group, prev_group); 
					}
				}

				prev_group = group;
			}

			if (m_cReadRes != IFH_GO) {
				return(IICV_ERROR);
			}
			else
				return(IICV_OK);

			
		case FHL_DOT_DEF	:

			if (m_uiOperation & IICV_COLOR_LINES) {
				setColor(IFH_CONTROL);
				copyToRTF();
			}

			m_cReadRes = readDef(&defTok, &value);
			// as 'def' parameters are optionals, must end reading at NEWLINE
			// or at EOF; until then, there could be parameters.
			// note that m_iTok is checked and not defTok; this is because
			// it is m_iTok that keeps track of the last token read and not defTok
			// (example: if reading x=1, defTok will be 'x' but m_iTok will be'FHL_FLOAT')
			while( m_iTok != LEXER_NEWLINE && m_iTok != LEXER_EOF) {

				// ok the token has been read, but could also have NOT been a 'def' token;
				// so proceed only if so, otherwise mark the error in color and go on to next one
				if( m_cReadRes == IFH_GO) {

					// color as text only if the 'def' read is valid in this context 
					// ('def's are many more), then read the value
					if( defTok == 'x' || defTok == 'y' || defTok == 'z' || defTok == 'h' ||
						defTok == 'w' || defTok == FHL_RHO || defTok == FHL_SIGMA ||
						defTok == FHL_NHINC || defTok == FHL_NWINC || defTok == FHL_RW || 
						defTok == FHL_RH) {

						if (m_uiOperation & IICV_COLOR_LINES) {
							setColor(IFH_TEXT);
							copyToRTF();
						}

						switch ( defTok )
						{
							case 'x' :
								m_defX.val = value;
								m_defX.isDef = 1;
								break;
							case 'y' :
								m_defY.val = value;
								m_defY.isDef = 1;
								break;
							case 'z' :
								m_defZ.val = value;
								m_defZ.isDef = 1;
								break;
							case 'h' :
								m_defH.val = value;
								m_defH.isDef = 1;
								break;
							case 'w' :
								m_defW.val = value;
								m_defW.isDef = 1;
								break;
							case FHL_RHO :
								m_defRho.val = value;
								m_defRho.isDef = 1;
								break;
							case FHL_SIGMA :
								m_defSigma.val = value;
								m_defSigma.isDef = 1;
								break;
							case FHL_NHINC :
								m_defNhinc.val = value;
								m_defNhinc.isDef = 1;
								break;
							case FHL_NWINC :
								m_defNwinc.val = value;
								m_defNwinc.isDef = 1;
								break;
							case FHL_RW :
								m_defRw.val = value;
								m_defRw.isDef = 1;
								break;
							case FHL_RH :
								m_defRh.val = value;
								m_defRh.isDef = 1;
								break;
							default:
								genError(1000, "internal error");
						}
					}
					// otherwise, a 'def' has been read, but was not valid in this context,
					// so color as error
					else {
						if (m_uiOperation & IICV_COLOR_LINES) {
							setColor(IFH_ERROR);
							copyToRTF();
						}
					}

				}
				else {
					// in this case, a 'def' token was expected, but none was found:
					// mark text as error
					if (m_uiOperation & IICV_COLOR_LINES) {
						setColor(IFH_ERROR);
						copyToRTF();
					}

					// enable reading of next definition (otherwise readDef() would skip)
					m_cReadRes = IFH_GO;
				}

				m_cReadRes = readDef(&defTok, &value);
			}

			if (m_cReadRes != IFH_GO) {
				return(IICV_ERROR);
			}
			else
				return(IICV_OK);

		case FHL_DOT_END :

			if (m_uiOperation & IICV_COLOR_LINES) {
				setColor(IFH_CONTROL);
				copyToRTF();
			}

			m_cDotEnd = 1;

			return(IICV_OK);

		case FHL_NODE :

			if (m_uiOperation & IICV_COLOR_LINES) {
				setColor(IFH_ELEMENT);
				copyToRTF();
			}

			xread = yread = zread = 0;

			strcpy(name[0], m_sString);

			m_cReadRes = readDef(&defTok, &value);
			// as 'def' parameters are optionals, must end reading at NEWLINE
			// or at EOF; until then, there could be parameters.
			// note that m_iTok is checked and not defTok; this is because
			// it is m_iTok that keeps track of the last token read and not defTok
			// (example: if reading x=1, defTok will be 'x' but m_iTok will be'FHL_FLOAT')
			while( m_iTok != LEXER_NEWLINE && m_iTok != LEXER_EOF) {

				// ok the token has been read, but could also have NOT been a 'def' token;
				// so proceed only if so, otherwise mark the error in color and go on to next one
				if( m_cReadRes == IFH_GO) {

					// color as text only if the 'def' read is valid in this context 
					// ('def's are many more), then read the value
					if( defTok == 'x' || defTok == 'y' || defTok == 'z') {
						if (m_uiOperation & IICV_COLOR_LINES) {
							setColor(IFH_TEXT);
							copyToRTF();
						}

						switch ( defTok )
						{
							case 'x' :
								nodex = value;
								xread = 1;
								break;
							case 'y' :
								nodey = value;
								yread = 1;
								break;
							case 'z' :
								nodez = value;
								zread = 1;
								break;
							default:
								genError(1001, "internal error");
						}
					}
					// otherwise, a 'def' has been read, but was not valid in this context,
					// so color as error
					else {
						if (m_uiOperation & IICV_COLOR_LINES) {
							setColor(IFH_ERROR);
							copyToRTF();
						}
					}

				}
				else {
					// in this case, a 'def' token was expected, but none was found:
					// mark text as error
					if (m_uiOperation & IICV_COLOR_LINES) {
						setColor(IFH_ERROR);
						copyToRTF();
					}

					// enable reading of next definition (otherwise readDef() would skip)
					m_cReadRes = IFH_GO;
				}

				m_cReadRes = readDef(&defTok, &value);
			}

			if (m_uiOperation & IICV_CONVERT_TO_MODEL) {

				err = 0;

				if (zread == 0) 
					if (m_defZ.isDef == 1) 
						nodez = m_defZ.val;
					else {
						genError(9, "missing z parameter for node definition");
						err = 1;
					}
				if (yread == 0) 
					if (m_defY.isDef == 1) 
						nodey = m_defY.val;	
					else {
						genError(10, "missing y parameter for node definition");
						err = 1;
					} 
				if (xread == 0) 
					if (m_defX.isDef == 1) 
						nodez = m_defX.val;
					else {
						genError(11, "missing x parameter for node definition");
						err = 1;
					}

				// if missing any of the x,y or z parameters, return with error
				if(err == 1)
					return(IICV_ERROR);

				/* make sure node isn't already defined */
				if (get_nodegroup_from_name(name[0], &node) != NULL) {
					genError(26, "a node with the same name has already been defined");
					return(IICV_ERROR);
				}

				makegroup(name[0], nodex, nodey, nodez);
			}

			if (m_cReadRes != IFH_GO) {
				return(IICV_ERROR);
			}
			else
				return(IICV_OK);

		case FHL_SEGMENT :

			if (m_uiOperation & IICV_COLOR_LINES) {
				setColor(IFH_ELEMENT);
				copyToRTF();
			}

			hread = wread  = wxread = wyread = wzread = 0; 
			widthdir.sx = widthdir.sy = widthdir.sz = 0.0 ;  

			/* read name */
			strcpy(segname, m_sString);

			/* read nodenames */
			m_cReadRes = readNode(node1name, IFH_MAX_NAME_LEN, 1);
			if (m_cReadRes == IFH_GO) {
				if (m_uiOperation & IICV_COLOR_LINES) {
					setColor(IFH_ELEMENT);
					copyToRTF();
				}
			}
			else {
				genError(21, "first node name does not begin with 'n'");
			}

			m_cReadRes |= readNode(node2name, IFH_MAX_NAME_LEN, 1);

			if( m_cReadRes == IFH_GO ) {

				if (m_uiOperation & IICV_COLOR_LINES) {
					setColor(IFH_ELEMENT);
					copyToRTF();
				}

				if (m_uiOperation & IICV_CONVERT_TO_MODEL) {

					group1 = get_nodegroup_from_name(node1name, &node1);
					group2 = get_nodegroup_from_name(node2name, &node2);
					if (group1 == NULL) {
						genError(13, "unknown node name '%s'",node1name);
						return(IICV_ERROR);
					}
					if (group2 == NULL) {
						genError(13, "unknown node name '%s'",node2name);
						return(IICV_ERROR);
					}
  
					merge_nodegroups(group1, group2); 
				}

				m_cReadRes = readDef(&defTok, &value);
				// as 'def' parameters are optionals, must end reading at NEWLINE
				// or at EOF; until then, there could be parameters.
				// note that m_iTok is checked and not defTok; this is because
				// it is m_iTok that keeps track of the last token read and not defTok
				// (example: if reading x=1, defTok will be 'x' but m_iTok will be'FHL_FLOAT')
				while( m_iTok != LEXER_NEWLINE && m_iTok != LEXER_EOF) {

					// ok the token has been read, but could also have NOT been a 'def' token;
					// so proceed only if so, otherwise mark the error in color and go on to next one
					if( m_cReadRes == IFH_GO) {

						// color as text only if the 'def' read is valid in this context 
						// ('def's are many more), then read the value
						if( defTok == 'h' || defTok == 'w' || defTok == FHL_RHO || defTok == FHL_SIGMA ||
							defTok == FHL_NHINC || defTok == FHL_NWINC || defTok == FHL_RW || 
							defTok == FHL_RH || defTok == FHL_WX || defTok == FHL_WY || defTok == FHL_WZ) {

							if (m_uiOperation & IICV_COLOR_LINES) {
								setColor(IFH_TEXT);
								copyToRTF();
							}
	
							switch ( defTok )
							{
								case 'h' :
									height = value;
									hread = 1;
									break;
								case 'w' :
									width = value;
									wread = 1;
									break;
								case FHL_SIGMA :
									break; // TBC warning : if needed, fill in future
								case FHL_RHO :
									break; // TBC warning : if needed, fill in futuredefault:
								case FHL_NHINC :
									break; // TBC warning : if needed, fill in future
								case FHL_NWINC :
									break; // TBC warning : if needed, fill in future
								case FHL_WX :
									widthdir.sx = value;
									wxread = 1;
									break;
								case FHL_WY :
									widthdir.sy = value;
									wyread = 1;
									break;
								case FHL_WZ :
									widthdir.sz = value;
									wzread = 1;
									break;
								case FHL_RW :
									break; // TBC warning : if needed, fill in future
								case FHL_RH :
									break; // TBC warning : if needed, fill in future
								default:
									genError(1002, "internal error");
							}
						}
						// otherwise, a 'def' has been read, but was not valid in this context,
						// so color as error
						else {
							if (m_uiOperation & IICV_COLOR_LINES) {
								setColor(IFH_ERROR);
								copyToRTF();
							}
						}
	
					}
					else {
						// in this case, a 'def' token was expected, but none was found:
						// mark text as error
						if (m_uiOperation & IICV_COLOR_LINES) {
							setColor(IFH_ERROR);
							copyToRTF();
						}
	
						// enable reading of next definition (otherwise readDef() would skip)
						m_cReadRes = IFH_GO;
					}

					m_cReadRes = readDef(&defTok, &value);
				}

				if (m_uiOperation & IICV_CONVERT_TO_MODEL) {

					err = 0;

					if (hread == 0) 
						if (m_defH.isDef == 1) 
							height = m_defH.val;
						else {
							genError(14, "missing h parameter for segment definition");
							err = 1;
						}
					if (wread == 0) 
						if (m_defW.isDef == 1) 
							width = m_defW.val;
						else {
							genError(15, "missing w parameter for segment definition");
							err = 1;
						}
	
					/* if segment-specific face orientation specified.. */ 
					if ( widthdir.sx != 0 || widthdir.sy != 0 || widthdir.sz != 0) {
						if ( (wxread + wyread + wzread) != 3) {
							genError(17, "not all of wx, wy and wz specified for segment");
							err = 1;
						}
						else {
							norm = sqrt(widthdir.sx*widthdir.sx + widthdir.sy*widthdir.sy + widthdir.sz*widthdir.sz);
							widthdir.sx /= norm;
							widthdir.sy /= norm;
							widthdir.sz /= norm;
						}
					}  
					/* else apply defaults */ 
					else { 
						widthdir.sz = 0; 
						dx = (node1->point.x) - (node2->point.x); 
						dy = (node1->point.y) - (node2->point.y); 
						if( dx == 0 && dy == 0 )  { 
							widthdir.sx = 1; 
							widthdir.sy = 0; 
						} 
						else { 
							norm = sqrt(dx*dx + dy*dy); 
							/* must be orthogonal to segment length on plane xy */
							widthdir.sx = dy/norm; 
							widthdir.sy = -dx/norm;  
						}
					}
	
					// if missing any of the parameters, return with error
					if(err == 1)
						return(IICV_ERROR);

					makeseg(segname, node1, node2, height, width, widthdir);
				}

				return(IICV_OK);
			}
			else {
				genError(22, "second node name does not begin with 'n'");
				return(IICV_ERROR);
			}

		case FHL_GROUND :

			if (m_uiOperation & IICV_COLOR_LINES) {
				setColor(IFH_ELEMENT);
				copyToRTF();
			}

			char parName[IFH_MAX_GPAR_CHARS];
			double cosangle;
			unsigned int coordFlags;
			double nodeCoord[3];
			double dummyGndPlPar[20];
			CHole *holeTmp;
			CGroundPlane gp;
			
			gp.m_sNonuniGndName[0] = '\0';
			gp.m_dSegwid1 = gp.m_dSegwid2 = -1;
			gp.m_clRel.x = gp.m_clRel.y = gp.m_clRel.z = 0.0;
			gp.m_clHoleList = NULL;
			strcpy(gp.m_sGndPlaneName, m_sString);

			coordFlags = 0;
			err = 0;


			// As gnd plane parameters are optionals, must end reading at NEWLINE
			// or at EOF; until then, there could be parameters.
			// note that m_iTok is checked and not defTok; this is because
			// it is m_iTok that keeps track of the last token read and not defTok
			// (example: if reading x1=1.0, defTok will be FHL_GNDVERTEX but m_iTok will be FHL_FLOAT)
			m_cReadRes = readGParam(parName, &defTok, &value);
			while(  m_iTok != LEXER_NEWLINE && m_iTok != LEXER_EOF ) {
				
				// ok the token has been read, but could also have NOT been a gnd plane token;
				// so proceed only if so, otherwise mark the error in color and go on to next one
				if( m_cReadRes == IFH_GO) {
		
					if ( parName[0] == 'x' ) {

						switch( parName[1] )
						{
							case '1' :
								gp.m_clCorners[0].x = value;
								coordFlags |= 1;
								break;
							case '2' :
								gp.m_clCorners[1].x = value;
								coordFlags |= 2;
								break;
							case '3' :
								gp.m_clCorners[2].x = value;
								coordFlags |= 4;
								break;
						}
					}

					else if ( parName[0] == 'y' ) {

						switch( parName[1] )
						{
							case '1' :
								gp.m_clCorners[0].y = value;
								coordFlags |= 8;
								break;
							case '2' :
								gp.m_clCorners[1].y = value;
								coordFlags |= 16;
								break;
							case '3' :
								gp.m_clCorners[2].y = value;
								coordFlags |= 32;
								break;
						}
					}

					else if ( parName[0] == 'z' ) {

						switch( parName[1] )
						{
							case '1' :
								gp.m_clCorners[0].z = value;
								coordFlags |= 64;
								break;
							case '2' :
								gp.m_clCorners[1].z = value;
								coordFlags |= 128;
								break;
							case '3' :
								gp.m_clCorners[2].z = value;
								coordFlags |= 256;
								break;
						}
					}

					else if( _stricmp( parName, "thick") == 0) {
						gp.m_dThickness = value;
						coordFlags |= 512;
					}

					else if( _stricmp( parName, "seg1") == 0) {
						gp.m_iSeg1 = (long)value;
						coordFlags |= 1024;
					}

					else if( _stricmp( parName, "seg2") == 0) {
						gp.m_iSeg2 = (long)value;
						coordFlags |= 2048;
					}

					//
					// end of required parameters


					else if( _stricmp( parName, "segwid1") == 0) {
						gp.m_dSegwid1 = value;
					}
					
					else if( _stricmp( parName, "segwid2") == 0) {
						gp.m_dSegwid2 = value;
					}

					else if( _stricmp( parName, "relx") == 0) {
						gp.m_clRel.x = value;
					}

					else if( _stricmp( parName, "rely") == 0) {
						gp.m_clRel.y = value;
					}

					else if( _stricmp( parName, "relz") == 0) {
						gp.m_clRel.z = value;
					}

					else if( defTok == FHL_SIGMA ) {
						// do nothing: this parameter is not needed for geometry
					}

					else if( defTok == FHL_RHO ) {
						// do nothing: this parameter is not needed for geometry
					}

					else if( defTok == FHL_NHINC ) {
						// do nothing: this parameter is not needed for geometry
					}

					else if( defTok == FHL_RH ) {
						// do nothing: this parameter is not needed for geometry
					}

					else if( _stricmp( parName, "file") == 0) {
						// so this is a non-uniform gnd plane
						getTok();
						if( m_iTok != '=') {
							genError(38, "'=' expected, found '%s'", m_sString);
							err = 1;
						}
						else {
							getTok();
							if( m_iTok != FHL_TEXT && m_iTok != FHL_NODE &&
								m_iTok != FHL_SEGMENT && m_iTok != FHL_GROUND) {
								genError(39, "non-uniform plane name expected, found '%s'", m_sString);
								err = 1;
							}
							else {
								strcpy(gp.m_sNonuniGndName, m_sString);
								coordFlags |= 4096;
							}
						}
					}

					else if( _stricmp( parName, "hole") == 0) {

						getTok();
						if( m_iTok != FHL_TEXT || (_stricmp(m_sString, "point") != 0 &&
													_stricmp(m_sString, "rect") != 0 &&
													_stricmp(m_sString, "circle") != 0) ) {
							genError(41, "hole type expected, found '%s'", m_sString);
							err = 1;
						}
						else {

							holeTmp = new CHole;
							if(_stricmp(m_sString, "point") == 0) {
								holeTmp->m_cHoleType = IFH_HOLE_POINT;
								readGndValues(holeTmp->m_dValues, 3, &err);
							}
							else if(_stricmp(m_sString, "rect") == 0) {
								holeTmp->m_cHoleType = IFH_HOLE_RECT;
								readGndValues(holeTmp->m_dValues, 6, &err);
							} 
							// must be 'circle'!
							else {
								holeTmp->m_cHoleType = IFH_HOLE_CIRCLE;
								readGndValues(holeTmp->m_dValues, 4, &err);
							}

							if( err == 0 ) {
								// everything ok, add to list of holes
								holeTmp->m_pNext = gp.m_clHoleList;
								gp.m_clHoleList = holeTmp;
							}
							else
								// something was wrong, so free memory
								delete holeTmp;

						}
					}

					else if( _stricmp( parName, "contact") == 0) {

						// TBC warning: complete for contact generation utility
						// (now we only read parameters but do not generate
						// non-uniform planes)
						getTok();
						if( m_iTok != FHL_TEXT || (_stricmp(m_sString, "equiv_rect") != 0 &&
													_stricmp(m_sString, "decay_rect") != 0 &&
													_stricmp(m_sString, "connection") != 0 &&
													_stricmp(m_sString, "trace") != 0 &&
													_stricmp(m_sString, "point") != 0 &&
													_stricmp(m_sString, "line") != 0 &&
													_stricmp(m_sString, "rect") != 0 &&
													_stricmp(m_sString, "initial_grid") != 0 &&
													_stricmp(m_sString, "initial_mesh_grid") != 0) ) {
							genError(41, "contact type expected, found '%s'", m_sString);
							err = 1;
						}
						else {

							if(_stricmp(m_sString, "equiv_rect") == 0) {
								// node name must follow
								getTok();
								if( m_iTok != FHL_NODE) {
									genError(49, "node name expected, found '%s'", m_sString);
									err = 1;
								}

								// read node parameters

								// copy name
								strcpy(node1name, m_sString);
								// read coordinates
								readGndValues(dummyGndPlPar, 5, &err);
								// make sure node isn't already defined
								if (get_nodegroup_from_name(node1name, &node) != NULL) {
									genError(50, "a node with name %s has already been defined", node1name);
									return(IICV_ERROR);
								}
								
								makegroup(node1name, dummyGndPlPar[0], dummyGndPlPar[1], dummyGndPlPar[2]);
							}
							else if(_stricmp(m_sString, "decay_rect") == 0) {
								readGndValues(dummyGndPlPar, 9, &err);
							} 
							else if(_stricmp(m_sString, "connection") == 0) {
								// node name must follow
								getTok();
								if( m_iTok != FHL_NODE) {
									genError(51, "node name expected, found '%s'", m_sString);
									err = 1;
								}

								// read node parameters

								// copy name
								strcpy(node1name, m_sString);
								// read coordinates
								readGndValues(dummyGndPlPar, 6, &err);
								// make sure node isn't already defined
								if (get_nodegroup_from_name(node1name, &node) != NULL) {
									genError(52, "a node with name %s has already been defined", node1name);
									return(IICV_ERROR);
								}
								
								makegroup(node1name, dummyGndPlPar[0], dummyGndPlPar[1], dummyGndPlPar[2]);
							} 
							else if(_stricmp(m_sString, "trace") == 0) {
								readGndValues(dummyGndPlPar, 8, &err);
							} 
							else if(_stricmp(m_sString, "point") == 0) {
								readGndValues(dummyGndPlPar, 5, &err);
							} 
							else if(_stricmp(m_sString, "line") == 0) {
								readGndValues(dummyGndPlPar, 8, &err);
							} 
							else if(_stricmp(m_sString, "rect") == 0) {
								readGndValues(dummyGndPlPar, 7, &err);
							} 
							else if(_stricmp(m_sString, "initial_grid") == 0) {
								readGndValues(dummyGndPlPar, 2, &err);
							} 
							// must be 'initial_mesh_grid'!
							else {
								readGndValues(dummyGndPlPar, 2, &err);
							}
						}
					}

					else if( defTok == FHL_NODE ) {

						// define node
						// TBC warning: node position shouldn't be the one
						// specified, but the GND plane nearest node! 
						// (see FastHenry manual)

						// copy name
						strcpy(node1name, m_sString);
						// read coordinates
						readGndNode(nodeCoord, &err);
						// make sure node isn't already defined
						if (get_nodegroup_from_name(node1name, &node) != NULL) {
							genError(26, "a node with name %s has already been defined", node1name);
							return(IICV_ERROR);
						}

						makegroup(node1name, nodeCoord[0], nodeCoord[1], nodeCoord[2]);
					}
				}

				// if anything ok, simply color line in IFH_TEXT color
				if(m_cReadRes == IFH_GO &&  err==0) {
					if (m_uiOperation & IICV_COLOR_LINES) {
						setColor(IFH_TEXT);
						copyToRTF();
					}
				}
				// if there had been an error using directly getTok, must skip new readGParam
				// or will lose a token (probably the newline!)
				else if (err==1) {
					m_cReadRes = IFH_NONGO;
					err = 0;

					// in this case, a particular token was expected, but was not found:
					// mark text as error
					if (m_uiOperation & IICV_COLOR_LINES) {
						setColor(IFH_ERROR);
						copyToRTF();
					}
				}				
				// if there had been an input error detected by readGParam, mark in IFH_ERROR color
				else if(m_cReadRes != IFH_GO) {
					
					// enable reading of next definition (otherwise readGParam() would skip)
					m_cReadRes = IFH_GO;

					// in this case, a gnd plane token was expected, but none was found:
					// mark text as error
					if (m_uiOperation & IICV_COLOR_LINES) {
						setColor(IFH_ERROR);
						copyToRTF();
					}
				}


				m_cReadRes = readGParam(parName, &defTok, &value);
			}


			// verify that all required parameters are present
			if( (coordFlags & 1023) != 1023) {
				if( !(coordFlags & 1))
					genError(25, "missing parameter x1 in ground plane definition");
				if( !(coordFlags & 2))
					genError(27, "missing parameter x2 in ground plane definition");
				if( !(coordFlags & 4))
					genError(28, "missing parameter x3 in ground plane definition");
				if( !(coordFlags & 8))
					genError(29, "missing parameter y1 in ground plane definition");
				if( !(coordFlags & 16))
					genError(30, "missing parameter y2 in ground plane definition");
				if( !(coordFlags & 32))
					genError(31, "missing parameter y3 in ground plane definition");
				if( !(coordFlags & 64))
					genError(32, "missing parameter z1 in ground plane definition");
				if( !(coordFlags & 128))
					genError(33, "missing parameter z2 in ground plane definition");
				if( !(coordFlags & 256))
					genError(34, "missing parameter z3 in ground plane definition");
				if( !(coordFlags & 512))
					genError(35, "missing parameter thick in ground plane definition");

				return(IICV_ERROR);
			}

			if( (!(coordFlags & 1024) || !(coordFlags & 2048)) && !(coordFlags & 4096)) {
				if( !(coordFlags & 1024) && !(coordFlags & 2048) && !(coordFlags & 4096)) {
					genError(48, "missing either seg1, seg2 or file specification in plane definition");
					return(IICV_ERROR);
				}
				if( !(coordFlags & 1024) || !(coordFlags & 2048)) {
					if( !(coordFlags & 1024))
						genError(36, "missing parameter seg1 in ground plane definition");
					if( !(coordFlags & 2048))
						genError(37, "missing parameter seg2 in ground plane definition");

					return(IICV_ERROR);
				}
			}

			if( (coordFlags & 1024) && (coordFlags & 4096) ) {
				genError(40, "both uniform plane (seg1,seg2) and nonuniform plane file '%s' specified", gp.m_sNonuniGndName);
				return(IICV_ERROR);
			}

			if (m_uiOperation & IICV_CONVERT_TO_MODEL) {

				// prepare plane variables
				//

				// compute director parameters of segment 1 
				// remark: vector direction opposite to FH uniform gnd plane
				gp.m_clVector1.sx = gp.m_clCorners[1].x -  gp.m_clCorners[0].x;
				gp.m_clVector1.sy = gp.m_clCorners[1].y -  gp.m_clCorners[0].y;
				gp.m_clVector1.sz = gp.m_clCorners[1].z -  gp.m_clCorners[0].z;

				// compute director parameters of segment 2
				gp.m_clVector2.sx = gp.m_clCorners[2].x -  gp.m_clCorners[1].x;
				gp.m_clVector2.sy = gp.m_clCorners[2].y -  gp.m_clCorners[1].y;
				gp.m_clVector2.sz = gp.m_clCorners[2].z -  gp.m_clCorners[1].z;

				// check perpendicularity of segments to create valid ground plane
				cosangle = cosAngleBetweenVectors(gp.m_clVector1, gp.m_clVector2);
				// should be (around) zero
				if( cosangle > COSANGLETOL || cosangle < -COSANGLETOL) {
					genError(45, "the tree corners of plane '%s' don't seem to identify perpendicular vectors", gp.m_sGndPlaneName);
				}
				// if everything ok
				else {
					// find fourth corner point of the plane (see readGeom.c from FastHenry)
					gp.m_clCorners[3].x = gp.m_clCorners[2].x + gp.m_clCorners[0].x - gp.m_clCorners[1].x;
					gp.m_clCorners[3].y = gp.m_clCorners[2].y + gp.m_clCorners[0].y - gp.m_clCorners[1].y;
					gp.m_clCorners[3].z = gp.m_clCorners[2].z + gp.m_clCorners[0].z - gp.m_clCorners[1].z;

					// find dir orthogonal to segment 1 and segment 2   
					orthPlane( gp.m_clVector1, gp.m_clVector2, &(gp.m_clPerpVersor) );
	
					// calculate perpendicular vector 
					gp.m_clPerpVector.sx = gp.m_clPerpVersor.sx*gp.m_dThickness; 
					gp.m_clPerpVector.sy = gp.m_clPerpVersor.sy*gp.m_dThickness; 
					gp.m_clPerpVector.sz = gp.m_clPerpVersor.sz*gp.m_dThickness;

					// compute len of edges
					gp.m_dLen1 = module(gp.m_clVector1);
					gp.m_dLen2 = module(gp.m_clVector2);

					// compute normalized director parameters of segment 1   
					gp.m_clVersor1.sx = gp.m_clVector1.sx/gp.m_dLen1;
					gp.m_clVersor1.sy = gp.m_clVector1.sy/gp.m_dLen1;
					gp.m_clVersor1.sz = gp.m_clVector1.sz/gp.m_dLen1;
		
					// compute normalized director parameters of segment 2
					gp.m_clVersor2.sx = gp.m_clVector2.sx/gp.m_dLen2;
					gp.m_clVersor2.sy = gp.m_clVector2.sy/gp.m_dLen2;
					gp.m_clVersor2.sz = gp.m_clVector2.sz/gp.m_dLen2;

					if( m_uiOperation & IFH_BLOCK_GND_PLANES )
						makeBlockGndPlane(&gp);
					else {
						if( gp.m_sNonuniGndName[0] == '\0' )
							makeUniformGndPlane(&gp);
						else
							// TBC warning: makeNonUniformGndPlane()
							makeBlockGndPlane(&gp);
					}
				}
			}

			// delete hole list 
			while(gp.m_clHoleList != NULL) {
				holeTmp = gp.m_clHoleList;
				gp.m_clHoleList = gp.m_clHoleList->m_pNext;
				delete holeTmp;
			}

			if (m_cReadRes != IFH_GO) {
				return(IICV_ERROR);
			}
			else
				return(IICV_OK);

		default:

			// handles lexer error; cannot be in 'case' statement, because code errors
			// are formed or-ing the LEXER_ERROR value with the error code
			if( m_iTok & LEXER_ERROR) {

				if ( (m_uiOperation & IICV_GENERATE_ERRORS) ) {
					if( m_iTok & LEXER_TOKEN_OVERFLOW)
						genError(10, "token too long: '%s'", m_sString);
					else if( m_iTok & LEXER_UNKNOWN_TOKEN)
						genError(55, "unknown token");
					else if( m_iTok & LEXER_BUFFER_OVERFLOW)
						genError(1002, "internal error");
					// manage all unrecognized errors
					else
						genError(1012, "internal error");
				}

				if ( (m_uiOperation & IICV_COLOR_LINES) ) {
					setColor(IFH_ERROR);
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

GROUPS *CImportFastHenry::makegroup(char *name, double x, double y, double z, bool isPseudoNode)
{
	GROUPS *group;
	NODES *node;

	node = new NODES;
	node->name = (char *)MemAlloc( (strlen(name) + 1), sizeof(char));
	strcpy(node->name, name);
	node->point.x = x;
	node->point.y = y;
	node->point.z = z;
	node->num_segs = 0; 
	node->segments = NULL;
	node->endsegment = NULL; 
	node->next = NULL;
	node->isPseudoNode = isPseudoNode;

	group = new GROUPS;
	group->num_nodes = 1;
	group->nodes = node;
	group->endnode = node;
	group->next = NULL;
	group->name1 = NULL; 
	group->name2 = NULL; 
 
	/* add group to linked list of groups */
	if (m_indsys.groups == NULL) {
		m_indsys.groups = group;
		m_indsys.endgroup = group;
		group->prev = NULL; 
	}
	else {
		m_indsys.endgroup->next = group;
		group->prev = m_indsys.endgroup; 
		m_indsys.endgroup = group;
	}
	m_indsys.num_groups++;

	return group;
}


void CImportFastHenry::makeseg(char *name, NODES *node0, NODES *node1,
							   double height, double width, PARAM widthdir) 
// 'width', 'height' are width and height to cross section
// 'node0', 'node1' are nodes at ends 
{
	SEGMENTS *seg, *seg1;

	seg = new SEGMENTS;

	seg->name = (char *)MemAlloc(strlen(name) + 1, sizeof(char));
	strcpy(seg->name, name);
	seg->widthdir = widthdir;
	seg->width = width;
	seg->height = height;
	seg->node[0] = node0;
	seg->node[1] = node1;
	seg->next = NULL;

	/* add segment to linked list */
	if (node0->segments == NULL) {
		node0->segments = seg;
	}
	else { 
		seg1 = node0->segments;  
		while( seg1->next != NULL )  
			seg1 = seg1->next; 
 
		seg1->next = seg;
	}
	node0->num_segs++;
}

GROUPS *CImportFastHenry::get_nodegroup_from_name(char *name, NODES **node) 
{ 
	GROUPS *group; 
	NODES *node_scan; 
 
	group = m_indsys.groups;  
	while(group != NULL) {  

		/* scan all groups */ 
		node_scan = group->nodes; 
		while(node_scan != NULL && _stricmp(name, node_scan->name) != 0) 
			node_scan = node_scan->next; 
 
		if( node_scan != NULL ) { 
			*node = node_scan;   
		break; 
		} 
		group = group->next; 
	} 
  return group; 
} 

void CImportFastHenry::merge_nodegroups(GROUPS *group1, GROUPS *group2) 
{  
	// if already same group, do nothing
	if (group1 == group2) {
		return;
	}

	group1->endnode->next = group2->nodes; 
	group1->endnode = group2->endnode; 
	group1->num_nodes += group2->num_nodes; 

	/* what happens if group to be deleted is first or last */ 

	if(group2->prev != NULL)   
		// substitute the next in the previous group
		group2->prev->next = group2->next; 
	else 
		// if was first, then the next in the list will be the new first
		m_indsys.groups = group2->next; 

	if(group2->next != NULL)   
		group2->next->prev = group2->prev; 
	else 
		m_indsys.endgroup = group2->prev; 
   
	m_indsys.num_groups--; 
	free(group2);  

} 

void CImportFastHenry::makeBlockGndPlane(CGroundPlane *gp)
{

	C3DShell *currShell;
	double hx, hy, hz; 
	SPOINTS pointlist1[4], pointlist2[4];

	// calculate the heigth of parallelepiped corners from original 3 (4) corners 
	// defining the plane 
	hx = gp->m_clPerpVector.sx / 2.0;
	hy = gp->m_clPerpVector.sy / 2.0;
	hz = gp->m_clPerpVector.sz / 2.0;
		
	// assign corner points and create parallelepiped
	pointlist1[0].x = gp->m_clCorners[0].x - hx ; 
	pointlist1[0].y = gp->m_clCorners[0].y - hy ;
	pointlist1[0].z = gp->m_clCorners[0].z - hz ; 
	pointlist1[1].x = gp->m_clCorners[1].x - hx ; 
	pointlist1[1].y = gp->m_clCorners[1].y - hy ;
	pointlist1[1].z = gp->m_clCorners[1].z - hz ; 
	pointlist1[2].x = gp->m_clCorners[2].x - hx ; 
	pointlist1[2].y = gp->m_clCorners[2].y - hy ;
	pointlist1[2].z = gp->m_clCorners[2].z - hz ; 
	pointlist1[3].x = gp->m_clCorners[3].x - hx ; 
	pointlist1[3].y = gp->m_clCorners[3].y - hy ;
	pointlist1[3].z = gp->m_clCorners[3].z - hz ; 

	pointlist2[0].x = gp->m_clCorners[0].x + hx ; 
	pointlist2[0].y = gp->m_clCorners[0].y + hy ;
	pointlist2[0].z = gp->m_clCorners[0].z + hz ; 
	pointlist2[1].x = gp->m_clCorners[1].x + hx ; 
	pointlist2[1].y = gp->m_clCorners[1].y + hy ;
	pointlist2[1].z = gp->m_clCorners[1].z + hz ; 
	pointlist2[2].x = gp->m_clCorners[2].x + hx ; 
	pointlist2[2].y = gp->m_clCorners[2].y + hy ;
	pointlist2[2].z = gp->m_clCorners[2].z + hz ; 
	pointlist2[3].x = gp->m_clCorners[3].x + hx ; 
	pointlist2[3].y = gp->m_clCorners[3].y + hy ;
	pointlist2[3].z = gp->m_clCorners[3].z + hz ;

	// make new shell for each ground plane (TBC warning: different philosophy 
	// from what made for segments) 
	currShell = new C3DShell(m_pCurrShell);

	createParallelepiped(pointlist1, pointlist2, currShell);

}

void CImportFastHenry::makeUniformGndPlane(CGroundPlane *gp)
{

	C3DShell *currShell;
	PARAM offset;
	double hx, hy, hz, wx, wy, wz; 
	SPOINTS pointlist1[4], pointlist2[4];
	double radius, offsetx, offsety, side;
	int i, j, i1, i2, j1, j2, istart, iend, jstart, jend;
	int nodes_up, nodes_down, nodes_left, nodes_right;
	CHole *tmphole;


	// segfull1 is the full width of segments in direction parallel to plane edge 1,
	// see fig.4 in FastHenry user's manual
	gp->m_dSegfull1 = gp->m_dLen2 / gp->m_iSeg2;
	gp->m_dSegfull2 = gp->m_dLen1 / gp->m_iSeg1;

	// test if segwids were specified and, if so, if they are correct

	if( gp->m_dSegwid1 < 0.0) {
		gp->m_dSegwid1 = gp->m_dSegfull1;
	}
	else {
		if( gp->m_dSegwid1 > gp->m_dSegfull1 ) {
			genWarning(3, "segwid1 greater than segment separation, using segwid1 = %f", gp->m_dSegfull1);
			gp->m_dSegwid1 = gp->m_dSegfull1;
		}
	}
	if( gp->m_dSegwid2 < 0.0) {
		gp->m_dSegwid2 = gp->m_dSegfull2;
	}
	else {
		if( gp->m_dSegwid2 > gp->m_dSegfull2 ) {
			genWarning(4, "segwid2 greater than segment separation, using segwid2 = %f", gp->m_dSegfull2);
			gp->m_dSegwid2 = gp->m_dSegfull2;
		}
	}

	// the number of nodes is equal to the number of segments along the edge plus one;
	// (note that e.g. seg1 refers to the # of segs PARALLEL to edge 1, see FastHenry user's guide)
	gp->m_clNodeMatrix = (CNode **)NodeMatrixAlloc(gp->m_iSeg1+1, gp->m_iSeg2+1);

	// find increments from middle point to side points

	// from middle to first one
	gp->m_clD1.sx = gp->m_clVector1.sx / gp->m_iSeg1;
	gp->m_clD1.sy = gp->m_clVector1.sy / gp->m_iSeg1;
	gp->m_clD1.sz = gp->m_clVector1.sz / gp->m_iSeg1;
	gp->m_dD1mod = module(gp->m_clD1);

	// from middle to second one
	gp->m_clD2.sx = gp->m_clVector2.sx / gp->m_iSeg2;
	gp->m_clD2.sy = gp->m_clVector2.sy / gp->m_iSeg2;
	gp->m_clD2.sz = gp->m_clVector2.sz / gp->m_iSeg2;
	gp->m_dD2mod = module(gp->m_clD2);

	// module of the diagonal 
	gp->m_dDiagmod = sqrt(gp->m_dD1mod*gp->m_dD1mod + gp->m_dD2mod*gp->m_dD2mod);

	for(i=0; i <= gp->m_iSeg1; i++) {
		for(j=0; j <= gp->m_iSeg2; j++) {
			// assign node coordinates and set evey node as not deleted
			gp->m_clNodeMatrix[i][j].m_dX = gp->m_clCorners[0].x + (gp->m_clD1.sx * i) + (gp->m_clD2.sx * j);
			gp->m_clNodeMatrix[i][j].m_dY = gp->m_clCorners[0].y + (gp->m_clD1.sy * i) + (gp->m_clD2.sy * j);
			gp->m_clNodeMatrix[i][j].m_dZ = gp->m_clCorners[0].z + (gp->m_clD1.sz * i) + (gp->m_clD2.sz * j);
			gp->m_clNodeMatrix[i][j].m_cDeleted = FALSE;
		}
	}

	// mark nodes which are to be 'deleted' (not used in forming segments) 
	// because part of a hole
	// remark: remember that holes positions are offset of vector gp->m_clRel
	for( tmphole = gp->m_clHoleList; tmphole != NULL; tmphole = tmphole->m_pNext) {

		switch( tmphole->m_cHoleType ) {

		case IFH_HOLE_POINT :

			findNearestNode(tmphole->m_dValues[0] + gp->m_clRel.x, tmphole->m_dValues[1] + gp->m_clRel.y, 
							tmphole->m_dValues[2] + gp->m_clRel.z, gp, &i, &j);
			// mark the node as deleted
			gp->m_clNodeMatrix[i][j].m_cDeleted = TRUE;

			break;

		case IFH_HOLE_RECT :

			findNearestNode(tmphole->m_dValues[0] + gp->m_clRel.x, tmphole->m_dValues[1] + gp->m_clRel.y, 
							tmphole->m_dValues[2] + gp->m_clRel.z, gp, &i1, &j1);
			findNearestNode(tmphole->m_dValues[3] + gp->m_clRel.x, tmphole->m_dValues[4] + gp->m_clRel.y, 
							tmphole->m_dValues[5] + gp->m_clRel.z, gp, &i2, &j2);

			if(i1 <= i2) {
				istart = i1;
				iend = i2;
			}
			else {
				istart = i2; 
				iend = i1;
			}
			if(j1 <= j2) {
				jstart = j1;
				jend = j2;
			}
			else {
				jstart = j2;
				jend = j1;
			}

			for( i=istart; i<=iend; i++)
				for( j=jstart; j<=jend; j++)
					// mark the nodes as deleted
					gp->m_clNodeMatrix[i][j].m_cDeleted = TRUE;
			break;

		case IFH_HOLE_CIRCLE :

			findNearestNode(tmphole->m_dValues[0] + gp->m_clRel.x, tmphole->m_dValues[1] + gp->m_clRel.y, 
							tmphole->m_dValues[2] + gp->m_clRel.z, gp, &i1, &j1);

			radius = tmphole->m_dValues[3];

			// find the components in the direction of the plane edges of the vector
			// connecting the 'node nearest to the centre' to the 'centre of the circle'
			// (that is, the offset of the centre node in respect to the real centre)
			offset.sx = tmphole->m_dValues[0] - gp->m_clNodeMatrix[i1][j1].m_dX;
			offset.sy = tmphole->m_dValues[1] - gp->m_clNodeMatrix[i1][j1].m_dY;
			offset.sz = tmphole->m_dValues[2] - gp->m_clNodeMatrix[i1][j1].m_dZ;
			offsetx = dotProduct(offset, gp->m_clVersor1);
			offsety = dotProduct(offset, gp->m_clVersor2);

			// but if offset greater than radius, generate warning and break
			if( module(offset) > radius ) {
				genWarning(5, "discretization too coarse for circular hole of radius %f", radius);
				break;
			}

			// nodes_up and nodes_down are the number of nodes along plane edge 1 of the hole span;
			// then we will step from nodes_down to nodes_up and for each of this abscissa (edge 1) 
			// we will remove all necessary nodes along the ordinate (edge 2)
			nodes_up = (int) ((radius - offsetx) / gp->m_dD1mod);
			nodes_down = (int) ((-radius - offsetx) / gp->m_dD1mod);

			for( i=nodes_down; i<=nodes_up; i++) {
				// find extent of circle along the ordinate
				side = sqrt(radius*radius - (offsetx+i*gp->m_dD1mod)*(offsetx+i*gp->m_dD1mod));
				nodes_left = (int) ((-side - offsety)/gp->m_dD2mod);
				nodes_right = (int) ((side - offsety)/gp->m_dD2mod);

				for(j=nodes_left; j<=nodes_right; j++)
					// delete node only if internal in the plane!
					if( i+i1 >= 0 && i+i1 <= gp->m_iSeg1 && j+j1 >= 0 && j+j1 <= gp->m_iSeg2)
						// note that point (i,j) is in offset in respect to circle centre (i1,j1) 
						gp->m_clNodeMatrix[i+i1][j+j1].m_cDeleted = TRUE;
			}

			break;

		}	// end of switch statement
	} // end of holes loop

	// make new shell for each ground plane (TBC warning: different philosophy 
	// from what made for segments) 
	currShell = new C3DShell(m_pCurrShell);

	// calculate the heigth of parallelepiped corners from original 3 (4) corners 
	// defining the plane 
	hx = gp->m_clPerpVector.sx / 2.0;
	hy = gp->m_clPerpVector.sy / 2.0;
	hz = gp->m_clPerpVector.sz / 2.0;

	// calculate the width of segments composing the plane parallel to edge 1 (so width
	// vector direction is parallel to edge 2)
	wx = gp->m_clVersor2.sx * gp->m_dSegwid1 / 2.0;
	wy = gp->m_clVersor2.sy * gp->m_dSegwid1 / 2.0; 
	wz = gp->m_clVersor2.sz * gp->m_dSegwid1 / 2.0; 

	// layout segments along plane edge 1
	for( i=0; i<gp->m_iSeg2+1; i++) {
		for( j=0; j<gp->m_iSeg1; j++) {
			if( gp->m_clNodeMatrix[j][i].m_cDeleted != TRUE && gp->m_clNodeMatrix[j+1][i].m_cDeleted != TRUE) {

				// assign corner points and create segment
				pointlist1[0].x = gp->m_clNodeMatrix[j][i].m_dX - hx - wx; 
				pointlist1[0].y = gp->m_clNodeMatrix[j][i].m_dY - hy - wy;
				pointlist1[0].z = gp->m_clNodeMatrix[j][i].m_dZ - hz - wz; 
				pointlist1[1].x = gp->m_clNodeMatrix[j][i].m_dX + hx - wx; 
				pointlist1[1].y = gp->m_clNodeMatrix[j][i].m_dY + hy - wy;
				pointlist1[1].z = gp->m_clNodeMatrix[j][i].m_dZ + hz - wz; 
				pointlist1[2].x = gp->m_clNodeMatrix[j][i].m_dX + hx + wx; 
				pointlist1[2].y = gp->m_clNodeMatrix[j][i].m_dY + hy + wy;
				pointlist1[2].z = gp->m_clNodeMatrix[j][i].m_dZ + hz + wz; 
				pointlist1[3].x = gp->m_clNodeMatrix[j][i].m_dX - hx + wx; 
				pointlist1[3].y = gp->m_clNodeMatrix[j][i].m_dY - hy + wy;
				pointlist1[3].z = gp->m_clNodeMatrix[j][i].m_dZ - hz + wz; 

				pointlist2[0].x = gp->m_clNodeMatrix[j+1][i].m_dX - hx - wx; 
				pointlist2[0].y = gp->m_clNodeMatrix[j+1][i].m_dY - hy - wy;
				pointlist2[0].z = gp->m_clNodeMatrix[j+1][i].m_dZ - hz - wz; 
				pointlist2[1].x = gp->m_clNodeMatrix[j+1][i].m_dX + hx - wx; 
				pointlist2[1].y = gp->m_clNodeMatrix[j+1][i].m_dY + hy - wy;
				pointlist2[1].z = gp->m_clNodeMatrix[j+1][i].m_dZ + hz - wz; 
				pointlist2[2].x = gp->m_clNodeMatrix[j+1][i].m_dX + hx + wx; 
				pointlist2[2].y = gp->m_clNodeMatrix[j+1][i].m_dY + hy + wy;
				pointlist2[2].z = gp->m_clNodeMatrix[j+1][i].m_dZ + hz + wz; 
				pointlist2[3].x = gp->m_clNodeMatrix[j+1][i].m_dX - hx + wx; 
				pointlist2[3].y = gp->m_clNodeMatrix[j+1][i].m_dY - hy + wy;
				pointlist2[3].z = gp->m_clNodeMatrix[j+1][i].m_dZ - hz + wz; 

				createParallelepiped(pointlist1, pointlist2, currShell);
			}
		}
	}

	// calculate the width of segments composing the plane parallel to edge 2 (so width
	// vector direction is parallel to edge 1)
	wx = gp->m_clVersor1.sx * gp->m_dSegwid2 / 2.0;
	wy = gp->m_clVersor1.sy * gp->m_dSegwid2 / 2.0; 
	wz = gp->m_clVersor1.sz * gp->m_dSegwid2 / 2.0; 

	// layout segments along plane edge 2
	for( i=0; i<gp->m_iSeg2; i++) {
		for( j=0; j<gp->m_iSeg1+1; j++) {
			if( gp->m_clNodeMatrix[j][i].m_cDeleted != TRUE && gp->m_clNodeMatrix[j][i+1].m_cDeleted != TRUE) {

				// assign corner points and create segment
				pointlist1[0].x = gp->m_clNodeMatrix[j][i].m_dX - hx - wx; 
				pointlist1[0].y = gp->m_clNodeMatrix[j][i].m_dY - hy - wy;
				pointlist1[0].z = gp->m_clNodeMatrix[j][i].m_dZ - hz - wz; 
				pointlist1[1].x = gp->m_clNodeMatrix[j][i].m_dX + hx - wx; 
				pointlist1[1].y = gp->m_clNodeMatrix[j][i].m_dY + hy - wy;
				pointlist1[1].z = gp->m_clNodeMatrix[j][i].m_dZ + hz - wz; 
				pointlist1[2].x = gp->m_clNodeMatrix[j][i].m_dX + hx + wx; 
				pointlist1[2].y = gp->m_clNodeMatrix[j][i].m_dY + hy + wy;
				pointlist1[2].z = gp->m_clNodeMatrix[j][i].m_dZ + hz + wz; 
				pointlist1[3].x = gp->m_clNodeMatrix[j][i].m_dX - hx + wx; 
				pointlist1[3].y = gp->m_clNodeMatrix[j][i].m_dY - hy + wy;
				pointlist1[3].z = gp->m_clNodeMatrix[j][i].m_dZ - hz + wz; 

				pointlist2[0].x = gp->m_clNodeMatrix[j][i+1].m_dX - hx - wx; 
				pointlist2[0].y = gp->m_clNodeMatrix[j][i+1].m_dY - hy - wy;
				pointlist2[0].z = gp->m_clNodeMatrix[j][i+1].m_dZ - hz - wz; 
				pointlist2[1].x = gp->m_clNodeMatrix[j][i+1].m_dX + hx - wx; 
				pointlist2[1].y = gp->m_clNodeMatrix[j][i+1].m_dY + hy - wy;
				pointlist2[1].z = gp->m_clNodeMatrix[j][i+1].m_dZ + hz - wz; 
				pointlist2[2].x = gp->m_clNodeMatrix[j][i+1].m_dX + hx + wx; 
				pointlist2[2].y = gp->m_clNodeMatrix[j][i+1].m_dY + hy + wy;
				pointlist2[2].z = gp->m_clNodeMatrix[j][i+1].m_dZ + hz + wz; 
				pointlist2[3].x = gp->m_clNodeMatrix[j][i+1].m_dX - hx + wx; 
				pointlist2[3].y = gp->m_clNodeMatrix[j][i+1].m_dY - hy + wy;
				pointlist2[3].z = gp->m_clNodeMatrix[j][i+1].m_dZ - hz + wz; 

				createParallelepiped(pointlist1, pointlist2, currShell);
			}
		}
	}

	// must remember to deallocate memory!;
	NodeMatrixDelete(gp->m_clNodeMatrix, gp->m_iSeg1+1, gp->m_iSeg2+1);

}

void CImportFastHenry::findNearestNode(double x, double y, double z, CGroundPlane *gp, int *i, int *j)
{
	double proj1, proj2, dist;
	PARAM vector;
	int count1, count2;
	CNode *node;

	// calculate vector connecting plane origin corner to given point 
	vector.sx = x - gp->m_clCorners[0].x;
	vector.sy = y - gp->m_clCorners[0].y;
	vector.sz = z - gp->m_clCorners[0].z;

	// calculate projections of the vector along plane edges
	proj1 = dotProduct(vector, gp->m_clVersor1);
	proj2 = dotProduct(vector, gp->m_clVersor2);
	
	// note that count1, count2 are integers, so + 0.5 is used to approximate
	// to the next higher int if greater than x.5 and to the next lower int otherwise
	count1 = proj1/gp->m_dD1mod + 0.5;
	count2 = proj2/gp->m_dD2mod + 0.5;

	if( count1 > gp->m_iSeg1 || count1 < 0 || count2 > gp->m_iSeg2 || count2 < 0) {
		genError(46, "performing holes found point (%f,%f,%f) outside the plane", x, y, z);
		
		// return an erroneous condition
		*i = *j = -1;
		return;
	}

	node = &(gp->m_clNodeMatrix[count1][count2]);
	*i = count1;
	*j = count2;

	dist = module(x - node->m_dX, y - node->m_dY, z - node->m_dZ);
	if( (dist - gp->m_dDiagmod/2.0) > IFH_TOL * gp->m_dDiagmod/2.0 ) {
		genWarning(6, "node (%f,%f,%f) is too far (dist = %f) from plane, could be not on it", x, y, z, dist);
	}
}

double **CImportFastHenry::NodeMatrixAlloc(int rows, int cols)
{
	double **temp;
	int i;

	temp = (double **) new double[rows];
 	for(i = 0; i < rows; i++) 
		temp[i] = (double *) new CNode[cols];

	return(temp);
}

void CImportFastHenry::NodeMatrixDelete(CNode **nodematrix, int rows, int cols)
{
	double **temp;
	int i;

	temp = (double **)nodematrix;
 	for(i = 0; i < rows; i++) 
		delete temp[i];

	delete temp;
}

// main import function (overridden)
int CImportFastHenry::import(unsigned int operation, FILE *fp,
							  CColorEditView *colorEditView, C3DShell *model,
							  long baselineno, long startline, long endline,
							  float xtran, float ytran, float ztran)
{
	char answer;

	// initialize default vars
	m_defX.isDef = 0;
	m_defY.isDef = 0;
	m_defZ.isDef = 0;
	m_defH.isDef = 0;
	m_defW.isDef = 0;
	m_defRho.isDef = 0;
	m_defSigma.isDef = 0;
	m_defNhinc.isDef = 0;
	m_defNwinc.isDef = 0;
	m_defRw.isDef = 0;
	m_defRh.isDef = 0;

	// initialize FastHenry geometry structures
	m_indsys.num_groups = 0;
	m_indsys.title = NULL; 
	m_indsys.units = NULL; 
	m_indsys.groups = NULL; 
	m_indsys.endgroup = NULL; 

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
		// init current shell into which import FastHenry data
		m_pCurrShell = m_pShell;
	}


	// store translation
	m_fXtran = xtran;
	m_fYtran = ytran;
	m_fZtran = ztran;

	// initialize lexer
	initLexer();

	// flag for '.end' command presence in processed input
	m_cDotEnd = 0;

	// if importing from file, skip first line, as required by FastHenry;
	// also skip if first line in view
	if ( (m_uiOperation & IICV_FILE_SOURCE) || ( !(m_uiOperation & IICV_FILE_SOURCE) && (startline == 0) ) ) {
		// skip FastHenry header (first line)
		getTok();
		syncNewLine(IICV_SYNC_NO_ERROR);
		if (m_uiOperation & IICV_COLOR_LINES) {
			setColor(IFH_COMMENT);
			copyToRTF();
			m_pColorEditView->setRTFLine();
		}
	}
	else
		// if not already acquired token, because read (and skipped) the first line, read
		// the first token, to be able to start
		getTok();

	// process rest of file, line by line
	//

	while( m_iTok != LEXER_EOF ) {
	
		answer = processLine();

		if ( answer == IICV_ERROR ) {
			// if error, resync the input stream to next line 
			syncNewLine(IICV_SYNC_WITH_ERROR);
			if (m_uiOperation & IICV_COLOR_LINES) {
				setColor(IFH_ERROR);
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
				setColor(IFH_ERROR);
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


	// if IICV_CONVERT_TO_MODEL flag, convert the acquired structure to C3DShell
	if (m_uiOperation & IICV_CONVERT_TO_MODEL) {
		if( m_cDotEnd == 0 ) {
			genError(19, "unexpected end of FastHenry2 data");
		}
		convertToModel();
	}

	// number of cut chars (in case of line too long)
	return(m_iCutChars);
}


void CImportFastHenry::convertToModel() 
{ 
	GROUPS *group;
	C3DShell *currShell;

	/* scan all groups */ 
    group = m_indsys.groups;
    while( group != NULL ) {

		// make new shell for each group of conductors
		currShell = new C3DShell(m_pCurrShell);
		
		// add geometric entities equivalent to segments to the current shell 
		convertGroup(group, currShell);
		group = group->next;
    }
 
	// free all allocated memory for FastHenry geometry
    freeHenry();   
}

void CImportFastHenry::convertGroup(GROUPS *group, C3DShell *myShell) 
{
	NODES *node, *node0, *node1; 
	SEGMENTS *seg; 
	SPOINTS pointlist1[4], pointlist2[4];  

	node = group->nodes; 
	while( node != NULL) { 
		seg = node->segments;
		while( seg != NULL ) {
			node0 = seg->node[0]; 
			node1 = seg->node[1]; 

			// determine corners coordinates
			corners( node0->point, node1->point, seg->widthdir, 
                 seg->width, seg->height, pointlist1, pointlist2); 

			// and create corresponding parallelepiped
			createParallelepiped(pointlist1, pointlist2, myShell);
		          
			seg=seg->next;
		}
		node = node->next; 
	} 
} 

// the two pointlists represent two opposite faces of the parallelepiped; note that vertex order
// should be the same for both faces or stray results will be generated
void CImportFastHenry::createParallelepiped(SPOINTS pointlist1[4], SPOINTS pointlist2[4], C3DShell *myShell) 
{

	C3DVector pointGeo[2][4];
	C3DVector pGeo[4];

	// create segment face panels   
	pointGeo[0][0].pos( SCALE*pointlist1[0].x, SCALE*pointlist1[0].y, SCALE*pointlist1[0].z);
	pointGeo[0][1].pos( SCALE*pointlist1[1].x, SCALE*pointlist1[1].y, SCALE*pointlist1[1].z);
	pointGeo[0][2].pos( SCALE*pointlist1[2].x, SCALE*pointlist1[2].y, SCALE*pointlist1[2].z);
	pointGeo[0][3].pos( SCALE*pointlist1[3].x, SCALE*pointlist1[3].y, SCALE*pointlist1[3].z);
	pointGeo[1][0].pos( SCALE*pointlist2[0].x, SCALE*pointlist2[0].y, SCALE*pointlist2[0].z);
	pointGeo[1][1].pos( SCALE*pointlist2[1].x, SCALE*pointlist2[1].y, SCALE*pointlist2[1].z);
	pointGeo[1][2].pos( SCALE*pointlist2[2].x, SCALE*pointlist2[2].y, SCALE*pointlist2[2].z);
	pointGeo[1][3].pos( SCALE*pointlist2[3].x, SCALE*pointlist2[3].y, SCALE*pointlist2[3].z);
			
	// Front face 
	pGeo[0] = pointGeo[0][0]; 
	pGeo[1] = pointGeo[0][1];
	pGeo[2] = pointGeo[0][2];
	pGeo[3] = pointGeo[0][3];
	myShell->MakeFace( pGeo, 4 ); 
	// Back face
	pGeo[0] = pointGeo[1][0];
	pGeo[1] = pointGeo[1][1];
	pGeo[2] = pointGeo[1][2];
	pGeo[3] = pointGeo[1][3];
	myShell->MakeFace( pGeo, 4 ); 
	// Top face
	pGeo[0] = pointGeo[0][0];
	pGeo[1] = pointGeo[0][1];
	pGeo[2] = pointGeo[1][1];
	pGeo[3] = pointGeo[1][0];
	myShell->MakeFace( pGeo, 4 ); 
	// Bottom face
	pGeo[0] = pointGeo[0][2];
	pGeo[1] = pointGeo[0][3];
	pGeo[2] = pointGeo[1][3];
	pGeo[3] = pointGeo[1][2];
	myShell->MakeFace( pGeo, 4 ); 
	// Left face
	pGeo[0] = pointGeo[0][0];
	pGeo[1] = pointGeo[0][3];
	pGeo[2] = pointGeo[1][3];
	pGeo[3] = pointGeo[1][0];
	myShell->MakeFace( pGeo, 4 ); 
	// Rigth face
	pGeo[0] = pointGeo[0][1];
	pGeo[1] = pointGeo[0][2];
	pGeo[2] = pointGeo[1][2];
	pGeo[3] = pointGeo[1][1];
	myShell->MakeFace( pGeo, 4 ); 
}

char *CImportFastHenry::MemAlloc(int number, int size)
{
 
  char *res;
 
  res = (char *)malloc(number*size);

  if (res == NULL) {
    genError(20, "Out of memory");
  }
 
  return res;
}

// free memory allocated for FastHenry geometry
void CImportFastHenry::freeHenry()  
{
  GROUPS *group, *group1;
  NODES *node, *node1;
  SEGMENTS *seg, *seg1; 

  group = m_indsys.groups; 

  while( group != NULL ) { 
    node = group->nodes;
    while( node != NULL) {
      seg = node->segments; 
      while( seg != NULL ) { 
        seg1= seg; 
        seg=seg->next; 
		free(seg1->name);
        free(seg1); 
      }
      node1 = node; 
      node = node->next;
	  free(node1->name);
      free(node1); 
    }
    group1 = group;
    group = group->next; 
	free(group1->name1);
	free(group1->name2);
    free(group1); 
  }  

  free(m_indsys.units);
} 

/*
  Given the director parameters of a plane and of a vector, the function
  returns the director parameters of a versor orthogonal to the first
  one and lying on the plane, in the struct pointed to by res
  (input vector cannot be orthogonal to plane)
  (This can be seen as returning a versor orthogonal to the first two)
*/
int CImportFastHenry::orthPlane( PARAM vector, PARAM plane,
								PARAM *res ) 
{
	double kx, ky, kz, den;

	kz = plane.sx*vector.sy-vector.sx*plane.sy;   
	kx = plane.sy*vector.sz - vector.sy*plane.sz;   
	ky = vector.sx*plane.sz-plane.sx*vector.sz;   

	if( kz != 0.0 ) {  
		res->sz = 1; 
		res->sx = kx/kz;  
		res->sy = ky/kz;  
	}
	else if( ky != 0.0 ) {
		res->sz = 0; 
		res->sy = 1;  
		res->sx = kx/ky;   
	}  
	else if( kx != 0.0 ) {  
		res->sx = kx; 
		res->sy = 0; 
		res->sz = 0; 
	} 
	else 
		// input vector orthogonal to plane!
		return(1);

	den = sqrt(res->sx*res->sx + res->sy*res->sy + res->sz*res->sz);  
	res->sx = res->sx/den;  
	res->sy = res->sy/den;  
	res->sz = res->sz/den;  

	return(0); 
}

/* 
  Given two SPOINTS defining a 3D segment, a widthdir specifying the cosine directors 
  of a vector oriented along endfaces width, width and heigth of a segment, returns
  two arrays of four SPOINTS containing segment vertexs for each end face  
*/ 
void CImportFastHenry::corners( SPOINTS point1, SPOINTS point2, PARAM widthdir,
		double width, double height, SPOINTS *pointlist1, SPOINTS *pointlist2) 
{
	PARAM vector, res;
	double wx, wy, wz, hx, hy, hz; 

	/* compute director parameters of segment */  
	vector.sx = point2.x-point1.x;
	vector.sy = point2.y-point1.y;
	vector.sz = point2.z-point1.z;

	/* find dir orthogonal to segment and widthdir */  
	orthPlane( vector, widthdir, &res );

	/* distance of corners from node SPOINTS */ 
	wx = widthdir.sx*width/2;
	wy = widthdir.sy*width/2; 
	wz = widthdir.sz*width/2; 
	hx = res.sx*height/2; 
	hy = res.sy*height/2; 
	hz = res.sz*height/2;
 
	pointlist1[0].x = point1.x + wx  + hx ; 
	pointlist1[0].y = point1.y + wy  + hy ;
	pointlist1[0].z = point1.z + wz  + hz ; 
	pointlist1[1].x = point1.x - wx  + hx ;
	pointlist1[1].y = point1.y - wy  + hy ;
	pointlist1[1].z = point1.z - wz  + hz ;   
	pointlist1[2].x = point1.x - wx  - hx ;
	pointlist1[2].y = point1.y - wy  - hy ;
	pointlist1[2].z = point1.z - wz  - hz ;
	pointlist1[3].x = point1.x + wx  - hx ;
	pointlist1[3].y = point1.y + wy  - hy ;
	pointlist1[3].z = point1.z + wz  - hz ;
 
	pointlist2[0].x = point2.x + wx  + hx ;
	pointlist2[0].y = point2.y + wy  + hy ;
	pointlist2[0].z = point2.z + wz  + hz ;
	pointlist2[1].x = point2.x - wx  + hx ;
	pointlist2[1].y = point2.y - wy  + hy ;
	pointlist2[1].z = point2.z - wz  + hz ;
	pointlist2[2].x = point2.x - wx  - hx ; 
	pointlist2[2].y = point2.y - wy  - hy ;
	pointlist2[2].z = point2.z - wz  - hz ;
	pointlist2[3].x = point2.x + wx  - hx ;
	pointlist2[3].y = point2.y + wy  - hy ;
	pointlist2[3].z = point2.z + wz  - hz ;
} 
