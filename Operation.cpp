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


// Operation.cpp : implementation of the Non-Manifold Geometry basic operations
// Enrico Di Lorenzo 2002/12/30

#include "stdafx.h"

#include <math.h>

#include "ErrorHandler.h"
#include "Operation.h"
#include "Global.h"


// Compute plane of a polygon, using Newell's method 
// Plane normal direction is so that N x point1-point3 
// is directed inside the polygon
//
//         3
//         |\
//    N    | \
//     \   |  \
//      \  |   \
//       \ |    \
//        \|     \
//         1------2
//
// If plane equation can be found, return C3D_OK and plane 'plane',
// otherwise return error condition

int C3DOperation::PlaneFromPolygon(C3DPlane &plane, C3DList<C3DEdgeUse> &edgeuses)
{
	C3DList<C3DEdgeUse>::iterator it;
	C3DVector normal, midpoint, point1, point2;
	int pointnum;
	double mod;

	pointnum = 0;

	// scan all points
    for(it = edgeuses.begin(); it != edgeuses.end(); it++) {

		// get edge vertexes coordinates;
		// note that edgeuse direction is important!
		point1 = it->GetFirstVertexUse()->pVertex->pos;
		point2 = it->GetSecondVertexUse()->pVertex->pos;

		// calculate components of normal vector, using the 
		// fact that the area of the trapezoids projected on the
		// xy, xz, yz planes is proportional to the z, y, x
		// components of the normal
        normal.x += (point1.y - point2.y) * (point1.z + point2.z);
		normal.y += (point1.z - point2.z) * (point1.x + point2.x);
		normal.z += (point1.x - point2.x) * (point1.y + point2.y);

		// calculates the mean point of the polygon
		midpoint += point1;
		pointnum++;
	}

	mod = Mod(normal);

	// if the result of the cross product is too small, 
	// can only means that the points are almost collinear
	if( mod < m_dTol ) {
		return C3D_POINTS_ARE_COLLINEAR;
	}

	// unify normal vector
	normal = normal / mod;

	// compute mean point
	midpoint /= pointnum;

	plane.m_vecNormal = normal;
	plane.m_dD = DotProd(normal, midpoint);

	return C3D_OK;
}

// Compute plane equation, given three non-collinear points.
// Plane normal direction is point2-point1 x point3-point1
//
//         3
//         |\
//    N    | \
//     \   |  \
//      \  |   \
//       \ |    \
//        \|     \
//         1------2
//
// If plane equation can be found, return C3D_OK and plane 'plane',
// otherwise return error condition

int C3DOperation::PlaneFromThreePoints(C3DPlane &plane, C3DVector &point1,
									   C3DVector &point2, C3DVector &point3)
{
	C3DVector normal;
	double mod;

	normal = CrossProd(point2-point1, point3-point1);
	mod = Mod(normal);

	// if the result of the cross product is too small, 
	// can only means that the points are almost collinear
	if( mod < m_dTol ) {
		return C3D_POINTS_ARE_COLLINEAR;
	}

	// unify normal vector
	normal = normal / mod;

	plane.m_vecNormal = normal;
	plane.m_dD = DotProd(normal, point1);

	return C3D_OK;
}

// Compute plane equation, given non-parallel vectors and a point, 
// all lying on the plane.
// Plane normal direction is vector1 x vector2
//
//             3
//    ^        |\
//    |   N    | \
//    |    \   |  \
//    |     \  |   \
//    |      \ |    \
//    |       \|     \
//    |        1------2
// Vector2
//             -------> Vector1
//
//
// If plane equation can be found, return C3D_OK and plane 'plane',
// otherwise return error condition

int C3DOperation::PlaneFromTwoVectorsAndPoint(C3DPlane &plane, C3DVector &vector1,
									   C3DVector &vector2, C3DVector &point)
{
	C3DVector normal;
	double mod;

	normal = CrossProd(vector1, vector2);
	mod = Mod(normal);

	// if the result of the cross product is too small, 
	// can only means that the vectors are almost parallel
	if( mod < m_dTol ) {
		return C3D_VECTORS_ARE_PARALLEL;
	}

	// unify normal vector
	normal = normal / mod;

	plane.m_vecNormal = normal;
	plane.m_dD = DotProd(normal, point);

	return C3D_OK;
}

// Finds the closest points on two skewed lines
//
// computes point1 on line1 closest to line2
// computes point2 on line2 closest to line1
//
// Note that if lines cross, point1 and point2 can be coincident

int C3DOperation::LineToLineClosestPoints(C3DVector &point1, C3DVector &point2,
										  C3DLine &line1, C3DLine &line2)
{
	C3DVector connline;
	C3DPlane plane1, plane2;
	int ret1, ret2;

    // connecting line is perpendicular to both
    connline = CrossProd(line1, line2);

	// lines are nearly parallel -> all points are equally close
	if( Mod(connline) < m_dTol ) {
		point1 = line1.m_vecPoint;
		point2 = line2.m_vecPoint;

		return C3D_LINES_ARE_PARALLEL;
	}


    // form plane1 containing line1, parallel to connline
    ret1 = PlaneFromTwoVectorsAndPoint(plane1, connline, line1.m_vecDir, line1.m_vecPoint);

    // form plane2 containing line2, parallel to connline
    ret2 = PlaneFromTwoVectorsAndPoint(plane2, connline, line2.m_vecDir, line2.m_vecPoint);

	// as connline was already checked, this condition should never happen
	if( ret1 != C3D_OK || ret2 != C3D_OK ) {
		return C3D_INTERNAL_ERROR;
	}

    // closest point on line1 is obtained intersecting line1 with plane2
	ret1 = IntersLinePlane(point1, line1, plane2);

     // closest point on line2 is obtained intersecting line2 with plane1
	ret2 = IntersLinePlane(point2, line2, plane1);

	// as connline was already checked, this condition should never happen
	if( ret1 != C3D_OK || ret2 != C3D_OK ) {
		return C3D_INTERNAL_ERROR;
	}

	return C3D_OK;
}

/*C3DVector C3DOperation::TranslatePoint(C3DVector &point)
{
	return (point + m_vecShift);
}
*/




