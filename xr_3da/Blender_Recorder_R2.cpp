#include "stdafx.h"
#pragma hdrstop

#include "blenders\Blender_Recorder.h"
#include "blenders\Blender.h"

#define	BIND_DECLARE(xf)	\
	class cl_xform_##xf	: public R_constant_setup {	virtual void setup (R_constant* C) { RCache.xforms.set_c_##xf (C); } }; \
	static cl_xform_##xf	binder_##xf

BIND_DECLARE(w);
BIND_DECLARE(v);
BIND_DECLARE(p);
BIND_DECLARE(wv);
BIND_DECLARE(vp);
BIND_DECLARE(wvp);

void	CBlender_Compile::SetMapping	()
{
	// Standart constant-binding
	r2_Constant				("m_W",		&binder_w);
	r2_Constant				("m_V",		&binder_v);
	r2_Constant				("m_P",		&binder_p);
	r2_Constant				("m_WV",	&binder_wv);
	r2_Constant				("m_VP",	&binder_vp);
	r2_Constant				("m_WVP",	&binder_wvp);
}

void	CBlender_Compile::r2_Pass		(LPCSTR _vs, LPCSTR _ps, BOOL bZtest, BOOL bZwrite,	BOOL bABlend, u32 abSRC, u32 abDST, BOOL aTest, u32 aRef)
{
	RS.Invalidate			();
	ctable.clear			();
	passTextures.clear		();
	passMatrices.clear		();
	passConstants.clear		();
	dwStage					= 0;

	// Setup FF-units (Z-buffer, blender)
	PassSET_ZB				(bZtest,bZwrite);
	PassSET_Blend			(bABlend,abSRC,abDST,aTest,aRef);

	// Create shaders
	SPS* ps					= Device.Shader._CreatePS			(_ps);
	SVS* vs					= Device.Shader._CreateVS			(_vs);
	dest.ps					= ps->ps;
	dest.vs					= vs->vs;
	ctable.merge			(&ps->constants);
	ctable.merge			(&vs->constants);
	SetMapping				();
}

void	CBlender_Compile::r2_Constant	(LPCSTR name, R_constant_setup* s)
{
	R_ASSERT				(s);
	R_constant*	C			= ctable.get(name);
	if (C)					C->handler	= s;
}

void	CBlender_Compile::r2_Sampler	(LPCSTR name, LPCSTR texture, u32 address, u32 fmin, u32 fmip, u32 fmag, u32 element)
{
	// Find index
	R_constant*	C			= ctable.get(name);
	if (0==C)				return;
	R_ASSERT				(C->type == RC_sampler);
	u32 stage				= C->samp.index;
	R_ASSERT				(stage<16);

	// Create texture
	while (stage>=passTextures.size())	passTextures.push_back	(NULL);
	if (passTextures[stage])	Device.Shader._DeleteTexture	(passTextures[stage]);
	passTextures[stage]			= Device.Shader._CreateTexture	(texture);

	// Sampler states
	RS.SetSAMP				(stage,D3DSAMP_ADDRESSU,	address);
	RS.SetSAMP				(stage,D3DSAMP_ADDRESSV,	address);
	RS.SetSAMP				(stage,D3DSAMP_MINFILTER,	fmin);
	RS.SetSAMP				(stage,D3DSAMP_MIPFILTER,	fmip);
	RS.SetSAMP				(stage,D3DSAMP_MAGFILTER,	fmag);
	RS.SetSAMP				(stage,D3DSAMP_ELEMENTINDEX,element);
}

void	CBlender_Compile::r2_Sampler_rtf(LPCSTR name, LPCSTR texture, u32 element/* =0 */)
{
	r2_Sampler	(name,texture,D3DTADDRESS_CLAMP,D3DTEXF_POINT,D3DTEXF_NONE,D3DTEXF_POINT,element);
}

void	CBlender_Compile::r2_End		()
{
	dest.constants			= Device.Shader._CreateConstantTable(ctable);
	dest.state				= Device.Shader._CreateState		(RS.GetContainer());
	dest.T					= Device.Shader._CreateTextureList	(passTextures);
	dest.M					= 0;
	dest.C					= 0;
	SH->Passes.push_back	(Device.Shader._CreatePass(dest));
}
