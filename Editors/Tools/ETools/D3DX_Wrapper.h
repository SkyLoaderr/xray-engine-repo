#ifndef D3DX_WraperH
#define D3DX_WraperH
#pragma once

#ifdef CREATEDX_EXPORTS
#define ETOOLS_API __declspec( dllexport ) 
#else
#define ETOOLS_API __declspec( dllimport ) 
#endif

extern "C" {
	namespace D3DX{
		ETOOLS_API UINT WINAPI D3DXGetDriverLevel(LPDIRECT3DDEVICE9 pDevice);
	}
}

using namespace D3DX;

#endif