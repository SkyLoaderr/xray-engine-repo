#include "stdafx.h"
#pragma hdrstop

#include "blenders\Blender_Recorder.h"
#include "blenders\Blender.h"

void	CBlender_Compile::r2_Pass		(LPCSTR _vs, LPCSTR _ps, BOOL bZtest, BOOL bZwrite,	BOOL bABlend, u32 abSRC, u32 abDST, BOOL aTest, u32 aRef)
{
	RS.Invalidate			();
	passTextures.clear		();
	dwStage					= 0;

	// Setup FF-units (Z-buffer, blender)
	PassSET_ZB				(bZtest,bZwrite);
	PassSET_Blend			(bABlend,abSRC,abDST,aTest,aRef);

	// Create shaders
	SPS* ps					= Device.Shader._CreatePS			(_ps);
	SVS* vs					= Device.Shader._CreateVS			(_vs);
	dest.ps					= ps->ps;
	dest.vs					= vs->vs;
	dest.constants			= Device.Shader._CreateConstantTable(&ps->constants,&vs->constants);
}

void	CBlender_Compile::r2_Sampler	(LPCSTR name, LPCSTR texture, u32 address, u32 fmin, u32 fmip, u32 fmag, u32 element)
{
	// Find index
	R_constant*	C			= dest.constants->get(name);
	R_ASSERT				(C);
	R_ASSERT				(C->type == RC_sampler);
	u32 stage				= C->samp.index;

	// Create texture
	while (stage>=passTextures.size())	passTextures.push_back(NULL);
	passTextures[stage]		= Device.Shader._CreateTexture(texture);

	// Sampler states
	RS.SetSAMP				(stage,D3DSAMP_ADDRESSU,	address);
	RS.SetSAMP				(stage,D3DSAMP_ADDRESSV,	address);
	RS.SetSAMP				(stage,D3DSAMP_MINFILTER,	fmin);
	RS.SetSAMP				(stage,D3DSAMP_MIPFILTER,	fmip);
	RS.SetSAMP				(stage,D3DSAMP_MAGFILTER,	fmag);
	RS.SetSAMP				(stage,D3DSAMP_ELEMENTINDEX,element);
}

void	CBlender_Compile::r2_End		()
{
	dest.state				= Device.Shader._CreateState		(RS.GetContainer());
	dest.T					= Device.Shader._CreateTextureList	(passTextures);
	dest.M					= 0;
	dest.C					= 0;
	SH->Passes.push_back	(Device.Shader._CreatePass(dest));
}
