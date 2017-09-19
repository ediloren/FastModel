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


// Geometry.h : definition of geometry-related classes
// Enrico Di Lorenzo 2003/03/12

#ifndef C3D_GEOMETRY_DEFS
#define C3D_GEOMETRY_DEFS

#include <math.h>

#include "Global.h"

/////////////////////////////////////////////////////
// 3D objects classes
// these are the classes which describes 3D objects
//


// C3D object types
//
#define C3D_UNINIT_ID		0x00000000
#define C3D_LISTHEAD_ID		0x00000001
#define C3D_POINTER_ID		0x00000002
#define C3D_VERTEX_ID		0x00010000
#define C3D_VERTEXUSE_ID	0x00020000
#define C3D_EDGE_ID			0x00030000
#define C3D_EDGEUSE_ID		0x00040000
#define C3D_LOOP_ID			0x00050000
#define C3D_FACEGEO_ID		0x00060000
#define C3D_FACE_ID			0x00070000
#define C3D_SHELL_ID		0x00080000
#define C3D_REGION_ID		0x00090000
#define C3D_MODEL_ID		0x000a0000
// face orientation w.r.t. plane unit vector 
#define C3D_O_SAME		0
#define C3D_O_REVERSE	1
// loop including / excluding space flag 
#define C3D_INCLUDING	0
#define C3D_EXCLUDING	1
// edgeuse direction w.r.t. edge
#define C3D_D_SAME		0
#define C3D_D_OPPOSITE	1

// Binary Space Partition tree definitions 
//
#define C3D_BSP_NONE		0
#define C3D_BSP_LEFT		1
#define C3D_BSP_RIGHT		2
#define C3D_BSP_ONPLANE	3

#define C3D_BSP_DIRX		0
#define C3D_BSP_DIRY		1
#define C3D_BSP_DIRZ		2

#define C3D_BSP_TOL		1E-12

// color definitions

#define C3D_HAS_COLOR	1
#define C3D_NO_COLOR	0

// anticipate class definition
class C3DOperation;

// declared classes
class C3DVector;
class C3DPlane;
class C3DBBox;
class C3DEntity;
class C3DVertex;
class C3DVertexUse;
class C3DEdge;
class C3DEdgeUse;
class C3DLoop;
class C3DFace;
class C3DShell;
class C3DRegion;
class C3DModel;


class C3DColor 
{
public:
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;

	C3DColor() {
		red = 0;
		green = 0;
		blue = 0;
		alpha = 255;
	}
};

// double linked list base class
template<class T> class C3DList 
{
	// the basic list cell 
	class Cell
	{
	public:
		Cell *next;
		Cell *prev;
		T *data;

		Cell(T *dat, Cell *nxt, Cell *prv) : data(dat), next(nxt), prev(prv) {}
		Cell(T *dat) : data(dat), next(NULL), prev(NULL) {}
	};

	// head of the list
	class Cell *head;
	// tail of the list
	class Cell *tail;
	// number of elements in list
	long elenum;

public:
	inline C3DList() : head(NULL), tail(NULL), elenum(0) {}
	inline ~C3DList() {clear();}

	// declaration required
	class iterator; 
	// make it friend
	friend class iterator;
	// definition
	class iterator 
	{
	public:
		Cell *p;
		// constructors
		iterator(Cell* cell) : p(cell) {}
		// copy-constructor
		iterator(const iterator& it) : p(it.p) {}
		// end sentinel iterator
		iterator() : p(NULL) {}

		inline void assign(T *newdata)
		{
			p->data = newdata;
		}

		inline void operator++() 
		{
			if(p != NULL)
				p = p->next;
		}

		inline void operator++(int)
		{
			operator++();
		}

		inline void operator--() 
		{
			if(p != NULL)
				p = p->prev;
		}

		inline void operator--(int)
		{
			operator--();
		}

		inline T *operator*()
		{
			if (!p) 
				return NULL;
			else 
				return (p->data);
		}

		inline T *operator->()
		{
			if (!p) 
				return NULL;
			else 
				return p->data;
		}

		inline bool operator==(const iterator &it)
		{
			return (p == it.p);
		}

		inline bool operator!=(const iterator &it)
		{
			return (p != it.p);
		}
	};


	inline iterator begin()
	{
		iterator it(head);

		return it;
	}

	inline iterator end()
	{
		iterator it;

		// end iterator points to NULL
		return it;
	}

	inline iterator rbegin()
	{
		iterator it(tail);

		return it;
	}

	inline iterator rend()
	{
		iterator it;

		// end iterator points to NULL
		return it;
	}

	inline iterator get_head()
	{
		iterator it(head);

		return it;
	}

	inline iterator get_tail()
	{
		iterator it(tail);

		return it;
	}

	inline void push_front(T *dat)
	{
		Cell *tmp;

		// if first element in list
		if( head == NULL ) {
			ASSERT(tail == NULL);
			head = tail = new Cell(dat);
		}
		else {
			tmp = head;
			// create new head cell, shifting old head ahead
			head = new Cell(dat, head, NULL);
			// adjust prev pointer of old head
			tmp->prev = head;
		}

		// increment the element counter
		elenum++;
	}

	inline void push_back(T *dat)
	{
		Cell *tmp;

		// if first element in list
		if( head == NULL ) {
			ASSERT(tail == NULL);
			head = tail = new Cell(dat);
		}
		else {
			tmp = tail;
			// create new tail cell, shifting old tail backward
			tail = new Cell(dat, NULL, tail);
			// adjust next pointer of old head
			tmp->next = tail;
		}

		// increment the element counter
		elenum++;
	}

	inline T *pop_front()
	{
		T *tmpData;
		Cell *tmpCell;

		if(head != NULL) {

			// save data to return
			tmpData = head->data;

			// if only one element in list
			if(elenum == 1) {
				ASSERT(head->next == head);
				ASSERT(head->prev == head);
				ASSERT(head == tail);
				delete head;
				head = tail = NULL;
			}
			else {
				tmpCell = head;
				// set new head
				head = head->next;
				// null prev link (is new head)
				head->prev = NULL;
				// delete old head
				delete tmpCell;
			}

			// decrement the element counter
			elenum--;

			return tmpData;
		}
		else
			return NULL;
	}

	inline T *pop_back()
	{
		T *tmpData;
		Cell *tmpCell;

		if(tail != NULL) {

			// save data to return
			tmpData = tail->data;

			// if only one element in list
			if(elenum == 1) {
				ASSERT(tail->next == tail);
				ASSERT(tail->prev == tail);
				ASSERT(head == tail);
				delete tail;
				head = tail = NULL;
			}
			else {
				tmpCell = tail;
				// set new head
				tail = tail->prev;
				// null prev link (is new head)
				tail->next = NULL;
				// delete old head
				delete tmpCell;
			}

			// decrement the element counter
			elenum--;

			return tmpData;
		}
		else
			return NULL;
	}

	inline T *pop(iterator &it)
	{
		T *tmpData;
		Cell *tmpCell;

		tmpCell = it.p;

		if(tmpCell != NULL) {
			tmpData = tmpCell->data;

			// if only one element in list
			if(elenum == 1) {
				ASSERT(tmpCell == head);
				ASSERT(head == tail);
				delete head;
				head = tail = NULL;
			}
			else if (tmpCell == head) {
				ASSERT(head->next);
				// shift head forward 
				head = head->next;
				head->prev = NULL;
				// delete old head
				delete tmpCell;		
			}
			else if (tmpCell == tail) {
				ASSERT(tail->prev);
				// shift tail backward 
				tail = tail->prev;
				tail->next = NULL;
				// delete old tail
				delete tmpCell;		
			}
			else {
				ASSERT(tmpCell->next);
				ASSERT(tmpCell->prev);
				// relink prev and next cells to skip cell to be popped
				tmpCell->prev->next = tmpCell->next;
				tmpCell->next->prev = tmpCell->prev;
				// delete old head
				delete tmpCell;
			}

			// decrement the element counter
			elenum--;

			return tmpData;
		}
		else
			return NULL;
	}

	inline void clear()
	{
		Cell *cell, *tmp;

		// fast erase: no need to relink cells,
		// since are all deleted
		for(cell = head; cell != NULL; cell = tmp) {
			tmp = cell->next;
			delete cell;
		}

		head = tail = NULL;
		elenum = 0;
	}

	// erase all elements between b and e (included)
	// could be optimized, see clear()
	inline void erase(iterator &b, iterator &e)
	{
		iterator it;

		for(it = b; it != e; it++)
			pop(it);

		// pop last element
		pop(e);
	}

	// return the length of the controlled sequence
	inline long size()
	{
		return elenum;
	}

	// return the length of the controlled sequence
	inline bool is_empty()
	{
		return (elenum == 0);
	}

	// find an element in the controlled sequence
	inline iterator find(T *dat)
	{
		iterator it;

		for(it = begin(); it != end(); it++)
			if( *it == dat)
				return it;

		// not found
		return NULL;
	}
};

// vector class specialized for 3d operations
class C3DVector  
{
public:
	double			x;
	double			y;
	double			z;

	C3DVector() : x(0.0), y(0.0), z(0.0) {}

	C3DVector(double xcoord, double ycoord, double zcoord) : 
				x(xcoord), y(ycoord), z(zcoord) {}

	friend double DotProd(C3DVector &vect1, C3DVector &vect2);
	friend double Mod(C3DVector &vect);
	friend C3DVector Cross(C3DVector &vect1, C3DVector &vect2);

	inline void pos(double xcoord, double ycoord, double zcoord)
	{
		x = xcoord;
		y = ycoord;
		z = zcoord;
	}

	inline bool operator==(C3DVector &point)
	{ 
		return( (*this - point).Mod() < C3D_BSP_TOL );
	}

	inline void operator=(C3DVector &point)
	{ 
		x = point.x;
		y = point.y;
		z = point.z;
	}

	inline void operator+=(C3DVector &point)
	{ 
		x += point.x;
		y += point.y;
		z += point.z;
	}

	inline C3DVector operator+(C3DVector &point)
	{ 
		C3DVector newPoint;

		newPoint.x = x + point.x;
		newPoint.y = y + point.y;
		newPoint.z = z + point.z;
	
		return newPoint; 
	}

	inline void operator-=(C3DVector &point)
	{ 
		x -= point.x;
		y -= point.y;
		z -= point.z;
	}

	inline C3DVector operator-(C3DVector &point)
	{ 
		C3DVector newPoint;

		newPoint.x = x - point.x;
		newPoint.y = y - point.y;
		newPoint.z = z - point.z;
	
		return newPoint; 
	}

	inline C3DVector operator*(const double &scalar)
	{
		C3DVector newPoint;
		
		newPoint.x = x * scalar;
		newPoint.y = y * scalar;
		newPoint.z = z * scalar;
		
		return newPoint; 
	}

	inline C3DVector operator*(const C3DVector &point)
	{
		C3DVector newPoint;
		
		newPoint.x = x * point.x;
		newPoint.y = y * point.y;
		newPoint.z = z * point.z;
		
		return newPoint; 
	}

	inline C3DVector operator/(const double &scalar)
	{
		C3DVector newPoint;
		
		if( scalar == 0 ) {
			GlbSysErrorMsg("C3DVector operator/ tried to divide by zero"); 
			return *this;
		}

		newPoint.x = x / scalar;
		newPoint.y = y / scalar;
		newPoint.z = z / scalar;
		
		return newPoint; 
	}

	inline C3DVector &operator/=(const double &scalar)
	{	
		if( scalar == 0 ) {
			GlbSysErrorMsg("C3DVector operator/ tried to divide by zero"); 
			return *this;
		}

		x /= scalar;
		y /= scalar;
		z /= scalar;
		
		return (*this); 
	}

	inline double Mod()
	{
		return sqrt(DotProd(*this, *this));
	}

	// array is 0-based (C/C++ convention)
    inline double &operator[](int i)
    { 
		if( i == 0 )
			return x;
		if( i == 1 )
			return y;
		if( i == 2 )
			return z;
		
		GlbSysErrorMsg("Tried to access C3DVector element %i (out of range)", i); 
		return z;
	}

	inline bool operator<(C3DVector &point)
	{ 
		return( x < point.x && y < point.y && z < point.z );
	}

	inline bool operator>(C3DVector &point)
	{ 
		return( x > point.x && y > point.y && z > point.z );
	}

};

inline double DotProd(C3DVector &vect1, C3DVector &vect2)
{ 
	C3DVector newPoint;
	
	return vect1.x*vect2.x + vect1.y*vect2.y + vect1.z*vect2.z;
}

inline double Mod(C3DVector &vect)
{
	return sqrt(DotProd(vect, vect));
}

inline C3DVector CrossProd(C3DVector &vect1, C3DVector &vect2) 
{
	C3DVector tmp;

	tmp.x = vect1.y*vect2.z - vect1.z*vect2.y;
	tmp.y = -vect1.x*vect2.z + vect1.z*vect2.x;
	tmp.z = vect1.x*vect2.y - vect1.y*vect2.x;

	return tmp;
}

// A line in 3D can be either represented as the intersection of two planes or,
// in parametric form, as a vector along the line direction and a point
// on the line (parameter origin on the line) 
class C3DLine
{
public:
	C3DVector	m_vecDir;
	C3DVector	m_vecPoint;

	// default constructor
	C3DLine() 
	{
		m_vecDir.x = 0.0;
		m_vecDir.y = 0.0;
		m_vecDir.z = 1.0;
		m_vecPoint.x = 0.0;
		m_vecPoint.y = 0.0;
		m_vecPoint.z = 0.0;
	}
};

inline C3DVector CrossProd(C3DLine &line1, C3DLine &line2) 
{
	return CrossProd(line1.m_vecDir, line2.m_vecDir);
}

// A plane in 3D is represented with the equation
// ax + by + cz + d = 0
// where a,b,c are the components of the plane normal vector
// and -d is the distance of the plane from the origin
// ( a(x-x0) + b(y-y0) + c(z-z0) = 0 is the equation, where (x0,y0,z0)
//   is a point on the plane. Then, -d = (a*x0+b*y0+c*z0) = dot_prod(normal,(x0,y0,z0))
//   is the distance of the plane from the origin)
class C3DPlane  
{
public:
	C3DVector	m_vecNormal;
	double		m_dD;

	// default constructor
	C3DPlane() 
	{
		m_vecNormal.x = 0.0;
		m_vecNormal.y = 0.0;
		m_vecNormal.z = 1.0;
		m_dD = 0.0;
	}
};

class C3DBBox
{
public:
	bool			empty;
	C3DVector		min_point;
	C3DVector		max_point;

	C3DBBox() : empty(true) {}

	void Clear()
	{
		min_point = C3DVector(0.0, 0.0, 0.0);
		max_point = C3DVector(0.0, 0.0, 0.0);
		empty = true;
	}

	inline void operator=(C3DBBox &bbox)
	{ 
		if (bbox.empty == FALSE) {
			min_point = bbox.min_point;
			max_point = bbox.max_point;
			empty = FALSE;
		}
		else 
			empty = TRUE;
	}

	inline void operator=(C3DVector &point)
	{ 
		min_point = point;
		max_point = point;
		empty = FALSE;
	}

	inline void operator+=(C3DBBox &bbox)
	{ 
		if (bbox.empty == FALSE) {
			if (empty == FALSE) {
				min_point.x = bbox.min_point.x < min_point.x ? bbox.min_point.x : min_point.x;
				min_point.y = bbox.min_point.y < min_point.y ? bbox.min_point.y : min_point.y;
				min_point.z = bbox.min_point.z < min_point.z ? bbox.min_point.z : min_point.z;
				max_point.x = bbox.max_point.x > max_point.x ? bbox.max_point.x : max_point.x;
				max_point.y = bbox.max_point.y > max_point.y ? bbox.max_point.y : max_point.y;
				max_point.z = bbox.max_point.z > max_point.z ? bbox.max_point.z : max_point.z;
			}
			else {
				min_point = bbox.min_point;
				max_point = bbox.max_point;
				empty = FALSE;
			}
		}
	}

	inline void operator+=(C3DVector &point)
	{ 
		if (empty == FALSE) {
			min_point.x = point.x < min_point.x ? point.x : min_point.x;
			min_point.y = point.y < min_point.y ? point.y : min_point.y;
			min_point.z = point.z < min_point.z ? point.z : min_point.z;
			max_point.x = point.x > max_point.x ? point.x : max_point.x;
			max_point.y = point.y > max_point.y ? point.y : max_point.y;
			max_point.z = point.z > max_point.z ? point.z : max_point.z;
		}
		else {
			min_point = point;
			max_point = point;
			empty = FALSE;
		}
	}

	inline C3DBBox operator+(C3DBBox &bbox)
	{ 
		C3DBBox newBbox;

		if (bbox.empty == FALSE) {
			if( empty == FALSE) {
				newBbox.min_point.x = bbox.min_point.x < min_point.x ? bbox.min_point.x : min_point.x;
				newBbox.min_point.y = bbox.min_point.y < min_point.y ? bbox.min_point.y : min_point.y;
				newBbox.min_point.z = bbox.min_point.z < min_point.z ? bbox.min_point.z : min_point.z;
				newBbox.max_point.x = bbox.max_point.x > max_point.x ? bbox.max_point.x : max_point.x;
				newBbox.max_point.y = bbox.max_point.y > max_point.y ? bbox.max_point.y : max_point.y;
				newBbox.max_point.z = bbox.max_point.z > max_point.z ? bbox.max_point.z : max_point.z;
				newBbox.empty = FALSE;
			}
			else {
				newBbox.min_point = bbox.min_point;
				newBbox.max_point = bbox.max_point;
				newBbox.empty = FALSE;
			}
		}
		else {
			if (empty == FALSE) {
				newBbox.min_point = min_point;
				newBbox.max_point = max_point;
				newBbox.empty = FALSE;
			}
			else {
				newBbox.empty = TRUE;
			}
		}

		return newBbox; 
	}

	inline C3DBBox operator+(C3DVector &point)
	{ 
		C3DBBox newBbox;

		if (empty == FALSE) {
			newBbox.min_point.x = point.x < min_point.x ? point.x : min_point.x;
			newBbox.min_point.y = point.y < min_point.y ? point.y : min_point.y;
			newBbox.min_point.z = point.z < min_point.z ? point.z : min_point.z;
			newBbox.max_point.x = point.x > max_point.x ? point.x : max_point.x;
			newBbox.max_point.y = point.y > max_point.y ? point.y : max_point.y;
			newBbox.max_point.z = point.z > max_point.z ? point.z : max_point.z;
			newBbox.empty = FALSE;
		}
		else {
			newBbox.min_point = point;
			newBbox.max_point = point;
			newBbox.empty = FALSE;
		}

		return newBbox; 
	}

};

class C3DEntity 
{
public:
	long			id;
	long			index;

	C3DEntity() : id(C3D_UNINIT_ID), index(0) {}
	C3DEntity(long newid) : id(newid), index(0) {}
};

class C3DVertex : public C3DEntity
{
public:
	// vertex spatial position
	C3DVector		pos;
	// radial vertexes list
	C3DList<C3DVertexUse> vertexUseList;

	// constructors & destructors
	C3DVertex(C3DVector newpos) : C3DEntity(C3D_VERTEX_ID), pos(newpos) {}
	~C3DVertex() {}

	void SetPos(C3DVector newpos) 
	{
		pos = newpos;
	}
};

class C3DVertexUse : public C3DEntity
{
public:
	// pointer to vertex used
	C3DVertex		*pVertex;
	// vertexuse father: can be shell, loop or edge
	union {
		C3DEntity		*pFather;
		C3DShell		*pFatherShell;
		C3DLoop			*pFatherLoop;
		C3DEdge			*pFatherEdge;
	};

	// constructors & destructors
	C3DVertexUse(C3DVector *newPos) : C3DEntity(C3D_VERTEXUSE_ID), pFatherEdge(NULL) 
	{
		C3DVertex newVert(*newPos);
		CBSPTree::iterator it;

		// if there's no vertex alredy in given position
		if( vertexTree.find(it, &newVert) == false ) {
			// create new vertex
			pVertex = new C3DVertex(*newPos);
			// and insert it in tree
			vertexTree.insert(it, pVertex);
		}
		else
			// otherwise reference to existing one
			pVertex = *it;

		// add new vertexuse to vertex vertexuse list
		pVertex->vertexUseList.push_front(this);

	}

	~C3DVertexUse()
	{
		C3DList<C3DVertexUse>::iterator it;

		// remove vertexuse from vertex vertexUseList

		it = pVertex->vertexUseList.find(this);
		// the vertexuse must be in vertexuse list
		ASSERT(*it != NULL);
		pVertex->vertexUseList.pop(it);

		// remove vertex from BSP tree and delete it,
		// if this was its last vertexuse
		if( pVertex->vertexUseList.is_empty() == true ) {
			vertexTree.erase(pVertex);
			delete pVertex;
		}
	}

	void SetFather(C3DEdge *pFather)
	{
		pFatherEdge = pFather;
	}
	void SetFather(C3DLoop *pFather)
	{
		pFatherLoop = pFather;
	}
	void SetFather(C3DShell *pFather)
	{
		pFatherShell = pFather;
	}


protected:
	class CBSPTree;

	// The BSP tree must be in common with all C3DVertexUse,
	// therefore is declared static
	static CBSPTree vertexTree;

	// Cartesian Binary Space Partition tree,
	// for maintaining unique vertex positions in space
	class CBSPTree 
	{
	public:
		// tree node
		class Node 
		{
		public:
			char divDir;
			union {
				double divPlane;
				C3DVertex *pObj;
			};
			bool child;
			Node *up;
			Node *left;
			Node *right;

			Node()
			{
			}
			Node(Node *upnode, C3DVertex *ppoint) : 
				up(upnode), pObj(ppoint), left(NULL), right(NULL), child(true)
			{
			}
		};

		// CBSPTree iterator
		class iterator;
		friend class iterator;
			
		class iterator 
		{
			friend class CBSPTree;
			Node *currentNode;
		public:
			iterator() : currentNode(NULL)
			{
			}
			iterator(CBSPTree& starttree) : currentNode(starttree.root)
			{	
			}
			iterator(iterator &it) : currentNode(it.currentNode)
			{	
			}
			iterator(Node *node) : currentNode(node)
			{	
			}
			C3DVertex *operator*() {
				if( currentNode == NULL )
					return NULL;

				if(currentNode->child == true)
					return currentNode->pObj;
				else
					return NULL;
			}
		};

	protected:
		// tree variables
		Node *root;
		double m_dTol;

	public:
		CBSPTree() : root(NULL), m_dTol(C3D_BSP_TOL) {}
		CBSPTree(C3DVertex *pObj) : m_dTol(C3D_BSP_TOL)
		{
			init(pObj);
		}
		void init(C3DVertex *pObj)
		{
			root = new Node;
			root->divDir = C3D_BSP_NONE;
			root->child = true;
			root->up = NULL;
			root->left = NULL;
			root->right = NULL;
			root->pObj = pObj;
		}
		void insert(C3DVertex *ppoint)
		{
			// if first vertex in tree, must init
			if(root == NULL)
				init(ppoint);
			else
				insert(root, ppoint);
		}
		void insert(iterator it, C3DVertex *ppoint)
		{
			// if first vertex in tree, must init
			if(root == NULL)
				init(ppoint);
			// otherwise, *it must be != NULL
			else {
				ASSERT(*it);
				insert(it.currentNode, ppoint);
			}
		}
		bool find(iterator &it, C3DVertex *ppoint)
		{
			Node *node;
			bool res;
			
			res = find(root, ppoint, &node);
			
			it.currentNode = node;
			
			return res;
		}
		bool erase(iterator &it) 
		{
			Node *father, *thischild, *otherchild;

			// check iterator; this validates not only
			// the object but also the pointer and the
			// childness status
			if(*it == NULL)
				return false;

			// remove point from tree

			// the old father will become the new node
			thischild = it.currentNode;
			father = thischild->up;

			// if father is NULL, this should be the root
			if(father == NULL) {
				ASSERT(thischild == root);
				// delete the only node in the tree
				delete root;
				// and set root to NULL, so everybody knows 
				// that the tree is empty
				root = NULL;

				return true;
			}

			// find out the other node between the two children
			if(father->left == thischild)
				otherchild = father->right;
			else
				otherchild = father->left;

			ASSERT(thischild);
			ASSERT(otherchild);

			// Copy from other child to father, according to otherchild's
			// childness status.
			// Copy everything but 'up', which must stay the same
			if( otherchild->child == true ) {
				father->pObj = otherchild->pObj;
				father->child = true;
				father->left = father->right = NULL;		
			}
			else {
				father->divDir = otherchild->divDir;
				father->divPlane = otherchild->divPlane;
				father->child = false;
				father->left = otherchild->left;
				father->right = otherchild->right;
				father->left->up = father;
				father->right->up = father;
			}

			// delete children
			delete thischild;
			delete otherchild;

			return true;

		}
		void erase(C3DVertex *ppoint)
		{
			iterator it;
			
			if(find(it, ppoint))
				erase(it);
		}

	protected:
		char planeSide(C3DVertex *ppoint, Node *node);
		bool insert(Node *node, C3DVertex *ppoint);
		bool find(Node *node, C3DVertex *ppoint, Node **lastnode);
	
			
	};

};

// TBC warning: remember that when father is set, bbox must propagate!


class C3DEdge : public C3DEntity
{
public:
	// pointers to edge vertex uses 
	C3DVertexUse	*pVertexUse1;
	C3DVertexUse	*pVertexUse2;
	// radial edges list
	C3DList<C3DEdgeUse> edgeUseList;

	// constructors & destructors
	C3DEdge(C3DVertexUse *vu1, C3DVertexUse *vu2) : C3DEntity(C3D_EDGE_ID), pVertexUse1(vu1),
													pVertexUse2(vu2) 
	{
		// set vertexuses father
		pVertexUse1->pFatherEdge = pVertexUse2->pFatherEdge = this;
	}

	~C3DEdge() 
	{
		// must exist!
		ASSERT(pVertexUse1);
		ASSERT(pVertexUse2);

		delete pVertexUse1;
		delete pVertexUse2;
	}
};

class C3DEdgeUse : public C3DEntity
{
public:
	// pointer to edge used
	C3DEdge		*pEdge;
	// direction w.r.t. pEdge
	char		direction;
	// edgeuse father: can be shell, loop or edge
	union {
		C3DEntity		*pFather;
		C3DLoop			*pFatherLoop;
		C3DShell		*pFatherShell;
	};

	// constructors & destructors

	// using this version of the constructor, vu1 and vu2 must be
	// two already-existing vertexuses; they should not have been 
	// created ad-hoc, since it's the edge which references the
	// vertexuses, not the edgeuse!
	C3DEdgeUse(C3DVertexUse *vu1, C3DVertexUse *vu2, C3DLoop *father) : C3DEntity(C3D_EDGEUSE_ID),
															pFatherLoop(father), pEdge(NULL)
	{
		CreateEdgeUse(vu1, vu2);
	}

	C3DEdgeUse(C3DVector *v1, C3DVector *v2, C3DLoop *father) : C3DEntity(C3D_EDGEUSE_ID),
															pFatherLoop(father), pEdge(NULL)
	{
		C3DVertexUse *vu1, *vu2;

		// Must create vertexuses to know if there already is 
		// an edge connecting the two given points v1 and v2.
		// If not, can create a new edge referencing the two
		// edgeuses; but if exists, it does already
		// reference two other vertexuses, so must delete
		// these ones (since the edgeuse references the edge,
		// NOT the vertexuses!)

		vu1 = new C3DVertexUse(v1);
		vu2 = new C3DVertexUse(v2);

		if(CreateEdgeUse(vu1, vu2) == true) {
			delete vu1;
			delete vu2;
		}
	}

	C3DEdgeUse(C3DVector *v1, C3DVector *v2, C3DShell *father) : C3DEntity(C3D_EDGEUSE_ID),
															pFatherShell(father), pEdge(NULL)
	{
		C3DVertexUse *vu1, *vu2;

		// Must create vertexuses to know if there already is 
		// an edge connecting the two given points v1 and v2.
		// If not, can create a new edge referencing the two
		// edgeuses; but if exists, it does already
		// reference two other vertexuses, so must delete
		// these ones (since the edgeuse references the edge,
		// NOT the vertexuses!)

		vu1 = new C3DVertexUse(v1);
		vu2 = new C3DVertexUse(v2);

		if(CreateEdgeUse(vu1, vu2) == true) {
			delete vu1;
			delete vu2;
		}
	}

	~C3DEdgeUse()
	{
		C3DList<C3DEdgeUse>::iterator it;

		// remove edgeuse from edge's edgeUseList

		it = pEdge->edgeUseList.find(this);
		// the edgeuse must be in vertexuse list
		ASSERT(*it != NULL);
		pEdge->edgeUseList.pop(it);

		// delete edge, if this was its last edgeuse
		if( pEdge->edgeUseList.is_empty() == true ) {
			delete pEdge;
		}
	}

	C3DVertexUse *GetFirstVertexUse()
	{
		if(direction == C3D_D_SAME) 
			return pEdge->pVertexUse1;
		else
			return pEdge->pVertexUse2;
	}

	C3DVertexUse *GetSecondVertexUse()
	{
		if(direction == C3D_D_SAME) 
			return pEdge->pVertexUse2;
		else
			return pEdge->pVertexUse1;
	}

private:
	// if edge between given vertexuses already exists, return true 
	bool CreateEdgeUse(C3DVertexUse *vu1, C3DVertexUse *vu2)
	{
		C3DVertex *v1, *v2;
		C3DEdge *edge;
		C3DList<C3DVertexUse>::iterator it;
		bool reverseDir, ret;

		// set return value to 'edge not existing'
		ret = false;
		// do not reverse edgeuse direction w.r.t edge 
		reverseDir = false;

		// get vertexuse's vertexes
		v1 = vu1->pVertex;
		v2 = vu2->pVertex;

		// v1 is the vertex with the smaller vertexuse list 
		if( v2->vertexUseList.size() < v1->vertexUseList.size()) {
			v1 = vu2->pVertex;
			v2 = vu1->pVertex;
			// reverse direction, because going to scan edges going
			// from vu2 to vu1
			reverseDir = !reverseDir;
		}

		// scan every vertexues in vertex list
		for(it = v1->vertexUseList.begin(); it != v1->vertexUseList.end(); it++) {
			// copy reference to father in local variable
			edge = it->pFatherEdge; 
			// if vertexuse father exists
			if( edge != NULL ) {
				// if father is really an edge
				if( edge->id == C3D_EDGE_ID ) {
					// if an edge which connects the two given vertexuse vertexes 
					// already exists, this will be another edgeuse of its
					if( edge->pVertexUse1->pVertex == v2 ) {
						// reference the existing edge
						pEdge = edge;
						// edge exists
						ret = true;
						// reverse direction, because edge goes from v2 to v1
						reverseDir = !reverseDir;
						break;
					}
					if( edge->pVertexUse2->pVertex == v2 ) {
						// reference the existing edge
						pEdge = edge;
						// edge exists
						ret = true;
						break;
					}
				}
			}
		}

		// no edge already exists, must create one
		if(pEdge == NULL) {
			pEdge = new C3DEdge(vu1, vu2);
			// in this case direction is of course the same 
			direction = C3D_D_SAME;
		}
		else
			// reflect direction w.r.t. edge
			reverseDir == true ? direction = C3D_D_OPPOSITE : direction = C3D_D_SAME;

		// add new edgeuse to edge's edgeuse list
		pEdge->edgeUseList.push_front(this);

		return ret;
	}

};

class C3DLoop : public C3DEntity
{
public:
	// loop type, i.e., including / excluding space
	char	type;
	// Both edgeuses and vertex can be child. If list of edgeuses
	// in loop is empty, then vertexuse is the child and vice-versa.
	C3DList<C3DEdgeUse>	edgeUseList;
	C3DVertexUse	    *pVertexUse;
	// loop bounding box
	C3DBBox			bbox;
	// loop father: can be face or shell
	union {
		C3DEntity		*pFather;
		C3DFace			*pFatherFace;
		C3DShell		*pFatherShell;
	};

	// constructors & destructors
	C3DLoop() : C3DEntity(C3D_LOOP_ID), pVertexUse(NULL), pFatherFace(NULL) {}
	C3DLoop(C3DFace *father) : C3DEntity(C3D_LOOP_ID), pVertexUse(NULL), pFatherFace(father) {}
	C3DLoop(C3DShell *father) : C3DEntity(C3D_LOOP_ID), pVertexUse(NULL), pFatherShell(father) {}
	C3DLoop(C3DFace *father, C3DList<C3DVector> &points); 
	~C3DLoop();

	C3DLoop *CopyLoop(C3DOperation &op, C3DEntity *father);
};

class C3DFace : public C3DEntity
{
public:
	// plane on which face lies
	C3DPlane		plane;
	// face orientation w.r.t. plane normal (same or opposite)
	char				orientation;
	// face bounding box
	C3DBBox			bbox;
	// list of loops in face
	C3DList<C3DLoop>	loopList;
	// face father: can only be shell
	C3DShell			*pShell;
	// color paramters
	char hasColor;
	float density;
	// face color
	C3DColor rgb;

	// constructors & destructors
	C3DFace() : C3DEntity(C3D_FACE_ID), pShell(NULL), hasColor(C3D_NO_COLOR) {}
	C3DFace(C3DShell *father) : C3DEntity(C3D_FACE_ID), pShell(father), hasColor(C3D_NO_COLOR) {}
	C3DFace(C3DShell *father, C3DList<C3DVector> &points);
	~C3DFace();

	void EnlargeBBox(C3DBBox enlBbox);
	C3DFace *CopyFace(C3DOperation &op);
};

class C3DShell : public C3DEntity
{
public:
	// shell father: can only be another shell
	C3DShell			*pShell;
	// shell bounding box
	C3DBBox				bbox;
	// list of shells in shell
	C3DList<C3DShell>	shellList;
	// list of faces in shell
	C3DList<C3DFace>	faceList;
	// list of loops in shell
	C3DList<C3DLoop>	loopList;
	// list of edges in shell
	C3DList<C3DEdgeUse>	edgeUseList;
	// vertexuse in shell (if any)
	C3DVertexUse		*pVertexUse;

	// constructors & destructors
	C3DShell() : C3DEntity(C3D_SHELL_ID), pShell(NULL), pVertexUse(NULL) {}
	C3DShell(C3DShell *pFather) : C3DEntity(C3D_SHELL_ID), pShell(pFather), pVertexUse(NULL) 
	{
		// add new subshell to parent shell's shell list
		pShell->shellList.push_front(this);
	}
	~C3DShell();

	C3DFace *MakeFace(C3DList<C3DVector> &points);
	C3DFace *MakeFace(C3DVector points[], int numPoints, float density = 0.0, bool hascolor = false, unsigned long color = 0);
	void EnlargeBBox(C3DBBox enlBbox);
	void UpdateBBox();
	void Translate(C3DVector shift);
	void Scale(C3DVector scale);
	void RotateX(double angle);
	void RotateY(double angle);
	void RotateZ(double angle);

protected:
	void Transform(C3DOperation &op);

};

#endif //C3D_GEOMETRY_DEFS

