// HW.cpp: implementation of the CHW class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)

#include "HW.h"

ENGINE_API CHW		HW;

#ifdef DEBUG
IDirect3DStateBlock9*	dwDebugSB = 0;
#endif

void CHW::Reset		()
{
#ifdef DEBUG
	_RELEASE		(dwDebugSB);
#endif
	_RELEASE		(pBaseZB);
	_RELEASE		(pBaseRT);

#ifndef _EDITOR
	selectResolution(DevPP.BackBufferWidth,DevPP.BackBufferHeight);
#endif

	while	(TRUE)	{
		HRESULT _hr							= HW.pDevice->Reset	(&DevPP);
		if (SUCCEEDED(_hr))					break;
		Sleep								(100);
	}

	R_CHK			(pDevice->GetRenderTarget			(0,&pBaseRT));
	R_CHK			(pDevice->GetDepthStencilSurface	(&pBaseZB));
#ifdef DEBUG
	R_CHK			(pDevice->CreateStateBlock			(D3DSBT_ALL,&dwDebugSB));
#endif
}

void CHW::CreateD3D	()
{
	hD3D9            			= LoadLibrary("d3d9.dll");
	R_ASSERT2	           	 	(hD3D9,"Can't find 'd3d9.dll'\nPlease install latest version of DirectX before running this program");
    typedef IDirect3D9 * WINAPI _Direct3DCreate9(UINT SDKVersion);
    _Direct3DCreate9* createD3D	= (_Direct3DCreate9*)GetProcAddress(hD3D9,"Direct3DCreate9");	R_ASSERT(createD3D);
    HW.pD3D 					= createD3D( D3D_SDK_VERSION );
    R_ASSERT					(HW.pD3D);
}
void CHW::DestroyD3D()
{
	_RELEASE					(HW.pD3D);
    FreeLibrary					(hD3D9);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
D3DFORMAT CHW::selectDepthStencil	(D3DFORMAT fTarget)
{
	// R2 hack
#pragma todo("R2 need to specify depth format")
	if (strstr(Core.Params,"-r2"))	return D3DFMT_D24S8;

	// R1 usual
	static	D3DFORMAT	fDS_Try1[6] =
	{D3DFMT_D24S8,D3DFMT_D24X4S4,D3DFMT_D32,D3DFMT_D24X8,D3DFMT_D16,D3DFMT_D15S1};

	D3DFORMAT*	fDS_Try			= fDS_Try1;
	int			fDS_Cnt			= 6;

	for (int it = 0; it<fDS_Cnt; it++){
		if (SUCCEEDED(pD3D->CheckDeviceFormat(
			DevAdapter,DevT,fTarget,
			D3DUSAGE_DEPTHSTENCIL,D3DRTYPE_SURFACE,fDS_Try[it])))
		{
            if( SUCCEEDED( pD3D->CheckDepthStencilMatch(
				DevAdapter,DevT,
                fTarget, fTarget, fDS_Try[it]) ) )
            {
				return fDS_Try[it];
            }
		}
	}
	return D3DFMT_UNKNOWN;
}

void	CHW::DestroyDevice	()
{
	_RELEASE				(pBaseZB);
	_RELEASE				(pBaseRT);
#ifdef DEBUG
	_RELEASE				(dwDebugSB);
#endif
	_SHOW_REF				("DeviceREF:",HW.pDevice);
	_RELEASE				(HW.pDevice);
	DestroyD3D				();
}

void	CHW::selectResolution	(u32 &dwWidth, u32 &dwHeight)
{
	// Select width/height
	dwWidth	= psCurrentMode;
	switch (dwWidth) {
	case 320:	dwHeight = 240;		break;
	case 512:	dwHeight = 384;		break;
	case 640:	dwHeight = 480;		break;
	case 800:	dwHeight = 600;		break;
	case 1024:	dwHeight = 768;		break;
	case 1280:	dwHeight = 960;		break;
	case 1600:	dwHeight = 1200;	break;
	default:	dwWidth  = 1024; dwHeight = 768; break;
	}
}

u32		CHW::CreateDevice		(HWND m_hWnd,u32 &dwWidth,u32 &dwHeight)
{
	CreateD3D				();

	// General - select adapter and device
	BOOL  bWindowed			= !psDeviceFlags.is(rsFullscreen);
	DevAdapter				= D3DADAPTER_DEFAULT;
	DevT					= Caps.bForceGPU_REF?D3DDEVTYPE_REF:D3DDEVTYPE_HAL;

#ifdef DEBUG
	// Look for 'NVIDIA NVPerfHUD' adapter
	// If it is present, override default settings
	for (UINT Adapter=0;Adapter<pD3D->GetAdapterCount();Adapter++)	{
		D3DADAPTER_IDENTIFIER9 Identifier;
		HRESULT Res=pD3D->GetAdapterIdentifier(Adapter,0,&Identifier);
		if (SUCCEEDED(Res) && (xr_strcmp(Identifier.Description,"NVIDIA NVPerfHUD")==0))
		{
			DevAdapter	=Adapter;
			DevT		=D3DDEVTYPE_REF;
			break;
		}
	}
#endif

	u32 dwWindowStyle=0;
#ifndef _EDITOR
	// Set window properties depending on what mode were in.
	if (bWindowed)	SetWindowLong( m_hWnd, GWL_STYLE, dwWindowStyle=(WS_BORDER|WS_DLGFRAME|WS_VISIBLE) );
	else			SetWindowLong( m_hWnd, GWL_STYLE, dwWindowStyle=(WS_POPUP|WS_VISIBLE) );

	// Select width/height
	selectResolution	(dwWidth,dwHeight);
#endif
	// Display the name of video board
	D3DADAPTER_IDENTIFIER9	adapterID;
	R_CHK	(pD3D->GetAdapterIdentifier(DevAdapter,0,&adapterID));
	Msg		("* GPU: %s",adapterID.Description);

	// Retreive windowed mode
	D3DDISPLAYMODE mWindowed;
	R_CHK(pD3D->GetAdapterDisplayMode(DevAdapter, &mWindowed));

	// Select back-buffer & depth-stencil format
	D3DFORMAT&	fTarget	= Caps.fTarget;
	D3DFORMAT&	fDepth	= Caps.fDepth;
	if (bWindowed)
	{
		fTarget = mWindowed.Format;
		R_CHK(pD3D->CheckDeviceType	(DevAdapter,DevT,fTarget,fTarget,TRUE));
		fDepth  = selectDepthStencil(fTarget);
	} else {
		switch (psCurrentBPP) {
		case 32:
			fTarget = D3DFMT_X8R8G8B8;
			if (SUCCEEDED(pD3D->CheckDeviceType(DevAdapter,DevT,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_R8G8B8;
			if (SUCCEEDED(pD3D->CheckDeviceType(DevAdapter,DevT,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_UNKNOWN;
			break;
		case 16:
		default:
			fTarget = D3DFMT_R5G6B5;
			if (SUCCEEDED(pD3D->CheckDeviceType(DevAdapter,DevT,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_X1R5G5B5;
			if (SUCCEEDED(pD3D->CheckDeviceType(DevAdapter,DevT,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_X4R4G4B4;
			if (SUCCEEDED(pD3D->CheckDeviceType(DevAdapter,DevT,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_UNKNOWN;
			break;
		}
		fDepth  = selectDepthStencil(fTarget);
	}
	R_ASSERT				(fTarget != D3DFMT_UNKNOWN);
	R_ASSERT				(fDepth  != D3DFMT_UNKNOWN);

    // Set up the presentation parameters
	D3DPRESENT_PARAMETERS&	P	= DevPP;
    ZeroMemory				( &P, sizeof(P) );

	// Back buffer
	P.BackBufferWidth		= dwWidth;
    P.BackBufferHeight		= dwHeight;
	P.BackBufferFormat		= fTarget;
	P.BackBufferCount		= 1;

	// Multisample
    P.MultiSampleType		= D3DMULTISAMPLE_NONE;
	P.MultiSampleQuality	= 0;

	// Windoze
    P.SwapEffect			= bWindowed?D3DSWAPEFFECT_COPY:D3DSWAPEFFECT_DISCARD;
	P.hDeviceWindow			= m_hWnd;
    P.Windowed				= bWindowed;

	// Depth/stencil
	P.EnableAutoDepthStencil= TRUE;
    P.AutoDepthStencilFormat= fDepth;
	P.Flags					= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;

	// Refresh rate
	P.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
    if( !bWindowed )		P.FullScreen_RefreshRateInHz	= selectRefresh	(dwWidth,dwHeight,fTarget);
    else					P.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;

    // Create the device
	u32 GPU = selectGPU();
    R_CHK(HW.pD3D->CreateDevice(DevAdapter,
								DevT,
                                m_hWnd,
								GPU,
								&P,
                                &pDevice ));

	switch (GPU)
	{
	case D3DCREATE_SOFTWARE_VERTEXPROCESSING:
		Log	("* Vertex Processor: SOFTWARE");
		break;
	case D3DCREATE_MIXED_VERTEXPROCESSING:
		Log	("* Vertex Processor: MIXED");
		break;
	case D3DCREATE_HARDWARE_VERTEXPROCESSING:
		Log	("* Vertex Processor: HARDWARE");
		break;
	case D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_PUREDEVICE:
		Log	("* Vertex Processor: PURE HARDWARE");
		break;
	}

	// Capture misc data
#ifdef DEBUG
	R_CHK	(pDevice->CreateStateBlock			(D3DSBT_ALL,&dwDebugSB));
#endif
	R_CHK	(pDevice->GetRenderTarget			(0,&pBaseRT));
	R_CHK	(pDevice->GetDepthStencilSurface	(&pBaseZB));
	u32	memory									= pDevice->GetAvailableTextureMem	();
	Msg		("*     Texture memory: %d M",		memory/(1024*1024));
	Msg		("*          DDI-level: %2.1f",		float(D3DXGetDriverLevel(pDevice))/100.f);
	return dwWindowStyle;
}

u32	CHW::selectPresentInterval	()
{
	D3DCAPS9	caps;
	pD3D->GetDeviceCaps(DevAdapter,DevT,&caps);

	if (psDeviceFlags.is(rsNoVSync)) {
		if (caps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE)
			return D3DPRESENT_INTERVAL_IMMEDIATE;
		if (caps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE)
			return D3DPRESENT_INTERVAL_ONE;
	}
	return D3DPRESENT_INTERVAL_DEFAULT;
}

u32 CHW::selectGPU ()
{
	if (Caps.bForceGPU_SW) return D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DCAPS9	caps;
	pD3D->GetDeviceCaps(DevAdapter,DevT,&caps);

    if(caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		if (Caps.bForceGPU_NonPure)	return D3DCREATE_HARDWARE_VERTEXPROCESSING;
		else {
			if (caps.DevCaps&D3DDEVCAPS_PUREDEVICE) return D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_PUREDEVICE;
			else return D3DCREATE_HARDWARE_VERTEXPROCESSING;
		}
		// return D3DCREATE_MIXED_VERTEXPROCESSING;
	} else return D3DCREATE_SOFTWARE_VERTEXPROCESSING;
}

u32 CHW::selectRefresh(u32 dwWidth, u32 dwHeight, D3DFORMAT fmt)
{
	if (psDeviceFlags.is(rsRefresh60hz))	return D3DPRESENT_RATE_DEFAULT;
	else
	{
		u32 selected	= D3DPRESENT_RATE_DEFAULT;
		u32 count		= pD3D->GetAdapterModeCount(DevAdapter,fmt);
		for (u32 I=0; I<count; I++)
		{
			D3DDISPLAYMODE	Mode;
			pD3D->EnumAdapterModes(DevAdapter,fmt,I,&Mode);
			if (Mode.Width==dwWidth && Mode.Height==dwHeight)
			{
				if (Mode.RefreshRate>selected) selected = Mode.RefreshRate;
			}
		}
		return selected;
	}
}

BOOL	CHW::support	(D3DFORMAT fmt, DWORD type, DWORD usage)
{
	HRESULT hr		= pD3D->CheckDeviceFormat(DevAdapter,DevT,fTarget,usage,(D3DRESOURCETYPE)type,fmt);
	if (FAILED(hr))	return FALSE;
	else			return TRUE;
}
