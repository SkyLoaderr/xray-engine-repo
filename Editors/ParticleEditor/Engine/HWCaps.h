#ifndef _HW_CAPS_
#define _HW_CAPS_
#pragma once

#define	CAP_VERSION(a,b)	(((a)<<4) | (b))

class ENGINE_API CHWCaps {
public:
	struct		caps_Geometry
	{
		DWORD	dwVersion	: 8;
		DWORD	dwRegisters	: 16;
		DWORD	bSoftware	: 1;
		DWORD	bPointSprites:1;
		DWORD	bMPS		: 1;		// matrix-palette-skinning
		DWORD	bNPatches	: 1;
	};
	struct		caps_Texturing
	{
		DWORD	dwVersion	: 8;		// pixel shader version
		DWORD	dwRegisters	: 16;
		DWORD	dwStages	: 8;		// number of tex-stages
		DWORD	bNonPow2	: 1;
		DWORD	bCubemap	: 1;
		DWORD	op_DP3		: 1;
		DWORD	op_LERP		: 1;
		DWORD	op_MAD		: 1;
		DWORD	op_reg_TEMP : 1;
	};
public:
	// force flags
	BOOL			bForceVertexFog;
	BOOL			bForceMultipass;
	BOOL			bForceGPU_SW;
	BOOL			bForceGPU_NonPure;
	BOOL			bShowOverdraw;

	// device format
	D3DFORMAT		fTarget;
	D3DFORMAT		fDepth;
	DWORD			dwRefreshRate;
	
	// caps itself
	caps_Geometry	vertex;
	caps_Texturing	pixel;
	
	BOOL			bStencil;			// stencil buffer present
	BOOL			bTableFog;			// 

	// some precalculated values
	D3DSTENCILOP	soDec, soInc;		// best stencil OPs for shadows
	DWORD			dwMaxStencilValue;  // maximum value the stencil buffer can hold

	void		Update(void);
};

#endif
