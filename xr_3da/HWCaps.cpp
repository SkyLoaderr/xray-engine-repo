#include "stdafx.h"
#include "hwcaps.h"
#include "hw.h"

void CHWCaps::Update()
{
	D3DCAPS8 caps;
	HW.pDevice->GetDeviceCaps(&caps);

	bSoftware		= (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)==0;
	bIndexedBlend	= (caps.VertexProcessingCaps&D3DVTXPCAPS_NO_VSDT_UBYTE4)!=0;

	dwNumBlendStages = _MIN(caps.MaxTextureBlendStages,caps.MaxSimultaneousTextures);

	bTableFog	=	BOOL(caps.RasterCaps&D3DPRASTERCAPS_FOGTABLE);
	bWFog		=	BOOL(caps.RasterCaps&D3DPRASTERCAPS_WFOG);

	// Stencil relative caps
    DWORD dwStencilCaps = caps.StencilCaps;
    if( (!(dwStencilCaps & D3DSTENCILCAPS_INCR) && !(dwStencilCaps & D3DSTENCILCAPS_INCRSAT))
       ||(!(dwStencilCaps & D3DSTENCILCAPS_DECR) && !(dwStencilCaps & D3DSTENCILCAPS_DECRSAT)))
    {
		soDec = soInc = D3DSTENCILOP_KEEP;
		dwMaxStencilValue = 0;
    }
    else
    {
        // Prefer sat ops that cap at 0/max, but can use other ones as long as enough stencil bits
        soInc=(dwStencilCaps & D3DSTENCILCAPS_INCRSAT)? D3DSTENCILOP_INCRSAT:D3DSTENCILOP_INCR;
        soDec=(dwStencilCaps & D3DSTENCILCAPS_DECRSAT)? D3DSTENCILOP_DECRSAT:D3DSTENCILOP_DECR;
	    dwMaxStencilValue=(1<<8)-1;
    }
	bPointSpritesHW	= FALSE;

	// FORCE (overwrite) flags
	if (bForceVertexFog)		bTableFog			=	false;
	if (bForceMultipass)		dwNumBlendStages	=	1;
}
