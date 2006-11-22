// HW.cpp: implementation of the CHW class.
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#pragma warning(disable:4995)
#include <d3dx9.h>
#pragma warning(default:4995)

#include "HW.h"
#include "xr_IOconsole.h"

void	fill_vid_mode_list			(CHW* _hw);
void	free_vid_mode_list			();

ENGINE_API CHW			HW;

#ifdef DEBUG
IDirect3DStateBlock9*	dwDebugSB = 0;
#endif

void CHW::Reset		(HWND hwnd)
{
#ifdef DEBUG
	_RELEASE			(dwDebugSB);
#endif
	_RELEASE			(pBaseZB);
	_RELEASE			(pBaseRT);

#ifndef _EDITOR
#ifndef DEDICATED_SERVER
	BOOL	bWindowed		= !psDeviceFlags.is	(rsFullscreen);
#else
	BOOL	bWindowed		= TRUE;
#endif

	selectResolution		(DevPP.BackBufferWidth, DevPP.BackBufferHeight, bWindowed);
	// Windoze
	DevPP.SwapEffect			= bWindowed?D3DSWAPEFFECT_COPY:D3DSWAPEFFECT_DISCARD;
	DevPP.Windowed				= bWindowed;
	DevPP.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
	if( !bWindowed )		DevPP.FullScreen_RefreshRateInHz	= selectRefresh	(DevPP.BackBufferWidth,DevPP.BackBufferHeight,Caps.fTarget);
	else					DevPP.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;
#endif

	_SHOW_REF				("* RESET: before: DeviceREF:",HW.pDevice);
	while	(TRUE)	{
		HRESULT _hr							= HW.pDevice->Reset	(&DevPP);
		if (SUCCEEDED(_hr))					break;
		Msg		("! ERROR: [%dx%d]: %s",DevPP.BackBufferWidth,DevPP.BackBufferHeight,Debug.error2string(_hr));
		Sleep	(100);
	}
	_SHOW_REF				("* RESET: after : DeviceREF:",HW.pDevice);

	R_CHK				(pDevice->GetRenderTarget			(0,&pBaseRT));
	R_CHK				(pDevice->GetDepthStencilSurface	(&pBaseZB));
#ifdef DEBUG
	R_CHK				(pDevice->CreateStateBlock			(D3DSBT_ALL,&dwDebugSB));
#endif
#ifndef _EDITOR
	updateWindowProps	(hwnd);
#endif
}

xr_token*				vid_mode_token = NULL;

void CHW::CreateD3D	()
{
#ifndef DEDICATED_SERVER
	LPCSTR		_name			= "d3d9.dll";
#else
	LPCSTR		_name			= "d3d9-null.dll";
#endif

	hD3D9            			= LoadLibrary(_name);
	R_ASSERT2	           	 	(hD3D9,"Can't find 'd3d9.dll'\nPlease install latest version of DirectX before running this program");
    typedef IDirect3D9 * WINAPI _Direct3DCreate9(UINT SDKVersion);
    _Direct3DCreate9* createD3D	= (_Direct3DCreate9*)GetProcAddress(hD3D9,"Direct3DCreate9");	R_ASSERT(createD3D);
    this->pD3D 					= createD3D( D3D_SDK_VERSION );
    R_ASSERT2					(this->pD3D,"Please install DirectX 9.0c");
}

void CHW::DestroyD3D()
{
	_RELEASE					(this->pD3D);
    FreeLibrary					(hD3D9);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
D3DFORMAT CHW::selectDepthStencil	(D3DFORMAT fTarget)
{
	// R2 hack
#pragma todo("R2 need to specify depth format")
	if (psDeviceFlags.test(rsR2))	return D3DFMT_D24S8;

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
#ifdef _EDITOR
	_RELEASE				(HW.pDevice);
#else
	_SHOW_REF				("DeviceREF:",HW.pDevice);
	while (HW.pDevice->Release())	;	//.
	HW.pDevice				= 0		;
#endif    
	DestroyD3D				();
	
#ifndef _EDITOR
	free_vid_mode_list		();
#endif
}
void	CHW::selectResolution	(u32 &dwWidth, u32 &dwHeight, BOOL bWindowed)
{
	fill_vid_mode_list			(this);
#ifdef DEDICATED_SERVER
	dwWidth		= 640;
	dwHeight	= 480;
#else
	if(bWindowed)
	{
		dwWidth		= psCurrentVidMode[0];
		dwHeight	= psCurrentVidMode[1];
	}else //check
	{
		string64					buff;
		sprintf						(buff,"%dx%d",psCurrentVidMode[0],psCurrentVidMode[1]);
		
		if(_ParseItem(buff,vid_mode_token)==u32(-1)) //not found
		{ //select safe
			sprintf					(buff,"vid_mode %s",vid_mode_token[0].name);
			Console->Execute		(buff);
		}

		dwWidth						= psCurrentVidMode[0];
		dwHeight					= psCurrentVidMode[1];
	}
#endif

}

void		CHW::CreateDevice		(HWND m_hWnd)
{
	CreateD3D				();

	// General - select adapter and device
#ifdef DEDICATED_SERVER
	BOOL  bWindowed			= TRUE;
#else
	BOOL  bWindowed			= !psDeviceFlags.is(rsFullscreen);
#endif

	DevAdapter				= D3DADAPTER_DEFAULT;
	DevT					= Caps.bForceGPU_REF?D3DDEVTYPE_REF:D3DDEVTYPE_HAL;

//. #ifdef DEBUG
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
//. #endif


	// Display the name of video board
	D3DADAPTER_IDENTIFIER9	adapterID;
	R_CHK	(pD3D->GetAdapterIdentifier(DevAdapter,0,&adapterID));
	Msg		("* GPU [vendor:%X]-[device:%X]: %s",adapterID.VendorId,adapterID.DeviceId,adapterID.Description);

	u16	drv_Product		= HIWORD(adapterID.DriverVersion.HighPart);
	u16	drv_Version		= LOWORD(adapterID.DriverVersion.HighPart);
	u16	drv_SubVersion	= HIWORD(adapterID.DriverVersion.LowPart);
	u16	drv_Build		= LOWORD(adapterID.DriverVersion.LowPart);
	Msg		("* GPU driver: %d.%d.%d.%d",u32(drv_Product),u32(drv_Version),u32(drv_SubVersion), u32(drv_Build));

	Caps.id_vendor	= adapterID.VendorId;
	Caps.id_device	= adapterID.DeviceId;

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
			fTarget = D3DFMT_A8R8G8B8;
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

	if ((D3DFMT_UNKNOWN==fTarget) || (D3DFMT_UNKNOWN==fTarget))	{
		Msg					("Failed to initialize graphics hardware.\nPlease try to restart the game.");
		FlushLog			();
		MessageBox			(NULL,"Failed to initialize graphics hardware.\nPlease try to restart the game.","Error!",MB_OK|MB_ICONERROR);
		TerminateProcess	(GetCurrentProcess(),0);
	}


    // Set up the presentation parameters
	D3DPRESENT_PARAMETERS&	P	= DevPP;
    ZeroMemory				( &P, sizeof(P) );

#ifndef _EDITOR
	selectResolution	(P.BackBufferWidth, P.BackBufferHeight, bWindowed);
#endif
// Back buffer
//.	P.BackBufferWidth		= dwWidth;
//. P.BackBufferHeight		= dwHeight;
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
	P.Flags					= 0;	//. D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;

	// Refresh rate
	P.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
    if( !bWindowed )		P.FullScreen_RefreshRateInHz	= selectRefresh	(P.BackBufferWidth, P.BackBufferHeight,fTarget);
    else					P.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;

    // Create the device
	u32 GPU		= selectGPU();	
	HRESULT R	= HW.pD3D->CreateDevice(DevAdapter,
										DevT,
										m_hWnd,
										GPU | D3DCREATE_MULTITHREADED,	//. ? locks at present
										&P,
										&pDevice );
	
	if (FAILED(R))	{
		R	= HW.pD3D->CreateDevice(	DevAdapter,
										DevT,
										m_hWnd,
										GPU | D3DCREATE_MULTITHREADED,	//. ? locks at present
										&P,
										&pDevice );
	}
	if (D3DERR_DEVICELOST==R)	{
		// Fatal error! Cannot create rendering device AT STARTUP !!!
		Msg					("Failed to initialize graphics hardware.\nPlease try to restart the game.");
		FlushLog			();
		MessageBox			(NULL,"Failed to initialize graphics hardware.\nPlease try to restart the game.","Error!",MB_OK|MB_ICONERROR);
		TerminateProcess	(GetCurrentProcess(),0);
	};
	R_CHK		(R);

	_SHOW_REF	("* CREATE: DeviceREF:",HW.pDevice);
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
#ifndef _EDITOR
	updateWindowProps							(m_hWnd);
	fill_vid_mode_list							(this);
#endif
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
	HRESULT hr		= pD3D->CheckDeviceFormat(DevAdapter,DevT,Caps.fTarget,usage,(D3DRESOURCETYPE)type,fmt);
	if (FAILED(hr))	return FALSE;
	else			return TRUE;
}

void	CHW::updateWindowProps	(HWND m_hWnd)
{
//	BOOL	bWindowed				= strstr(Core.Params,"-dedicated") ? TRUE : !psDeviceFlags.is	(rsFullscreen);
#ifndef DEDICATED_SERVER
	BOOL	bWindowed				= !psDeviceFlags.is	(rsFullscreen);
#else
	BOOL	bWindowed				= TRUE;
#endif
	
	u32		dwWindowStyle			= 0;
	// Set window properties depending on what mode were in.
	if (bWindowed)		{
		SetWindowLong	( m_hWnd, GWL_STYLE, dwWindowStyle=(WS_BORDER|WS_DLGFRAME|WS_VISIBLE|WS_SYSMENU|WS_MINIMIZEBOX ) );
		// When moving from fullscreen to windowed mode, it is important to
		// adjust the window size after recreating the device rather than
		// beforehand to ensure that you get the window size you want.  For
		// example, when switching from 640x480 fullscreen to windowed with
		// a 1000x600 window on a 1024x768 desktop, it is impossible to set
		// the window size to 1000x600 until after the display mode has
		// changed to 1024x768, because windows cannot be larger than the
		// desktop.

		RECT			m_rcWindowBounds;
		BOOL			bCenter = FALSE;
		if (strstr(Core.Params, "-center_screen"))	bCenter = TRUE;

#ifdef DEDICATED_SERVER
		bCenter			= TRUE;
#endif

		if(bCenter){
			RECT				DesktopRect;
			
			GetClientRect		(GetDesktopWindow(), &DesktopRect);

			SetRect(			&m_rcWindowBounds, 
								(DesktopRect.right-DevPP.BackBufferWidth)/2, 
								(DesktopRect.bottom-DevPP.BackBufferHeight)/2, 
								(DesktopRect.right+DevPP.BackBufferWidth)/2, 
								(DesktopRect.bottom+DevPP.BackBufferHeight)/2			);
		}else{
			SetRect(			&m_rcWindowBounds,
								0, 
								0, 
								DevPP.BackBufferWidth, 
								DevPP.BackBufferHeight );
		};

		AdjustWindowRect		(	&m_rcWindowBounds, dwWindowStyle, FALSE );
		SetWindowPos			(	m_hWnd, 
									HWND_TOP,	
									m_rcWindowBounds.left, 
									m_rcWindowBounds.top,
									( m_rcWindowBounds.right - m_rcWindowBounds.left ),
									( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
									SWP_SHOWWINDOW|SWP_NOCOPYBITS|SWP_DRAWFRAME );
	}
	else
	{
		SetWindowLong			( m_hWnd, GWL_STYLE, dwWindowStyle=(WS_POPUP|WS_VISIBLE) );
	}

#ifndef DEDICATED_SERVER
		ShowCursor	(FALSE);
#endif
}


struct _uniq_mode
{
	_uniq_mode(LPCSTR v):_val(v){}
	LPCSTR _val;
	bool operator() (LPCSTR _other) {return !stricmp(_val,_other);}
};

#ifndef _EDITOR
void free_vid_mode_list()
{
	for( int i=0; vid_mode_token[i].name; i++ )
	{
		xr_free					(vid_mode_token[i].name);
	}
	xr_free						(vid_mode_token);
	vid_mode_token				= NULL;
}

void	fill_vid_mode_list			(CHW* _hw)
{
	if(vid_mode_token != NULL)		return;
	xr_vector<LPCSTR>	_tmp;
	u32 cnt = _hw->pD3D->GetAdapterModeCount	(_hw->DevAdapter, _hw->Caps.fTarget);

    u32 i;
	for(i=0; i<cnt;++i)
	{
		D3DDISPLAYMODE	Mode;
		string32		str;

		_hw->pD3D->EnumAdapterModes(_hw->DevAdapter, _hw->Caps.fTarget, i, &Mode);
		if(Mode.Width < 800)		continue;

		sprintf						(str,"%dx%d", Mode.Width, Mode.Height);
	
		if(_tmp.end() != std::find_if(_tmp.begin(), _tmp.end(), _uniq_mode(str)))
			continue;

		_tmp.push_back				(NULL);
		_tmp.back()					= xr_strdup(str);
	}

	u32 _cnt						= _tmp.size()+1;

	vid_mode_token					= xr_alloc<xr_token>(_cnt);

	vid_mode_token[_cnt-1].id			= -1;
	vid_mode_token[_cnt-1].name		= NULL;

	Msg("Avialable video modes[%d]:",_tmp.size());
	for(i=0; i<_tmp.size();++i)
	{
		vid_mode_token[i].id		= i;
		vid_mode_token[i].name		= _tmp[i];
		Msg							("[%s]",_tmp[i]);
	}
}
#endif

