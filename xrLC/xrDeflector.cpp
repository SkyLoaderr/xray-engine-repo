#include "stdafx.h"
#include "resource.h"
#include "build.h"
#include "xrIsect.h"
#include "math.h"

void blit	(LPDWORD dest, DWORD ds_x, DWORD ds_y, LPDWORD src, DWORD ss_x, DWORD ss_y, DWORD px, DWORD py, DWORD aREF)
{
	R_ASSERT(ds_x>=(ss_x+px));
	R_ASSERT(ds_y>=(ss_y+py));
	for (DWORD y=0; y<ss_y; y++)
		for (DWORD x=0; x<ss_x; x++)
		{
			DWORD dx = px+x;
			DWORD dy = py+y;
			DWORD sc = src[y*ss_x+x];
			if (RGBA_GETALPHA(sc)>=aREF) dest[dy*ds_x+dx] = sc;
		}
}

void blit_r	(LPDWORD dest, DWORD ds_x, DWORD ds_y, LPDWORD src, DWORD ss_x, DWORD ss_y, DWORD px, DWORD py, DWORD aREF)
{
	R_ASSERT(ds_x>=(ss_y+px));
	R_ASSERT(ds_y>=(ss_x+py));
	for (DWORD y=0; y<ss_y; y++)
		for (DWORD x=0; x<ss_x; x++)
		{
			DWORD dx = px+y;
			DWORD dy = py+x;
			DWORD sc = src[y*ss_x+x];
			if (RGBA_GETALPHA(sc)>=aREF) dest[dy*ds_x+dx] = sc;
		}
}

//-------------------------------------
vecDefl					g_deflectors;
CDeflector*				Deflector = 0;

IC BOOL UVpointInside(UVpoint &P, UVtri &T)
{
	Fvector B;
	return T.isInside(P,B);
}

CDeflector::CDeflector()
{
	Deflector	= this;
	N.set		(0,1,0);
	Sphere.P.set(flt_max,flt_max,flt_max);
	Sphere.R	= 0;
	dwWidth		= 0;
	dwHeight	= 0;
	bMerged		= FALSE;
	UVpolys.reserve	(32);
	layers.reserve	(2);
}
CDeflector::~CDeflector()
{
	for (DWORD I=0; I<layers.size(); I++)
		_FREE	(layers[I].lm.pSurface);
	layers.clear();
}

VOID CDeflector::OA_Export()
{
	if (UVpolys.empty()) return;

	// Correct normal
	//  (semi-proportional to pixel density)
	N.set			(0,0,0);
	float density	= 0;
	float fcount	= 0;
	for (UVIt it = UVpolys.begin(); it!=UVpolys.end(); it++)
	{
		Face	*F = it->owner;
		Fvector	SN;
		SN.set	(F->N);
		SN.mul	(F->CalcArea());
		N.add	(SN);

		density	+= F->Shader().lm_density;
		fcount	+= 1.f;
	}
	N.normalize();
	density /= fcount;
	
	// Orbitrary Oriented Ortho - Projection
	Fmatrix		mView;
    Fvector		at,from,up,right,y;
	at.set		(0,0,0);
	from.add	(at,N );
	y.set		(0,1,0);
	if (fabsf(N.y)>.99f) y.set(1,0,0);
	right.crossproduct(y,N);	right.normalize_safe();
	up.crossproduct(N,right);	up.normalize_safe();
	mView.build_camera(from,at,up);

	Fbox bb; bb.invalidate();
	for (it = UVpolys.begin(); it!=UVpolys.end(); it++)
	{
		UVtri	*T = it;
		Face	*F = T->owner;
		Fvector	P;	// projected

		for (int i=0; i<3; i++) {
			mView.transform_tiny	(P,F->v[i]->P);
			T->uv[i].set			(P.x,P.y);
			bb.modify				(F->v[i]->P);
		}
	}
	bb.getsphere(Sphere.P,Sphere.R);

	// UV rect
	UVpoint		min,max,size;
	GetRect		(min,max);
	size.sub	(max,min);

	// Surface
	dwWidth		= iCeil(size.u*g_params.m_lm_pixels_per_meter*density+.5f); clamp(dwWidth, 1ul,512ul-2*BORDER);
	dwHeight	= iCeil(size.v*g_params.m_lm_pixels_per_meter*density+.5f); clamp(dwHeight,1ul,512ul-2*BORDER);
}

BOOL CDeflector::OA_Place	(Face *owner)
{
	float cosa = N.dotproduct(owner->N);
	if (cosa<cosf(deg2rad(g_params.m_lm_split_angle))) return FALSE;

	UVtri				T;
	T.owner				= owner;
	owner->pDeflector	= this;
	UVpolys.push_back	(T);
	return TRUE;
}

void CDeflector::OA_Place	(vecFace& lst)
{
	UVpolys.clear	();
	UVpolys.reserve	(lst.size());
	for (DWORD I=0; I<lst.size(); I++)
	{
		UVtri T;
		Face* F			= lst[I];
		T.owner			= F;
		F->pDeflector	= this;
		UVpolys.push_back(T);
	}
}

VOID CDeflector::GetRect	(UVpoint &min, UVpoint &max)
{
	// Calculate bounds
	vector<UVtri>::iterator it=UVpolys.begin();
	min = max = it->uv[0];
	for (;it != UVpolys.end(); it++)
	{
		for (int i=0; i<3; i++) {
			min.min(it->uv[i]);
			max.max(it->uv[i]);
		}
	}
}

void CDeflector::RemapUV	(vector<UVtri>& dest, DWORD base_u, DWORD base_v, DWORD size_u, DWORD size_v, DWORD lm_u, DWORD lm_v, BOOL bRotate)
{
	dest.clear	();
	dest.reserve(UVpolys.size());
	
	// UV rect (actual)
	UVpoint		a_min,a_max,a_size;
	GetRect		(a_min,a_max);
	a_size.sub	(a_max,a_min);
	
	// UV rect (dedicated)
	UVpoint		d_min,d_max,d_size;
	d_min.u		= (float(base_u)+.5f)/float(lm_u);
	d_min.v		= (float(base_v)+.5f)/float(lm_v);
	d_max.u		= (float(base_u+size_u)-.5f)/float(lm_u);
	d_max.v		= (float(base_v+size_v)-.5f)/float(lm_v);
	if (d_min.u>=d_max.u)	{ d_min.u=d_max.u=(d_min.u+d_max.u)/2; d_min.u-=EPS_S; d_max.u+=EPS_S; }
	if (d_min.v>=d_max.v)	{ d_min.v=d_max.v=(d_min.v+d_max.v)/2; d_min.v-=EPS_S; d_max.v+=EPS_S; }
	d_size.sub	(d_max,d_min);
	
	// Remapping
	UVpoint		tc;
	UVtri		tnew;
	if (bRotate)	{
		for (UVIt it = UVpolys.begin(); it!=UVpolys.end(); it++)
		{
			UVtri&	T	= *it;
			tnew.owner	= T.owner;
			for (int i=0; i<3; i++) 
			{
				tc.u = ((T.uv[i].v-a_min.v)/a_size.v)*d_size.u + d_min.u;
				tc.v = ((T.uv[i].u-a_min.u)/a_size.u)*d_size.v + d_min.v;
				tnew.uv[i].set(tc);
			}
			dest.push_back	(tnew);
		}
	} else {
		for (UVIt it = UVpolys.begin(); it!=UVpolys.end(); it++)
		{
			UVtri&	T	= *it;
			tnew.owner	= T.owner;
			for (int i=0; i<3; i++) 
			{
				tc.u = ((T.uv[i].u-a_min.u)/a_size.u)*d_size.u + d_min.u;
				tc.v = ((T.uv[i].v-a_min.v)/a_size.v)*d_size.v + d_min.v;
				tnew.uv[i].set(tc);
			}
			dest.push_back	(tnew);
		}
	}
}

void CDeflector::RemapUV(DWORD base_u, DWORD base_v, DWORD size_u, DWORD size_v, DWORD lm_u, DWORD lm_v, BOOL bRotate)
{
	vector<UVtri>	tris_new;
	RemapUV			(tris_new,base_u,base_v,size_u,size_v,lm_u,lm_v,bRotate);
	UVpolys			= tris_new;
}

CDeflector::Layer* CDeflector::GetLayer(int base)
{
	for (DWORD I=0; I<layers.size(); I++)
		if (layers[I].base_id == base)	return &layers[I];
	return 0;
}
