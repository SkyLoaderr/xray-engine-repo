// HW.h: interface for the CHW class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HW_H__0E25CF4A_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
#define AFX_HW_H__0E25CF4A_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_

#pragma once

class ENGINE_API CHW {
public:
	IDirect3D8* 			pD3D;		// direct 3d
	IDirect3DDevice8*       pDevice;	// render device

	CHWCaps					Caps;

	CHW()
	{ ZeroMemory(this, sizeof(CHW)); };

	void					CreateD3D				();
	void					DestroyD3D				();
	DWORD					CreateDevice			(HWND hw,DWORD &dwWidth,DWORD &dwHeight);
	void					DestroyDevice			();

	D3DFORMAT				selectDepthStencil		(D3DFORMAT);
	DWORD					selectPresentInterval	();
	DWORD					selectGPU				();
	DWORD					selectRefresh			(DWORD dwWidth, DWORD dwHeight);

#ifdef DEBUG
	void	Validate(void)	{	VERIFY(pDevice); VERIFY(pD3D); };
#else
	void	Validate(void)	{};
#endif
};

extern ENGINE_API CHW HW;

#endif // !defined(AFX_HW_H__0E25CF4A_FFEC_11D3_B4E3_4854E82A090D__INCLUDED_)
