#include "stdafx.h"
#pragma hdrstop

#include "hwcaps.h"
#include "hw.h"

void CHWCaps::Update()
{
	D3DCAPS9 caps;
	HW.pDevice->GetDeviceCaps(&caps);

	// ***************** GEOMETRY
	geometry.dwVersion_major	= u32 ( (caps.VertexShaderVersion&(0xf << 8ul))>>8 );
	geometry.dwVersion_minor	= u32 ( (caps.VertexShaderVersion&0xf) );
	geometry.bSoftware			= (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)==0;
	geometry.bPointSprites		= FALSE;
	geometry.bNPatches			= (caps.DevCaps & D3DDEVCAPS_NPATCHES)!=0;
	geometry.bMPS				= (caps.DeclTypes & D3DDTCAPS_UBYTE4)!=0;
	geometry.dwRegisters		= (caps.MaxVertexShaderConst);
	geometry.dwClipPlanes		= _min(caps.MaxUserClipPlanes,15);
	IDirect3DQuery9*	q_vc;
	D3DDEVINFO_VCACHE	vc;
	HRESULT _hr			= HW.pDevice->CreateQuery(D3DQUERYTYPE_VCACHE,&q_vc);
	if (FAILED(_hr))
	{
		vc.OptMethod			= 0;
		vc.CacheSize			= 16;
		geometry.dwVertexCache	= 16;
	} else {
		q_vc->Issue			(D3DISSUE_END);
		q_vc->GetData		(&vc,sizeof(vc),D3DGETDATA_FLUSH);
		_RELEASE			(q_vc);
		if (1==vc.OptMethod	)	geometry.dwVertexCache	= vc.CacheSize;
		else					geometry.dwVertexCache	= 16;
	}
	Msg					("* GPU vertex cache: %s, %d",(1==vc.OptMethod)?"recognized":"unrecognized",u32(geometry.dwVertexCache));

	// ***************** PIXEL processing
	raster.dwVersion_major		= u32 ( (caps.PixelShaderVersion&(0xf << 8ul))>>8 );
	raster.dwVersion_minor		= u32 ( (caps.PixelShaderVersion&0xf) );
	raster.dwStages				= caps.MaxSimultaneousTextures;
	raster.bNonPow2				= ((caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL)!=0)  || ((caps.TextureCaps & D3DPTEXTURECAPS_POW2)==0);
	raster.bCubemap				= (caps.TextureCaps & D3DPTEXTURECAPS_CUBEMAP)!=0;
	raster.op_DP3				= (caps.TextureOpCaps & D3DTEXOPCAPS_DOTPRODUCT3)!=0;
	raster.op_LERP				= (caps.TextureOpCaps & D3DTEXOPCAPS_LERP)!=0;
	raster.op_MAD				= (caps.TextureOpCaps & D3DTEXOPCAPS_MULTIPLYADD)!=0;
	raster.op_reg_TEMP			= (caps.PrimitiveMiscCaps & D3DPMISCCAPS_TSSARGTEMP)!=0;

	// ***************** Compatibility : vertex shader
	if (0==raster.dwVersion_major)	geometry.dwVersion_major=0;		// Disable VS if no PS
#ifdef _EDITOR
	geometry.dwVersion	= 0;
#endif

	//
	bTableFog			=	FALSE;	//BOOL	(caps.RasterCaps&D3DPRASTERCAPS_FOGTABLE);

	// Detect if stencil available
	bStencil			=	FALSE;
	IDirect3DSurface9*	surfZS=0;
	D3DSURFACE_DESC		surfDESC;
	CHK_DX		(HW.pDevice->GetDepthStencilSurface(&surfZS));
	R_ASSERT	(surfZS);
	CHK_DX		(surfZS->GetDesc(&surfDESC));
	_RELEASE	(surfZS);

	switch		(surfDESC.Format)
	{
	case D3DFMT_D15S1:		bStencil = TRUE;	break;
	case D3DFMT_D24S8:		bStencil = TRUE;	break;
	case D3DFMT_D24X4S4:	bStencil = TRUE;	break;
	}

	// Scissoring
	if (caps.RasterCaps & D3DPRASTERCAPS_SCISSORTEST)	bScissor	= TRUE;
	else												bScissor	= FALSE;

	// Stencil relative caps
    u32 dwStencilCaps = caps.StencilCaps;
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

	// DEV INFO
}
