// LightShadows.h: interface for the CLightShadows class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTSHADOWS_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_)
#define AFX_LIGHTSHADOWS_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_
#pragma once

#include "scenegraph.h"

class ENGINE_API	CObject;
class ENGINE_API	xrLIGHT;

class ENGINE_API	CLightShadows  : public pureDeviceCreate, public pureDeviceDestroy
{
	friend class pred_casters;
private:
	//
	typedef	SceneGraph::mapMatrixItem::TNode	NODE;		
	struct	caster
	{
		CObject*			O;
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
	CObject*				current;
	vector<caster>			casters;
	vector<shadow>			shadows;
	vector<tess_tri>		tess;

	CRT*					RT;
	CRT*					RT_temp;
	Shader*					sh_Texture;
	Shader*					sh_BlurTR;
	Shader*					sh_BlurRT;
	CVS*					vs_Blur;
	Shader*					sh_World;
	CVS*					vs_World;
	Shader*					sh_Screen;
	CVS*					vs_Screen;
private:
public:
	void					set_object		(CObject*	O);
	void					add_element		(NODE*		N);
	void					calculate		();
	void					render			();

	virtual	void			OnDeviceCreate	();
	virtual	void			OnDeviceDestroy	();
	
	CLightShadows			();
	~CLightShadows			();
};

#endif // !defined(AFX_LIGHTSHADOWS_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_)
