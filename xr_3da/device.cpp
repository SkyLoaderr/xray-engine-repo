#include "stdafx.h"
#include "hw.h"
#include "device.h"
#include "log.h"

ENGINE_API CRenderDevice Device;

//-----------------------------------------------------------------------------
// Name: WndProc()
// Desc: Static msg handler which passes messages to the application class.
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch(uMsg)
    {
	case WM_ACTIVATE:
		{
			WORD fActive	= LOWORD(wParam);
			BOOL fMinimized = (BOOL) HIWORD(wParam);
			Device.bActive	= (fActive!=WA_INACTIVE) && (!fMinimized);
			if (Device.bActive)	{
				Device.seqAppActivate.Process	(rp_AppActivate);
				ShowCursor(FALSE);
			} else	{
				Device.seqAppDeactivate.Process(rp_AppDeactivate);
				ShowCursor(TRUE);
			}
		}
		return 0;
	case WM_SETCURSOR:
		return 1;
	case WM_SYSCOMMAND:
		// Prevent moving/sizing and power loss in fullscreen mode
		switch( wParam ){
		case SC_MOVE:
		case SC_SIZE:
		case SC_MAXIMIZE:
		case SC_MONITORPOWER:
			return 1;
			break;
		}
		break;
	case WM_CLOSE:
		return 0;
	case WM_KEYDOWN:
		break;
    default:
        break;
    }
    return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

void CRenderDevice::overdrawBegin	()
{
    // Turn stenciling
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILENABLE,		TRUE			));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILFUNC,		D3DCMP_ALWAYS	));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILREF,		0				));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILMASK,		0x00000000		));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILWRITEMASK,	0xffffffff		));

    // Increment the stencil buffer for each pixel drawn
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILZFAIL,		D3DSTENCILOP_INCRSAT	));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP		));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILPASS,		D3DSTENCILOP_INCRSAT	));
}

void CRenderDevice::overdrawEnd		()
{
    // Set up the stencil states
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP	));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP	));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILPASS,		D3DSTENCILOP_KEEP	));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILFUNC,		D3DCMP_EQUAL		));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILMASK,		0xff				));

    // Set the background to black
	CHK_DX(HW.pDevice->Clear(0,0,D3DCLEAR_TARGET,D3DCOLOR_XRGB(255,0,0),0,0));

	// Draw a rectangle wherever the count equal I
	Primitive.Reset	();
	Shader.SetNULL	();
	Shader.SetupPass(0);

	CHK_DX(HW.pDevice->SetVertexShader	( FVF::F_TL ));

	// Render gradients
	for (int I=0; I<12; I++ ) 
	{
		DWORD	_c	= I*256/13;
		DWORD	c	= D3DCOLOR_XRGB(_c,_c,_c);
		
		FVF::TL	pv[4];
		pv[0].set(float(0),			float(dwHeight),	c,0,0);			
		pv[1].set(float(0),			float(0),			c,0,0);					
		pv[2].set(float(dwWidth),	float(dwHeight),	c,0,0);	
		pv[3].set(float(dwWidth),	float(0),			c,0,0);
		
		CHK_DX(HW.pDevice->SetRenderState	( D3DRS_STENCILREF,		I	));
		CHK_DX(HW.pDevice->DrawPrimitiveUP	( D3DPT_TRIANGLESTRIP,	2,	pv, sizeof(FVF::TL) ));
	}
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILENABLE,		FALSE ));
}

void CRenderDevice::Begin(void)
{
	HW.Validate	();
    if (HW.pDevice->TestCooperativeLevel()!=D3D_OK)
	{
		Sleep	(500);
		Destroy	();
		Create	();
	}

	CHK_DX(HW.pDevice->BeginScene());
	CHK_DX(HW.pDevice->Clear(0,0,
		D3DCLEAR_ZBUFFER|
		((psDeviceFlags&rsClearBB)?D3DCLEAR_TARGET:0)|
		(HW.Caps.bStencil?D3DCLEAR_STENCIL:0),
		D3DCOLOR_XRGB(0,255,0),1,0
		));
	Streams.BeginFrame();
	if (HW.Caps.bShowOverdraw)	overdrawBegin	();
	FPU::m24r();
}

void CRenderDevice::End(void)
{
	VERIFY(HW.pDevice);

	if (HW.Caps.bShowOverdraw)	overdrawEnd		();

	// end scene
	Shader.SetNULL	();
	Primitive.Reset	();
    CHK_DX(HW.pDevice->EndScene());

	CHK_DX(HW.pDevice->Present( NULL, NULL, NULL, NULL ));
}

void CRenderDevice::Create	() {
	if (bReady)	return;		// prevent double call
	Log("Starting RENDER device...");

	DWORD dwWindowStyle = HW.CreateDevice(m_hWnd,dwWidth,dwHeight);
	fWidth_2	= float(dwWidth/2);
	fHeight_2	= float(dwHeight/2);
	fFOV		= 90.f;
    {
        // When moving from fullscreen to windowed mode, it is important to
        // adjust the window size after recreating the device rather than
        // beforehand to ensure that you get the window size you want.  For
        // example, when switching from 640x480 fullscreen to windowed with
        // a 1000x600 window on a 1024x768 desktop, it is impossible to set
        // the window size to 1000x600 until after the display mode has
        // changed to 1024x768, because windows cannot be larger than the
        // desktop.
        if( !(psDeviceFlags&rsFullscreen))
        {
		    RECT m_rcWindowBounds = {0, 0, dwWidth, dwHeight };
			AdjustWindowRect( &m_rcWindowBounds, dwWindowStyle, FALSE );
            SetWindowPos( m_hWnd, HWND_TOP,
                          m_rcWindowBounds.left, m_rcWindowBounds.top,
                          ( m_rcWindowBounds.right - m_rcWindowBounds.left ),
                          ( m_rcWindowBounds.bottom - m_rcWindowBounds.top ),
                          SWP_SHOWWINDOW|SWP_NOCOPYBITS|SWP_DRAWFRAME );
        }
    }

    // Hide the cursor if necessary
	ShowCursor(FALSE);

	// after creation
	bReady			= TRUE;

	// General Render States
	mView.identity();
	mProject.identity();
	mFullTransform.identity();
	vCameraPosition.set(0,0,0);
	vCameraDirection.set(0,0,1);
	vCameraTop.set(0,1,0);
	vCameraRight.set(1,0,0);

	HW.Caps.Update();
	for (DWORD i=0; i<HW.Caps.dwNumBlendStages; i++) 
	{
		if (psDeviceFlags&rsAnisotropic)	{
			CHK_DX(HW.pDevice->SetTextureStageState( i, D3DTSS_MINFILTER,	D3DTEXF_ANISOTROPIC ));
			CHK_DX(HW.pDevice->SetTextureStageState( i, D3DTSS_MAGFILTER,	D3DTEXF_ANISOTROPIC ));
			CHK_DX(HW.pDevice->SetTextureStageState( i, D3DTSS_MIPFILTER,	D3DTEXF_LINEAR		));
			CHK_DX(HW.pDevice->SetTextureStageState( i, D3DTSS_MAXANISOTROPY, 16				));
		} else {
			CHK_DX(HW.pDevice->SetTextureStageState( i, D3DTSS_MINFILTER,	D3DTEXF_LINEAR 		));
			CHK_DX(HW.pDevice->SetTextureStageState( i, D3DTSS_MAGFILTER,	D3DTEXF_LINEAR 		));
			CHK_DX(HW.pDevice->SetTextureStageState( i, D3DTSS_MIPFILTER,	D3DTEXF_LINEAR		));
		}
		float fBias = -1.f;
		CHK_DX(HW.pDevice->SetTextureStageState( i, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias))));
	}
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_DITHERENABLE,		TRUE				));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_COLORVERTEX,		TRUE				));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILENABLE,		FALSE				));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_ZENABLE,			TRUE				));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_SHADEMODE,			D3DSHADE_GOURAUD	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW			));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_ALPHAFUNC,			D3DCMP_GREATER		));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_LOCALVIEWER,		FALSE				));

	CHK_DX(HW.pDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE,D3DMCS_MATERIAL	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1	));

	if (psDeviceFlags&rsWireframe)	{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_FILLMODE,			D3DFILL_WIREFRAME	)); }
	else							{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_FILLMODE,			D3DFILL_SOLID		)); }
	if (psDeviceFlags&rsAntialias)	{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS,TRUE				));	}
	else							{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS,FALSE				)); }
	if (psDeviceFlags&rsNormalize)	{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_NORMALIZENORMALS,	TRUE				)); }
	else							{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_NORMALIZENORMALS,	FALSE				)); }

	// ******************** Fog parameters
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGCOLOR,			0					));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_RANGEFOGENABLE,	FALSE				));
	if (HW.Caps.bTableFog)	{
		CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGTABLEMODE,	D3DFOG_LINEAR		));
		CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGVERTEXMODE,	D3DFOG_NONE			));
	} else {
		CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGTABLEMODE,	D3DFOG_NONE			));
		CHK_DX(HW.pDevice->SetRenderState( D3DRS_FOGVERTEXMODE,	D3DFOG_LINEAR		));
	}

	// Signal everyone - device created
	Gamma.Update				();
	Shader.OnDeviceCreate		();
	seqDevCreate.Process		(rp_DeviceCreate);
	Primitive.OnDeviceCreate	();
	dwFrame						= 0;

	// Create TL-primitive
	{
		const DWORD dwTriCount = 1024;
		const DWORD dwIdxCount = dwTriCount*2*3;
		WORD	*Indices = 0;
		DWORD	dwUsage=D3DUSAGE_WRITEONLY;
		if (HW.Caps.bSoftware)	dwUsage|=D3DUSAGE_SOFTWAREPROCESSING;
		R_CHK(HW.pDevice->CreateIndexBuffer(dwIdxCount*2,dwUsage,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&Streams_QuadIB));
		R_CHK(Streams_QuadIB->Lock(0,0,(BYTE**)&Indices,D3DLOCK_NOSYSLOCK));
		{
			int		Cnt = 0;
			int		ICnt= 0;
			for (int i=0; i<dwTriCount; i++)
			{
				Indices[ICnt++]=Cnt+0;
				Indices[ICnt++]=Cnt+1;
				Indices[ICnt++]=Cnt+2;
				
				Indices[ICnt++]=Cnt+3;
				Indices[ICnt++]=Cnt+2;
				Indices[ICnt++]=Cnt+1;
				
				Cnt+=4;
			}
		}
		R_CHK(Streams_QuadIB->Unlock());
	}
}

void CRenderDevice::Destroy	(void) {
	if (!bReady) return;

	Log("Destroying Direct3D...");

	ShowCursor					(TRUE);
	HW.Validate					();

	// before destroy
	bReady = FALSE;
	Shader.OnDeviceDestroy		();
	Primitive.OnDeviceDestroy	();
	seqDevDestroy.Process		(rp_DeviceDestroy);
	Streams.OnDeviceDestroy		();

	// real destroy
	_RELEASE					(Streams_QuadIB);
	HW.DestroyDevice			();
}

void __cdecl mt_Thread(void *ptr) {
	while (true) {
		// waiting for Device permission to execute
		EnterCriticalSection(&Device.mt_csEnter);

		if (Device.mt_bMustExit) {
			Device.mt_bMustExit = FALSE;				// Important!!!
			LeaveCriticalSection(&Device.mt_csEnter);	// Important!!!
			return;
		}
		// we has granted permission to execute
		Device.seqFrameMT.Process	(rp_Frame);

		// now we give control to device - signals that we are ended our work
		LeaveCriticalSection(&Device.mt_csEnter);
		// waits for device signal to continue - to start again
		EnterCriticalSection(&Device.mt_csLeave);
		// returns sync signal to device
		LeaveCriticalSection(&Device.mt_csLeave);
	}
}

void CRenderDevice::Run()
{
    MSG         msg;
    BOOL		bGotMsg;

	Create();
	Log("Starting engine...");

	// Startup timers and calculate timer delta
	TimerGlobal.Start			();
	dwTimeGlobal				= 0;
	Timer_MM_Delta				= 0;
	{
		DWORD time_mm			= timeGetTime	();
		while (timeGetTime()==time_mm);			// wait for next tick
		DWORD time_system		= timeGetTime	();
		DWORD time_local		= TimerAsync	();
		Timer_MM_Delta			= time_system-time_local;
	}

	// Start all threads
	InitializeCriticalSection	(&mt_csEnter);
	InitializeCriticalSection	(&mt_csLeave);
	EnterCriticalSection		(&mt_csEnter);
	mt_bMustExit				= FALSE;
    _beginthread				( mt_Thread, 0, (void *) 0  );

	// Message cycle
	CloseLogWindow				();
    PeekMessage					( &msg, NULL, 0U, 0U, PM_NOREMOVE );
    while( WM_QUIT != msg.message  )
    {
        bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
        if( bGotMsg )
        {
              TranslateMessage( &msg );
              DispatchMessage( &msg );
         }
        else
        {
			if (bReady && bActive) {
				FrameMove();

				// Render
				CHK_DX(HW.pDevice->SetTransform(D3DTS_PROJECTION, mProject.d3d()));
				CHK_DX(HW.pDevice->SetTransform(D3DTS_VIEW, mView.d3d()));
				mFullTransform.mul	( mProject,mView				);

				// *** Render

				// *** Resume threads
				// Capture end point - thread must run only ONE cycle
				// Release start point - allow thread to run
				EnterCriticalSection(&mt_csLeave);
				LeaveCriticalSection(&mt_csEnter);

				Statistic.RenderTOTAL_Real.FrameStart	();
				Statistic.RenderTOTAL_Real.Begin		();
				Begin				();
				seqRender.Process	(rp_Render);
				Statistic.Show		();
				End					();
				Statistic.RenderTOTAL_Real.End			();
				Statistic.RenderTOTAL_Real.FrameEnd		();
				Statistic.RenderTOTAL.accum	= Statistic.RenderTOTAL_Real.accum;

				// *** Suspend threads
				// Capture startup point
				// Release end point - allow thread to wait for startup point
				EnterCriticalSection(&mt_csEnter);
				LeaveCriticalSection(&mt_csLeave);
			}
        }
    }

	// Stop Balance-Thread
	mt_bMustExit = TRUE;
	LeaveCriticalSection(&mt_csEnter);
	while (mt_bMustExit) Sleep(0);
	DeleteCriticalSection(&mt_csEnter);
	DeleteCriticalSection(&mt_csLeave);

	Destroy	();
}

void CRenderDevice::FrameMove()
{
	dwFrame++;

	// Timer
	float fPreviousFrameTime = Timer.GetAsync(); Timer.Start();	// previous frame
	fTimeDelta = 0.1f * fTimeDelta + 0.9f*fPreviousFrameTime;	// smooth random system activity - worst case ~7% error
	if (fTimeDelta>.06666f) fTimeDelta=.06666f;					// limit to 15fps minimum

	u64	qTime		= TimerGlobal.GetElapsed();
	fTimeGlobal		= float(qTime)*CPU::cycles2seconds;

	dwTimeGlobal	= DWORD((qTime*u64(1000))/CPU::cycles_per_second);
	dwTimeDelta		= iFloor(fTimeDelta*1000.f+0.5f);

	// Frame move
	Statistic.EngineTOTAL.Begin	();
	seqFrame.Process(rp_Frame);
	Statistic.EngineTOTAL.End	();
}
