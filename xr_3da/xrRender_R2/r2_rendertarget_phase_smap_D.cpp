#include "stdafx.h"

void	CRenderTarget::phase_smap_direct()
{
	dwWidth						= DSM_size;
	dwHeight					= DSM_size;
	RCache.set_RT				(rt_smap_d->pRT,		0);
	RCache.set_RT				(NULL,					1);
	RCache.set_RT				(NULL,					2);
	RCache.set_ZB				(rt_smap_d_ZB);
	RImplementation.rmNormal	();

	// Clear viewport
	m_pd3dDevice->Clear						(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0L);
	m_pd3dDevice->SetRenderState			(D3DRS_CULLMODE, D3DCULL_CW);
}

