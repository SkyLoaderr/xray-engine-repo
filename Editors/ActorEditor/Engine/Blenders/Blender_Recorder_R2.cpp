#include "stdafx.h"
#pragma hdrstop

#include "ResourceManager.h"
#include "blenders\Blender_Recorder.h"
#include "blenders\Blender.h"

void	CBlender_Compile::r_Pass		(LPCSTR _vs, LPCSTR _ps, BOOL bFog, BOOL bZtest, BOOL bZwrite,	BOOL bABlend, u32 abSRC, u32 abDST, BOOL aTest, u32 aRef)
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
	PassSET_LightFog		(FALSE,bFog);

	// Create shaders
	SPS* ps					= Device.Resources->_CreatePS			(_ps);
	SVS* vs					= Device.Resources->_CreateVS			(_vs);
	dest.ps					= ps;
	dest.vs					= vs;
	ctable.merge			(&ps->constants);
	ctable.merge			(&vs->constants);
	SetMapping				();
}

void	CBlender_Compile::r_Constant	(LPCSTR name, R_constant_setup* s)
{
	R_ASSERT				(s);
	R_constant*	C			= ctable.get(name);
	if (C)					C->handler	= s;
}

void	CBlender_Compile::r_Sampler		(LPCSTR _name, LPCSTR texture, bool b_ps1x_ProjectiveDivide, u32 address, u32 fmin, u32 fmip, u32 fmag, u32 element)
{
	//
	string256				name;
	strcpy					(name,_name);
	if (strext(name)) *strext(name)=0;

	// Find index
	R_constant*	C			= ctable.get(name);
	if (0==C)				return;
	R_ASSERT				(C->type == RC_sampler);
	u32 stage				= C->samp.index;
	R_ASSERT				(stage<16);

	// Create texture
	while (stage>=passTextures.size())	passTextures.push_back		(NULL);
	passTextures[stage]		= Device.Resources->_CreateTexture		(texture);

	// Sampler states
	RS.SetSAMP				(stage,D3DSAMP_ADDRESSU,	address);
	RS.SetSAMP				(stage,D3DSAMP_ADDRESSV,	address);
	RS.SetSAMP				(stage,D3DSAMP_MINFILTER,	fmin);
	RS.SetSAMP				(stage,D3DSAMP_MIPFILTER,	fmip);
	RS.SetSAMP				(stage,D3DSAMP_MAGFILTER,	fmag);
	RS.SetSAMP				(stage,D3DSAMP_ELEMENTINDEX,element);
	if	(b_ps1x_ProjectiveDivide)	RS.SetTSS	(stage,D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 | D3DTTFF_PROJECTED);
	else							RS.SetTSS	(stage,D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
}

void	CBlender_Compile::r_Sampler_rtf	(LPCSTR name, LPCSTR texture, bool b_ps1x_ProjectiveDivide, u32 element/* =0 */)
{
	r_Sampler	(name,texture,b_ps1x_ProjectiveDivide,D3DTADDRESS_CLAMP,D3DTEXF_POINT,D3DTEXF_NONE,D3DTEXF_POINT,element);
}
void	CBlender_Compile::r_Sampler_clf	(LPCSTR name, LPCSTR texture, bool b_ps1x_ProjectiveDivide, u32 element/* =0 */)
{
	r_Sampler	(name,texture,b_ps1x_ProjectiveDivide,D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR,element);
}

void	CBlender_Compile::r_End			()
{
	dest.constants			= Device.Resources->_CreateConstantTable(ctable);
	dest.state				= Device.Resources->_CreateState		(RS.GetContainer());
	dest.T					= Device.Resources->_CreateTextureList	(passTextures);
	dest.M					= 0;
	dest.C					= 0;
	SH->Passes.push_back	(Device.Resources->_CreatePass(dest.state,dest.ps,dest.vs,dest.constants,dest.T,dest.M,dest.C));
}
