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


// ImportFastHenry.h : interface of the CImportFastHenry class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef IMPORT_FASTHENRY_H
#define IMPORT_FASTHENRY_H

#include "StdAfx.h"

#include <stdio.h>
#include <math.h> 

#include "ColorEditView.h"
#include "FastHenryLexer.h"
#include "ImportInColorView.h"

// define types of import operations; 
// note that values >= 256 are free to be defined by overloaded classes,
// while values < 256 are defined in base class CImportInColorView
#define IFH_BLOCK_GND_PLANES		256

// these are the codes for setColor;
// but they are not used here, in contrast with ImportFastCap,
// except IFH_ERROR
// TBC warning: should use them!
#define IFH_COMMENT		1
#define IFH_CONTROL		2
#define IFH_ELEMENT		3
#define IFH_ERROR		4
#define IFH_DEFAULT		5
#define IFH_TEXT		6

// defines for 'read' routines answers
#define IFH_GO			0
#define IFH_NONGO		1

// types of gnd plane holes
#define IFH_HOLE_POINT		0
#define IFH_HOLE_RECT		1
#define IFH_HOLE_CIRCLE		2

#define IFH_MAX_NAME_LEN	80
#define IFH_MAX_GPAR_CHARS	10
#define IFH_PIDIV2			1.570796327

// tolerance in cosine of angle (used to verify that vectors are perpendicular), 
// corresponding to 1 degree
#define COSANGLETOL		0.01745240643728
// tolerance in dimension measurements
#define IFH_TOL			1E-3

// The following classes are derived from the header 
// for FastHenry to FastCap converter (converthenry) 

#define FALSE 0 
#define TRUE 1 

#ifndef MAX
#define MAX(A,B)  ( (A) > (B) ? (A) : (B) )
#endif

class PARAM
{ 
public:
   double sx;
   double sy; 
   double sz;
}; 

class SPOINTS
{
public:
  double x;
  double y;
  double z;
};

class NODES;

class SEGMENTS
{
public:
   SEGMENTS *next;
   char *name;
   double width;
   double height;
   PARAM widthdir;
   NODES *node[2];
};
 
class NODES
{  
public:
   NODES *next; 
   char *name; 
   SPOINTS point; 
   int num_segs; 
   SEGMENTS *segments;  
   SEGMENTS *endsegment;
   bool isPseudoNode;
}; 

class GROUPS
{  
public:
   GROUPS *next; 
   GROUPS *prev; 
   char *name1; 
   char *name2; 
   int num_nodes; 
   NODES *nodes;  
   NODES *endnode; 
}; 

class SYS
{
public:
   int num_groups;
   char *title;
   char *units;
   GROUPS *groups;
   GROUPS *endgroup;
};

class CHole 
{
public:
  char m_cHoleType;
  // for hole types point, rect and circle max number of parameters is 6
  double m_dValues[6];                 
  CHole *m_pNext;
};

class CNode
{
public:
  double m_dX;
  double m_dY;
  double m_dZ;
  char m_cDeleted;
};

class CGroundPlane
{
public:
	char m_sGndPlaneName[IFH_MAX_NAME_LEN], m_sNonuniGndName[IFH_MAX_NAME_LEN];
	PARAM m_clVector1, m_clVector2, m_clPerpVector, m_clVersor1, m_clVersor2, m_clPerpVersor;
	SPOINTS m_clCorners[4], m_clRel;
	int m_iSeg1, m_iSeg2;
	double m_dLen1, m_dLen2;
	double m_dThickness, m_dSegwid1, m_dSegwid2;
	// uniform plane vars
	PARAM m_clD1, m_clD2;
	double m_dD1mod, m_dD2mod, m_dDiagmod, m_dSegfull1, m_dSegfull2;
	CNode **m_clNodeMatrix;
	CHole *m_clHoleList;
};

// this are the import class definitions
class CDefaultToken
{
public:
	float	val;
	char	isDef;
};

class CImportFastHenry : public CImportInColorView, public CFastHenryLexer
{
public:
	CImportFastHenry();

protected:
	char readFloat(float *result);
	char readText(char *result, int maxchar);
	char readNode(char *result, int maxchar, char errOnNewLine);
	char readDef(int *defTok, float *value);
	char readGParam(char *result, int *defTok, float *value);
	char readGndValues(double *vals, int valuesnum, char *err);
	char readGndNode(double *vals, char *err);
	GROUPS *makegroup(char *name, double x, double y, double z, bool isPseudoNode = false);
	void makeseg(char *name, NODES *node0, NODES *node1,
							   double height, double width, PARAM widthdir); 
	GROUPS *get_nodegroup_from_name(char *name, NODES **node);
	void merge_nodegroups(GROUPS *group1, GROUPS *group2);
	void makeBlockGndPlane(CGroundPlane *gp);
	void makeUniformGndPlane(CGroundPlane *gp);
	void findNearestNode(double x, double y, double z, CGroundPlane *gp, int *i, int *j);
	double **NodeMatrixAlloc(int rows, int cols);
	void NodeMatrixDelete(CNode **nodematrix, int rows, int cols);
	void convertToModel();
	void convertGroup(GROUPS *group, C3DShell *myShell); 
	void createParallelepiped(SPOINTS pointlist1[4], SPOINTS pointlist2[4], C3DShell *myShell);
	char *MemAlloc(int number, int size);
	void freeHenry(); 
	inline double cosAngleBetweenVectors(PARAM vector1, PARAM vector2)
	{
		double dotval, mod1, mod2;

		dotval = dotProduct(vector1, vector2);
		mod1 = module(vector1);
		mod2 = module(vector2);

		return( dotval / (mod1 * mod2) );
	}
	inline double dotProduct(PARAM vector1, PARAM vector2)
	{
		return( vector1.sx * vector2.sx + vector1.sy * vector2.sy + vector1.sz * vector2.sz );
	}

	inline double module(PARAM vector)
	{
		return( sqrt(vector.sx * vector.sx + vector.sy * vector.sy + vector.sz * vector.sz) );
	}	
	inline double module(double sx, double sy, double sz)
	{
		return( sqrt(sx * sx + sy * sy + sz * sz) );
	}	
	int orthPlane( PARAM vector, PARAM plane, PARAM *res ); 
	void corners( SPOINTS point1, SPOINTS point2, PARAM widthdir,
		double width, double height, SPOINTS *pointlist1, SPOINTS *pointlist2); 

	float 	m_fXtran;
	float 	m_fYtran;
	float 	m_fZtran;
	C3DShell *m_pCurrShell;
	char m_cReadRes, m_cDotEnd;
	SYS m_indsys; 
	CDefaultToken m_defX, m_defY, m_defZ, m_defH, m_defW;
	CDefaultToken m_defRho, m_defSigma, m_defNhinc, m_defNwinc;
	CDefaultToken m_defRw, m_defRh;

// CImportInColorView overrides
public:
	// overloaded import function
	int import(unsigned int operation, FILE *fp, CColorEditView *colorEditView,
		C3DShell *model, long baselineno, long startline, long endline,
		float xtran, float ytran, float ztran);
							 
protected:
	void setColor(unsigned int colorIndex);
	char processLine();

// CLexer overrides
	inline int getInputChar();
	inline int getTok();
	inline int  moveForward(unsigned int *position);
};


#endif
