// LightShadows.h: interface for the CLightShadows class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTSHADOWS_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_)
#define AFX_LIGHTSHADOWS_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_
#pragma once

#include "scenegraph.h"

class ENGINE_API	CObject;
class ENGINE_API	xrLIGHT;

class ENGINE_API	CLightShadows  
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
		svector<NODE*,32>	nodes;
	};
private:
	CObject*				current;
	vector<caster>			casters;
	vector<int>				id;
	vector<xrLIGHT*>		lights;
	CRT*					
private:
public:
	void					set_object	(CObject*	O);
	void					add_element	(NODE*		N);
	void					calculate	();
	void					render		();

	CLightShadows			();
	~CLightShadows			();
};

#endif // !defined(AFX_LIGHTSHADOWS_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_)
