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


// Geometry.cpp : implementation of the geometry-related classes
// Enrico Di Lorenzo 2003/03/12

#include "stdafx.h"

#include <stdio.h>
#include "Geometry.h"
#include "ErrorHandler.h"
#include "Operation.h"

// Static data members must be initialized at file scope, even
// if private.
//
// static initializations:

C3DVertexUse::CBSPTree C3DVertexUse::vertexTree;


///////////////////////////////////////////////////////////////
// C3D geometry classes implementations

// Find 'ppoint' in the BSP tree, if existing
// Return true if existing and, in this case, 'lastnode' will point to node
// containing the vertex
// Return false if not existing and 'lastnode' will point to last node tested
// (so it is possible to insert the vertex being searched without scanning
// again the tree); however, if 'node' is NULL, 'lastnode' will point to NULL 
bool C3DVertexUse::CBSPTree::find(Node *node, C3DVertex *ppoint, Node **lastnode)
{
	char side;
	double dist;
	
	// if tree not empty
	if(node != NULL) {
		// if current node not a child
		if(node->child == false) {
			// decide on which side the point lies
			side = planeSide(ppoint, node);
			// and recursively visit tree
			if(side == C3D_BSP_LEFT) {
				return find(node->left, ppoint, lastnode);
			}
			// this covers both the 'right' and 'onplane' cases
			else {
				return find(node->right, ppoint, lastnode);
			}
		}
		// if node is a child
		else {
			// calculate distance of point in node from
			// point being searched for
			dist = Mod(ppoint->pos - node->pObj->pos);

			// set 'lastnode' to last node visited
			*lastnode = node;

			// if point already exist
			if(dist < m_dTol) {
				return true;
			}
			else
				return false;
		}
	}
	
	*lastnode = NULL;

	return false;
}

// find on which side of plane in 'node' the ppoint lies
char C3DVertexUse::CBSPTree::planeSide(C3DVertex *ppoint, Node *node)
{
	double var, dist;
	
	if( node->divDir == C3D_BSP_DIRX )
		var = ppoint->pos.x;
	else if( node->divDir == C3D_BSP_DIRY )
		var = ppoint->pos.y;
	else
		var = ppoint->pos.z;
	
	dist = var - node->divPlane;
	
	if( fabs(dist) * 2.0 < m_dTol )
		return C3D_BSP_ONPLANE;
	
	if( dist > 0 )
		return C3D_BSP_RIGHT;
	else 
		return C3D_BSP_LEFT;
}

// Insert 'ppoint' in BSP tree
// Return true if the point was already in the list
bool C3DVertexUse::CBSPTree::insert(Node *node, C3DVertex *ppoint)
{
	char side;
	bool present;
	double fdx, fdy, fdz, dx, dy, dz;
	Node *node1, *node2;
	
	// if insertion point not null
	if(node != NULL) {
		// if not a child
		if(node->child == false) {
			// decide on which side the point lies 
			side = planeSide(ppoint, node);
			if( side == C3D_BSP_LEFT) {
				return insert(node->left, ppoint);
			}
			else if(side == C3D_BSP_RIGHT) {
				return insert(node->right, ppoint);
			}
			else {
				// point is on the plane, insert in both left and right
				// branches. But if point is already in tree (seen in 
				// left branch) don't bother following also right branch
				present = insert(node->left, ppoint);
				if(!present)
					return insert(node->right, ppoint);
				else
					return true;
			}
		}
		// if node is a child
		else {
			dx = ppoint->pos.x - node->pObj->pos.x;
			dy = ppoint->pos.y - node->pObj->pos.y;
			dz = ppoint->pos.z - node->pObj->pos.z;
			fdx = fabs(dx);
			fdy = fabs(dy);
			fdz = fabs(dz);
			
			// if point already exist
			if(fdx < m_dTol && fdy < m_dTol && fdz < m_dTol)
				return true;
			
			// otherwise insert it

			node1 = new Node(node, node->pObj);
			node2 = new Node(node, ppoint);
			
			node->child = false;
			
			if(fdx > fdz) {
				if(fdx > fdy) {
					node->divDir = C3D_BSP_DIRX;
					node->divPlane = (ppoint->pos.x + node->pObj->pos.x) / 2.0;
					if( dx > 0 ) {
						node->left = node1;
						node->right = node2;
					}
					else {
						node->right = node1;
						node->left = node2;
					}	
				}
				else {
					node->divDir = C3D_BSP_DIRY;
					node->divPlane = (ppoint->pos.y + node->pObj->pos.y) / 2.0;
					if( dy > 0 ) {
						node->left = node1;
						node->right = node2;
					}
					else {
						node->right = node1;
						node->left = node2;
					}	
				}
			}
			else {
				if(fdz > fdy) {
					node->divDir = C3D_BSP_DIRZ;
					node->divPlane = (ppoint->pos.z + node->pObj->pos.z) / 2.0;
					if( dz > 0 ) {
						node->left = node1;
						node->right = node2;
					}
					else {
						node->right = node1;
						node->left = node2;
					}	
				}
				else {
					node->divDir = C3D_BSP_DIRY;
					node->divPlane = (ppoint->pos.y + node->pObj->pos.y) / 2.0;
					if( dy > 0 ) {
						node->left = node1;
						node->right = node2;
					}
					else {
						node->right = node1;
						node->left = node2;
					}	
				}
			}
			
			return false;
		}
	}
	
	return false;
}

C3DLoop::C3DLoop(C3DFace *father, C3DList<C3DVector> &points) : C3DEntity(C3D_LOOP_ID), pVertexUse(NULL),
																pFatherFace(father)
{
	C3DList<C3DVector>::iterator it1, it2;
	C3DEdgeUse *edgeuse;
	
	// loop is including space 
	type = C3D_INCLUDING;
	
	// Create loop edges
	for(it1 = points.begin(), it2 = it1, it2++; it2 != points.end(); it1++, it2++) {
		
		// create new edgeuse
		edgeuse = new C3DEdgeUse(*it1, *it2, this);
		
		// add to list tail, to preserve loop orientation
		edgeUseList.push_back(edgeuse);
		
		// enlarge its own bbox
		bbox += **it1;
	}
	// create last edgeuse to close loop
	edgeuse = new C3DEdgeUse(*it1, *(points.begin()), this);
	edgeUseList.push_back(edgeuse);
	bbox += **it1;
	
	// propagate bbox upwards
	pFatherFace->EnlargeBBox(bbox);
}

C3DLoop::~C3DLoop()
{
	C3DList<C3DEdgeUse>::iterator it;
	
	// delete all loops in face list
	for(it = edgeUseList.begin(); it != edgeUseList.end(); it++)
		delete *it;
	edgeUseList.clear();
	
	if( pVertexUse != NULL )
		delete pVertexUse;
}

// create a copy of 'this' loop in given face or shell ('father'),
// according to given transform function
C3DLoop *C3DLoop::CopyLoop(C3DOperation &op, C3DEntity *father)
{
	C3DLoop *newLoop;
	C3DEdgeUse *newEdgeuse;
	C3DList<C3DEdgeUse>::iterator it;
	C3DVector v1, v2;

	// create empty new loop

	// if father is face
	if(father->id == C3D_FACE_ID)
		newLoop = new C3DLoop((C3DFace*)father);
	// otherwise father is shell
	else
		newLoop = new C3DLoop((C3DShell*)father);

	// copy all edges from 'this' to 'newLoop'
	for(it = edgeUseList.begin(); it != edgeUseList.end(); it++) {

		// get edge vertices
		v1 = it->GetFirstVertexUse()->pVertex->pos;
		v2 = it->GetSecondVertexUse()->pVertex->pos;

		// transform them
		v1 = op.TransformFunction(v1);
		v2 = op.TransformFunction(v2);

		// create new edgeuse
		newEdgeuse = new C3DEdgeUse(&v1, &v2, this);
		
		// add to list tail, to preserve loop orientation
		newLoop->edgeUseList.push_back(newEdgeuse);
		
		// enlarge newLoop bbox
		newLoop->bbox += v1;
	}

	if( pVertexUse != NULL ) {
		// get vertexuse position
		v1 = pVertexUse->pVertex->pos;

		// transform it
		v1 = op.TransformFunction(v1);

		// create new vertexuse
		newLoop->pVertexUse = new C3DVertexUse(&v1);

		// enlarge newLoop bbox
		newLoop->bbox += v1;
	}

	// propagate bbox upwards

	// if father is face
	if(father->id == C3D_FACE_ID)
		newLoop->pFatherFace->EnlargeBBox(newLoop->bbox);	
	// otherwise father is shell
	else
		newLoop->pFatherShell->EnlargeBBox(newLoop->bbox);	

	return newLoop;
}

C3DFace::C3DFace(C3DShell *father, C3DList<C3DVector> &points) : C3DEntity(C3D_FACE_ID), pShell(father), hasColor(C3D_NO_COLOR)
{
	C3DLoop *loop;
	int res;
	C3DOperation op;
	
	// setting pShell(father) is important to allow bounding boxes
	// to propagate upwards when creating loops
	
	// create loop
	loop = new C3DLoop(this, points);
	
	// add loop to loop list
	loopList.push_front(loop);
	
	// Calculate face normal. Normal is such that Cross(normal, edge) will
	// point inwards
	res = op.PlaneFromPolygon(plane, loop->edgeUseList);

	// check that it was possible to calculate the normal
	ASSERT( res == C3D_OK );
}

C3DFace::~C3DFace() 
{
	C3DList<C3DLoop>::iterator it;
	
	// delete all loops in face list
	for(it = loopList.begin(); it != loopList.end(); it++)
		delete *it;
	loopList.clear();
}

// create a copy of 'this' face according to given transform function
C3DFace *C3DFace::CopyFace(C3DOperation &op)
{
	C3DFace *newFace;
	C3DLoop *newLoop;
	C3DList<C3DLoop>::iterator it;
	int res;

	// create empty new face
	newFace = new C3DFace(pShell);

	// copy all loops from 'this' to 'newFace'
	for(it = loopList.begin(); it != loopList.end(); it++) {
		newLoop = it->CopyLoop(op, newFace);
		newFace->loopList.push_front(newLoop);
	}

	// Calculate face normal. Normal is such that Cross(normal, edge) will
	// point inwards
	// Coluld avoid re-calculating the normal but in this case should calculate
	// how the plane equation changes according to the given transform function
	// (e.g. it is different to rotate or to translate)
	// Note also that normal is calculated using only first loop; should 
	// perform a mean instead, in case more than one loop is present
	res = op.PlaneFromPolygon(plane, loopList.begin()->edgeUseList);

	// check that it was possible to calculate the normal
	ASSERT( res == C3D_OK );

	return newFace;
}

void C3DFace::EnlargeBBox(C3DBBox enlBbox)
{
	// enlarge its own bbox
	bbox += enlBbox;
	
	// propagate bbox upwards
	if(	pShell != NULL ) 
		pShell->EnlargeBBox(bbox);
}

C3DShell::~C3DShell()
{
	C3DList<C3DShell>::iterator its;
	C3DList<C3DFace>::iterator itf;
	C3DList<C3DLoop>::iterator itl;
	C3DList<C3DEdgeUse>::iterator iteu;
	
	for(its = shellList.begin(); its != shellList.end(); its++)
		delete *its;
	shellList.clear();
	
	for(itf = faceList.begin(); itf != faceList.end(); itf++)
		delete *itf;
	faceList.clear();

	for(itl = loopList.begin(); itl != loopList.end(); itl++)
		delete *itl;
	loopList.clear();

	for(iteu = edgeUseList.begin(); iteu != edgeUseList.end(); iteu++)
		delete *iteu;
	edgeUseList.clear();

	if( pVertexUse != NULL )
		delete pVertexUse;
}

C3DFace *C3DShell::MakeFace(C3DList<C3DVector> &points)
{
	C3DFace *face;
	
	// create new face
	face = new C3DFace(this, points);
	
	// add face to face list
	faceList.push_front(face);

	return face;
}

C3DFace *C3DShell::MakeFace(C3DVector points[], int numPoints, float density, bool hascolor, unsigned long color)
{
	C3DFace *face;
	C3DList<C3DVector> pointList;
	int i;

	// build a point list, using push_back to preserve the original 
	// point ordering (which determines the face normal orientation)
	for(i=0; i < numPoints; i++) {
		pointList.push_back(&points[i]);
	}

	// create new face
	face = new C3DFace(this, pointList);

	face->density = density;

	if(hascolor == true) {
		face->rgb.red = (unsigned char) (color >> 24);
		face->rgb.green = (unsigned char) ((color & 0x00FF0000) >> 16);
		face->rgb.blue = (unsigned char) ((color & 0x0000FF00) >> 8);
		face->rgb.alpha = (unsigned char) ((color & 0x000000FF));
		face->hasColor = true;
	}
	
	// add face to face list
	faceList.push_front(face);

	return face;
}

void C3DShell::EnlargeBBox(C3DBBox enlBbox)
{
	// enlarge its own bbox
	bbox += enlBbox;
	
	// propagate bbox upwards
	if(	pShell != NULL ) 
		pShell->EnlargeBBox(bbox);
}

// recalculate the bbox according to subshell bboxes
void C3DShell::UpdateBBox()
{
	C3DList<C3DShell>::iterator its;
	C3DList<C3DFace>::iterator itf;
	C3DList<C3DLoop>::iterator itl;
	C3DList<C3DEdgeUse>::iterator iteu;
	C3DVector v1, v2;

	// clear bbox (will be rebuilt) 
	bbox.Clear();

	// add subshell bboxes 
	for(its = shellList.begin(); its != shellList.end(); its++)
		bbox += its->bbox;

	// then must add all face, loop, edge and vertex bboxes,
	// because we have cleared the shell bbox

	for(itf = faceList.begin(); itf != faceList.end(); itf++) {
		bbox += itf->bbox;
	}

	for(itl = loopList.begin(); itl != loopList.end(); itl++) {
		bbox += itl->bbox;
	}

	for(iteu = edgeUseList.begin(); iteu != edgeUseList.end(); iteu++) {
		// get edge vertices
		v1 = iteu->GetFirstVertexUse()->pVertex->pos;
		v2 = iteu->GetSecondVertexUse()->pVertex->pos;
		// enlarge shell bbox
		bbox += v1;
		bbox += v2;
	}

	if( pVertexUse != NULL ) {
		// get vertexuse position
		v1 = pVertexUse->pVertex->pos;
		// enlarge shell bbox
		bbox += v1;
	}

	// update parent bbox, if parent exists
	if(pShell != NULL)
		pShell->UpdateBBox();
}

C3DVector pippo(C3DVector & v)
{
	return v;
}

void C3DShell::Translate(C3DVector shift)
{
	C3DOperation op;
	
	op.SetTranslationShift(shift);

	Transform(op);
}

void C3DShell::Scale(C3DVector scale)
{
	C3DOperation op;
	
	op.SetScaleVector(scale);

	Transform(op);
}

void C3DShell::RotateX(double angle)
{
	C3DOperation op;
	
	op.SetRotationAngleAndOperation(angle, C3D_TRANS_ROTATEX);

	Transform(op);
}

void C3DShell::RotateY(double angle)
{
	C3DOperation op;
	
	op.SetRotationAngleAndOperation(angle, C3D_TRANS_ROTATEY);

	Transform(op);
}

void C3DShell::RotateZ(double angle)
{
	C3DOperation op;
	
	op.SetRotationAngleAndOperation(angle, C3D_TRANS_ROTATEZ);

	Transform(op);
}

void C3DShell::Transform(C3DOperation &op)
{
	C3DList<C3DShell>::iterator its;
	C3DList<C3DFace>::iterator itf;
	C3DList<C3DLoop>::iterator itl;
	C3DList<C3DEdgeUse>::iterator iteu;

	C3DFace *faceTmp;
	C3DLoop *loopTmp;
	C3DEdgeUse	*edgeuseTmp;
	C3DVertexUse	*vertexuseTmp;
	C3DVector v1, v2;

	// clear bbox (will be rebuilt)
	bbox.Clear();

	for(its = shellList.begin(); its != shellList.end(); its++)
		its->Transform(op);
	
	for(itf = faceList.begin(); itf != faceList.end(); itf++) {
		// create a second face according to given transform function
		faceTmp = itf->CopyFace(op);
		// delete current face (cut!)
		delete *itf;
		// and associate pointer to newly created face (paste!) 
		itf.assign(faceTmp);
	}

	for(itl = loopList.begin(); itl != loopList.end(); itl++) {
		// create a second loop according to given transform function
		loopTmp = itl->CopyLoop(op, this);
		// delete current loop (cut!)
		delete *itl;
		// and associate pointer to newly created loop (paste!) 
		itl.assign(loopTmp);
	}

	for(iteu = edgeUseList.begin(); iteu != edgeUseList.end(); iteu++) {

		// get edge vertices
		v1 = iteu->GetFirstVertexUse()->pVertex->pos;
		v2 = iteu->GetSecondVertexUse()->pVertex->pos;

		// transform them
		v1 = op.TransformFunction(v1);
		v2 = op.TransformFunction(v2);

		// create new edgeuse
		edgeuseTmp = new C3DEdgeUse(&v1, &v2, this);
		// delete current edgeuse (cut!)
		delete *iteu;
		// and associate pointer to newly created edgeuse (paste!) 
		iteu.assign(edgeuseTmp);

		// enlarge shell bbox
		bbox += v1;
		bbox += v2;
	}

	if( pVertexUse != NULL ) {
		// get vertexuse position
		v1 = pVertexUse->pVertex->pos;

		// transform it
		v1 = op.TransformFunction(v1);

		// create new vertexuse
		vertexuseTmp = new C3DVertexUse(&v1);
		// delete current vertexuse (cut!)
		delete pVertexUse;
		// and associate pointer to newly created vertexuse (paste!) 
		pVertexUse = vertexuseTmp;

		// enlarge shell bbox
		bbox += v1;
	}

	// update parent bbox, if parent exists
	if(pShell != NULL)
		pShell->UpdateBBox();
}

