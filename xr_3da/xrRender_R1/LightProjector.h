// LightShadows.h: interface for the CLightShadows class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTPRJ_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_)
#define AFX_LIGHTPRJ_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_
#pragma once

#include "fstaticrender_scenegraph.h"

class CLightProjector		: public pureDeviceCreate, public pureDeviceDestroy
{
	friend class			pred_sorter;
private:
	//
	typedef	SceneGraph::mapMatrixItem::TNode	NODE;		
	struct	recv
	{
		IRenderable*		O;
		Fvector				C;
		float				D;
		Fmatrix				UVgen;
	};
private:
	IRenderable*			current;
	xr_vector<recv>			receivers;

	CRT*					RT;
	CRT*					RT_temp;
	Shader*					sh_BlurTR;
	Shader*					sh_BlurRT;
	SGeometry*				geom_Blur;
	Shader*					sh_Screen;
	SGeometry*				geom_Screen;
public:
	void					set_object		(IRenderable*	O);
	BOOL					shadowing		()			{ return current!=0; }
	void					calculate		();
	void					setup			(int slot);
	void					finalize		()			{ receivers.clear(); }

	void					render			();

	virtual	void			OnDeviceCreate	();
	virtual	void			OnDeviceDestroy	();
	
	CLightProjector			();
	~CLightProjector		();
};

#endif // !defined(AFX_LIGHTPRJ_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_)
