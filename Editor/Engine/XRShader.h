// XRShader.h: interface for the CXRShader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XRSHADER_H__DCA7E752_D211_4499_A9EA_1976C523C069__INCLUDED_)
#define AFX_XRSHADER_H__DCA7E752_D211_4499_A9EA_1976C523C069__INCLUDED_
#pragma once

#include "xrShaderDef.h"
#include "xrShaderTypes.h"
#include "tss.h"

//////////////////////////////////////////////////////////////////////
// Compiled shader (ready to use :) //////////////////////////////////
//////////////////////////////////////////////////////////////////////
struct ENGINE_API SH_Stage
{
public:
	sh_name				Tname;
	BOOL				bNeedXFORM;
	Fmatrix				XForm;
	Fmatrix				mPlanar;
	SH_StageDef::ETCSource tcs;
	DWORD				tcm;			// mask for tc-modifiers
	float				tcm_scaleU,tcm_scaleV;
	float				tcm_rotate;
	float				tcm_scrollU,tcm_scrollV;
	SH_Function			tcm_stretch;

	void				Calculate		(float t);
	void				CompileInit		(SH_StageDef& DEF);
	void				Compile			(SH_ShaderDef& SHADER, SH_StageDef& DEF, DWORD ID, CSimulatorRS& RS);
	void				CompileTextures	(CXRShader* S, DWORD ID, tex_names& T, tex_vector& H );
};

struct ENGINE_API SH_Pass
{
public:
	DWORD				SB;
	BOOL				bCalcFactor;
	SH_Function			Factor;
	DWORD				Factor_Calculated;
	DWORD				Stages_Count;
	SH_Stage			Stages	[sh_STAGE_MAX];

	void				CompileInit		(SH_PassDef& DEF);
	void				Compile			(SH_ShaderDef& SHADER, DWORD ID, CSimulatorRS& RS);
	void				CompileTextures	(CXRShader* S, DWORD ID, tex_names& T, tex_handles& H);
	void				Release			()
	{
		CHK_DX(HW.pDevice->DeleteStateBlock(SB));
		SB=0;
	}

	void				Calculate	(float t);
	void				Apply		()
	{
		if (bCalcFactor) {
			CHK_DX(HW.pDevice->SetRenderState(
				D3DRS_TEXTUREFACTOR,Factor_Calculated
				));
		}
		CHK_DX(HW.pDevice->ApplyStateBlock(SB));
		for (DWORD I=0; I<Stages_Count; I++) 
		{
			SH_Stage &S = Stages[I];
			if (S.bNeedXFORM) 
				CHK_DX(HW.pDevice->SetTransform(
				(D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0+I),S.XForm.d3d()));
		}
	}
};

class ENGINE_API CXRShader  
{
	struct RFlags
	{
		DWORD	iPriority	:	2;
		DWORD	bStrictB2F	:	1;
		DWORD	bLighting	:	1;
		DWORD	bLightmaps	:	1;
	};
public:
	sh_name				cName;
	DWORD				dwFrame;
	RFlags				Flags;
	DWORD				Passes_Count;
	SH_Pass				Passes	[sh_PASS_MAX];

	void				CompileInit		(SH_ShaderDef& DEF);
	void				Compile			(SH_ShaderDef& DEF);
	void				CompileTextures	(tex_names &T, tex_handles &H);

	CXRShader()
	{
		ZeroMemory(this,sizeof(*this));
	}
	IC void				Release		()
	{
		for (DWORD I=0; I<Passes_Count; I++)
			Passes[I].Release();
	}
	IC void				Activate	()
	{
		if (Device.dwFrame != dwFrame) {
			dwFrame = Device.dwFrame;
			for (DWORD I=0; I<Passes_Count; I++)
				Passes[I].Calculate(Device.fTimeGlobal);
		}
	}
	IC void				ApplyPass	(DWORD ID)
	{
		VERIFY(ID<Passes_Count);
		Passes[ID].Apply();
	}
};

#endif // !defined(AFX_XRSHADER_H__DCA7E752_D211_4499_A9EA_1976C523C069__INCLUDED_)
