#include "stdafx.h"
#include "HW.h"

#pragma hdrstop

ENGINE_API CHW HW;

#define _SHOW_REF(msg, x)   { VERIFY(x); x->AddRef(); Log(msg,x->Release()); }

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
	{D3DFMT_D24X8,D3DFMT_D16,D3DFMT_D15S1,D3DFMT_D24S8,D3DFMT_D24X4S4,D3DFMT_D32};

	static	D3DFORMAT	fDS_Try2[2] =
	{D3DFMT_D24S8,D3DFMT_D24X4S4};

	D3DFORMAT*	fDS_Try = fDS_Try1;
	int			fDS_Cnt	= 6;
	if (HW.Caps.bShowOverdraw) { fDS_Try = fDS_Try2; fDS_Cnt=2; }

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

void CHW::CreateDevice		(HWND m_hWnd,DWORD &dwWidth,DWORD &dwHeight)
{
	CreateD3D				();

	// Display the name of video board
	D3DADAPTER_IDENTIFIER8	adapterID;
	R_CHK(pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT,D3DENUM_NO_WHQL_LEVEL,&adapterID));
	ELog.Msg(mtInformation,"* Video board: %s",adapterID.Description);

	// Retreive windowed mode
	D3DDISPLAYMODE mWindowed;
	R_CHK(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mWindowed));

	// Select back-buffer & depth-stencil format
	D3DFORMAT	fTarget,fDepth;
	fTarget = mWindowed.Format;
	R_CHK(pD3D->CheckDeviceType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,fTarget,fTarget,TRUE));

	fDepth  = selectDepthStencil(fTarget);

	R_ASSERT(fTarget != D3DFMT_UNKNOWN);
	R_ASSERT(fDepth  != D3DFMT_UNKNOWN);

    // Set up the presentation parameters
	D3DPRESENT_PARAMETERS P;
    ZeroMemory( &P, sizeof(P) );

    P.Windowed				= true;

	// Back buffer
	P.BackBufferWidth		= dwWidth;
    P.BackBufferHeight		= dwHeight;
	P.BackBufferFormat		= fTarget;
	P.BackBufferCount		= 1;

	// Multisample
	P.MultiSampleType		= D3DMULTISAMPLE_NONE;

	// Windoze
    P.SwapEffect			= D3DSWAPEFFECT_DISCARD;
	P.hDeviceWindow			= m_hWnd;

	// Depth/stencil
	P.EnableAutoDepthStencil= TRUE;
    P.AutoDepthStencilFormat= fDepth;

	// Refresh rate
	P.FullScreen_RefreshRateInHz		= D3DPRESENT_RATE_DEFAULT;
	P.FullScreen_PresentationInterval	= D3DPRESENT_INTERVAL_DEFAULT;

    // Create the device
	DWORD GPU = selectGPU();
    R_CHK(HW.pD3D->CreateDevice(D3DADAPTER_DEFAULT,
								D3DDEVTYPE_HAL,
                                m_hWnd,
								GPU,//|D3DCREATE_FPU_PRESERVE,
								&P,
                                &pDevice ));

	switch (GPU)
	{
	case D3DCREATE_SOFTWARE_VERTEXPROCESSING:
		ELog.Msg(mtInformation,"* Geometry Processor: SOFTWARE");
		break;
	case D3DCREATE_MIXED_VERTEXPROCESSING:
		ELog.Msg(mtInformation,"* Geometry Processor: MIXED");
		break;
	case D3DCREATE_HARDWARE_VERTEXPROCESSING:
		ELog.Msg(mtInformation,"* Geometry Processor: HARDWARE");
		break;
	case D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_PUREDEVICE:
		ELog.Msg(mtInformation,"* Geometry Processor: PURE HARDWARE");
		break;
	}
}

DWORD CHW::selectGPU ()
{
	if (Caps.bForceSWTransform) return D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	D3DCAPS8	caps;
	pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);

    if(caps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT) {
	    if (psDeviceFlags&rsForceHWTransform){
			if (caps.DevCaps&D3DDEVCAPS_PUREDEVICE) return D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_PUREDEVICE;
			else return D3DCREATE_HARDWARE_VERTEXPROCESSING;
        }else return D3DCREATE_MIXED_VERTEXPROCESSING;
	} else return D3DCREATE_SOFTWARE_VERTEXPROCESSING;
}


