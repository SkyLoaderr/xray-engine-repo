//. OLES: implementation of Light Space Perspective Shadow Maps
//. note regarding D3DX: 
//.		I really dislike everything what comes without source and I don't normally use D3DX.
//.		So please forgive me, if I duplicate some of its functionality
//.
//. following "include" section directly copied from PracticalPSM.cpp
//. why nVidia (or Gary :) dislike precompiled headers? :(
#define STRICT
#include <math.h>
#include <D3DX9.h>
#include <DX9SDKSampleFramework/DX9SDKSampleFramework.h>
#pragma warning(disable : 4786)
#include <vector>
#pragma warning(disable : 4786)
#include <assert.h>
#include "PracticalPSM.h"

#include "common.h"

//////////////////////////////////////////////////////////////////////////
typedef		D3DXVECTOR2	vector2;
typedef		D3DXVECTOR3 vector3;
typedef		D3DXVECTOR4 vector4;
typedef		D3DXMATRIX	matrix;
using namespace	std;

//. OLES: I don't know D3DX's equivalent for the following
inline		vector3	xform			(const vector3 v, const matrix& m)
{
	float	x = v.x*m._11 + v.y*m._21 + v.z*m._31 + m._41;
	float	y = v.x*m._12 + v.y*m._22 + v.z*m._32 + m._42;
	float	z = v.x*m._13 + v.y*m._23 + v.z*m._33 + m._43;
	float	w = v.x*m._14 + v.y*m._24 + v.z*m._34 + m._44;
	return	vector3(x/w,y/w,z/w);	// RVO
}
inline		void	xform_array		(vector3* dst, const vector3* src, const matrix& m, int count)
{
	for (int p=0; p<count; p++)	dst[p] = xform(src[p],m);
}

// isLeft(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2 on the line
//            <0 for P2 right of the line
//    See: the January 2001 Algorithm on Area of Triangles
inline		float	isLeft	( vector2 P0, vector2 P1, vector2 P2 )	{	return (P1.x - P0.x)*(P2.y - P0.y) - (P2.x - P0.x)*(P1.y - P0.y); }

// chainHull_2D(): Andrew's monotone chain 2D convex hull algorithm
//     Input:  P[] = an array of 2D points
//                   presorted by increasing x- and y-coordinates
//             n = the number of points in P[]
//     Output: H[] = an array of the convex hull vertices (max is n)
//     Return: the number of points in H[]
int			chainHull_2D	( vector2* P, int n, vector2* H )
{
	// the output array H[] will be used as the stack
	int    bot=0, top=(-1);  // indices for bottom and top of the stack
	int    i;                // array scan index

	// Get the indices of points with min x-coord and min|max y-coord
	int minmin = 0, minmax;
	float xmin = P[0].x;
	for (i=1; i<n; i++)
		if (P[i].x != xmin) break;
	minmax = i-1;
	if (minmax == n-1) {       // degenerate case: all x-coords == xmin
		H[++top] = P[minmin];
		if (P[minmax].y != P[minmin].y) // a nontrivial segment
			H[++top] = P[minmax];
		H[++top] = P[minmin];           // add polygon endpoint
		return top+1;
	}

	// Get the indices of points with max x-coord and min|max y-coord
	int maxmin, maxmax = n-1;
	float xmax = P[n-1].x;
	for (i=n-2; i>=0; i--)
		if (P[i].x != xmax) break;
	maxmin = i+1;

	// Compute the lower hull on the stack H
	H[++top] = P[minmin];      // push minmin point onto stack
	i = minmax;
	while (++i <= maxmin)
	{
		// the lower line joins P[minmin] with P[maxmin]
		if (isLeft( P[minmin], P[maxmin], P[i]) >= 0 && i < maxmin)
			continue;          // ignore P[i] above or on the lower line

		while (top > 0)        // there are at least 2 points on the stack
		{
			// test if P[i] is left of the line at the stack top
			if (isLeft( H[top-1], H[top], P[i]) > 0)
				break;         // P[i] is a new hull vertex
			else
				top--;         // pop top point off stack
		}
		H[++top] = P[i];       // push P[i] onto stack
	}

	// Next, compute the upper hull on the stack H above the bottom hull
	if (maxmax != maxmin)      // if distinct xmax points
		H[++top] = P[maxmax];  // push maxmax point onto stack
	bot = top;                 // the bottom point of the upper hull stack
	i = maxmin;
	while (--i >= minmax)
	{
		// the upper line joins P[maxmax] with P[minmax]
		if (isLeft( P[maxmax], P[minmax], P[i]) >= 0 && i > minmax)
			continue;          // ignore P[i] below or on the upper line

		while (top > bot)    // at least 2 points on the upper stack
		{
			// test if P[i] is left of the line at the stack top
			if (isLeft( H[top-1], H[top], P[i]) > 0)
				break;         // P[i] is a new hull vertex
			else
				top--;         // pop top point off stack
		}
		H[++top] = P[i];       // push P[i] onto stack
	}
	if (minmax != minmin)
		H[++top] = P[minmin];  // push joining endpoint onto stack

	return top+1;
}

//-----------------------------------------------------------------------------
// Name: BuildPSMProjectionMatrix
// Desc: Builds a perspective shadow map transformation matrix
//.OLES: note: the result should go to m_LightViewProj
//-----------------------------------------------------------------------------
void CPracticalPSM::BuildTSMProjectionMatrix	()
{
	// The following function(s) performs more work than needed
	// But it is used here for convenience
	ComputeVirtualCameraParameters	();					//. to compute caster/receiver objects
	BuildOrthoShadowProjectionMatrix();					//. simple ortho-transform

	// Find inverse transform
	matrix				modelView, 	modelView_inv;
	matrix				modelViewProjection, modelViewProjection_inv;
	D3DXMatrixMultiply	(&modelView, &m_World, &m_View);
	D3DXMatrixInverse	(&modelView_inv,0,&modelView);
	D3DXMatrixMultiply	(&modelViewProjection, &modelView, &m_Projection);
	D3DXMatrixInverse	(&modelViewProjection_inv,0,&modelViewProjection);

	// calculate view-frustum bounds in world space
	// note: D3D uses [0..1] range for Z
	vector3		A[8];
	A[0]= vector3( -1, -1,  0);	A[1]= vector3( -1, -1, +1);
	A[2]= vector3( -1, +1, +1);	A[3]= vector3( -1, +1,  0);
	A[4]= vector3( +1, +1, +1);	A[5]= vector3( +1, +1,  0);
	A[6]= vector3( +1, -1, +1);	A[7]= vector3( +1, -1,  0);

	std::vector<vector3>		points(8);	//
	xform_array					(&*points.begin(),A,modelViewProjection_inv,8);

	// find center-line
	vector3		cl_near,cl_far;
	cl_near		= xform			(vector3(0,0,0),modelViewProjection_inv);
	cl_far		= xform			(vector3(0,0,1),modelViewProjection_inv);

	// xform frustum + center-line to light-space
	std::vector<vector3>		ls_points	(8);
	vector3						ls_cl_near,ls_cl_far;
	xform_array					(&*ls_points.begin(),&*points.begin(),m_LightViewProj,8);
	ls_cl_near	= xform			(cl_near,m_LightViewProj);
	ls_cl_far	= xform			(cl_far,m_LightViewProj);

	// find 2D convex hull
	std::vector<vector2>		ls_chull	(8);
	{
		std::vector<vector2>		input;
		for (int i=0; i<8; i++)		input.push_back	(vector2(ls_points[i].x,ls_points[i].y));
		ls_chull.resize				(chainHull_2D(&*input.begin(),8,&*ls_chull.begin()));
	}
}
