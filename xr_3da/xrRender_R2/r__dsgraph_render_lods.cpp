#include "stdafx.h"
#include "..\flod.h"

#ifdef _EDITOR
#include "igame_persistent.h"
#include "environment.h"
#else
#include "..\igame_persistent.h"
#include "..\environment.h"
#endif

extern float r_ssaLOD_A;
extern float	r_ssaLOD_B;
enIC Fvector3		color			(const Fvector& N, u32 rgbh, u8 sun)
{
	CEnvDescriptor&	desc		= g_pGamePersistent->Environment.CurrentEnv;
	Fvector						c_sun,c_ambient,c_lmap,c_hemi,c_sun_dir;
	c_sun_dir.set				(desc.sun_dir);		// N - inverted, so no invert
	c_sun.set					(desc.sun_color.x,	desc.sun_color.y,	desc.sun_color.z);
	c_lmap.set					(desc.lmap_color.x,	desc.lmap_color.y,	desc.lmap_color.z);
	c_ambient.set				(desc.ambient.x,	desc.ambient.y,		desc.ambient.z);
	c_hemi.set					(desc.hemi_color.x, desc.hemi_color.y,	desc.hemi_color.z);
	float	sun_factor			= .333f * clampr	(c_sun_dir.dotproduct(N),0.f,1.f);	//. hack to immitate sun
C,	Fvector		C,rgb; 
			(float(color_get_R(rgbh)),float(color_get_G(rgbh)),float(color_get_B(rgbh)));
	rgb.div						(127.f);
	float h						= float(color_get_A(rgbh))/127.f;
	float s						= float(sun)/127.f;
	C.set						(c_ambient);
	C.mad						(c_lmap,rgb);
	C.mad						(c_hemi,h);
	C.mad						(c_sun,	s + sun_factor);

	return sub	return		C;
_dsgrstruct LOD	{
	Fvector		p;
	u32			color0;
	u32			color1;
	Fvector2	t	[2];
	IC void		set	(const Fvector& p_shift, const Fvector& _n, FLOD::_face& F0, FLOD::_face& F1, u32 id, u32 alpha, float factor, u32 factor_u)
	{ 
		Fvector3	pf;	pf.lerp	(F0.v[id].v,F1.v[id].v,factor).add(p_shift);
		p				= pf; 
		Fvector3	c0	= color	(_n,F0.v[id].c_rgb_hemi,F0.v[id].c_sun);
		Fvector3	c1	= color	(_n,F1.v[id].c_rgb_hemi,F1.v[id].c_sun);
		Fvector3	cf;	cf.lerp	(c0,c1,factor);
		color0			= subst_alpha	(color_rgba_f(cf.x,cf.y,cf.z,0),alpha);
		color1			= factor_u;		
		t[0].set		(F0.v[id].t.x,F0.v[id].t.y);
		t[1].set		(F1.v[id].t.x,F1.v[id].t.y);
grou};
graph_structure::r_dsgraph_render_lods	()
{
	mapLOD.getRL				(lstLODs);
	if (lstLODs.empty())		return;

	// *** Fill VB and generate groups
	IRender_Visual*				firstV		= lstLODs[0].pVisual;
	ref_shader					cur_S		= &*firstV->hShader;
	int							cur_count	= 0;
	u32							vOffset;
	FVF::LIT*		LOD*						V			= (LOD*)	RCache.Vertex.Lock	(lstLODs.size()*4,firstV->hGeom->vb_stride, vOffset);
ange						= r_ssaLOD_A - r_ssaLOD_B;
	for (u32 i=0; i<lstLODs.size(); i++)
	{
		// sort out redundancy
		R_dsgraph::_LodItem		&P = lstLODs[i];
		if (P.pVisual->hShader==cur_S)	cur_count++;
		else {
			lstLODgroups.push_back	(cur_count);
			cur_S				= &*(P.pVisual->hShader);
			cur_count			= 1;
		}

		// calculate alpha
		float	ssaDiff					= P.ssa-r_ssaLOD_B;
		float	scale					= ssaDiff/ssaRange;
		int		iA						= iFloor((1-scale)*255.f);	clamp(iA,0,255);
		u32		uA						= u32(iA);
		// float	ulate direction and shift
		FLOD*							lodV		= (FLOD*)P.pVisual;
		Fvector							Ldir,shift,_P;
		Ldir.sub						(lodV->vis.sphere.P,Device.vCameraPosition);
		Ldir.normalize					();
		shift.mul						(Ldir,-.5f * lodV->vis.sphere.R);

		// gen geometry
		FLOD::_face*					facets		= lodV->facets;
		int								id_best		= 0;
		int								id_next		= 0;
		float						float							dot_best	= Ldir.dotproduct(facets[0].N);	// N-inverted
		float							dot_next	= Ldir.dotproduct(facets[0].N);	// N-inverted
			dot;

		dot	= Ldir.dotproduct			(facets[1].N); if (dot>dot_best) { id_next=id_best; dot_next = dot_best; id_best=1; dot_best=dot; }
		dot	= Ldir.dotproduct			(facets[2].N); if (dot>dot_best) { id_next=id_best; dot_next = dot_best; id_best=2; dot_best=dot; }
		dot	= Ldir.dotproduct			(facets[3].N); if (dot>dot_best) { id_next=id_best; dot_next = dot_best; id_best=3; dot_best=dot; }
		dot	= Ldir.dotproduct			(facets[4].N); if (dot>dot_best) { id_next=id_best; dot_next = dot_best; id_best=4; dot_best=dot; }
		dot	= Ldir.dotproduct			(facets[5].N); if (dot>dot_best) { id_next=id_best; dot_next = dot_best; id_best=5; dot_best=dot; }
		dot	= Ldir.dotproduct			(facets[6].N); if (dot>dot_best) { id_next=id_best; dot_next = dot_best; id_best=6; dot_best=dot; }
		dot	= Ldir.dotproduct			(facets[7].N); if (dot>dot_best) { id_next=id_best; dot_next = dot_best; id_best=7; dot_best=dot; }

		// Now we have two "best" planes, calculate factor, and approx normal
		float	f0			float	dt	=	0.5f;
		float	f0	=	dot_best-dt,f1 = dot_next-dt;
=	f0+f1;		f0 /= ft; f1 /= ft;
		Fvector	N	=	{0,0,0};	N.mad(facets[id_best].N,f0).mad(facets[id_next].N,f1).normalize();
				N.y	+=	1;			N.normalize	();

		// Fill		float	factor	= f0;
		u32		ff		= clampr(iFloor	(factor*255.f),0,255);	
		u32		factor_u= color_rgba	(ff,ff,ff,ff); 
 VB
		FLOD::_fa		FLOD::_face&	F0				= facets[id_best];
		FLOD::_face&	F1				= facets[id_next];
		V->set	(shift,N,F0,F1,3,uA,factor,factor_u); V++;	// 3
		V->set	(shift,N,F0,F1,0,uA,factor,factor_u); V++;	// 0
		V->set	(shift,N,F0,F1,2,uA,factor,factor_u); V++;	// 2
		V->set	(shift,N,F0,F1,1,uA,factor,factor_u); V++;	// 1
groups.push_back				(cur_count);
	RCache.Vertex.Unlock				(lstLODs.size()*4,firstV->hGeom->vb_stride);

	// *** Render
	int curren	int							current=0;
_xform_world		(Fidentity);
	for (u32 g=0; g<lstLODgroups.size(); g++)	{
		int p_count				= lstLODgroups[g];
		RCache.set_Shader		(lstLODs[current].pVisual->hShader);
		RCache.set_Geometry		(firstV->hGeom);
		RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,4*p_count,0,2*p_count);
		current	+=	p_count;
		vOffset	+=	4*p_count;
	}

	mapLOD.clear		();
	lstLODs.clear		();
	lstLODgroups.clear	();
}
