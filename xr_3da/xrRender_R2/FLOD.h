#pragma once
#include "../fhierrarhyvisual.h"

class	FLOD	:	public FHierrarhyVisual
{
	typedef FHierrarhyVisual inherited;
public:
	struct _vertex
	{
		Fvector		v;
		Fvector2	t;
		u32			c_rgb_hemi;	// rgb,hemi
		u8			c_sun;
	};
	struct _face
	{
		_vertex		v[4]	;
		Fvector		N		;
	};
	struct _hw 
	{
		Fvector		p		;
		Fvector		n		;
		u32			color	;
		u32			factor	;
		Fvector2	t0		;
		Fvector2	t1		;
		IC void		set		(const Fvector& _p, const Fvector& _n, u32 C, u32 F, float u1, float v1, float u2, float v2) { p.set(_p); n.set(_n); color=C; factor=F; t0.set(u1,v1); t1.set(u2,v2); }
	};
	static const u32	F_HW	= D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX2;

	ref_geom		geom		;
	_face			facets		[8];
	float			lod_factor	;
public:
	virtual void Render			(float LOD		);									// LOD - Level Of Detail  [0.0f - min, 1.0f - max], Ignored
	virtual void Load			(LPCSTR N, IReader *data, u32 dwFlags);
	virtual void Copy			(IRender_Visual *pFrom	);
};
