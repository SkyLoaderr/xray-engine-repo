// Frustum.cpp: implementation of the CFrustum class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Frustum.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EFC_Visible testSI(CFrustum& F, Fvector& C, float r, BYTE& tmp)
{
	return F.testSphere(C,r,tmp);
}

void CFrustum::CreateFromPoints(Fvector& COP, Fvector* p, int count)
{
	VERIFY(count<FRUSTUM_MAXPLANES);
	VERIFY(count>=3);

	_clear();
	for (int i=1; i<count; i++)
		_add(COP,p[i-1],p[i]);
	_add(COP,p[count-1],p[0]);
}

void CFrustum::CreateFromPlanes(Fplane* p, int count){
	for (int k=0; k<count; k++)
		planes[k].set(p[k]);

	for (int i=0;i<count;i++)
	{
		float denom = 1.0f / planes[i].n.magnitude();// Get magnitude of Vector
		planes[i].n.x	*= denom;
		planes[i].n.y	*= denom;
		planes[i].n.z	*= denom;
		planes[i].d		*= denom;
	}

	p_count = count;
}

void CFrustum::CreateFromPortal(sPoly* poly)
{
	Fplane	P;
	P.build	((*poly)[0],(*poly)[1],(*poly)[2]);

	if (poly->size()>6) {
		SimplifyPoly_AABB(poly,P);
		P.build	((*poly)[0],(*poly)[1],(*poly)[2]);
	}

	// Check plane orientation relative to viewer
	// and reverse if needed
	if (P.classify(Device.vCameraPosition)<0)
	{
		reverse(poly->begin(),poly->end());
		P.build	((*poly)[0],(*poly)[1],(*poly)[2]);
	}

	// Base creation
	CreateFromPoints(Device.vCameraPosition,poly->begin(),poly->size());

	// Near clipping plane
	_add(P);

	// Far clipping plane
	Fmatrix &M	= Device.mFullTransform;
	P.n.x	= -(M._14 - M._13);
	P.n.y	= -(M._24 - M._23);
	P.n.z	= -(M._34 - M._33);
	P.d		= -(M._44 - M._43);
	float denom = 1.0f / P.n.magnitude();
	P.n.x	*= denom;
	P.n.y	*= denom;
	P.n.z	*= denom;
	P.d		*= denom;
	_add	(P);
}

void CFrustum::SimplifyPoly_AABB(sPoly* poly, Fplane& plane)
{
	Fmatrix		mView,mInv;
    Fvector		from,up,right,y;
	from.set	((*poly)[0]);
	y.set		(0,1,0);
	if (fabsf(plane.n.y)>0.99f) y.set(1,0,0);
	right.crossproduct		(y,plane.n);
	up.crossproduct			(plane.n,right);
	mView.build_camera_dir	(from,plane.n,up);

	// Project and find extents
	Fvector2	min,max;
	min.set		(flt_max,flt_max);
	max.set		(flt_min,flt_min);
	for (DWORD i=0; i<poly->size(); i++)
	{
		Fvector2 tmp;
		mView.transform_tiny32(tmp,(*poly)[i]);
		min.min(tmp.x,tmp.y);
		max.max(tmp.x,tmp.y);
	}

	// Build other 2 points and inverse project
	Fvector2	p1,p2;
	p1.set		(min.x,max.y);
	p2.set		(max.x,min.y);
	mInv.invert	(mView);
	poly->clear	();
	
	mInv.transform_tiny23(poly->last(),min);	poly->inc();
	mInv.transform_tiny23(poly->last(),p1);		poly->inc();
	mInv.transform_tiny23(poly->last(),max);	poly->inc();
	mInv.transform_tiny23(poly->last(),p2);		poly->inc();
}

void CFrustum::CreateOccluder(Fvector* p, int count, CFrustum& clip)
{
	VERIFY(count<FRUSTUM_SAFE);
	VERIFY(count>=3);

	BOOL	edge[FRUSTUM_SAFE];
	float	cls	[FRUSTUM_SAFE];
	ZeroMemory(edge,sizeof(edge));
	for (int i=0; i<clip.p_count; i++)
	{
		// classify all points relative to plane #i
		Fplane &P = clip.planes[i];
		for (int j=0; j<count; j++) cls[j]=fabsf(P.classify(p[j]));

		// test edges to see which lies directly on plane
		for (j=0; j<count; j++) {
			if (cls[j]<EPS_L)
			{
				int next = j+1; if (next>=count) next=0;
				if (cls[next]<EPS_L) {
					// both points lies on plane - mark as 'open'
					edge[j] = true;
				}
			}
		}
	}

	// here we have all edges marked accordenly to 'open' / 'closed' classification
	_clear	();
	_add	(p[0],p[1],p[2]);		// main plane
	for (i=0; i<count; i++)
	{
		if (!edge[i]) {
			int next = i+1; if (next>=count) next=0;
			_add(Device.vCameraPosition,p[i],p[next]);
		}
	}
}

sPoly*	CFrustum::ClipPoly(sPoly& S, sPoly& D)
{
	sPoly*	src		= &D;
	sPoly*	dest	= &S;
	for (int i=0; i<p_count; i++)
	{
		// cache plane and swap lists
		Fplane &P = planes[i];
		swap		(src,dest);
		dest->clear	();

		// classify all points relative to plane #i
		float	cls	[FRUSTUM_SAFE];
		for (DWORD j=0; j<src->size(); j++) cls[j]=P.classify((*src)[j]);

		// clip everything to this plane
		cls[src->size()] = cls[0];
		src->push_back((*src)[0]);
		Fvector D; float denum,t;
		for (j=0; j<src->size()-1; j++)
		{
			if ((*src)[j].similar((*src)[j+1],EPS_S)) continue;

			if (negative(cls[j]))
			{
				dest->push_back((*src)[j]);
				if (positive(cls[j+1])) 
				{
					// segment intersects plane
					D.sub((*src)[j+1],(*src)[j]);
					denum = P.n.dotproduct(D);
					if (denum!=0) {
						t = -cls[j]/denum; //VERIFY(t<=1.f && t>=0);
						dest->last().direct((*src)[j],D,t);
						dest->inc();
					}
				}
			} else {
				// J - outside
				if (negative(cls[j+1]))
				{
					// J+1  - inside
					// segment intersects plane
					D.sub((*src)[j+1],(*src)[j]);
					denum = P.n.dotproduct(D);
					if (denum!=0) {
						t = -cls[j]/denum; //VERIFY(t<=1.f && t>=0);
						dest->last().direct((*src)[j],D,t);
						dest->inc();
					}
				}
			}
		}

		// here we end up with complete polygon in 'dest' which is inside plane #i
		if (dest->size()<3) return 0;
	}
	return dest;
}

BOOL CFrustum::CreateFromClipPoly(Fvector* p, int count, CFrustum& clip)
{
	VERIFY(count<FRUSTUM_MAXPLANES);
	VERIFY(count>=3);

	sPoly	poly1	(p,count);
	sPoly	poly2;
	sPoly*	dest	= clip.ClipPoly(poly1,poly2);

	// here we end up with complete frustum-polygon in 'dest'
	if (dest) {
		CreateFromPoints(Device.vCameraPosition,dest->begin(),dest->size());
		return true;
	} else return false;
}

void CFrustum::CreateFromViewMatrix(Fmatrix &M)
{
	// Left clipping plane
	planes[0].n.x	= -(M._14 + M._11);
	planes[0].n.y	= -(M._24 + M._21);
	planes[0].n.z	= -(M._34 + M._31);
	planes[0].d		= -(M._44 + M._41);
	
	// Right clipping plane
	planes[1].n.x	= -(M._14 - M._11);
	planes[1].n.y	= -(M._24 - M._21);
	planes[1].n.z	= -(M._34 - M._31);
	planes[1].d		= -(M._44 - M._41);
	
	// Top clipping plane
	planes[2].n.x	= -(M._14 - M._12);
	planes[2].n.y	= -(M._24 - M._22);
	planes[2].n.z	= -(M._34 - M._32);
	planes[2].d		= -(M._44 - M._42);
	
	// Bottom clipping plane
	planes[3].n.x	= -(M._14 + M._12);
	planes[3].n.y	= -(M._24 + M._22);
	planes[3].n.z	= -(M._34 + M._32);
	planes[3].d		= -(M._44 + M._42);

	/*
	// Near clipping plane
	planes[4].n.x	= -(M._14 + M._13);
	planes[4].n.y	= -(M._24 + M._23);
	planes[4].n.z	= -(M._34 + M._33);
	planes[4].d		= -(M._44 + M._43);
	*/

	// Far clipping plane
	planes[4].n.x	= -(M._14 - M._13);
	planes[4].n.y	= -(M._24 - M._23);
	planes[4].n.z	= -(M._34 - M._33);
	planes[4].d		= -(M._44 - M._43);

	for (int i=0;i<5;i++)
	{
		float denom = 1.0f / planes[i].n.magnitude();// Get magnitude of Vector
		planes[i].n.x	*= denom;
		planes[i].n.y	*= denom;
		planes[i].n.z	*= denom;
		planes[i].d		*= denom;
	}

	p_count = 5;
}
void CFrustum::CreateFromMatrix(Fmatrix &M)
{
	// Left clipping plane
	planes[0].n.x	= -(M._14 + M._11);
	planes[0].n.y	= -(M._24 + M._21);
	planes[0].n.z	= -(M._34 + M._31);
	planes[0].d		= -(M._44 + M._41);
	
	// Right clipping plane
	planes[1].n.x	= -(M._14 - M._11);
	planes[1].n.y	= -(M._24 - M._21);
	planes[1].n.z	= -(M._34 - M._31);
	planes[1].d		= -(M._44 - M._41);
	
	// Top clipping plane
	planes[2].n.x	= -(M._14 - M._12);
	planes[2].n.y	= -(M._24 - M._22);
	planes[2].n.z	= -(M._34 - M._32);
	planes[2].d		= -(M._44 - M._42);
	
	// Bottom clipping plane
	planes[3].n.x	= -(M._14 + M._12);
	planes[3].n.y	= -(M._24 + M._22);
	planes[3].n.z	= -(M._34 + M._32);
	planes[3].d		= -(M._44 + M._42);

	for (int i=0;i<4;i++)
	{
		float denom = 1.0f / planes[i].n.magnitude();// Get magnitude of Vector
		planes[i].n.x	*= denom;
		planes[i].n.y	*= denom;
		planes[i].n.z	*= denom;
		planes[i].d		*= denom;
	}

	p_count = 4;
}