// HW.h: interface for the CHW class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HW_H__0E25CF4A_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_HW_H__0E25CF4A_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_

#pragma once

class ENGINE_API CHW
{
public:
	IDirect3D9* 			pD3D;		// D3D
	IDirect3DDevice9*		pDevice;	// render device

	IDirect3DSurface9*		pBaseRT;
	IDirect3DSurface9*		pBaseZB;
	IDirect3DSurface9*		pTempZB;

	CHWCaps					Caps;

	CHW()
	{
		pD3D		= NULL;
		pDevice		= NULL;
		pBaseRT		= NULL;
		pBaseZB		= NULL;
		pTempZB		= NULL;
	};

	void					CreateD3D				();
	void					DestroyD3D				();
	u32						CreateDevice			(HWND hw,u32 &dwWidth,u32 &dwHeight);
	void					DestroyDevice			();

	D3DFORMAT				selectDepthStencil		(D3DFORMAT);
	u32						selectPresentInterval	();
	u32						selectGPU				();
	u32						selectRefresh			(u32 dwWidth, u32 dwHeight, D3DFORMAT fmt);

#ifdef DEBUG
	void	Validate(void)	{	VERIFY(pDevice); VERIFY(pD3D); };
#else
	void	Validate(void)	{};
#endif
};

extern ENGINE_API CHW HW;

#endif // !defined(AFX_HW_H__0E25CF4A_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
