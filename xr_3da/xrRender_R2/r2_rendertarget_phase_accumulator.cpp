#include "stdafx.h"

// ONLY ONCE PER FRAME
void	CRenderTarget::phase_accumulator()
{
	dwWidth						= Device.dwWidth;
	dwHeight					= Device.dwHeight;
	RCache.set_RT				(rt_Accumulator->pRT,	0);
	RCache.set_RT				(NULL,					1);
	RCache.set_RT				(NULL,					2);
	RCache.set_ZB				(HW.pBaseZB);
	RImplementation.rmNormal	();

	m_pd3dDevice->Clear						(0L, NULL, D3DCLEAR_TARGET, 0x00, 1.0f, 0L);

	// Set up the stencil states
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILENABLE,		TRUE				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILFUNC,		D3DCMP_LESSEQUAL	);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILREF,			0x01				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILMASK,		0xff				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILWRITEMASK,	0x00				);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP	);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILPASS,		D3DSTENCILOP_KEEP	);
	m_pd3dDevice->SetRenderState			( D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP	);
}

