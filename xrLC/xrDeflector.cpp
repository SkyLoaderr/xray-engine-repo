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
			if (RGBA_GETALPHA(sc)<aREF)	sc=0;
			dest[dy*ds_x+dx] = sc;
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
			if (RGBA_GETALPHA(sc)<aREF)	sc=0;
			dest[dy*ds_x+dx] = sc;
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
	lm.pSurface = 0;
	lm_rad		= 0;
	lm_spans	= 0;
}
CDeflector::~CDeflector()
{
	_FREE		(lm.pSurface);
	_FREE		(lm_rad);
	_FREE		(lm_spans);
}

VOID CDeflector::OA_Export()
{
	if (tris.empty()) return;

	// Correct normal
	//  (semi-proportional to pixel density)
	N.set			(0,0,0);
	float density	= 0;
	float fcount	= 0;
	for (UVIt it = tris.begin(); it!=tris.end(); it++)
	{
		Face	*F = it->owner;
		Fvector	SN;
		SN.set	(F->N);
		SN.mul	(F->CalcArea());
		N.add	(SN);

		density	+= F->Shader().cl_LM_Density;
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
	for (it = tris.begin(); it!=tris.end(); it++)
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
	bb.getsphere(Center,Radius);

	// UV rect
	UVpoint		min,max,size;
	GetRect		(min,max);
	size.sub	(max,min);

	// Surface
	lm.dwWidth  = iCeil(size.u*g_params.m_lm_pixels_per_meter*density+.5f); clamp(lm.dwWidth, 1ul,512ul-2*BORDER);
	lm.dwHeight = iCeil(size.v*g_params.m_lm_pixels_per_meter*density+.5f); clamp(lm.dwHeight,1ul,512ul-2*BORDER);
	lm.bHasAlpha= FALSE;
	iArea		= (lm.dwWidth+2*BORDER)*(lm.dwHeight+2*BORDER);

	// Addressing
	RemapUV		(0,0,lm.dwWidth,lm.dwHeight,lm.dwWidth,lm.dwHeight,FALSE);
}

BOOL CDeflector::OA_Place(Face *owner)
{
	float cosa = N.dotproduct(owner->N);
	if (cosa<cosf(deg2rad(g_params.m_lm_split_angle))) return FALSE;

	UVtri T;
	T.owner = owner;
	tris.push_back(T);
	owner->pDeflector = this;
	return TRUE;
}

VOID CDeflector::GetRect(UVpoint &min, UVpoint &max)
{
	// Calculate bounds
	vector<UVtri>::iterator it=tris.begin();
	min = max = it->uv[0];
	for (;it != tris.end(); it++)
	{
		for (int i=0; i<3; i++) {
			min.min(it->uv[i]);
			max.max(it->uv[i]);
		}
	}
}

void CDeflector::RemapUV(DWORD base_u, DWORD base_v, DWORD size_u, DWORD size_v, DWORD lm_u, DWORD lm_v, BOOL bRotate)
{
	FPU::m64r	();
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
	if (bRotate)	{
		for (UVIt it = tris.begin(); it!=tris.end(); it++)
		{
			UVtri&	T  = *it;
			for (int i=0; i<3; i++) 
			{
				tc.u = ((T.uv[i].v-a_min.v)/a_size.v)*d_size.u + d_min.u;
				tc.v = ((T.uv[i].u-a_min.u)/a_size.u)*d_size.v + d_min.v;
				T.uv[i].set(tc);
			}
		}
	} else {
		for (UVIt it = tris.begin(); it!=tris.end(); it++)
		{
			UVtri&	T  = *it;
			for (int i=0; i<3; i++) 
			{
				tc.u = ((T.uv[i].u-a_min.u)/a_size.u)*d_size.u + d_min.u;
				tc.v = ((T.uv[i].v-a_min.v)/a_size.v)*d_size.v + d_min.v;
				T.uv[i].set(tc);
			}
		}
	}
	FPU::m24r	();
}

VOID CDeflector::Capture		(CDeflector *D, int b_u, int b_v, int s_u, int s_v, BOOL bRotated)
{
	// Allocate 512x512 texture if needed
	if (0==lm.pSurface)	{
		DWORD	size	= 512*512*4;
		lm.pSurface		= LPDWORD(malloc(size));
		ZeroMemory		(lm.pSurface,size);
	}

	// Addressing
	D->RemapUV		(b_u+BORDER,b_v+BORDER,s_u-2*BORDER,s_v-2*BORDER,512,512,bRotated);

	// Capture faces and setup their coords
	for (UVIt T=D->tris.begin(); T!=D->tris.end(); T++)
	{
		UVtri	P		= *T;
		Face	*F		= P.owner;
		F->pDeflector	= this;
		F->AddChannel	(P.uv[0], P.uv[1], P.uv[2]);
		tris.push_back	(P);
	}

	// Perform BLIT
	if (!bRotated) 
	{
		DWORD real_H	= (D->lm.dwHeight	+ 2*BORDER);
		DWORD real_W	= (D->lm.dwWidth	+ 2*BORDER);
		blit	(lm.pSurface,512,512,D->lm.pSurface,real_W,real_H,b_u,b_v,254-BORDER);
	} else {
		DWORD real_H	= (D->lm.dwHeight	+ 2*BORDER);
		DWORD real_W	= (D->lm.dwWidth	+ 2*BORDER);
		blit_r	(lm.pSurface,512,512,D->lm.pSurface,real_W,real_H,b_u,b_v,254-BORDER);
	}
}
