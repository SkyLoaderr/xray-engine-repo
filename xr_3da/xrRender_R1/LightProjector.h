// LightShadows.h: interface for the CLightShadows class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTPRJ_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_)
#define AFX_LIGHTPRJ_H__CFA216D9_CACB_4515_9FBE_7C531649168F__INCLUDED_
#pragma once

#include "r__dsgraph_types.h"

class CLightProjector
{
	friend class			pred_sorter;
private:
	//
	typedef	R_dsgraph::_MatrixItem	NODE;		
	struct	recv
	{
		IRenderable*		O;
		Fvector				C;
		float				D;
		Fmatrix				UVgen;
		Fvector				UVclamp_min;
		Fvector				UVclamp_max;
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

	ref_str					c_xform;
	ref_str					c_clamp;
	ref_str					c_factor;
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
