#include "stdafx.h"
#include "resource.h"
#include "build.h"
#include "xrIsect.h"
#include "math.h"

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

#ifdef DEFL_SHOW
	WNDCLASS C = {CS_OWNDC, deflector_proc, 0,0, GetModuleHandle(0), 0,0,(HBRUSH)GetStockObject(BLACK_BRUSH), 0, "DEFL" };
	RegisterClass(&C);
	hw = CreateWindow(
		"DEFL",
		"Deflector",
		WS_SIZEBOX|WS_VISIBLE,
		0,0,700,700,
		0,0,0,0);
	VERIFY(hw);
	hPen = CreatePen(PS_SOLID, 1, RGB(0,255,0));
#endif
}
CDeflector::~CDeflector()
{
	_FREE(lm.pSurface);

#ifdef DEFL_SHOW
	DeleteObject(hPen);
	DestroyWindow(hw);
#endif
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

	for (it = tris.begin(); it!=tris.end(); it++)
	{
		UVtri	*T = it;
		Face	*F = T->owner;
		Fvector	P;	// projected

		for (int i=0; i<3; i++) {
			mView.transform_tiny(P,F->v[i]->P);
			T->uv[i].u=P.x;	T->uv[i].v=P.y;
		}
	}

	// UV rect
	UVpoint		min,max,size;
	GetRect		(min,max);
	size.sub	(max,min);

	// Surface
	lm.dwWidth  = iCeil(size.u*g_params.m_lm_pixels_per_meter*density+.5f); clamp(lm.dwWidth, 1ul,512ul-2*BORDER);
	lm.dwHeight = iCeil(size.v*g_params.m_lm_pixels_per_meter*density+.5f); clamp(lm.dwHeight,1ul,512ul-2*BORDER);
	lm.bHasAlpha= FALSE;
	iArea		= (lm.dwWidth+2*BORDER)*(lm.dwHeight+2*BORDER);

	// Setup variables
	UVpoint		dim;
	dim.set		(float(lm.dwWidth), float(lm.dwHeight));

	// *** Addressing 
	// also calculate center & radius in 3D space
	Fbox bb; bb.invalidate();
	for (it = tris.begin(); it!=tris.end(); it++)
	{
		UVtri	*T  = it;
		Face	*F  = T->owner;
		UVtri	R;

		for (int i=0; i<3; i++) 
		{
			R.uv[i].u = (T->uv[i].u-min.u)/size.u; 
			R.uv[i].v = (T->uv[i].v-min.v)/size.v; 
			bb.modify(F->v[i]->P);
		}
		*T = R;
		T->owner = F;
	}
	bb.getsphere(Center,Radius);

//	lm.dwWidth	+= 2*BORDER;	// border
//	lm.dwHeight	+= 2*BORDER;	// border
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

VOID CDeflector::Capture		(CDeflector *D, int b_u, int b_v, int s_u, int s_v, BOOL bRotated)
{
	UVpoint half;	half.set	(.5f/512.f, .5f/512.f);
	UVpoint guard;	guard.set	(BORDER/512.f, BORDER/512.f);
	UVpoint scale;  scale.set	(float(s_u-2*BORDER)/512.f, float(s_v-2*BORDER)/512.f);	// take in mind border
	UVpoint base;	base.set	(float(b_u+BORDER)/512.f, float(b_v+BORDER)/512.f);					// offset in UV space
	
	if (!bRotated) 
	{
		// Normal
		for (UVIt T=D->tris.begin(); T!=D->tris.end(); T++)
		{
			UVtri	P;
			Face	*F = T->owner;
			
			P.uv[0].set(T->uv[0]); P.uv[0].mul(scale); P.uv[0].add(base); P.uv[0].add(guard); P.uv[0].add(half);
			P.uv[1].set(T->uv[1]); P.uv[1].mul(scale); P.uv[1].add(base); P.uv[1].add(guard); P.uv[1].add(half);
			P.uv[2].set(T->uv[2]); P.uv[2].mul(scale); P.uv[2].add(base); P.uv[2].add(guard); P.uv[2].add(half); 
			
			P.owner			= F;
			F->pDeflector	= this;
			F->AddChannel	(P.uv[0], P.uv[1], P.uv[2]);
			tris.push_back	(P);
		}
	} else {
		// Rotated
		for (UVIt T=D->tris.begin(); T!=D->tris.end(); T++)
		{
			UVtri	P;
			Face	*F = T->owner;
			
			P.uv[0].set(T->uv[0]); swap(P.uv[0].u,P.uv[0].v); P.uv[0].mul(scale); P.uv[0].add(base); P.uv[0].add(guard); P.uv[0].add(half);
			P.uv[1].set(T->uv[1]); swap(P.uv[1].u,P.uv[1].v); P.uv[1].mul(scale); P.uv[1].add(base); P.uv[1].add(guard); P.uv[1].add(half);
			P.uv[2].set(T->uv[2]); swap(P.uv[2].u,P.uv[2].v); P.uv[2].mul(scale); P.uv[2].add(base); P.uv[2].add(guard); P.uv[2].add(half); 
			
			P.owner			= F;
			F->pDeflector	= this;
			F->AddChannel	(P.uv[0], P.uv[1], P.uv[2]);
			tris.push_back	(P);
		}
	}
}
