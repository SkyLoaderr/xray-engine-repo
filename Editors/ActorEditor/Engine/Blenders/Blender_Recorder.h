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
	BOOL				bDetail;
	int					iElement;

public:
	CSimulator			RS;
	IBlender*			BT;
	ShaderElement*		SH;

private:
	SPass				dest;
	R_constant_table	ctable;

	STextureList		passTextures;
	SMatrixList			passMatrices;
	SConstantList		passConstants;
	u32					dwStage;

	string128			pass_vs;
	string128			pass_ps;

	u32					BC					(BOOL v)	{ return v?0xff:0; }
public:
	CSimulator&			R()					{ return RS; }
	
	void				SetParams			(int iPriority, bool bStrictB2F, bool bLighting, bool bPixelShader);
	void				SetMapping			();

	// R1-compiler
	void				PassBegin			();
	u32					Pass				()  { return SH->Passes.size(); }
	void				PassSET_ZB			(BOOL bZTest, BOOL bZWrite);
	void				PassSET_Blend		(BOOL bABlend, u32 abSRC, u32 abDST, BOOL aTest, u32 aRef);
	void				PassSET_Blend_BLEND	(BOOL bAref=FALSE, u32 ref=0)	{ PassSET_Blend	(TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA,bAref,ref);	}
	void				PassSET_Blend_SET	(BOOL bAref=FALSE, u32 ref=0)	{ PassSET_Blend	(FALSE,D3DBLEND_ONE,D3DBLEND_ZERO,bAref,ref);				}
	void				PassSET_Blend_ADD	(BOOL bAref=FALSE, u32 ref=0)	{ PassSET_Blend	(TRUE, D3DBLEND_ONE,D3DBLEND_ONE, bAref,ref);				}
	void				PassSET_Blend_MUL	(BOOL bAref=FALSE, u32 ref=0)	{ PassSET_Blend	(TRUE, D3DBLEND_DESTCOLOR,D3DBLEND_ZERO,bAref,ref);			}
	void				PassSET_Blend_MUL2X	(BOOL bAref=FALSE, u32 ref=0)	{ PassSET_Blend	(TRUE, D3DBLEND_DESTCOLOR,D3DBLEND_SRCCOLOR,bAref,ref);		}
	void				PassSET_LightFog	(BOOL bLight, BOOL bFog);
	void				PassSET_VS			(LPCSTR name);
	void				PassEnd				();
	void				PassTemplate_Detail	(LPCSTR T);

	void				StageBegin			();
	u32					Stage				()	{ return dwStage; }
	void				StageTemplate_LMAP0	();
	void				StageSET_Address	(u32 adr);
	void				StageSET_XForm		(u32 tf, u32 tc);
	void				StageSET_Color		(u32 a1, u32 op, u32 a2);
	void				StageSET_Color3		(u32 a1, u32 op, u32 a2, u32 a3);
	void				StageSET_Alpha		(u32 a1, u32 op, u32 a2);
	void				StageSET_TMC		(LPCSTR T, LPCSTR M, LPCSTR C, int UVW_channel);
	void				Stage_Texture		(LPCSTR name);
	void				Stage_Matrix		(LPCSTR name, int UVW_channel);
	void				Stage_Constant		(LPCSTR name);
	void				StageEnd			();

	// R2-compiler
	void				r2_Pass				(LPCSTR vs,		LPCSTR ps,		BOOL	bZtest=TRUE,				BOOL	bZwrite=TRUE,			BOOL	bABlend=FALSE,			u32	abSRC=D3DBLEND_ONE,		u32 abDST=D3DBLEND_ZERO,	BOOL aTest=FALSE,			u32 aRef=0);
	void				r2_Constant			(LPCSTR name,	R_constant_setup* s);
	void				r2_Sampler			(LPCSTR name,	LPCSTR texture, u32		address=D3DTADDRESS_WRAP,	u32		fmin=D3DTEXF_LINEAR,	u32		fmip=D3DTEXF_LINEAR,	u32 fmag=D3DTEXF_LINEAR,	u32 element=0);
	void				r2_Sampler_rtf		(LPCSTR name,	LPCSTR texture,	u32		element=0);
	void				r2_End				();

	CBlender_Compile	();
	~CBlender_Compile	();
	
	void				Compile				(ShaderElement* _SH);
};
#pragma pack(pop)

#endif // !defined(AFX_BLENDER_RECORDER_H__1F549674_8674_4EB2_95E6_E6BC19218A6C__INCLUDED_)
