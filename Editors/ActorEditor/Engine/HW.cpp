// HW.cpp: implementation of the CHW class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HW.h"

ENGINE_API CHW HW;

IDirect3DStateBlock9*	dwDebugSB = 0;

void CHW::CreateD3D()
{
    HW.pD3D = Direct3DCreate9( D3D_SDK_VERSION );
    R_ASSERT(HW.pD3D);

}
void CHW::DestroyD3D()
{
	_RELEASE(HW.pD3D);
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
			D3DADAPTER_DEFAULT,DevT,fTarget,
			D3DUSAGE_DEPTHSTENCIL,D3DRTYPE_SURFACE,fDS_Try[it])))
		{
            if( SUCCEEDED( pD3D->CheckDepthStencilMatch(
				D3DADAPTER_DEFAULT,DevT,
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
	_RELEASE				(dwDebugSB);
	_SHOW_REF				("DeviceREF:",HW.pDevice);
	_RELEASE				(HW.pDevice);
	DestroyD3D				();
}

u32 CHW::CreateDevice		(HWND m_hWnd,u32 &dwWidth,u32 &dwHeight)
{
	CreateD3D				();

	// General
	BOOL  bWindowed			= !psDeviceFlags.is(rsFullscreen);
	DevT					= Caps.bForceGPU_REF?D3DDEVTYPE_REF:D3DDEVTYPE_HAL;

	u32 dwWindowStyle=0;
#ifndef _EDITOR
	// Set window properties depending on what mode were in.
	if (bWindowed)	SetWindowLong( m_hWnd, GWL_STYLE, dwWindowStyle=(WS_BORDER|WS_DLGFRAME|WS_VISIBLE) );
	else			SetWindowLong( m_hWnd, GWL_STYLE, dwWindowStyle=(WS_POPUP|WS_VISIBLE) );

	// Select width/height
	dwWidth	= psCurrentMode;
	switch (dwWidth) {
	case 512:	dwHeight = 384;		break;
	case 640:	dwHeight = 480;		break;
	case 800:	dwHeight = 600;		break;
	case 1024:	dwHeight = 768;		break;
	case 1280:	dwHeight = 1024;	break;
	case 1600:	dwHeight = 1200;	break;
	default:	dwWidth  = 800; dwHeight = 600; break;
	}
#endif
	// Display the name of video board
	D3DADAPTER_IDENTIFIER9	adapterID;
	R_CHK	(pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT,0,&adapterID));
	Msg		("* GPU: %s",adapterID.Description);

	// Retreive windowed mode
	D3DDISPLAYMODE mWindowed;
	R_CHK(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mWindowed));

	// Select back-buffer & depth-stencil format
	D3DFORMAT&	fTarget	= Caps.fTarget;
	D3DFORMAT&	fDepth	= Caps.fDepth;
	if (bWindowed)
	{
		fTarget = mWindowed.Format;
		R_CHK(pD3D->CheckDeviceType	(D3DADAPTER_DEFAULT,DevT,fTarget,fTarget,TRUE));
		fDepth  = selectDepthStencil(fTarget);
	} else {
		switch (psCurrentBPP) {
		case 32:
			fTarget = D3DFMT_X8R8G8B8;
			if (SUCCEEDED(pD3D->CheckDeviceType(D3DADAPTER_DEFAULT,DevT,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_R8G8B8;
			if (SUCCEEDED(pD3D->CheckDeviceType(D3DADAPTER_DEFAULT,DevT,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_UNKNOWN;
			break;
		case 16:
		default:
			fTarget = D3DFMT_R5G6B5;
			if (SUCCEEDED(pD3D->CheckDeviceType(D3DADAPTER_DEFAULT,DevT,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_X1R5G5B5;
			if (SUCCEEDED(pD3D->CheckDeviceType(D3DADAPTER_DEFAULT,DevT,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_X4R4G4B4;
			if (SUCCEEDED(pD3D->CheckDeviceType(D3DADAPTER_DEFAULT,DevT,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_UNKNOWN;
			break;
		}
		fDepth  = selectDepthStencil(fTarget);
	}
	R_ASSERT				(fTarget != D3DFMT_UNKNOWN);
	R_ASSERT				(fDepth  != D3DFMT_UNKNOWN);

    // Set up the presentation parameters
	D3DPRESENT_PARAMETERS	P;
    ZeroMemory				( &P, sizeof(P) );

	// Back buffer
	P.BackBufferWidth		= dwWidth;
    P.BackBufferHeight		= dwHeight;
	P.BackBufferFormat		= fTarget;
	P.BackBufferCount		= 1;

	// Multisample
	if ((!bWindowed) && psDeviceFlags.is(rsAntialias))
	{
		P.MultiSampleType	= D3DMULTISAMPLE_2_SAMPLES;
	} else {
	    P.MultiSampleType	= D3DMULTISAMPLE_NONE;
	}
	P.MultiSampleQuality	= 0;

	// Windoze
    P.SwapEffect			= D3DSWAPEFFECT_DISCARD;
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
    R_CHK(HW.pD3D->CreateDevice(D3DADAPTER_DEFAULT,
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
	R_CHK	(pDevice->CreateStateBlock			(D3DSBT_ALL,&dwDebugSB));
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
	pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,DevT,&caps);

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
	pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,DevT,&caps);

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
		u32 count		= pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT,fmt);
		for (u32 I=0; I<count; I++)
		{
			D3DDISPLAYMODE	Mode;
			pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT,fmt,I,&Mode);
			if (Mode.Width==dwWidth && Mode.Height==dwHeight)
			{
				if (Mode.RefreshRate>selected) selected = Mode.RefreshRate;
			}
		}
		return selected;
	}
}
