#include "stdafx.h"

void	CRenderTarget::phase_occq	()
{
	u_setrt						( Device.dwWidth,Device.dwHeight,HW.pBaseRT,NULL,NULL,HW.pBaseZB);
	RCache.set_CullMode			( CULL_CCW	);
	RCache.set_Stencil			(TRUE,D3DCMP_LESSEQUAL,0x01,0xff,0x00);
	RCache.set_ColorWriteEnable	(FALSE);
	RCache.set_Shader			( s_occq	);
	//.
	/*
	u32		clr4clear							= color_rgba(0,0,0,0);	// 0x00
	CHK_DX	(HW.pDevice->Clear					( 0L, NULL, D3DCLEAR_TARGET, clr4clear, 1.0f, 0L));
	*/
}
