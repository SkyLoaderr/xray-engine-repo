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
		DWORD				C;
		Fmatrix				M;
		xrLIGHT*			L;
	};
private:
	CObject*				current;
	vector<caster>			casters;
	vector<shadow>			shadows;
	vector<int>				id;
	vector<xrLIGHT*>		lights;

	CRT*					RT;
	CRT*					RT_temp;
	Shader*					sh_Texture;
	Shader*					sh_Blur;
	Shader*					sh_World;
	Shader*					sh_Screen;
	CVertexStream*			vs_World;
	CVertexStream*			vs_Screen;
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
