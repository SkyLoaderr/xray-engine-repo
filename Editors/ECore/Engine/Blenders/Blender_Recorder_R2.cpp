#include "stdafx.h"
#pragma hdrstop

#include "ResourceManager.h"
#include "blenders\Blender_Recorder.h"
#include "blenders\Blender.h"

void	CBlender_Compile::r_Pass		(LPCSTR _vs, LPCSTR _ps, bool bFog, BOOL bZtest, BOOL bZwrite,	BOOL bABlend, D3DBLEND abSRC, D3DBLEND abDST, BOOL aTest, u32 aRef)
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

	// Last Stage - disable
	if (0==stricmp(_ps,"null"))	{
		RS.SetTSS				(0,D3DTSS_COLOROP,D3DTOP_DISABLE);
		RS.SetTSS				(0,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
	}
}

void	CBlender_Compile::r_Constant	(LPCSTR name, R_constant_setup* s)
{
	R_ASSERT				(s);
	ref_constant C			= ctable.get(name);
	if (C)					C->handler	= s;
}

u32		CBlender_Compile::i_Sampler		(LPCSTR _name)
{
	//
	string256				name;
	strcpy					(name,_name);
	if (strext(name)) *strext(name)=0;

	// Find index
	ref_constant C			= ctable.get(name);
	if (!C)					return	u32(-1);
	R_ASSERT				(C->type == RC_sampler);
	u32 stage				= C->samp.index;

	// Create texture
	// while (stage>=passTextures.size())	passTextures.push_back		(NULL);
	return					stage;
}
void	CBlender_Compile::i_Texture		(u32 s, LPCSTR name)
{
	if (name)	passTextures.push_back	(mk_pair(s, ref_texture(Device.Resources->_CreateTexture(name))));
}
void	CBlender_Compile::i_Projective	(u32 s, bool b)
{
	if	(b)	RS.SetTSS	(s,D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE | D3DTTFF_PROJECTED);
	else	RS.SetTSS	(s,D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
}
void	CBlender_Compile::i_Address		(u32 s, u32	address)
{
	RS.SetSAMP			(s,D3DSAMP_ADDRESSU,	address);
	RS.SetSAMP			(s,D3DSAMP_ADDRESSV,	address);
	RS.SetSAMP			(s,D3DSAMP_ADDRESSW,	address);
}
void	CBlender_Compile::i_Filter_Min		(u32 s, u32	f)
{
	RS.SetSAMP			(s,D3DSAMP_MINFILTER,	f);
}
void	CBlender_Compile::i_Filter_Mip		(u32 s, u32	f)
{
	RS.SetSAMP			(s,D3DSAMP_MIPFILTER,	f);
}
void	CBlender_Compile::i_Filter_Mag		(u32 s, u32	f)
{
	RS.SetSAMP			(s,D3DSAMP_MAGFILTER,	f);
}
void	CBlender_Compile::i_Filter			(u32 s, u32 _min, u32 _mip, u32 _mag)
{
	i_Filter_Min	(s,_min);
	i_Filter_Mip	(s,_mip);
	i_Filter_Mag	(s,_mag);
}
u32		CBlender_Compile::r_Sampler		(LPCSTR _name, LPCSTR texture, bool b_ps1x_ProjectiveDivide, u32 address, u32 fmin, u32 fmip, u32 fmag)
{
	dwStage					= i_Sampler	(_name);
	if (u32(-1)!=dwStage)
	{
		i_Texture				(dwStage,texture);

		// force ANISO-TF for "s_base"
		if ((0==xr_strcmp(_name,"s_base")) && (fmin==D3DTEXF_LINEAR))	fmin = D3DTEXF_ANISOTROPIC;
		if ((0==xr_strcmp(_name,"s_detail")) && (fmin==D3DTEXF_LINEAR))	fmin = D3DTEXF_ANISOTROPIC;

		// Sampler states
		i_Address				(dwStage,address);
		i_Filter				(dwStage,fmin,fmip,fmag);
		if (dwStage<4)			i_Projective		(dwStage,b_ps1x_ProjectiveDivide);
	}
	return	dwStage;
}

void	CBlender_Compile::r_Sampler_rtf	(LPCSTR name, LPCSTR texture, bool b_ps1x_ProjectiveDivide)
{
	r_Sampler	(name,texture,b_ps1x_ProjectiveDivide,D3DTADDRESS_CLAMP,D3DTEXF_POINT,D3DTEXF_NONE,D3DTEXF_POINT);
}
void	CBlender_Compile::r_Sampler_clf	(LPCSTR name, LPCSTR texture, bool b_ps1x_ProjectiveDivide)
{
	r_Sampler	(name,texture,b_ps1x_ProjectiveDivide,D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
}
void	CBlender_Compile::r_Sampler_clw	(LPCSTR name, LPCSTR texture, bool b_ps1x_ProjectiveDivide)
{
	u32 s			= r_Sampler	(name,texture,b_ps1x_ProjectiveDivide,D3DTADDRESS_CLAMP,D3DTEXF_LINEAR,D3DTEXF_NONE,D3DTEXF_LINEAR);
	if (u32(-1)!=s)	RS.SetSAMP	(s,D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
}
void	CBlender_Compile::r_End			()
{
	dest.constants			= Device.Resources->_CreateConstantTable(ctable);
	dest.state				= Device.Resources->_CreateState		(RS.GetContainer());
	dest.T					= Device.Resources->_CreateTextureList	(passTextures);
	dest.C					= 0;
#ifdef _EDITOR
	dest.M					= 0;
	SH->passes.push_back	(Device.Resources->_CreatePass(dest.state,dest.ps,dest.vs,dest.constants,dest.T,dest.M,dest.C));
#else
	SH->passes.push_back	(Device.Resources->_CreatePass(dest.state,dest.ps,dest.vs,dest.constants,dest.T, ref_matrix_list(0) ,dest.C));
#endif
}
