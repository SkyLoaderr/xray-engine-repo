#pragma once

#ifndef BORDER
#define BORDER 1
#endif

#include "hash2D.h"
#include "cl_defs.h"

struct UVtri : public _TCF 
{
	Face*	owner;
};

#define LT_DIRECT		0
#define LT_POINT		1
#define LT_SECONDARY	2

struct R_Light
{
    DWORD           type;				// Type of light source		
    Fcolor          diffuse;			// Diffuse color of light	
    Fvector         position;			// Position in world space	
    Fvector         direction;			// Direction in world space	
    float		    range;				// Cutoff range
	float			range2;				// ^2
    float	        attenuation0;		// Constant attenuation		
    float	        attenuation1;		// Linear attenuation		
    float	        attenuation2;		// Quadratic attenuation	
	float			energy;				// For radiosity ONLY
	
	Fvector			tri[3];

	R_Light()		{
		tri[0].set	(0,0,0);
		tri[1].set	(0,0,EPS_S);
		tri[2].set	(EPS_S,0,0);
	}
};

typedef hash2D<UVtri*,128,128>	HASH;

class CDeflector
{
public:
	vector<UVtri>		UVpolys;
	Fvector				N;
	struct Layer
	{
		b_light*		base;
		b_texture		lm;
		
		DWORD			Area ()	{ return (lm.dwWidth+2*BORDER)*(lm.dwHeight+2*BORDER); }

		Layer()			{ ZeroMemory(this,sizeof(*this)); }
	};
	vector<Layer>		layers;

	Fsphere				Sphere;

	DWORD				dwWidth;
	DWORD				dwHeight;
	BOOL				bMerged;

	vector<R_Light>		LightsSelected;
	RAPID::XRCollide	DB;
public:
	CDeflector					();
	~CDeflector					();

	void	OA_SetNormal		(Fvector &_N )	{ N.set(_N); N.normalize(); }
	BOOL	OA_Place			(Face *owner);
	void	OA_Place			(vecFace& lst);
	void	OA_Export			();
		
	void	GetRect				(UVpoint &min, UVpoint &max);
	Layer*	GetLayer			(b_light* base);
	DWORD	GetFaceCount()		{ return UVpolys.size();	};

	VOID	Light				(HASH& H);
	VOID	L_Direct			(HASH& H);
	VOID	L_Direct_Edge		(UVpoint& p1, UVpoint& p2, Fvector& v1, Fvector& v2, Fvector& N, float texel_size);
	VOID	L_Calculate			(HASH& H);

	WORD	GetBaseMaterial		() { return UVpolys.front().owner->dwMaterial;	}

	void	Bounds				(DWORD ID, Fbox2& dest)
	{
		UVtri& TC		= UVpolys[ID];
		dest.min.set	(TC.uv[0].conv());
		dest.max.set	(TC.uv[0].conv());
		dest.modify		(TC.uv[1].conv());
		dest.modify		(TC.uv[2].conv());
	}
	void	Bounds_Summary		(Fbox2& bounds)
	{
		bounds.invalidate();
		for (DWORD I=0; I<UVpolys.size(); I++)
		{
			Fbox2	B;
			Bounds	(I,B);
			bounds.merge(B);
		}
	}
	void	RemapUV				(vector<UVtri>& dest, DWORD base_u, DWORD base_v, DWORD size_u, DWORD size_v, DWORD lm_u, DWORD lm_v, BOOL bRotate);
	void	RemapUV				(DWORD base_u, DWORD base_v, DWORD size_u, DWORD size_v, DWORD lm_u, DWORD lm_v, BOOL bRotate);
};

typedef vector<UVtri>::iterator UVIt;

extern void		Jitter_Select	(UVpoint* &Jitter, DWORD& Jcount);
extern void		blit			(LPDWORD dest, DWORD ds_x, DWORD ds_y, LPDWORD src, DWORD ss_x, DWORD ss_y, DWORD px, DWORD py, DWORD aREF);
extern void		blit_r			(LPDWORD dest, DWORD ds_x, DWORD ds_y, LPDWORD src, DWORD ss_x, DWORD ss_y, DWORD px, DWORD py, DWORD aREF);
