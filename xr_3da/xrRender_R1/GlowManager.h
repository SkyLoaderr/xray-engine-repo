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
	}					flags		;
	float				fade		;
	ref_shader			shader		;
	u32					dwFrame		;

	Fvector				position	;
	Fvector				direction	;
	float				radius		;
	Fcolor				color		;

	// Ray-testing cache
	BOOL				bTestResult	;
	collide::ray_cache	RayCache	;
	u32					qid_pass	;
	u32					qid_total	;
public:
	CGlow();

	virtual void					set_active			(bool);
	virtual bool					get_active			();
	virtual void					set_position		(const Fvector& P);
	virtual void					set_direction		(const Fvector& P);
	virtual void					set_radius			(float			R);
	virtual void					set_texture			(LPCSTR			name);
	virtual void					set_color			(const Fcolor&	C);
	virtual void					set_color			(float r, float g, float b);
	virtual void					spatial_move		();
};

#define MAX_GlowsPerFrame	64

class CGlowManager 
{
	xr_vector<CGlow*>		Glows			;
	xr_vector<CGlow*>		Selected		;
	xr_vector<CGlow*>		SelectedToTest	;
	ref_geom				hGeom			;

	BOOL					b_hardware		;
	u32						dwTestID		;
public:
	void	add				(CGlow		*G);

	void	Load			(IReader	*fs);
	void	Unload			();

	void	render_sw		();
	void	render_hw		();
	void	Render			();

	CGlowManager			();
	~CGlowManager			();
};

#endif // !defined(AFX_GLOWMANAGER_H__EC35911F_479B_469A_845C_1A64D81D0326__INCLUDED_)
