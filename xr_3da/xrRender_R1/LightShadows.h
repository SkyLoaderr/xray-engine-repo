// LightShadows.h: interface for the CLightShadows class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTSHADOWS_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_)
#define AFX_LIGHTSHADOWS_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_
#pragma once

#include "fstaticrender_scenegraph.h"

class	ENGINE_API	CObject;
struct	ENGINE_API	Flight;

class CLightShadows			: public pureDeviceCreate, public pureDeviceDestroy
{
	friend class pred_casters;
private:
	//
	typedef	SceneGraph::mapMatrixItem::TNode	NODE;		
	struct	caster
	{
		IRenderable*		O;
		Fvector				C;
		float				D;
		svector<NODE,32>	nodes;
	};
	struct	shadow 
	{
		int					slot;
		Fvector				C;
		Fmatrix				M;
		Flight*				L;
	};
	struct	tess_tri
	{
		Fvector				v[3];
		Fvector				N;
	};
private:
	IRenderable*			current;
	xr_vector<caster>		casters;
	xr_vector<shadow>		shadows;
	xr_vector<tess_tri>		tess;

	CRT*					RT;
	CRT*					RT_temp;
	Shader*					sh_Texture;
	Shader*					sh_BlurTR;
	Shader*					sh_BlurRT;
	ref_geom				geom_Blur;
	Shader*					sh_World;
	ref_geom				geom_World;
	Shader*					sh_Screen;
	ref_geom				geom_Screen;
private:
public:
	void					set_object		(IRenderable*	O);
	void					add_element		(NODE*			N);
	void					calculate		();
	void					render			();

	virtual	void			OnDeviceCreate	();
	virtual	void			OnDeviceDestroy	();
	
	CLightShadows			();
	~CLightShadows			();
};

#endif // !defined(AFX_LIGHTSHADOWS_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_)
