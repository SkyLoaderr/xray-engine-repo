// Blender_Recorder.cpp: implementation of the CBlender_Recorder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Blender_Recorder.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender_Recorder::CBlender_Recorder		(Shader* _SH)
{
	RS.Invalidate	();
	SH =			_SH;
}
CBlender_Recorder::~CBlender_Recorder		()
{

}
void	CBlender_Recorder::SetParams		(int iPriority, bool bStrictB2F, bool bLighting, bool bPixelShader)
{
	SH->Flags.iPriority		= iPriority;
	SH->Flags.bStrictB2F	= bStrictB2F;
	SH->Flags.bLighting		= FALSE;
	SH->Flags.bPixelShader	= bPixelShader;
}

//
void	CBlender_Recorder::PassBegin		()
{
	RS.Invalidate			();
	passTextures.clear		();
	passMatrices.clear		();
	passConstants.clear		();
	dwStage					= 0;
}
void	CBlender_Recorder::PassEnd			()
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

void	CBlender_Recorder::PassSET_ZB		(BOOL bZTest, BOOL bZWrite)
{
	if (Pass())	bZWrite = FALSE;
	RS.SetRS	(D3DRS_ZFUNC,			bZTest?D3DCMP_LESSEQUAL:D3DCMP_ALWAYS);
	RS.SetRS	(D3DRS_ZWRITEENABLE,	BC(bZWrite));
}

void	CBlender_Recorder::PassSET_Blend	(BOOL bABlend, DWORD abSRC, DWORD abDST, BOOL bATest, DWORD aRef)
{
	RS.SetRS(D3DRS_ALPHABLENDENABLE,	BC(bABlend));
	RS.SetRS(D3DRS_SRCBLEND,			bABlend?abSRC:D3DBLEND_ONE	);
	RS.SetRS(D3DRS_DESTBLEND,			bABlend?abDST:D3DBLEND_ZERO	);
	RS.SetRS(D3DRS_ALPHATESTENABLE,		BC(bATest));
	if (bATest)
		RS.SetRS(D3DRS_ALPHAREF,		DWORD(aRef));
}
void	CBlender_Recorder::PassSET_LightFog	(BOOL bLight, BOOL bFog)
{
	RS.SetRS(D3DRS_LIGHTING,			BC(bLight));
	RS.SetRS(D3DRS_FOGENABLE,			BC(bFog));
	SH->Flags.bLighting					|= !!bLight;
}

//
void	CBlender_Recorder::StageBegin	()
{
}
void	CBlender_Recorder::StageEnd		()
{
	dwStage	++;
}
void	CBlender_Recorder::StageSET_Address(DWORD adr)
{
	RS.SetTSS	(Stage(),D3DTSS_ADDRESSU,	adr);
	RS.SetTSS	(Stage(),D3DTSS_ADDRESSV,	adr);
}
void	CBlender_Recorder::StageSET_XForm	(DWORD tf, DWORD tc)
{
	RS.SetTSS	(Stage(),D3DTSS_TEXTURETRANSFORMFLAGS,	tf);
	RS.SetTSS	(Stage(),D3DTSS_TEXCOORDINDEX,			tc);
}
void	CBlender_Recorder::StageSET_Color	(DWORD a1, DWORD op, DWORD a2)
{
	RS.SetColor	(Stage(),a1,op,a2);
}
void	CBlender_Recorder::StageSET_Alpha	(DWORD a1, DWORD op, DWORD a2)
{
	RS.SetAlpha	(Stage(),a1,op,a2);
}

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

void	CBlender_Recorder::Stage_Texture	(LPCSTR name, sh_list& lst)
{
	int id = ParseName(name);
	passTextures.push_back	(Device.Shader._CreateTexture((id>=0)?lst[id]:name));
}
void	CBlender_Recorder::Stage_Matrix		(LPCSTR name, sh_list& lst, int iChannel)
{
	int id = ParseName(name);
	CMatrix*	M	= Device.Shader._CreateMatrix	((id>=0)?lst[id]:name);
	passMatrices.push_back(M);

	// Setup transform pipeline
	DWORD	ID = Stage();
	if (M) {
		switch (M->dwMode)
		{
		case CMatrix::modeProgrammable:	StageSET_XForm	(D3DTTFF_COUNT2,D3DTSS_TCI_CAMERASPACEPOSITION|ID);					break;
		case CMatrix::modeTCM:			StageSET_XForm	(D3DTTFF_COUNT2,D3DTSS_TCI_PASSTHRU|iChannel);						break;
		case CMatrix::modeC_refl:		StageSET_XForm	(D3DTTFF_COUNT3,D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR|ID);			break;
		case CMatrix::modeS_refl:		StageSET_XForm	(D3DTTFF_COUNT2,D3DTSS_TCI_CAMERASPACENORMAL|ID);					break;
		default:	Device.Fatal		("Invalid TC source");
		}
	} else {
		// No XForm at all
		StageSET_XForm	(D3DTTFF_DISABLE,D3DTSS_TCI_PASSTHRU|iChannel);	
	}
}
void	CBlender_Recorder::Stage_Constant	(LPCSTR name, sh_list& lst)
{
	int id = ParseName(name);
	passConstants.push_back	(Device.Shader._CreateConstant((id>=0)?lst[id]:name));
}
