#ifndef _HW_CAPS_
#define _HW_CAPS_
#pragma once

#define	CAP_VERSION(a,b)	(((a)<<4) | (b))

class ENGINE_API CHWCaps {
public:
	struct		caps_Geometry
	{
		u32	dwVersion	: 8;
		u32	dwRegisters	: 16;
		u32	bSoftware	: 1;
		u32	bPointSprites:1;
		u32	bMPS		: 1;		// matrix-palette-skinning
		u32	bNPatches	: 1;
	};
	struct		caps_Texturing
	{
		u32	dwVersion	: 8;		// pixel shader version
		u32	dwRegisters	: 16;
		u32	dwStages	: 8;		// number of tex-stages
		u32	bNonPow2	: 1;
		u32	bCubemap	: 1;
		u32	op_DP3		: 1;
		u32	op_LERP		: 1;
		u32	op_MAD		: 1;
		u32	op_reg_TEMP : 1;
	};
public:
	// force flags
	BOOL			bForceVertexFog;
	BOOL			bForceMultipass;
	BOOL			bForceGPU_SW;
	BOOL			bForceGPU_NonPure;
	BOOL			SceneMode;

	// device format
	D3DFORMAT		fTarget;
	D3DFORMAT		fDepth;
	u32			dwRefreshRate;
	
	// caps itself
	caps_Geometry	vertex;
	caps_Texturing	pixel;
	
	BOOL			bStencil;			// stencil buffer present
	BOOL			bTableFog;			// 

	// some precalculated values
	D3DSTENCILOP	soDec, soInc;		// best stencil OPs for shadows
	u32			dwMaxStencilValue;  // maximum value the stencil buffer can hold

	void		Update(void);
};

#endif
