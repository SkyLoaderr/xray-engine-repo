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
}

void	CBlender_Compile::r2_End		()
{
	dest.state				= Device.Shader._CreateState		(RS.GetContainer());
	dest.T					= Device.Shader._CreateTextureList	(passTextures);
	dest.M					= 0;
	dest.C					= 0;
	SH->Passes.push_back	(Device.Shader._CreatePass(dest));
}
