// Blender_Recorder.cpp: implementation of the CBlender_Compile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Blender_Recorder.h"
#include "Blender.h"

int ParseName(LPCSTR N)
{
	if (0==strcmp(N,"$null"))	return -1;
	if (0==strcmp(N,"$base0"))	return	0;
	if (0==strcmp(N,"$base1"))	return	1;
	if (0==strcmp(N,"$base2"))	return	2;
	if (0==strcmp(N,"$base3"))	return	3;
	if (0==strcmp(N,"$base4"))	return	4;
	if (0==strcmp(N,"$base5"))	return	5;
	if (0==strcmp(N,"$base6"))	return	6;
	if (0==strcmp(N,"$base7"))	return	7;
	return -1;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Compile::CBlender_Compile		()
{
}
CBlender_Compile::~CBlender_Compile		()
{
}
void	CBlender_Compile::Compile		(ShaderElement* _SH)
{
	SH =			_SH;
	RS.Invalidate	();
	BT->Compile		(*this);
	if (bDetail && (!bEditor) && BT->canBeDetailed())		PassTemplate_Detail(BT->oT_Name);
}
void	CBlender_Compile::SetParams		(int iPriority, bool bStrictB2F, bool bLighting, bool bPixelShader)
{
	SH->Flags.iPriority		= iPriority;
	SH->Flags.bStrictB2F	= bStrictB2F;
	SH->Flags.bLighting		= FALSE;
	SH->Flags.bPixelShader	= bPixelShader;
}

//
void	CBlender_Compile::PassBegin		()
{
	RS.Invalidate			();
	passTextures.clear		();
	passMatrices.clear		();
	passConstants.clear		();
	dwStage					= 0;
}
void	CBlender_Compile::PassEnd			()
{
	// Last Stage - disable
	RS.SetTSS	(Stage(),D3DTSS_COLOROP,D3DTOP_DISABLE);
	RS.SetTSS	(Stage(),D3DTSS_ALPHAOP,D3DTOP_DISABLE);

	// Create pass
	CPass	P;
	P.dwStateBlock	= Device.Shader._CreateCode			(RS.GetContainer());
	P.T				= Device.Shader._CreateTextureList	(passTextures);
	P.M				= Device.Shader._CreateMatrixList	(passMatrices);
	P.C				= Device.Shader._CreateConstantList	(passConstants);
	SH->Passes.push_back(P);
}

void	CBlender_Compile::PassTemplate_Detail(LPCSTR Base)
{
	// Parse texture
	sh_list& lst=	L_textures;
	int id		=	ParseName(Base);
	LPCSTR N	=	Base;
	if (id>=0)	{
		if (id>=int(lst.size()))	Device.Fatal("Not enought textures for shader. Base texture: '%s'.",lst[0]);
		N = lst [id];
	}

	// 
	LPCSTR		T,M;
	if (!Device.Shader._GetDetailTexture(N,T,M))	return;

	// Detail
	PassBegin		();
	{
		PassSET_ZB				(TRUE,FALSE);
		PassSET_Blend_MUL2X		();
		PassSET_LightFog		(FALSE,FALSE);

		// Stage0 - Detail
		StageBegin				();
		StageSET_Color			(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
		StageSET_Alpha			(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
		StageSET_TMC			(T,M,"$null",0);
		StageSET_XForm			(D3DTTFF_COUNT2,D3DTSS_TCI_PASSTHRU|0);
		StageEnd				();

		// Stage0 - Detail
		StageBegin				();
		StageSET_Address		(D3DTADDRESS_CLAMP);
		StageSET_Color			(D3DTA_TEXTURE,	  D3DTOP_BLENDTEXTUREALPHA,	D3DTA_CURRENT);
		StageSET_Alpha			(D3DTA_TEXTURE,	  D3DTOP_SELECTARG2,		D3DTA_CURRENT);
		StageSET_TMC			("project","$user$detail","$null",1);
		StageEnd				();
	}
	PassEnd			();
}

void	CBlender_Compile::PassSET_ZB		(BOOL bZTest, BOOL bZWrite)
{
	if (Pass())	bZWrite = FALSE;
	RS.SetRS	(D3DRS_ZFUNC,			bZTest?D3DCMP_LESSEQUAL:D3DCMP_ALWAYS);
	RS.SetRS	(D3DRS_ZWRITEENABLE,	BC(bZWrite));
}

void	CBlender_Compile::PassSET_Blend	(BOOL bABlend, u32 abSRC, u32 abDST, BOOL bATest, u32 aRef)
{
	RS.SetRS(D3DRS_ALPHABLENDENABLE,	BC(bABlend));
	RS.SetRS(D3DRS_SRCBLEND,			bABlend?abSRC:D3DBLEND_ONE	);
	RS.SetRS(D3DRS_DESTBLEND,			bABlend?abDST:D3DBLEND_ZERO	);
	RS.SetRS(D3DRS_ALPHATESTENABLE,		BC(bATest));
	if (bATest)
		RS.SetRS(D3DRS_ALPHAREF,		u32(aRef));
}
void	CBlender_Compile::PassSET_LightFog	(BOOL bLight, BOOL bFog)
{
	RS.SetRS(D3DRS_LIGHTING,			BC(bLight));
	RS.SetRS(D3DRS_FOGENABLE,			BC(bFog));
	SH->Flags.bLighting					|= !!bLight;
}

//
void	CBlender_Compile::StageBegin	()
{
	StageSET_Address	(D3DTADDRESS_WRAP);	// Wrapping enabled by default
}
void	CBlender_Compile::StageEnd		()
{
	dwStage	++;
}
void	CBlender_Compile::StageSET_Address	(u32 adr)
{
	RS.SetTSS	(Stage(),D3DTSS_ADDRESSU,	adr);
	RS.SetTSS	(Stage(),D3DTSS_ADDRESSV,	adr);
}
void	CBlender_Compile::StageSET_XForm	(u32 tf, u32 tc)
{
	RS.SetTSS	(Stage(),D3DTSS_TEXTURETRANSFORMFLAGS,	tf);
	RS.SetTSS	(Stage(),D3DTSS_TEXCOORDINDEX,			tc);
}
void	CBlender_Compile::StageSET_Color	(u32 a1, u32 op, u32 a2)
{
	RS.SetColor	(Stage(),a1,op,a2);
}
void	CBlender_Compile::StageSET_Color3	(u32 a1, u32 op, u32 a2, u32 a3)
{
	RS.SetColor3(Stage(),a1,op,a2,a3);
}
void	CBlender_Compile::StageSET_Alpha	(u32 a1, u32 op, u32 a2)
{
	RS.SetAlpha	(Stage(),a1,op,a2);
}
void	CBlender_Compile::StageSET_TMC		(LPCSTR T, LPCSTR M, LPCSTR C, int UVW_channel)
{
	Stage_Texture		(T);
	Stage_Matrix		(M,UVW_channel);
	Stage_Constant		(C);
}

void	CBlender_Compile::StageTemplate_LMAP0	()
{
	StageSET_Address	(D3DTADDRESS_CLAMP);
	StageSET_Color		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
	StageSET_Alpha		(D3DTA_TEXTURE,	  D3DTOP_SELECTARG1,	D3DTA_DIFFUSE);
	StageSET_TMC		("$base1","$null","$null",1);
}

void	CBlender_Compile::Stage_Texture	(LPCSTR name)
{
	sh_list& lst=	L_textures;
	int id		=	ParseName(name);
	LPCSTR N	=	name;
	if (id>=0)	{
		if (id>=int(lst.size()))	Device.Fatal("Not enought textures for shader. Base texture: '%s'.",lst[0]);
		N = lst [id];
	}
	passTextures.push_back	(Device.Shader._CreateTexture(N));
//	if (passTextures.back()->isUser())	SH->userTex.push_back(passTextures.back());
}
void	CBlender_Compile::Stage_Matrix		(LPCSTR name, int iChannel)
{
	sh_list& lst= L_matrices; 
	int id		= ParseName(name);
	CMatrix*	M	= Device.Shader._CreateMatrix	((id>=0)?lst[id]:name);
	passMatrices.push_back(M);

	// Setup transform pipeline
	u32	ID = Stage();
	if (M) {
		switch (M->dwMode)
		{
		case CMatrix::modeProgrammable:	StageSET_XForm	(D3DTTFF_COUNT3,D3DTSS_TCI_CAMERASPACEPOSITION|ID);					break;
		case CMatrix::modeTCM:			StageSET_XForm	(D3DTTFF_COUNT2,D3DTSS_TCI_PASSTHRU|iChannel);						break;
		case CMatrix::modeC_refl:		StageSET_XForm	(D3DTTFF_COUNT3,D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR|ID);			break;
		case CMatrix::modeS_refl:		StageSET_XForm	(D3DTTFF_COUNT2,D3DTSS_TCI_CAMERASPACENORMAL|ID);					break;
		default:						StageSET_XForm	(D3DTTFF_DISABLE,D3DTSS_TCI_PASSTHRU|iChannel);						break;
		}
	} else {
		// No XForm at all
		StageSET_XForm	(D3DTTFF_DISABLE,D3DTSS_TCI_PASSTHRU|iChannel);	
	}
}
void	CBlender_Compile::Stage_Constant	(LPCSTR name)
{
	sh_list& lst= L_constants;
	int id		= ParseName(name);
	passConstants.push_back	(Device.Shader._CreateConstant((id>=0)?lst[id]:name));
}
