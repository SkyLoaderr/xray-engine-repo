// Blender_Recorder.h: interface for the CBlender_Recorder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_RECORDER_H__1F549674_8674_4EB2_95E6_E6BC19218A6C__INCLUDED_)
#define AFX_BLENDER_RECORDER_H__1F549674_8674_4EB2_95E6_E6BC19218A6C__INCLUDED_
#pragma once

#pragma pack(push,4)
#include "..\tss.h"

class ENGINE_API CBlender_Compile  
{
public:
	sh_list				L_textures;
	sh_list				L_constants;
	sh_list				L_matrices;
	BOOL				bEditor;
	BOOL				bLighting;
	BOOL				bDetail;
	int					iLayers;
	int					iLOD;

public:
	CSimulator			RS;
	CBlender*			BT;
	ShaderElement*		SH;

private:
	STextureList		passTextures;
	SMatrixList			passMatrices;
	SConstantList		passConstants;
	DWORD				dwStage;

	DWORD				BC					(BOOL v)	{ return v?0xff:0; }
public:
	CSimulator&			R()					{ return RS; }
	
	void				SetParams			(int iPriority, bool bStrictB2F, bool bLighting, bool bPixelShader);

	void				PassBegin			();
	DWORD				Pass				()  { return SH->Passes.size(); }
	void				PassSET_ZB			(BOOL bZTest, BOOL bZWrite);
	void				PassSET_LightFog	(BOOL bLight, BOOL bFog);
	void				PassSET_Blend		(BOOL bABlend, DWORD abSRC, DWORD abDST, BOOL aTest, DWORD aRef);
	void				PassSET_Blend_SET	()	{ PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,FALSE,0);				}
	void				PassSET_Blend_ADD	()	{ PassSET_Blend	(TRUE, D3DBLEND_ONE,D3DBLEND_ONE, FALSE,0);				}
	void				PassSET_Blend_MUL	()	{ PassSET_Blend	(TRUE, D3DBLEND_DESTCOLOR,D3DBLEND_ZERO,FALSE,0);		}
	void				PassSET_Blend_MUL2X	()	{ PassSET_Blend	(TRUE, D3DBLEND_DESTCOLOR,D3DBLEND_SRCCOLOR,FALSE,0);	}
	void				PassEnd				();
	void				PassTemplate_Detail	(LPCSTR T);

	void				StageBegin			();
	DWORD				Stage				()	{ return dwStage; }
	void				StageTemplate_LMAP0	();
	void				StageSET_Address	(DWORD adr);
	void				StageSET_XForm		(DWORD tf, DWORD tc);
	void				StageSET_Color		(DWORD a1, DWORD op, DWORD a2);
	void				StageSET_Color3		(DWORD a1, DWORD op, DWORD a2, DWORD a3);
	void				StageSET_Alpha		(DWORD a1, DWORD op, DWORD a2);
	void				StageSET_TMC		(LPCSTR T, LPCSTR M, LPCSTR C, int UVW_channel);
	void				Stage_Texture		(LPCSTR name);
	void				Stage_Matrix		(LPCSTR name, int UVW_channel);
	void				Stage_Constant		(LPCSTR name);
	void				StageEnd			();

	CBlender_Compile	();
	~CBlender_Compile	();
	
	void				Compile				(ShaderElement* _SH);
};
#pragma pack(pop)

#endif // !defined(AFX_BLENDER_RECORDER_H__1F549674_8674_4EB2_95E6_E6BC19218A6C__INCLUDED_)
