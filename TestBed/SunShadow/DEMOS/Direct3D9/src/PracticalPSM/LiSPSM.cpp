//. OLES: implementation of Light Space Perspective Shadow Maps
//. note regarding D3DX: 
//.		I really dislike everything what comes without source and I don't normally use D3DX.
//.		So please forgive me, if I duplicate some of its functionality
//.
//. following "include" section directly copied from PracticalPSM.cpp
//. why nVidia (or Gary :) dislike precompiled headers? :(
#define STRICT
#include <Windows.h>
#include <commctrl.h>
#include <math.h>
#include <D3DX9.h>
#include <d3dx9math.h>
#include <d3dx9effect.h>
#include <d3dx9shader.h>
#include <DX9SDKSampleFramework/DX9SDKSampleFramework.h>
#include <shared/GetFilePath.h>
#pragma warning(disable : 4786)
#include <vector>
#pragma warning(disable : 4786)
#include <assert.h>
//#include <shared/NVFileDialog.h>
//#include "MouseUIs.h"
#include "PracticalPSM.h"
//#include "Util.h"
//#include "Bounding.h"

#include "common.h"

// misc. functions necessary to run the program, but not critical to the algorithm...
// #include "FrameWork.inc"
// #include "Helpers.inc"
//////////////////////////////////////////////////////////////////////////
typedef		D3DXVECTOR3 vector3;
typedef		D3DXVECTOR4 vector4;
typedef		D3DXMATRIX	matrix;
using namespace	std;

//. OLES: I don't know D3DX's equivalent for the following
vector3	xform			(const vector3& v, const matrix& m)
{
	float	x = v.x*m._11 + v.y*m._21 + v.z*m._31 + m._41;
	float	y = v.x*m._12 + v.y*m._22 + v.z*m._32 + m._42;
	float	z = v.x*m._13 + v.y*m._23 + v.z*m._33 + m._43;
	float	w = v.x*m._14 + v.y*m._24 + v.z*m._34 + m._44;
	return	vector3(x/w,y/w,z/w);	// RVO
}
void	xform_array		(vector3* dst, const vector3* src, const matrix& m, int count)
{
	for (int p=0; p<count; p++)	dst[p] = xform(src[p],m);
}
void	calc_aabb		(vector3& min, vector3 &max, const vector3* ps, int count) 
{
	min = max = ps[0];
	for (int i=1; i<count; i++)
	{
		const vector3&	p = ps[i];
		for (int j=0; j<3; j++)
		{
			if (p[j]<min[j])		min[j]=p[j];
			else if (p[j]>max[j])	max[j]=p[j];
		}
	}
}
void	calc_xaabb		(vector3& min, vector3 &max, const vector3* ps, const matrix& m, int count) 
{
	min = max = xform(ps[0],m);
	for (int i=1; i<count; i++)
	{
		const vector3	p = xform(ps[i],m);
		for (int j=0; j<3; j++)
		{
			if (p[j]<min[j])		min[j]=p[j];
			else if (p[j]>max[j])	max[j]=p[j];
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// OLES: naive 3D clipper - roubustness around 0, but works for this sample
// note: normals points to 'outside'
//////////////////////////////////////////////////////////////////////////
const	float	_eps	= 0.000001f;
struct	DumbClipper
{
	vector<D3DXPLANE>	planes;
	BOOL				clip (vector3& p0, vector3& p1)		// returns TRUE if result meaningfull
	{
		float	t,denum;
		vector3	D;
		for (int it=0; it<planes.size(); it++)
		{
			D3DXPLANE&	P		= planes			[it];
			float		cls0	= D3DXPlaneDotCoord	(&P,&p0);
			float		cls1	= D3DXPlaneDotCoord	(&P,&p1);
			if (cls0>0 && cls1>0)	return			false;	// fully outside
		
			if (cls0>0)	{
				// clip p0
				D			= p1-p0;
				denum		= D3DXPlaneDotNormal(&P,&D);
				if (denum!=0) p0 = - D * cls0 / denum;
			}
			if (cls1>0)	{
				// clip p1
				D			= p0-p1;
				denum		= D3DXPlaneDotNormal(&P,&D);
				if (denum!=0) p1 = - D * cls1 / denum;
			}
		}
	}
};

//////////////////////////////////////////////////////////////////////////
// OLES: naive 3D convex hull - slow as hell ~O(N^2), and not useful for 
//		 generic cases, I think. But it works for out simple usages :)
// note: normals points to 'outside'
//////////////////////////////////////////////////////////////////////////
class	DumbConvexHull
{
public:
	struct	_poly
	{
		vector<vector3>	points;
		vector3			planeN;
		float			planeD;

		float			classify(vector3& p)	{	return D3DXVec3Dot(&planeN,&p)+planeD; 	}
	};
	vector<_poly>		polys;
public:
	void				compute	(DumbClipper& dest)
	{
		// planes
		for (int it=0; it<polys.size(); it++)
		{
			_poly&			P	= polys[it];
			vector3			t1	= P.points[0]-P.points[1], t2 = P.points[0]-P.points[2];
			D3DXVec3Cross		(&P.planeN,&t1,&t2);
			D3DXVec3Normalize	(&P.planeN,&P.planeN);
			P.planeD			= -D3DXVec3Dot(&P.planeN,&P.points[0]);
		}

		// cycle
		for (int i=0; i<polys.size(); i++)
		{
			_poly&	base		= polys	[i];
			bool	all_inside	= true;
			for	(int j=0; j<polys.size(); j++)
			{
				_poly&	test	= polys	[j];
				if (j==i)		continue;
				for (int k=0; k<test.points.size(); k++)
				{
					vector3&	v = test.points[k];
					if (base.classify(v)>_eps)			// point of another poly outside
						{ all_inside = false; break; }
				}
				if (!all_inside)	break;
			}
			if (!all_inside)	{
				// bad plane, remove
				polys.erase	(polys.begin()+i);
				i			--;
			}
		}

		// transfer
		for (int it=0; it<polys.size(); it++)
		{
			_poly&			P	= polys[it];
			dest.planes.push_back(D3DXPLANE(P.planeN.x,P.planeN.y,P.planeN.z,P.planeD));
		}
	}
};

//-----------------------------------------------------------------------------
// Name: BuildPSMProjectionMatrix
// Desc: Builds a perspective shadow map transformation matrix
//.OLES: note: the result should go to m_LightViewProj
//-----------------------------------------------------------------------------
void CPracticalPSM::BuildLIPSMProjectionMatrix	()
{
	// The following function performs more work than LiSPSM needs
	// But it is used here for convenience
	ComputeVirtualCameraParameters	();					//. to compute caster/receiver objects

	// Find inverse transform
	matrix				modelView;
	matrix				modelViewProjection;
	matrix				modelViewProjection_inv;
	D3DXMatrixMultiply	(&modelView, &m_World, &m_View);
	D3DXMatrixMultiply	(&modelViewProjection, &modelView, &m_Projection);
	D3DXMatrixInverse	(&modelViewProjection_inv,0,&modelViewProjection);

	// calculate view-frustum bounds in world space
	// note: D3D uses [0..1] range for Z
	vector3		A[8];
	A[0]		= vector3( -1, -1,  0);
	A[1]		= vector3( -1, -1, +1);
	A[2]		= vector3( -1, +1, +1);
	A[3]		= vector3( -1, +1,  0);
	A[4]		= vector3( +1, +1, +1);
	A[5]		= vector3( +1, +1,  0);
	A[6]		= vector3( +1, -1, +1);
	A[7]		= vector3( +1, -1,  0);

	std::vector<vector3>	points;
	points.resize			(8);
	xform_array				(&*points.begin(),A,modelViewProjection_inv,8);

	// Perform approximate clipping if requisted
	// This is similar to focusing shadowmap to the interesting parts only
	if (m_bUnitCubeClip)			{
		// Create convex volume enclosing the primary frustum and extended up to infinity
		// in the direction inverse to light
		DumbConvexHull			hull;
		std::vector<vector3>&	P = points;

		// Lets begin from base frustum
		int		facetable[6][4]	= 
		{
			{ 0, 3, 5, 7 },
			{ 1, 2, 3, 0 },
			{ 6, 7, 5, 4 },
			{ 4, 2, 1, 6 },
			{ 3, 2, 4, 5 },
			{ 1, 0, 7, 6 },
		};
		for (int plane=0; plane<6; plane++)
		{
			hull.polys.push_back(DumbConvexHull::_poly());
			for (int pt=0; pt<4; pt++)	hull.polys.back().points.push_back(points[facetable[plane][pt]]);
		}

		// Now extend this towards the light
		int		edgetable[12][2]	= 
		{
			{ }, 
		};


		// use shadow casters
		// note: actual clipping not implemented, so this is not conservative at all
		Frustum sceneFrustum		( &modelViewProjection );
		points.clear				();
		for (int it=0; it<m_ShadowCasterPoints.size(); it++)
		{
			BoundingBox&	bb		= m_ShadowCasterPoints[it];
			points.push_back		(vector3(bb.minPt.x,bb.minPt.y,bb.minPt.z));
			points.push_back		(vector3(bb.minPt.x,bb.minPt.y,bb.maxPt.z));
			points.push_back		(vector3(bb.minPt.x,bb.maxPt.y,bb.minPt.z));
			points.push_back		(vector3(bb.minPt.x,bb.maxPt.y,bb.maxPt.z));
			points.push_back		(vector3(bb.maxPt.x,bb.minPt.y,bb.minPt.z));
			points.push_back		(vector3(bb.maxPt.x,bb.minPt.y,bb.maxPt.z));
			points.push_back		(vector3(bb.maxPt.x,bb.maxPt.y,bb.minPt.z));
			points.push_back		(vector3(bb.maxPt.x,bb.maxPt.y,bb.maxPt.z));
		}
		for (int it=0; it<m_ShadowReceiverPoints.size(); it++)
		{
			BoundingBox&	bb		= m_ShadowReceiverPoints[it];
			points.push_back		(vector3(bb.minPt.x,bb.minPt.y,bb.minPt.z));
			points.push_back		(vector3(bb.minPt.x,bb.minPt.y,bb.maxPt.z));
			points.push_back		(vector3(bb.minPt.x,bb.maxPt.y,bb.minPt.z));
			points.push_back		(vector3(bb.minPt.x,bb.maxPt.y,bb.maxPt.z));
			points.push_back		(vector3(bb.maxPt.x,bb.minPt.y,bb.minPt.z));
			points.push_back		(vector3(bb.maxPt.x,bb.minPt.y,bb.maxPt.z));
			points.push_back		(vector3(bb.maxPt.x,bb.maxPt.y,bb.minPt.z));
			points.push_back		(vector3(bb.maxPt.x,bb.maxPt.y,bb.maxPt.z));
		}
		for (int it=0; it<points.size(); it++)
		{
			BoundingSphere	S;
			S.centerVec		= points[it];
			S.radius		= 0.f;
			if (!sceneFrustum.TestSphere(&S))	{
				points.erase(points.begin()+it);
				it--;
			}
		}
	}

	// this is the algorithm discussed in the paper
	matrix				lispsm;
    {
		// find the view and light direction + view-position 
		// normalization is unnecessary, but... :)
		matrix				inv_view;
		D3DXMatrixInverse	(&inv_view,0,&m_View);
		vector3				dir_light	(-m_lightDir), 
							dir_view	(inv_view._31,inv_view._32,inv_view._33),
							pos_view	(inv_view._41,inv_view._42,inv_view._43);
		D3DXVec3Normalize	(&dir_light,&dir_light);
		D3DXVec3Normalize	(&dir_view ,&dir_view );

		// up-vector
		vector3				up;
		{
			vector3				left;	//?
			//left is the normalized vector perpendicular to lightDir and viewDir
			//this means left is the normalvector of the yz-plane from the paper
			D3DXVec3Cross		(&left,&dir_light,&dir_view);
			//we now can calculate the rotated(in the yz-plane) viewDir vector
			//and use it as up vector in further transformations
			D3DXVec3Cross		(&up,&left,&dir_light);
			D3DXVec3Normalize	(&up,&up);
		}

		// relation
		float	dotProd		= D3DXVec3Dot	(&dir_light,&dir_view);
		float	cosGamma	= sqrt			(1.0f-dotProd*dotProd);

		// temporal light View
		// look from position(eyePos)
		// into direction(lightDir) 
		// with up vector(up)
		matrix		lightview;
		D3DXMatrixLookAtLH		(&lightview,&pos_view,&(pos_view+dir_light),&up);

		//transform the light volume points from world into light space
		//calculate the cubic hull (an AABB) 
		//of the light space extents of the intersection body B
		//and save the two extreme points min and max
		vector3			min,max;
		calc_xaabb		(min,max,&*points.begin(),lightview,points.size());

		{
			float	_z_near			= m_bUnitCubeClip?min.y:ZNEAR_MIN;
			float	_z_far			= m_bUnitCubeClip?max.y:ZFAR_MAX;

			//use the formulas of the paper to get n (and f)
			const float factor		= 1.0f/cosGamma;
			const float z_n			= factor*_z_near;			//often 1 
			const float d			= fabsf(_z_far-_z_near);	//(max[1]-min[1]), perspective transform depth //light space y extents
			const float z_f			= z_n + d*cosGamma;
			const float n			= m_fBiasLiSPSM * (z_n+sqrt(z_f*z_n))/cosGamma;
			const float f			= n+d;
			m_fLiSPSM_N				= n;

			//new observer point n-1 behind eye position
			vector3		pos			= pos_view - up * (n-ZNEAR_MIN);
			D3DXMatrixLookAtLH		(&lightview,&pos,&(pos+dir_light),&up);

			//one possibility for a simple perspective transformation matrix
			//with the two parameters n(near) and f(far) in y direction
			D3DXMatrixIdentity	(&lispsm);	// a = (f+n)/(f-n); b = -2*f*n/(f-n);
			lispsm._22	= (f+n)/(f-n);		// [ 1 0 0 0] 
			lispsm._24	= -2*f*n/(f-n);		// [ 0 a 0 b]
			lispsm._42	= 1;				// [ 0 0 1 0]
			lispsm._44	= 0;				// [ 0 1 0 0]
			D3DXMatrixTranspose	(&lispsm,&lispsm);

			//temporal arrangement for the transformation of the points to post-perspective space
			D3DXMatrixMultiply	(&lispsm, &lightview, &lispsm);	// ligthProjection = lispMtx*lightView

			//transform the light volume points from world into the distorted light space
			//calculate the cubic hull (an AABB) 
			//of the light space extents of the intersection body B
			//and save the two extreme points min and max
			calc_xaabb			(min,max,&*points.begin(),lispsm,points.size());
		}

		// refit to unit cube (in D3D way :)
		// this operation calculates a scale translate matrix that
		// maps the two extreme points min and max into (-1,-1,0) and (1,1,1)
		matrix	refit;
		{
			vector3	size			=	max - min;
			vector3	center			=	(max+min)/2.f;
					center.z		-=	size.z/2.f;
			
			matrix	translate,scale;
			D3DXMatrixTranslation	(&translate,-center.x,-center.y,-center.z);		//*. translate center to (0,0,0)
			D3DXMatrixScaling		(&scale,	2.f/size.x,2.f/size.y,1.f/size.z);	//*. scale: sx=2,sy=2,sz=1
			D3DXMatrixMultiply		(&refit, &translate, &scale);					//*. final: first-translate, then scale
		}

		//together
		D3DXMatrixMultiply			(&lispsm, &lispsm, &refit);						// ligthProjection = scaleTranslate*lispMtx
	}

	m_LightViewProj		= lispsm;
}
