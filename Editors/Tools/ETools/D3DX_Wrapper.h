#ifndef D3DX_WraperH
#define D3DX_WraperH
#pragma once

#ifdef CREATEDX_EXPORTS
#define ETOOLS_API __declspec( dllexport ) 
#else
#define ETOOLS_API __declspec( dllimport ) 
#endif

extern "C" {
	ETOOLS_API UINT WINAPI D3DX_GetDriverLevel(LPDIRECT3DDEVICE9 pDevice);
}
#define D3DXGetDriverLevel D3DX_GetDriverLevel
#endif