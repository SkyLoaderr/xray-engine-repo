#include "stdafx.h"
#include "resource.h"
#include "build.h"
#include "xrIsect.h"
#include "math.h"

#define BORDER 2

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

int		GetMapSize(float pix)
{
	pix *= g_params.m_lm_pixels_per_meter;

	if (pix<2)		return 2;
	if (pix<4)		return 4;		// smallest lightmap 4x4
	if (pix<8)		return 8;
	if (pix<16)		return 16;
	if (pix<32)		return 32;
	if (pix<64)		return 64;
	if (pix<128)	return 128;
	if (pix<256)	return 256;
	return 512;						// largest - 512x512
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
	lm.dwWidth  = iCeil(size.u*g_params.m_lm_pixels_per_meter*density); clamp(lm.dwWidth, 2ul,510ul);
	lm.dwHeight = iCeil(size.v*g_params.m_lm_pixels_per_meter*density); clamp(lm.dwHeight,2ul,510ul);
	lm.bHasAlpha= FALSE;
	iArea		= lm.dwWidth*lm.dwHeight;

	// Setup variables
	UVpoint		dim, guard, scale;
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

VOID CDeflector::Capture(CDeflector *D,UVpoint &fBase,UVpoint &fScale)
{
	UVpoint half; half.set(.5f/512.f, .5f/512.f);
	for (UVIt T=D->tris.begin(); T!=D->tris.end(); T++)
	{
		UVtri	P;
		Face	*F = T->owner;

		P.uv[0].set(T->uv[0]); P.uv[0].mul(fScale); P.uv[0].add(fBase); P.uv[0].add(half);
		P.uv[1].set(T->uv[1]); P.uv[1].mul(fScale); P.uv[1].add(fBase); P.uv[1].add(half);
		P.uv[2].set(T->uv[2]); P.uv[2].mul(fScale); P.uv[2].add(fBase); P.uv[2].add(half); 

		P.owner			= F;
		F->pDeflector	= this;
		F->AddChannel	(P.uv[0], P.uv[1], P.uv[2]);
		tris.push_back	(P);
	}
}
