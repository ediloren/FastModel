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


// CImportPatran import class - Enrico Di Lorenzo 2001/09/15
//

#include "stdafx.h"

#include "ImportPatran.h"
#include "fastmodelDoc.h"
#include "FastCapView.h"
#include "fastmodel.h"

CImportPatran::CImportPatran()
{
	strcpy(m_sErrorMsg, "IMPPATRAN"); 
}

// override standard lexer getInputChar function
inline int CImportPatran::getInputChar()
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
inline int CImportPatran::getTok()
{
	m_iTok = CPatranLexer::getTok();

	// concatenate any text read
	if ( (m_uiOperation & IICV_COLOR_LINES) ) {
		// spaces before token
		strcat(m_sImportText, m_sSpaceBuffer);
		// the token
		strcat(m_sImportText, m_sString);
	}

	return m_iTok;
}

// override to allow CImportInColorView to access CPatranLexer functions;
// see getTok for details; does not skip spaces to allow syncNewLine proper
// function (syncNewLine reads until newline found, then backs to last-1 token; 
// if spaces are skipped, last-1 token is not correct if line ends with spaces)
// however, this function is not used here.
inline int  CImportPatran::readTok(unsigned int *position)
{
	m_iTok = CPatranLexer::readTok(position);

	// concatenate any text read
	if ( (m_uiOperation & IICV_COLOR_LINES) ) {
		// spaces before token
		strcat(m_sImportText, m_sSpaceBuffer);
		// the token
		strcat(m_sImportText, m_sString);
	}

	return m_iTok;
}

// override to allow CImportInColorView to access CPatranLexer functions
inline int  CImportPatran::moveForward(unsigned int *position)
{
	return(CPatranLexer::moveForward(position));
}

void CImportPatran::setColor(unsigned int colorIndex)
{
	if ( colorIndex == IP_COMMENT )
		strcat(m_pColorEditView->m_sRTFLine, IICV_COMMENT_COLOR);
	else if ( colorIndex == IP_CONTROL )
		strcat(m_pColorEditView->m_sRTFLine, IICV_CONTROL_COLOR);
	else if ( colorIndex == IP_ELEMENT )
		strcat(m_pColorEditView->m_sRTFLine, IICV_ELEMENT_COLOR);
	else if ( (colorIndex == IP_ERROR) )
		strcat(m_pColorEditView->m_sRTFLine, IICV_ERROR_COLOR);
	else
		strcat(m_pColorEditView->m_sRTFLine, IICV_DEFAULT_COLOR);
}

char CImportPatran::readFloat(float *result)
{
	if(m_cReadRes == IP_GO) {
		getTok();

		if( m_iTok!=FCL_FLOAT ) {
			genError(1, "bad float data");
			return(IP_NONGO);
		}

		*result = m_fFloat;

		return(IP_GO);
	}
	else
		return(IP_NONGO);
}

char CImportPatran::readLong(long *result)
{
	long res;

	if(m_cReadRes == IP_GO) {
		getTok();

		res = (long) m_fFloat;

		if( m_iTok!=FCL_FLOAT || ((float)res) != m_fFloat ) {
			genError(1, "bad integer data");
			return(IP_NONGO);
		}

		*result = res;

		return(IP_GO);
	}
	else
		return(IP_NONGO);
}

char CImportPatran::readText(char *result, int maxchar)
{
	if(m_cReadRes == IP_GO) {
		getTok();

		if( m_iTok!=FCL_TEXT ) {
			genError(4, "bad text data '%s'", m_sString);
			return(IP_NONGO);
		}

		strncpy(result, m_sString, maxchar);

		return(IP_GO);
	}
	else
		return(IP_NONGO);
}

char CImportPatran::readTextOrNumber(char *result, int maxchar)
{
	if(m_cReadRes == IP_GO) {
		getTok();

		if( m_iTok!=FCL_TEXT && m_iTok!=FCL_FLOAT) {
			genError(5, "bad text or number data '%s'", m_sString);
			return(IP_NONGO);
		}

		strncpy(result, m_sString, maxchar);

		return(IP_GO);
	}
	else
		return(IP_NONGO);
}

char CImportPatran::processLine()
{
	char ret;
	long type, ID, IV, KC, N1, N2, N3, N4, N5;
	long nodenum, dummyl, node[4];
	long ndim, lshape, nodes, iconf, lph, lph_id;
	long elem1, elem2, elemindex;
	float x, y, z, dummyf;
	int i;
	CPatranCFEG *cfeg;

	// handles EOF and NEWLINE
	ret = CImportInColorView::processLine();

	if( ret != IICV_CONTINUE ) 
		return ret;

	switch (m_iTok)
	{
	
		case FCL_FLOAT :

			m_cReadRes = IP_GO;

			type = (long)m_fFloat;
			m_cReadRes |= readLong(&ID);
			m_cReadRes |= readLong(&IV);
			m_cReadRes |= readLong(&KC);
			m_cReadRes |= readLong(&N1);
			m_cReadRes |= readLong(&N2);
			m_cReadRes |= readLong(&N3);
			m_cReadRes |= readLong(&N4);
			m_cReadRes |= readLong(&N5);

			if( m_cReadRes == IP_GO) {
				if (m_uiOperation & IICV_COLOR_LINES) {
					setColor(IP_CONTROL);
					copyToRTF();
				}

				switch(type) {

				// if title line 
				case 25:

					// skip one line
					skipLines(1, IP_ELEMENT);

					return(IICV_OK);

				// summary data
				case 26:

					// allocate PATRAN structures
					m_pNodes = new CPatranNode[N1];
					m_lNodeIndex = 0;
					m_pElements = new CPatranElement[N2];
					m_lElementIndex = 0;

					// skip one line
					skipLines(1, IP_COMMENT);

					return(IICV_OK);

				// node data
				case 1:

					// position at end of previous line
					syncNewLine(IICV_SYNC_NO_ERROR);
					// add newline
					if (m_uiOperation & IICV_COLOR_LINES) {
						strcat(m_sImportText, "\\par ");
						copyToRTF();
					}
					// then read values
					m_cReadRes |= readFloat(&x);
					m_cReadRes |= readFloat(&y);
					m_cReadRes |= readFloat(&z);

					if(m_cReadRes == IP_GO) {

						if(m_pNodes == NULL) {
							genError(10, "no node array allocated, probably because summary data packet has not already been read");
							return(IICV_ERROR);
						}

						m_pNodes[m_lNodeIndex].m_lId = ID;
						m_pNodes[m_lNodeIndex].m_fX = x;
						m_pNodes[m_lNodeIndex].m_fY = y;
						m_pNodes[m_lNodeIndex].m_fZ = z;

						m_lNodeIndex++;

						if (m_uiOperation & IICV_COLOR_LINES) {
							setColor(IP_ELEMENT);
							copyToRTF();
						}
						
						// skip one line
						skipLines(1, IP_COMMENT);

						return(IICV_OK);
					}
					else {
						genError(20, "cannot read node coordinates");
						return(IICV_ERROR);
					}

				// element data
				case 2:

					if((IV != 3) && (IV != 4)) {
						skipLines(KC, IP_COMMENT);
						return(IICV_OK);
					}
					
					// position at end of previous line
					syncAndAddNewLine(IP_COMMENT);
					// then read values
					m_cReadRes |= readLong(&nodenum);
					m_cReadRes |= readLong(&dummyl);
					m_cReadRes |= readLong(&dummyl);
					m_cReadRes |= readLong(&dummyl);
					m_cReadRes |= readFloat(&dummyf);
					m_cReadRes |= readFloat(&dummyf);
					m_cReadRes |= readFloat(&dummyf);

					if(m_cReadRes == IP_GO) {
					
						// position at end of previous line
						syncAndAddNewLine(IP_ELEMENT);
						// then read values
						m_cReadRes |= readLong(&node[0]);
						m_cReadRes |= readLong(&node[1]);
						m_cReadRes |= readLong(&node[2]);
						if(nodenum == 4) 
							m_cReadRes |= readLong(&node[3]);

						if(m_cReadRes == IP_GO) {

							if(m_pElements == NULL) {
								genError(11, "no element array allocated, probably because summary data packet has not already been read");
								return(IICV_ERROR);
							}
	
							m_pElements[m_lElementIndex].m_lId = ID;
							m_pElements[m_lElementIndex].m_cNodenum = (char)nodenum;
							for(i=0; i<nodenum; i++)
								m_pElements[m_lElementIndex].m_lNodes[i] = node[i];

							m_lElementIndex++;

							if (m_uiOperation & IICV_COLOR_LINES) {
								setColor(IP_ELEMENT);
								copyToRTF();
							}
						
							return(IICV_OK);
						
						}
						else {
							genError(21, "cannot read element nodes");
							return(IICV_ERROR);
						}
					}
					else {
						genError(22, "cannot read element properties");
						return(IICV_ERROR);
					}

				// CFEG data
				case 45:

					// skipLines does not add a newline ('/par') after
					// last line skipped to allow use as last instruction
					// before return; but here a line is skipped in the
					// between, so must add the newline
					skipLines(1, IP_COMMENT);
					syncAndAddNewLine(IP_COMMENT);
					
					// then read values
					m_cReadRes |= readLong(&ndim);
					m_cReadRes |= readLong(&lshape);
					m_cReadRes |= readLong(&nodes);
					m_cReadRes |= readLong(&iconf);
					m_cReadRes |= readLong(&lph);
					m_cReadRes |= readLong(&lph_id);
					m_cReadRes |= readLong(&dummyl);
					m_cReadRes |= readLong(&dummyl);

					if(m_cReadRes == IP_GO) {

						// if not patch data, skip everything
						if(lph != 3) {
							if (m_uiOperation & IICV_COLOR_LINES) {
								setColor(IP_ELEMENT);
								copyToRTF();
							}
							skipLines(KC-2, IP_COMMENT);
							return(IICV_OK);
						}

						cfeg = new CPatranCFEG;

						// if first one
						if( m_pCFEGcurrent == NULL ) {
							m_pCFEGstart = m_pCFEGcurrent = cfeg;
							m_pCFEGstart->m_pNext = NULL;
							m_pCFEGstart->m_pPrev = NULL;
						}
						// otherwise link in chain
						else {
							m_pCFEGcurrent->m_pNext = cfeg;
							cfeg->m_pPrev = m_pCFEGcurrent;
							cfeg->m_pNext = NULL;
							m_pCFEGcurrent = cfeg;
						}

						cfeg->m_lID = ID;
						cfeg->m_lNELS = IV;
						cfeg->m_pElemList = new long[IV];
						cfeg->m_lLPH = lph;
						cfeg->m_lLPH_ID = lph_id;
						cfeg->m_lLSHAPE = lshape;
						cfeg->m_lNDIM = ndim;
						cfeg->m_lNODES = nodes;
						cfeg->m_lICONF = iconf;

						elemindex = 0;
						for (i=1; i<=IV/2 && m_cReadRes == IICV_OK; i++) {

							syncAndAddNewLine(IP_ELEMENT);

							m_cReadRes |= readLong(&dummyl);
							m_cReadRes |= readLong(&dummyl);
							m_cReadRes |= readLong(&dummyl);
							m_cReadRes |= readLong(&dummyl);
							m_cReadRes |= readLong(&elem1);
							m_cReadRes |= readLong(&dummyl);
							m_cReadRes |= readLong(&dummyl);
							m_cReadRes |= readLong(&dummyl);
							m_cReadRes |= readLong(&dummyl);
							m_cReadRes |= readLong(&elem2);
							cfeg->m_pElemList[elemindex++] = elem1;
							cfeg->m_pElemList[elemindex++] = elem2;

						}
						if (IV%2 && m_cReadRes == IICV_OK) {

							syncAndAddNewLine(IP_ELEMENT);

							m_cReadRes |= readLong(&dummyl);
							m_cReadRes |= readLong(&dummyl);
							m_cReadRes |= readLong(&dummyl);
							m_cReadRes |= readLong(&dummyl);
							m_cReadRes |= readLong(&elem1);
							cfeg->m_pElemList[elemindex++] = elem1;
						}

						if(m_cReadRes == IP_GO) {

							if (m_uiOperation & IICV_COLOR_LINES) {
								setColor(IP_ELEMENT);
								copyToRTF();
							}

							return(IICV_OK);

						}
						else {
							genError(31, "cannot read CFEG element data");
							return(IICV_ERROR);
						}
					}
					else {
						genError(32, "cannot read CFEG properties");
						return(IICV_ERROR);
					}

				// any other packet
				default:
					skipLines(KC, IP_COMMENT);
					return(IICV_OK);

				}
			}
			else {
				genError(40, "cannot read packet type");
				return(IICV_ERROR);
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

void CImportPatran::convertPatran2Model()
{
	CPatranCFEG	*cfeg;

	if (m_uiOperation & IICV_CONVERT_TO_MODEL) {
		cfeg = m_pCFEGstart;

		while (cfeg != NULL) {
			// if region type is 'patch'
			if (cfeg->m_lLPH == 3) {
				convertPatchToFaces(cfeg->m_lNELS, cfeg->m_pElemList);
			}
			cfeg = cfeg->m_pNext;
		}
	}
}

void CImportPatran::convertPatchToFaces(long nels, long *elemList)
{
	long i, j, elemNum;
	CPatranElement *elemPtr;
	CPatranNode *nodePtr;
	C3DVector pointGeo[4];

	for(i=0; i<nels; i++) {
		
		// get the element number (used as index in the elements array)
		elemNum = *(elemList + i);
		// in Patran Neutral file, elem number can be negative
		if( elemNum < 0 ) 
			elemNum = -elemNum;

		// get the element (remark: Patran arrays start at 1)
		elemPtr = &m_pElements[elemNum-1];

		// retrieve the nodes and hence the nodes positions
		for(j=0; j<elemPtr->m_cNodenum; j++) {
			// get the node (remark: Patran arrays start at 1)
			nodePtr = &m_pNodes[elemPtr->m_lNodes[j]-1];
			pointGeo[j].pos(nodePtr->m_fX + m_fXtran, 
						nodePtr->m_fY + m_fYtran, nodePtr->m_fZ + m_fZtran);
		}

		// create the face with 3 or 4 vertex as specified by elemPtr->m_cNodenum
		m_pCurrShell->MakeFace( pointGeo, elemPtr->m_cNodenum );
	}

}

// these are the compare function required and used
// by standard library function 'qsort'
static int compareNodes(const void *a, const void *b)
{
	CPatranNode *p1 = (CPatranNode *) a;
	CPatranNode *p2 = (CPatranNode *) b;
	long diff = p1->m_lId - p2->m_lId;

	if (diff > 0) {
		return 1;
	} else if (diff < 0) {
		return -1;
	} else {
		return 0;
	}
}

static int compareElements(const void *a, const void *b)
{
	CPatranElement *p1 = (CPatranElement *) a;
	CPatranElement *p2 = (CPatranElement *) b;
	long diff = p1->m_lId - p2->m_lId;

	if (diff > 0) {
		return 1;
	} else if (diff < 0) {
		return -1;
	} else {
		return 0;
	}
}

// main import function (overridden)
int CImportPatran::import(unsigned int operation, FILE *fp,
							  CColorEditView *colorEditView, C3DShell *model,
							  long baselineno, long startline, long endline,
							  float xtran, float ytran, float ztran)
{
	char answer;
	CPatranCFEG *cfeg, *nextcfeg;

	// call base function
	if( CImportInColorView::import(operation, fp, colorEditView, model, 
		baselineno, startline, endline) == IICV_ERROR )
		return 0;

	if (m_uiOperation & IICV_CONVERT_TO_MODEL) {
		// init current shell into which import Patran data
		m_pCurrShell = m_pShell;
	}


	// store translation
	m_fXtran = xtran;
	m_fYtran = ytran;
	m_fZtran = ztran;

	// initialize lexer
	initLexer();

	// init class vars
	m_pNodes = NULL;
	m_lNodeIndex = 0;
	m_pElements = NULL;
	m_lElementIndex = 0;
	m_pCFEGstart = NULL;
	m_pCFEGcurrent = NULL;


	// process rest of file, line by line
	//

	getTok();
	while( m_iTok != LEXER_EOF ) {
	
		answer = processLine();

		if ( answer == IICV_ERROR ) {
			// if error, resync the input stream to next line 
			syncNewLine(IICV_SYNC_WITH_ERROR);
			if (m_uiOperation & IICV_COLOR_LINES) {
				setColor(IP_ERROR);
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
				setColor(IP_ERROR);
				copyToRTF();

			}
		}
	}

	// sort node and element lists
	qsort(m_pNodes, m_lNodeIndex, sizeof(CPatranNode), compareNodes);
	qsort(m_pElements, m_lElementIndex, sizeof(CPatranElement), compareElements);

	// convert PATRAN to FastModel
	//
	convertPatran2Model();

	// delete allocated memory, if pointers not NULL:
	// node, element and CFEG lists

	if( m_pNodes != NULL )
		delete m_pNodes;
	if( m_pElements != NULL )
		delete m_pElements;

	cfeg = nextcfeg = m_pCFEGstart;
	while(cfeg != NULL) {
		nextcfeg = cfeg->m_pNext;
		delete cfeg->m_pElemList;
		delete cfeg;
		cfeg = nextcfeg;
	}
		

	// number of cut chars (in case of line too long)
	return(m_iCutChars);
}




