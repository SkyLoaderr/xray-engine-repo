// LightShadows.h: interface for the CLightShadows class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTSHADOWS_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_)
#define AFX_LIGHTSHADOWS_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_
#pragma once

#include "light.h"
#include "fstaticrender_scenegraph.h"

class	CLightShadows			
{
	friend class pred_casters;
private:
	//
	typedef	R_dsgraph::_MatrixItem		NODE;		
	struct	caster
	{
		IRenderable*		O;
		Fvector				C;
		float				D;
		svector<NODE,16>	nodes;
	};
	struct	shadow 
	{
		int					slot;
		Fvector				C;
		Fmatrix				M;
		light*				L;
	};
	struct	tess_tri
	{
		Fvector				v[3];
		Fvector				N;
	};
private:
	IRenderable*			current;
	xr_vector<caster*>		casters_pool;
	xr_vector<caster*>		casters;
	xr_vector<shadow>		shadows;
	xr_vector<tess_tri>		tess;
	xrXRC					xrc;

	ref_rt					RT;
	ref_rt					RT_temp;
	ref_shader				sh_Texture;
	ref_shader				sh_BlurTR;
	ref_shader				sh_BlurRT;
	ref_geom				geom_Blur;
	ref_shader				sh_World;
	ref_geom				geom_World;
	ref_shader				sh_Screen;
	ref_geom				geom_Screen;
private:
public:
	void					set_object		(IRenderable*	O);
	void					add_element		(NODE*			N);
	void					calculate		();
	void					render			();

	CLightShadows			();
	~CLightShadows			();
};

#endif // !defined(AFX_LIGHTSHADOWS_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_)
