// CreateDX.cpp : Defines the entry point for the DLL application.
//

#include	"stdafx.h"

// misc
__declspec( dllimport ) bool WINAPI FSColorPickerDoModal(unsigned int * currentColor, unsigned int * originalColor, const int initialExpansionState);
extern "C" __declspec(dllexport) bool FSColorPickerExecute(LPDWORD currentColor, LPDWORD originalColor, const int initialExpansionState){
	return FSColorPickerDoModal((unsigned int*)currentColor, (unsigned int*)originalColor, initialExpansionState);
}
namespace D3DX
{
	__declspec( dllexport ) UINT WINAPI D3DXGetDriverLevel(LPDIRECT3DDEVICE9 pDevice)
	{
		return ::D3DXGetDriverLevel(pDevice);
	}
}
