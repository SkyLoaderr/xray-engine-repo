#include "stdafx.h"
#include "IDirect3D9.h"

#include <stdlib.h>
#include <objbase.h>
#include <windows.h>

const GUID DECLSPEC_SELECTANY IID_IDirect3D9;

IDirect3D9::IDirect3D9()
{
	int x=0;
	x=x;
};

ULONG	IDirect3D9::AddRef(void)
{
	return 1;
}

ULONG	IDirect3D9::Release(void)
{
	delete this;
	return 0;
}

HRESULT IDirect3D9::QueryInterface(const IID &iid, void FAR* FAR* ppvObj)
{
	if (iid == IID_IUnknown || iid == IID_IDirect3D9)
	{
		*ppvObj = this;
		AddRef();
		return NOERROR;
	}
	return E_NOINTERFACE;
}

HRESULT IDirect3D9::RegisterSoftwareDevice(void* pInitializeFunction)
{
	return S_OK;
};
UINT IDirect3D9::GetAdapterCount()
{
	return 1;
};

HRESULT IDirect3D9::GetAdapterIdentifier(UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9* pIdentifier)
{
	return S_OK;
};
UINT IDirect3D9::GetAdapterModeCount( UINT Adapter,D3DFORMAT Format)
{
	return 1;
};

HRESULT IDirect3D9::EnumAdapterModes(UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE* pMode)
{
	return S_OK;
};
HRESULT IDirect3D9::GetAdapterDisplayMode( UINT Adapter,D3DDISPLAYMODE* pMode)
{
	return S_OK;
};
HRESULT IDirect3D9::CheckDeviceType( UINT Adapter,D3DDEVTYPE DevType,D3DFORMAT AdapterFormat,D3DFORMAT BackBufferFormat,BOOL bWindowed)
{
	return S_OK;
};
HRESULT IDirect3D9::CheckDeviceFormat( UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat)
{
	return S_OK;
};
HRESULT IDirect3D9::CheckDeviceMultiSampleType( UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD* pQualityLevels)
{
	return S_OK;
};
HRESULT IDirect3D9::CheckDepthStencilMatch( UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat)
{
	return S_OK;
};
HRESULT IDirect3D9::CheckDeviceFormatConversion( UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat)
{
	return S_OK;
};
HRESULT IDirect3D9::GetDeviceCaps( UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9* pCaps)
{
	return S_OK;
};
HMONITOR IDirect3D9::GetAdapterMonitor( UINT Adapter)
{
	return NULL;
};

HRESULT IDirect3D9::CreateDevice	(UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface)
{
	return S_OK;
};