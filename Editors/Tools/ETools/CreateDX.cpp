// CreateDX.cpp : Defines the entry point for the DLL application.
//

#include	"stdafx.h"
#include	"D3DX_Wrapper.h"

// misc
__declspec( dllimport ) bool WINAPI FSColorPickerDoModal(unsigned int * currentColor, unsigned int * originalColor, const int initialExpansionState);
extern "C" __declspec(dllexport) bool FSColorPickerExecute(LPDWORD currentColor, LPDWORD originalColor, const int initialExpansionState)
{
	return FSColorPickerDoModal((unsigned int*)currentColor, (unsigned int*)originalColor, initialExpansionState);
}

extern "C" ETOOLS_API UINT WINAPI D3DX_GetDriverLevel(LPDIRECT3DDEVICE9 pDevice)
{
	return ::D3DXGetDriverLevel(pDevice);
}
