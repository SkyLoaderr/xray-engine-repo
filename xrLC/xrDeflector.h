#pragma once

#ifndef BORDER
#define BORDER 1
#endif

#include "hash2D.h"

struct UVtri : public _TCF 
{
	Face*	owner;
};

#define LT_DIRECT		0
#define LT_POINT		1
#define LT_SECONDARY	2

struct R_Light
{
    u32           type;				// Type of light source		
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
typedef xr_vector<R_Light>			LSelection;

class CDeflector
{
public:
	xr_vector<UVtri>		UVpolys;
	Fvector				N;
	struct Layer
	{
		int				base_id;
		b_texture		lm;
		
		u32				Area ()	{ return (lm.dwWidth+2*BORDER)*(lm.dwHeight+2*BORDER); }
		
		Layer()			{ ZeroMemory(this,sizeof(*this)); }
	};
	xr_vector<Layer>		layers;
	
	Fsphere				Sphere;
	
	u32				dwWidth;
	u32				dwHeight;
	BOOL				bMerged;
public:
	CDeflector					();
	~CDeflector					();
		
	void	OA_SetNormal		(Fvector &_N )	{ N.set(_N); N.normalize(); }
	BOOL	OA_Place			(Face *owner);
	void	OA_Place			(vecFace& lst);
	void	OA_Export			();
		
	void	GetRect				(Fvector2 &min, Fvector2 &max);
	Layer*	GetLayer			(int base_id);
	u32		GetFaceCount()		{ return UVpolys.size();	};
		
	VOID	Light				(CDB::COLLIDER* DB, LSelection* LightsSelected, HASH& H	);
	VOID	L_Direct			(CDB::COLLIDER* DB, LSelection* LightsSelected, HASH& H  );
	VOID	L_Direct_Edge		(CDB::COLLIDER* DB, LSelection* LightsSelected, Fvector2& p1, Fvector2& p2, Fvector& v1, Fvector& v2, Fvector& N, float texel_size, Face* skip);
	VOID	L_Calculate			(CDB::COLLIDER* DB, LSelection* LightsSelected, HASH& H  );

	WORD	GetBaseMaterial		() { return UVpolys.front().owner->dwMaterial;	}

	void	Bounds				(u32 ID, Fbox2& dest)
	{
		UVtri& TC		= UVpolys[ID];
		dest.min.set	(TC.uv[0]);
		dest.max.set	(TC.uv[0]);
		dest.modify		(TC.uv[1]);
		dest.modify		(TC.uv[2]);
	}
	void	Bounds_Summary		(Fbox2& bounds)
	{
		bounds.invalidate();
		for (u32 I=0; I<UVpolys.size(); I++)
		{
			Fbox2	B;
			Bounds	(I,B);
			bounds.merge(B);
		}
	}
	void	RemapUV				(xr_vector<UVtri>& dest, u32 base_u, u32 base_v, u32 size_u, u32 size_v, u32 lm_u, u32 lm_v, BOOL bRotate);
	void	RemapUV				(u32 base_u, u32 base_v, u32 size_u, u32 size_v, u32 lm_u, u32 lm_v, BOOL bRotate);
};

typedef xr_vector<UVtri>::iterator UVIt;

extern void		Jitter_Select	(Fvector2* &Jitter, u32& Jcount);
extern void		blit			(u32* dest, u32 ds_x, u32 ds_y, u32* src, u32 ss_x, u32 ss_y, u32 px, u32 py, u32 aREF);
extern void		blit_r			(u32* dest, u32 ds_x, u32 ds_y, u32* src, u32 ss_x, u32 ss_y, u32 px, u32 py, u32 aREF);
extern void		LightPoint		(CDB::COLLIDER* DB, CDB::MODEL* MDL, Fcolor &C, Fvector &P, Fvector &N, R_Light* begin, R_Light* end, Face* skip, BOOL bUseFaceDisable=FALSE);
