// Blender_Recorder.cpp: implementation of the CBlender_Compile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "..\ResourceManager.h"
#include "Blender_Recorder.h"
#include "Blender.h"

static int ParseName(LPCSTR N)
{
	if (0==xr_strcmp(N,"$null"))	return -1;
	if (0==xr_strcmp(N,"$base0"))	return	0;
	if (0==xr_strcmp(N,"$base1"))	return	1;
	if (0==xr_strcmp(N,"$base2"))	return	2;
	if (0==xr_strcmp(N,"$base3"))	return	3;
	if (0==xr_strcmp(N,"$base4"))	return	4;
	if (0==xr_strcmp(N,"$base5"))	return	5;
	if (0==xr_strcmp(N,"$base6"))	return	6;
	if (0==xr_strcmp(N,"$base7"))	return	7;
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
void	CBlender_Compile::_cpp_Compile	(ShaderElement* _SH)
{
	SH =			_SH;
	RS.Invalidate	();

	// Analyze possibility to detail this shader
	detail_texture	= NULL;
	detail_scaler	= NULL;
	if (bDetail && BT->canBeDetailed())
	{
		// 
		sh_list& lst=	L_textures;
		int id		=	ParseName(BT->oT_Name);
		LPCSTR N	=	BT->oT_Name;
		if (id>=0)	{
			if (id>=int(lst.size()))	Debug.fatal("Not enought textures for shader. Base texture: '%s'.",lst[0]);
			N = lst [id];
		}
		if (!Device.Resources->_GetDetailTexture(N,detail_texture,detail_scaler))	bDetail	= FALSE;
	} else {
		bDetail	= FALSE;
	}

	// Compile
	BT->Compile		(*this);
}

void	CBlender_Compile::SetParams		(int iPriority, bool bStrictB2F)
{
	SH->Flags.iPriority		= iPriority;
	SH->Flags.bStrictB2F	= bStrictB2F;
	//SH->Flags.bLighting		= FALSE;
}

//
void	CBlender_Compile::PassBegin		()
{
	RS.Invalidate			();
	passTextures.clear		();
	passMatrices.clear		();
	passConstants.clear		();
	strcpy					(pass_ps,"null");
	strcpy					(pass_vs,"null");
	dwStage					= 0;
}

void	CBlender_Compile::PassEnd			()
{
	// Last Stage - disable
	RS.SetTSS				(Stage(),D3DTSS_COLOROP,D3DTOP_DISABLE);
	RS.SetTSS				(Stage(),D3DTSS_ALPHAOP,D3DTOP_DISABLE);

	// Create pass
	ref_state	state		= Device.Resources->_CreateState		(RS.GetContainer());
	ref_ps		ps			= Device.Resources->_CreatePS			(pass_ps);
	ref_vs		vs			= Device.Resources->_CreateVS			(pass_vs);
	ctable.merge			(&ps->constants);
	ctable.merge			(&vs->constants);
	SetMapping				();
	ref_ctable			ct	= Device.Resources->_CreateConstantTable(ctable);
	ref_texture_list	T 	= Device.Resources->_CreateTextureList	(passTextures);
	ref_matrix_list		M	= Device.Resources->_CreateMatrixList	(passMatrices);
	ref_constant_list	C	= Device.Resources->_CreateConstantList	(passConstants);

	ref_pass	_pass_		= Device.Resources->_CreatePass			(state,ps,vs,ct,T,M,C);
	SH->Passes.push_back	(_pass_);
}

void	CBlender_Compile::PassSET_PS		(LPCSTR name)
{
	strcpy	(pass_ps,name);
	strlwr	(pass_ps);
}

void	CBlender_Compile::PassSET_VS		(LPCSTR name)
{
	strcpy	(pass_vs,name);
	strlwr	(pass_vs);
}

void	CBlender_Compile::PassSET_ZB		(BOOL bZTest, BOOL bZWrite)
{
	if (Pass())	bZWrite = FALSE;
	RS.SetRS	(D3DRS_ZFUNC,			bZTest?D3DCMP_LESSEQUAL:D3DCMP_ALWAYS);
	RS.SetRS	(D3DRS_ZWRITEENABLE,	BC(bZWrite));
}

void	CBlender_Compile::PassSET_ablend_mode	(BOOL bABlend,	u32 abSRC, u32 abDST)
{
	if (bABlend && D3DBLEND_ONE==abSRC && D3DBLEND_ZERO==abDST)		bABlend = FALSE;
	RS.SetRS(D3DRS_ALPHABLENDENABLE,	BC(bABlend));
	RS.SetRS(D3DRS_SRCBLEND,			bABlend?abSRC:D3DBLEND_ONE	);
	RS.SetRS(D3DRS_DESTBLEND,			bABlend?abDST:D3DBLEND_ZERO	);
}
void	CBlender_Compile::PassSET_ablend_aref	(BOOL bATest,	u32 aRef)
{
	clamp		(aRef,0u,255u);
	RS.SetRS	(D3DRS_ALPHATESTENABLE,		BC(bATest));
	if (bATest)	RS.SetRS(D3DRS_ALPHAREF,	u32(aRef));
}

void	CBlender_Compile::PassSET_Blend	(BOOL bABlend, u32 abSRC, u32 abDST, BOOL bATest, u32 aRef)
{
	PassSET_ablend_mode					(bABlend,abSRC,abDST);
	PassSET_ablend_aref					(bATest,aRef);
}

void	CBlender_Compile::PassSET_LightFog	(BOOL bLight, BOOL bFog)
{
	RS.SetRS(D3DRS_LIGHTING,			BC(bLight));
	RS.SetRS(D3DRS_FOGENABLE,			BC(bFog));
	//SH->Flags.bLighting				|= !!bLight;
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
	RS.SetSAMP	(Stage(),D3DSAMP_ADDRESSU,	adr);
	RS.SetSAMP	(Stage(),D3DSAMP_ADDRESSV,	adr);
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
		if (id>=int(lst.size()))	Debug.fatal("Not enought textures for shader. Base texture: '%s'.",lst[0]);
		N = lst [id];
	}
	passTextures.push_back	(Device.Resources->_CreateTexture(N));
//	if (passTextures.back()->isUser())	SH->userTex.push_back(passTextures.back());
}
void	CBlender_Compile::Stage_Matrix		(LPCSTR name, int iChannel)
{
	sh_list& lst	= L_matrices; 
	int id			= ParseName(name);
	CMatrix*	M	= Device.Resources->_CreateMatrix	((id>=0)?lst[id]:name);
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
	passConstants.push_back	(Device.Resources->_CreateConstant((id>=0)?lst[id]:name));
}
