// HW.cpp: implementation of the CHW class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HW.h"

ENGINE_API CHW HW;


void CHW::CreateD3D()
{
    HW.pD3D = Direct3DCreate8( D3D_SDK_VERSION );
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

	static	D3DFORMAT	fDS_Try1[6] = 
	{D3DFMT_D16,D3DFMT_D15S1,D3DFMT_D24X8,D3DFMT_D24S8,D3DFMT_D24X4S4,D3DFMT_D32};
	
	static	D3DFORMAT	fDS_Try2[2] = 
	{D3DFMT_D24S8,D3DFMT_D24X4S4};

	D3DFORMAT*	fDS_Try = fDS_Try1;
	int			fDS_Cnt	= 6;
	if (psDeviceFlags & rsOverdrawView) { fDS_Try = fDS_Try2; fDS_Cnt=2; }

	for (int it = 0; it<fDS_Cnt; it++){
		if (SUCCEEDED(pD3D->CheckDeviceFormat(
			D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,fTarget,
			D3DUSAGE_DEPTHSTENCIL,D3DRTYPE_SURFACE,fDS_Try[it])))
		{
            if( SUCCEEDED( pD3D->CheckDepthStencilMatch( 
				D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,
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
	_SHOW_REF				("DeviceREF:",HW.pDevice);
	_RELEASE				(HW.pDevice);
	DestroyD3D				();
}

DWORD CHW::CreateDevice		(HWND m_hWnd,DWORD &dwWidth,DWORD &dwHeight)
{
	CreateD3D				();

	// General 
	BOOL  bWindowed	= !(psDeviceFlags&rsFullscreen);

	// Set window properties depending on what mode were in.
	DWORD dwWindowStyle;
	if (bWindowed)	SetWindowLong( m_hWnd, GWL_STYLE, dwWindowStyle=(WS_BORDER|WS_DLGFRAME|WS_VISIBLE) );
	else			SetWindowLong( m_hWnd, GWL_STYLE, dwWindowStyle=(WS_POPUP|WS_VISIBLE) );

	// Display the name of video board
	D3DADAPTER_IDENTIFIER8	adapterID;
	R_CHK(pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT,D3DENUM_NO_WHQL_LEVEL,&adapterID));
	Msg("* Video board: %s",adapterID.Description);

	// Retreive windowed mode
	D3DDISPLAYMODE mWindowed;
	R_CHK(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mWindowed));

	// Select width/height
	dwWidth	= psCurrentMode;
	switch (dwWidth) {
	case 640:	dwHeight = 480;		break;
	case 800:	dwHeight = 600;		break;
	case 1024:	dwHeight = 768;		break;
	case 1280:	dwHeight = 1024;	break;
	default:	dwWidth  = 640; dwHeight = 480; break;
	}

	// Select back-buffer & depth-stencil format
	D3DFORMAT	fTarget,fDepth;
	if (bWindowed)
	{
		fTarget = mWindowed.Format;
		R_CHK(pD3D->CheckDeviceType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,fTarget,fTarget,TRUE));

		fDepth  = selectDepthStencil(fTarget);
	} else {
		switch (psCurrentBPP) {
		case 32:
			fTarget = D3DFMT_X8R8G8B8;
			if (SUCCEEDED(pD3D->CheckDeviceType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_R8G8B8;
			if (SUCCEEDED(pD3D->CheckDeviceType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_UNKNOWN;
			break;
		case 16:
		default:
			fTarget = D3DFMT_R5G6B5;
			if (SUCCEEDED(pD3D->CheckDeviceType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_X1R5G5B5;
			if (SUCCEEDED(pD3D->CheckDeviceType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_X4R4G4B4;
			if (SUCCEEDED(pD3D->CheckDeviceType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,fTarget,fTarget,FALSE)))
				break;
			fTarget = D3DFMT_UNKNOWN;
			break;
		}
		fDepth  = selectDepthStencil(fTarget);
	}
	R_ASSERT(fTarget != D3DFMT_UNKNOWN);
	R_ASSERT(fDepth  != D3DFMT_UNKNOWN);

    // Set up the presentation parameters
	D3DPRESENT_PARAMETERS P;
    ZeroMemory( &P, sizeof(P) );

	// Back buffer
	P.BackBufferWidth		= dwWidth;
    P.BackBufferHeight		= dwHeight;
	P.BackBufferFormat		= fTarget;
	if (bWindowed)			P.BackBufferCount	= 1;
	else					P.BackBufferCount	= (psDeviceFlags&rsTriplebuffer)?2:1;

	// Multisample
    P.MultiSampleType		= D3DMULTISAMPLE_NONE; // pDeviceInfo->MultiSampleType;
    
	// Windoze
    P.SwapEffect			= D3DSWAPEFFECT_DISCARD;
	P.hDeviceWindow			= m_hWnd;
    P.Windowed				= bWindowed;

	// Depth/stencil
	P.EnableAutoDepthStencil= TRUE;
    P.AutoDepthStencilFormat= fDepth;

	// Refresh rate
    if( !bWindowed )
	{
		P.FullScreen_RefreshRateInHz		= selectRefresh			(dwWidth,dwHeight);
		P.FullScreen_PresentationInterval	= selectPresentInterval	();
	}
    else
	{
		P.FullScreen_RefreshRateInHz		= D3DPRESENT_RATE_DEFAULT;
		P.FullScreen_PresentationInterval	= D3DPRESENT_INTERVAL_DEFAULT;
	}

    // Create the device
	DWORD GPU = selectGPU();
    R_CHK(HW.pD3D->CreateDevice(D3DADAPTER_DEFAULT, 
								D3DDEVTYPE_HAL,
                                m_hWnd, 
								GPU,
								&P,
                                &pDevice ));

	switch (GPU)
	{
	case D3DCREATE_SOFTWARE_VERTEXPROCESSING:
		Log("* Geometry Processor: SOFTWARE");
		break;
	case D3DCREATE_MIXED_VERTEXPROCESSING:
		Log("* Geometry Processor: MIXED");
		break;
	case D3DCREATE_HARDWARE_VERTEXPROCESSING:
		Log("* Geometry Processor: HARDWARE");
		break;
	case D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_PUREDEVICE:
		Log("* Geometry Processor: PURE HARDWARE");
		break;
	}

	return dwWindowStyle;
}

DWORD	CHW::selectPresentInterval	()
{
	D3DCAPS8	caps;
	pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);

	if (psDeviceFlags&rsNoVSync) {
		if (caps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE) 
			return D3DPRESENT_INTERVAL_IMMEDIATE;
		if (caps.PresentationIntervals & D3DPRESENT_INTERVAL_ONE) 
			return D3DPRESENT_INTERVAL_ONE;
	}
	return D3DPRESENT_INTERVAL_DEFAULT;
}

DWORD CHW::selectGPU ()
{
	if (Caps.bForceSWTransform) return D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DCAPS8	caps;
	pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);

    if(caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT) {
		if (caps.DevCaps&D3DDEVCAPS_PUREDEVICE) return D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_PUREDEVICE;
		else return D3DCREATE_HARDWARE_VERTEXPROCESSING;
		// return D3DCREATE_MIXED_VERTEXPROCESSING;
	} else return D3DCREATE_SOFTWARE_VERTEXPROCESSING;
}

DWORD CHW::selectRefresh(DWORD dwWidth, DWORD dwHeight)
{
	DWORD count		= pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT);
	DWORD selected	= D3DPRESENT_RATE_DEFAULT;
	for (DWORD I=0; I<count; I++)
	{
		D3DDISPLAYMODE	Mode;
		pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT,I,&Mode);
		if (Mode.Width==dwWidth && Mode.Height==dwHeight)
		{
			if (Mode.RefreshRate>selected) selected = Mode.RefreshRate;
		}
	}
	return selected;
}
