// GlowManager.h: interface for the CGlowManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOWMANAGER_H__EC35911F_479B_469A_845C_1A64D81D0326__INCLUDED_)
#define AFX_GLOWMANAGER_H__EC35911F_479B_469A_845C_1A64D81D0326__INCLUDED_
#pragma once

#include "..\ispatial.h"
#include "..\xr_collide_defs.h"

class CGlow				: public IRender_Glow, public ISpatial
{
public:
	struct {
		u32				bActive	:	1;
	}					flags;
	float				fade;
	ref_shader			shader;
	u32					dwFrame;

	Fvector				position;
	float				range;
	Fcolor				color;

	// Ray-testing cache
	BOOL				bTestResult;
	Collide::ray_cache	RayCache;
public:
	CGlow();

	virtual void					set_active			(bool);
	virtual bool					get_active			();
	virtual void					set_position		(const Fvector& P);
	virtual void					set_range			(float R);
	virtual void					set_texture			(LPCSTR name);
	virtual void					set_color			(const Fcolor& C);
	virtual void					set_color			(float r, float g, float b);
	virtual void					spatial_move		();
};

#define MAX_GlowsPerFrame	64

class CGlowManager 
{
	xr_vector<CGlow*>		Glows;
	xr_vector<CGlow*>		Selected;
	ref_geom				hGeom;

	u32						dwTestID;
public:
	void	add				(CGlow		*G);

	void	Load			(IReader	*fs);
	void	Unload			();

	void	Render			();

	CGlowManager			();
	~CGlowManager			();
};

#endif // !defined(AFX_GLOWMANAGER_H__EC35911F_479B_469A_845C_1A64D81D0326__INCLUDED_)
