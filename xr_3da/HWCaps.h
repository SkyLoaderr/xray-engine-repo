#ifndef _HW_CAPS_
#define _HW_CAPS_
#pragma once

struct _version
{	WORD v1,v2,v3,v4; };
class ENGINE_API CHWCaps {
public:
	// force flags
	BOOL		bForceVertexFog;
	BOOL		bForceAltStencil;
	BOOL		bForceMultipass;
	BOOL		bForceSWTransform;
	BOOL		bForceDXT3;
	BOOL		bShowOverdraw;

	// caps itself
	BOOL		bSoftware;
	BOOL		bIndexedBlend;
	BOOL		bPointSpritesHW;
	DWORD		dwRefreshRate;
	DWORD		dwNumBlendStages;
	BOOL		bStencil;

	BOOL		bWFog;			//
	BOOL		bTableFog;

	// some precalculated values
	D3DSTENCILOP	soDec, soInc;		// best stencil OPs for shadows
	DWORD			dwMaxStencilValue;  // maximum value the stencil buffer can hold

	void	Update(void);
};

#endif
