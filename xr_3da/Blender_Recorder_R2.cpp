#include "stdafx.h"
#pragma hdrstop

#include "Blender_Recorder.h"
#include "Blender.h"

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
void	CBlender_Compile::r2_Sampler	(LPCSTR name,	LPCSTR texture, u32		address=D3DTADDRESS_WRAP,	u32 fmin=D3DTEXF_LINEAR,	u32 fmip=D3DTEXF_LINEAR,	u32 fmag=D3DTEXF_LINEAR,	u32 element=0)
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
