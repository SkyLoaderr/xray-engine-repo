// HW.h: interface for the CHW class.
//
//////////////////////////////////////////////////////////////////////
#ifndef HWH
#define HWH

#pragma once
#include "HWCaps.h"

class ENGINE_API CHW {
public:
	IDirect3D8* 			pD3D;		// direct 3d
	IDirect3DDevice8*       pDevice;	// render device

	CHWCaps					Caps;

	CHW()
	{ ZeroMemory(this, sizeof(CHW)); };

	void					CreateD3D				();
	void					DestroyD3D				();
	void					CreateDevice			(HWND hw,DWORD &dwWidth,DWORD &dwHeight);
	void					DestroyDevice			();

	D3DFORMAT				selectDepthStencil		(D3DFORMAT);
	DWORD					selectGPU				();

#ifdef DEBUG
	void	Validate(void)	{	VERIFY(pDevice); VERIFY(pD3D); };
#else
	void	Validate(void)	{};
#endif
};

extern ENGINE_API CHW HW;

#endif
