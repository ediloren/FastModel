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


// ErrorHandler.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include <stdio.h>
#include <string.h>

#include "DebugHandler.h"

/* this function collects all errors from C++ language functions */
void CDebugHandler::DebugMsg()
{
	fprintf(stdout, "%s\n", m_dmsg);
}

/* this function collects all errors from C language functions */
extern "C" int commonDebugMsg(char *s) 
{
	CDebugHandler dbgobj;

	strcpy(dbgobj.m_dmsg, s);
	dbgobj.DebugMsg();

    return(0);
}


