//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Frustum.h"
#include "UI_Main.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EFC_Visible testSI(CFrustum& F, Fvector& C, float r, BYTE& tmp)
{
	return F.testSphere(C,r,tmp);
}

void CFrustum::CreateFromPoints(Fvector* p, int count)
{
	VERIFY(count<FRUSTUM_MAXPLANES);
	VERIFY(count>=3);

	_clear();
	for (int i=1; i<count; i++)
		_add(UI->Device.m_Camera.GetPosition(),p[i-1],p[i]);
	_add(UI->Device.m_Camera.GetPosition(),p[count-1],p[0]);
}

sPoly*	CFrustum::ClipPoly(sPoly& S, sPoly& D) const
{
	sPoly*	src		= &D;
	sPoly*	dest	= &S;
	for (int i=0; i<p_count; i++)
	{
		// cache plane and swap lists
		const Fplane &P = planes[i];
		swap		(src,dest);
		dest->clear	();

		// classify all points relative to plane #i
		float	cls	[FRUSTUM_SAFE];
		for (int j=0; j<src->size(); j++) cls[j]=P.classify((*src)[j]);

		// clip everything to this plane
		cls[src->size()] = cls[0];
		src->push_back((*src)[0]);
		Fvector D; float denum,t;
		for (j=0; j<src->size()-1; j++)
		{
			if (cls[j]<0)
			{
				dest->push_back((*src)[j]);
				if (cls[j+1]>0)
				{
					// segment intersects plane
					D.sub((*src)[j+1],(*src)[j]);
					denum = P.n.dotproduct(D);
					if (denum!=0) {
						t = -cls[j]/denum; VERIFY(t<=1.f && t>=0);
						dest->last().direct((*src)[j],D,t);
						dest->inc();
					}
				}
			} else {
				// J - outside
				if (cls[j+1]<0)
				{
					// J+1  - inside
					// segment intersects plane
					D.sub((*src)[j+1],(*src)[j]);
					denum = P.n.dotproduct(D);
					if (denum!=0) {
						t = -cls[j]/denum; VERIFY(t<=1.f && t>=0);
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

bool CFrustum::CreateFromClipPoly(Fvector* p, int count, CFrustum& clip)
{
	VERIFY(count<FRUSTUM_MAXPLANES);
	VERIFY(count>=3);

	sPoly	poly1	(p,count);
	sPoly	poly2;
	sPoly*	dest	= clip.ClipPoly(poly1,poly2);

	// here we end up with complete frustum-polygon in 'dest'
	if (dest) {
		CreateFromPoints(dest->begin(),dest->size());
		return true;
	} else return false;
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

void CFrustum::CreateFromViewMatrix()
{
	Fmatrix &M	= UI->Device.m_FullTransform;

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

	// Near clipping plane
	planes[4].n.x	= -(M._14 + M._13);
	planes[4].n.y	= -(M._24 + M._23);
	planes[4].n.z	= -(M._34 + M._33);
	planes[4].d		= -(M._44 + M._43);

	// Far clipping plane
	planes[5].n.x	= -(M._14 - M._13);
	planes[5].n.y	= -(M._24 - M._23);
	planes[5].n.z	= -(M._34 - M._33);
	planes[5].d		= -(M._44 - M._43);

	for (int i=0;i<6;i++)
	{
		float denom = 1.0f / planes[i].n.magnitude();// Get magnitude of Vector
		planes[i].n.x	*= denom;
		planes[i].n.y	*= denom;
		planes[i].n.z	*= denom;
		planes[i].d		*= denom;
	}

	p_count = 6;
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








//---------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/*
bool FrustumPickFace(
	ICullPlane *pl, // four planes !
	Fvector& p0, Fvector& p1, Fvector& p2 )
{
	for( int i=0; i<4; i++)
		if( pl[i].dist_point( p0 )<0 &&
			pl[i].dist_point( p1 )<0 &&
			pl[i].dist_point( p2 )<0    )
				return false;
	return true;
};
bool Frustum2DPickFace(Fvector& lt, Fvector& rb, Fvector* p){
    Fvector b[4];
    b[0].x = min( lt.x, rb.x );    b[0].y = min( lt.y, rb.y );
    b[1].x = max( lt.x, rb.x );    b[1].y = min( lt.y, rb.y );
    b[2].x = max( lt.x, rb.x );    b[2].y = max( lt.y, rb.y );
    b[3].x = min( lt.x, rb.x );    b[3].y = max( lt.y, rb.y );
//    Fvector B;
    // selection box point in triangle
//    for (int k=0; k<4; k++) if (isInside(p,b[k],B)) return true;
    // tri point in selection box
    for (int v=0; v<3; v++)
        if ((p[v].x>b[0].x)&&(p[v].x<b[2].x)&&(p[v].y>b[0].y)&&(p[v].y<b[2].y)) return true;
    return false;
}
*/
