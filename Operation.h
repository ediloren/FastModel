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


// Boolean.h : Non-Manifold Geometry basic operations
// Enrico Di Lorenzo 2002/12/30

#ifndef C3DOPERATION_DEFS
#define C3DOPERATION_DEFS

#include "Geometry.h"

/////////////////////////////////////////////////////
// 
// 
//

#define C3D_OK						0
#define C3D_LINE_DONT_INTERSECT		1
#define C3D_LINE_IS_ON_PLANE		2
#define C3D_POINTS_ARE_COLLINEAR	3
#define C3D_VECTORS_ARE_PARALLEL	4
#define C3D_LINES_ARE_PARALLEL		5
#define C3D_INTERNAL_ERROR			1000

#define C3D_TRANS_TRANSLATE			0
#define C3D_TRANS_SCALE				1
#define C3D_TRANS_ROTATEX			2
#define C3D_TRANS_ROTATEY			3
#define C3D_TRANS_ROTATEZ			4

// Remark: if redefining C3D_OP_TOL, care that also C3D_PI had 
// at least the same number of decimals 
#define C3D_OP_TOL			1E-12
#define C3D_PI				3.1415926535897932384626433832795


class C3DOperation 
{
public:
	C3DOperation() : m_dTol(C3D_OP_TOL) {}

	inline double Dist(C3DVector &p1, C3DVector &p2)
	{
		return Mod(p1-p2);
	}

	inline double PointPlaneDist(C3DVector &point, C3DPlane &plane)
	{
		return ( DotProd(point, plane.m_vecNormal) + plane.m_dD );
	}

	// return point at relative distance t0 on segment point_a-point_b,
	// starting from point_a (t0 = 0)
	// point = (1-t0)*point_a + t0*point_b
	inline C3DVector PointOnSegment(C3DVector &point_a, C3DVector &point_b, double t0)
	{
		C3DVector point;

		point.x = (1-t0) * point_a.x + t0 * point_b.x;
		point.y = (1-t0) * point_a.y + t0 * point_b.y;
		point.z = (1-t0) * point_a.z + t0 * point_b.z;

		return point;
	}
		
	// Compute segment - plane intersection point
	// If intersection exists, return C3D_OK and point 'point',
	// otherwise return error condition
	inline int IntersSegmentPlane(C3DVector &point, C3DVector &point_a, C3DVector &point_b, C3DPlane &plane)
	{
		double dista, distb, len, t0;

		dista = PointPlaneDist(point_a, plane);
		distb = PointPlaneDist(point_b, plane);

		len = dista - distb;

		// if both points are on the same half-space side, 
		// the segment does not intersect the plane 
		if( (dista > m_dTol && distb > m_dTol) || (dista < m_dTol && distb < m_dTol) ) {
			return C3D_LINE_DONT_INTERSECT;
		}

		// if len == 0, endpoints are coincident
		if( len < m_dTol ) {
			// if also dista == 0, both points are on the plane; 
			// so return the first point (second would be ok either)
			if( dista < m_dTol ) {
				point = point_a;
				return C3D_OK;
			}
			// otherwise no intersection with plane (this condition should 
			// have been catched by the test on half space containing points;
			// however, due to combining tolerances (m_dTol) when calculating len,
			// the condition could have been missed
			return C3D_LINE_DONT_INTERSECT;
		}

		// compute relative intersection distance along segment
		t0 = dista / len;
		// determine intersection point at relative distance t0 from point_a
		point = PointOnSegment(point_a, point_b, t0);

		return C3D_OK;
	}

	// Compute line - plane intersection point
	// If intersection exists, return C3D_OK and point 'point',
	// otherwise return error condition
	inline int IntersLinePlane(C3DVector &point, C3DLine &line, C3DPlane &plane)
	{
		double dist, cosalpha, lenght;
		C3DVector point_b;

		// Calculate the angle between the line and the plane
		cosalpha = DotProd(plane.m_vecNormal, line.m_vecDir);
		// Compute distance of the line origin from plane
		dist = PointPlaneDist(line.m_vecPoint, plane);

		// if cos(angle) is almost 0, the line is parallel
		// to the plane
		if( cosalpha < m_dTol ) {
			// If line origin is on the plane, then all the line in ON the plane
			if( dist < m_dTol ) {
				return C3D_LINE_IS_ON_PLANE;
			}
			else
				return C3D_LINE_DONT_INTERSECT;
		}
		
		// compute lenght along line, starting from line origin,
		// to intersection point
		lenght = dist / cosalpha;

		// compute intersection point
		point = line.m_vecPoint + line.m_vecDir * lenght;

		return C3D_OK;
	}

	int PlaneFromPolygon(C3DPlane &plane, C3DList<C3DEdgeUse> &edgeuses);
	int PlaneFromThreePoints(C3DPlane &plane, C3DVector &point1,
							C3DVector &point2, C3DVector &point3);
	int PlaneFromTwoVectorsAndPoint(C3DPlane &plane, C3DVector &vector1,
									C3DVector &vector2, C3DVector &point);
	int LineToLineClosestPoints(C3DVector &point1, C3DVector &point2,
								C3DLine &line1, C3DLine &line2);

	C3DVector TransformFunction(C3DVector &point)
	{
		C3DVector v;

		if(m_iTranOperation == C3D_TRANS_TRANSLATE)
			return TranslatePoint(point, m_vecShift);
		else if(m_iTranOperation == C3D_TRANS_SCALE)
			return ScalePoint(point, m_vecScale);
		else if(m_iTranOperation == C3D_TRANS_ROTATEX)
			return RotatePointX(point);
		else if(m_iTranOperation == C3D_TRANS_ROTATEY)
			return RotatePointY(point);
		else if(m_iTranOperation == C3D_TRANS_ROTATEZ)
			return RotatePointZ(point);
		else {
			// always error
			ASSERT(NULL);
			// return dummy vector
			return v;
		}
	}

	void SetTranslationShift(C3DVector &shift)
	{
		m_vecShift = shift;
		m_iTranOperation = C3D_TRANS_TRANSLATE;
	}

	C3DVector TranslatePoint(C3DVector &point, C3DVector &shift)
	{
		return (point + shift);
	}

	void SetScaleVector(C3DVector &scale)
	{
		m_vecScale = scale;
		m_iTranOperation = C3D_TRANS_SCALE;
	}

	C3DVector ScalePoint(C3DVector &point, C3DVector &scale)
	{
		return (point * scale);
	}

	// angle is in degrees
	void SetRotationAngleAndOperation(double angle, int operation)
	{
		double alpha;

		alpha = angle * C3D_PI / 180.0;
		m_dSinAngle = sin(alpha);
		m_dCosAngle = cos(alpha);

		m_iTranOperation = operation;
	}

	// angle is in degrees
	C3DVector RotatePointX(C3DVector &point, double angle)
	{
		SetRotationAngleAndOperation(angle, C3D_TRANS_ROTATEX);
		return RotatePointX(point);
	}

	C3DVector RotatePointX(C3DVector &point)
	{
		C3DVector res;

		res.x = point.x;
		res.y = point.y * m_dCosAngle - point.z * m_dSinAngle;
		res.z = point.y * m_dSinAngle + point.z * m_dCosAngle;

		return res;
	}

	// angle is in degrees
	C3DVector RotatePointY(C3DVector &point, double angle)
	{
		SetRotationAngleAndOperation(angle, C3D_TRANS_ROTATEY);
		return RotatePointY(point);
	}

	C3DVector RotatePointY(C3DVector &point)
	{
		C3DVector res;

		res.x = point.x * m_dCosAngle + point.z * m_dSinAngle;
		res.y = point.y;
		res.z = - point.x * m_dSinAngle + point.z * m_dCosAngle;

		return res;
	}

	// angle is in degrees
	C3DVector RotatePointZ(C3DVector &point, double angle)
	{
		SetRotationAngleAndOperation(angle, C3D_TRANS_ROTATEZ);
		return RotatePointZ(point);
	}

	// angle is in degrees
	C3DVector RotatePointZ(C3DVector &point)
	{
		C3DVector res;

		res.x = point.x * m_dCosAngle - point.y * m_dSinAngle;
		res.y = point.x * m_dSinAngle + point.y * m_dCosAngle;
		res.z = point.z;

		return res;
	}


protected:
	C3DVector m_vecTol;
	double    m_dTol;

	C3DVector m_vecShift, m_vecScale;
	double m_dCosAngle, m_dSinAngle;
	int m_iTranOperation;

};

#endif //C3DOPERATION_DEFS

