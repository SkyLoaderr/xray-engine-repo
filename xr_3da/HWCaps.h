#ifndef _HW_CAPS_
#define _HW_CAPS_
#pragma once

struct _version
{	WORD v1,v2,v3,v4; };
class ENGINE_API CHWCaps {
public:
	struct		caps_Texturing
	{
		DWORD	dwVersion	: 8;		// pixel shader version
		DWORD	dwStages	: 8;		// number of tex-stages
		DWORD	bNonPow2	: 1;
		DWORD	bCubemap	: 1;
		DWORD	op_DP3		: 1;
		DWORD	op_LERP		: 1;
		DWORD	op_MAD		: 1;
		DWORD	op_reg_TEMP : 1;
	};
	struct		caps_Geometry
	{
		DWORD	dwVersion	: 8;
		DWORD	bSoftware	: 1;
		DWORD	bPointSprites:1;
		DWORD	bMPS		: 1;		// matrix-palette-skinning
		DWORD	bNPatches	: 1;
	};
public:
	// force flags
	BOOL			bForceVertexFog;
	BOOL			bForceMultipass;
	BOOL			bForceSWTransform;
	BOOL			bForceDXT3;
	BOOL			bShowOverdraw;

	// device format
	D3DFORMAT		fTarget;
	D3DFORMAT		fDepth;
	DWORD			dwRefreshRate;
	
	// caps itself
	caps_Texturing	pixel;
	caps_Geometry	vertex;
	
	BOOL			bStencil;			// stencil buffer present
	BOOL			bTableFog;			// 

	// some precalculated values
	D3DSTENCILOP	soDec, soInc;		// best stencil OPs for shadows
	DWORD			dwMaxStencilValue;  // maximum value the stencil buffer can hold

	void		Update(void);
};

#endif
