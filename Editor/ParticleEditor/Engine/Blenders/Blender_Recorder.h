// Blender_Recorder.h: interface for the CBlender_Recorder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_RECORDER_H__1F549674_8674_4EB2_95E6_E6BC19218A6C__INCLUDED_)
#define AFX_BLENDER_RECORDER_H__1F549674_8674_4EB2_95E6_E6BC19218A6C__INCLUDED_
#pragma once

#include "..\tss.h"

class ENGINE_API CBlender_Recorder  
{
	CSimulator			RS;
	Shader*				SH;

	STextureList		passTextures;
	SMatrixList			passMatrices;
	SConstantList		passConstants;
	DWORD				dwStage;

	DWORD				BC				(BOOL v)	{ return v?0xff:0; }
public:
	CSimulator&			R()				{ return RS; }

	void				PassBegin		();
	DWORD				Pass			()  { return SH->Passes.size(); }
	void				PassSET_ZB		(BOOL bZTest, BOOL bZWrite);
	void				PassSET_Blend	(BOOL bABlend, DWORD abSRC, DWORD abDST, BOOL aTest, DWORD aRef);
	void				PassSET_Blend_SET	()	{ PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,FALSE,0);				}
	void				PassSET_Blend_ADD	()	{ PassSET_Blend	(TRUE, D3DBLEND_ONE,D3DBLEND_ONE, FALSE,0);				}
	void				PassSET_Blend_MUL	()	{ PassSET_Blend	(TRUE, D3DBLEND_DESTCOLOR,D3DBLEND_ZERO,FALSE,0);		}
	void				PassSET_Blend_MUL2X	()	{ PassSET_Blend	(TRUE, D3DBLEND_DESTCOLOR,D3DBLEND_SRCCOLOR,FALSE,0);	}
	void				PassEnd			();

	void				StageBegin		();
	DWORD				Stage			()	{ return dwStage; }
	void				StageSET_Address(DWORD adr);
	void				StageSET_XForm	(DWORD tf, DWORD tc);
	void				StageSET_Color	(DWORD a1, DWORD op, DWORD a2);
	void				StageSET_Alpha	(DWORD a1, DWORD op, DWORD a2);
	void				Stage_Texture	(LPCSTR name, sh_list& lst);
	void				Stage_Matrix	(LPCSTR name, sh_list& lst, int iChannel);
	void				Stage_Constant	(LPCSTR name, sh_list& lst);
	void				StageEnd		();

	CBlender_Recorder	(Shader* _SH);
	~CBlender_Recorder	();
};

#endif // !defined(AFX_BLENDER_RECORDER_H__1F549674_8674_4EB2_95E6_E6BC19218A6C__INCLUDED_)
