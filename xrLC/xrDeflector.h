#pragma once

#ifndef BORDER
#define BORDER 1
#endif

#include "hash2D.h"

struct UVtri : public _TCF 
{
	Face*	owner;
};

typedef hash2D<UVtri*,128,128>	HASH;

class CDeflector
{
public:
	xr_vector<UVtri>	UVpolys;
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
	u32		GetFaceCount()		{ return (u32)UVpolys.size();	};
		
	VOID	Light				(CDB::COLLIDER* DB, base_lighting* LightsSelected, HASH& H	);
	VOID	L_Direct			(CDB::COLLIDER* DB, base_lighting* LightsSelected, HASH& H  );
	VOID	L_Direct_Edge		(CDB::COLLIDER* DB, base_lighting* LightsSelected, Fvector2& p1, Fvector2& p2, Fvector& v1, Fvector& v2, Fvector& N, float texel_size, Face* skip);
	VOID	L_Calculate			(CDB::COLLIDER* DB, base_lighting* LightsSelected, HASH& H  );

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
extern void		LightPoint		(CDB::COLLIDER* DB, CDB::MODEL* MDL, base_color &C, Fvector &P, Fvector &N, base_lighting& lights, u32 flags, Face* skip);

enum
{
	LP_DEFAULT			= 0,
	LP_UseFaceDisable	= (1<<0),
	LP_dont_rgb			= (1<<1),
	LP_dont_hemi		= (1<<2),
	LP_dont_sun			= (1<<3),
};
