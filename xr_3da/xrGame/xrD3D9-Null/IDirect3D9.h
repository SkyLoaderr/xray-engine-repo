//---------------------------------
#include <stdlib.h>
#include <objbase.h>
#include <windows.h>
//---------------------------------
#include "d3d9types.h"
#include "d3d9caps.h"

#include "IDirect3DDevice9.h"

/* IID_IDirect3D9 */
/* {81BDCBCA-64D4-426d-AE8D-AD0147F4275C} */
DEFINE_GUID(IID_IDirect3D9, 0x81bdcbca, 0x64d4, 0x426d, 0xae, 0x8d, 0xad, 0x1, 0x47, 0xf4, 0x27, 0x5c);

interface DECLSPEC_UUID("81BDCBCA-64D4-426d-AE8D-AD0147F4275C") IDirect3D9;

typedef interface IDirect3D9                    IDirect3D9;



#ifdef __cplusplus
extern "C" {
#endif

#undef INTERFACE
#define INTERFACE IDirect3D9

	DECLARE_INTERFACE_(IDirect3D9, IUnknown)
	{
		IDirect3D9();
		/*** IUnknown methods ***/
		STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj);// PURE;
		STDMETHOD_(ULONG,AddRef)(THIS);// PURE;
		STDMETHOD_(ULONG,Release)(THIS);// PURE;

		/*** IDirect3D9 methods ***/
		STDMETHOD(RegisterSoftwareDevice)(THIS_ void* pInitializeFunction);
		STDMETHOD_(UINT, GetAdapterCount)(THIS);
		STDMETHOD(GetAdapterIdentifier)(THIS_ UINT Adapter,DWORD Flags,D3DADAPTER_IDENTIFIER9* pIdentifier);
		STDMETHOD_(UINT, GetAdapterModeCount)(THIS_ UINT Adapter,D3DFORMAT Format);
		STDMETHOD(EnumAdapterModes)(THIS_ UINT Adapter,D3DFORMAT Format,UINT Mode,D3DDISPLAYMODE* pMode);
		STDMETHOD(GetAdapterDisplayMode)(THIS_ UINT Adapter,D3DDISPLAYMODE* pMode);
		STDMETHOD(CheckDeviceType)(THIS_ UINT Adapter,D3DDEVTYPE DevType,D3DFORMAT AdapterFormat,D3DFORMAT BackBufferFormat,BOOL bWindowed);
		STDMETHOD(CheckDeviceFormat)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,DWORD Usage,D3DRESOURCETYPE RType,D3DFORMAT CheckFormat);
		STDMETHOD(CheckDeviceMultiSampleType)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SurfaceFormat,BOOL Windowed,D3DMULTISAMPLE_TYPE MultiSampleType,DWORD* pQualityLevels);
		STDMETHOD(CheckDepthStencilMatch)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT AdapterFormat,D3DFORMAT RenderTargetFormat,D3DFORMAT DepthStencilFormat);
		STDMETHOD(CheckDeviceFormatConversion)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DFORMAT SourceFormat,D3DFORMAT TargetFormat);
		STDMETHOD(GetDeviceCaps)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,D3DCAPS9* pCaps);
		STDMETHOD_(HMONITOR, GetAdapterMonitor)(THIS_ UINT Adapter);
		STDMETHOD(CreateDevice)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface);

#ifdef D3D_DEBUG_INFO
		LPCWSTR Version;
#endif
	};

	typedef struct IDirect3D9 *LPDIRECT3D9, *PDIRECT3D9;

#ifdef __cplusplus
};
#endif