#include "stdafx.h"

void CRenderTarget::phase_bloom	()
{
	// Targets
	dwWidth								= Device.dwWidth	/2;
	dwHeight							= Device.dwHeight	/2;
	RCache.set_RT						(rt_Bloom_1->pRT,		0);
	RCache.set_RT						(NULL,					1);
	RCache.set_RT						(NULL,					2);
	RCache.set_ZB						(NULL);					// No need for ZBuffer at all
	RImplementation.rmNormal			();

	// Clear	- don't clear - it's stupid here :)
	// Stencil	- disable
	// Misc		- draw everything (no culling)
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILENABLE,		FALSE				));
	CHK_DX(HW.pDevice->SetRenderState	( D3DRS_CULLMODE,			D3DCULL_NONE		)); 	

	// Transfer into Bloom1
	
}
