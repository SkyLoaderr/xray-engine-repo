// LightShadows.h: interface for the CLightShadows class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTPRJ_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_)
#define AFX_LIGHTPRJ_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_
#pragma once

#include "fstaticrender_scenegraph.h"

class CLightProjector
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
		Fvector4			UVclamp;
	};
private:
	IRenderable*			current;
	xr_vector<recv>			receivers;

	ref_rt					RT;
	ref_rt					RT_temp;
	ref_shader				sh_BlurTR;
	ref_shader				sh_BlurRT;
	ref_geom				geom_Blur;
	ref_shader				sh_Screen;
	ref_geom				geom_Screen;
public:
	void					set_object		(IRenderable*	O);
	BOOL					shadowing		()			{ return current!=0; }
	void					calculate		();
	void					setup			(int slot);
	void					finalize		()			{ receivers.clear(); }

	void					render			();

	CLightProjector			();
	~CLightProjector		();
};

#endif // !defined(AFX_LIGHTPRJ_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_)
