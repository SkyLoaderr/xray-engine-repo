#include "stdafx.h"

ENGINE_API CRenderDevice Device;

void CRenderDevice::overdrawBegin	()
{
    // Turn stenciling
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILENABLE,		TRUE			));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILFUNC,		D3DCMP_ALWAYS	));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILREF,		0				));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILMASK,		0x00000000		));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILWRITEMASK,	0xffffffff		));

    // Increment the stencil buffer for each pixel drawn
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP		));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILPASS,		D3DSTENCILOP_INCRSAT	));

	if (1==HW.Caps.SceneMode)		
	{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILZFAIL,	D3DSTENCILOP_KEEP		)); }	// Overdraw
	else 
	{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILZFAIL,	D3DSTENCILOP_INCRSAT	)); }	// ZB access
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
	Primitive.Reset		();
	Shader.OnFrameEnd	();
	CHK_DX	(HW.pDevice->SetVertexShader( FVF::F_TL ));

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

void CRenderDevice::Begin	()
{
	HW.Validate	();
    if (HW.pDevice->TestCooperativeLevel()!=D3D_OK)
	{
		Sleep	(500);
		Destroy	();
		Create	();
	}

	CHK_DX		(HW.pDevice->BeginScene());
	Streams.BeginFrame();
	if (HW.Caps.SceneMode)	overdrawBegin	();
	FPU::m24r();
}

void CRenderDevice::Clear	()
{
	CHK_DX(HW.pDevice->Clear(0,0,
		D3DCLEAR_ZBUFFER|
		((psDeviceFlags&rsClearBB)?D3DCLEAR_TARGET:0)|
		(HW.Caps.bStencil?D3DCLEAR_STENCIL:0),
		D3DCOLOR_XRGB(0,255,0),1,0
		));
}

void CRenderDevice::End		(void)
{
	VERIFY	(HW.pDevice);

	if (HW.Caps.SceneMode)	overdrawEnd		();

	// 
	if (dwPrecacheFrame)
	{
		dwPrecacheFrame	--;
	}

	// end scene
	Shader.OnFrameEnd	();
	Primitive.Reset		();
    CHK_DX(HW.pDevice->EndScene());

	CHK_DX(HW.pDevice->Present( NULL, NULL, NULL, NULL ));
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

void CRenderDevice::PreCache	(DWORD amount)
{
	Msg				("* PCACHE: start for %d...",amount);
	dwPrecacheFrame	= dwPrecacheTotal = amount;
}

void CRenderDevice::Run			()
{
    MSG         msg;
    BOOL		bGotMsg;

	Create		();
	Log			("Starting engine...");

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
              TranslateMessage	( &msg );
              DispatchMessage	( &msg );
         }
        else
        {
			if (bReady && bActive) {
				FrameMove					( );

				// Precache
				if (dwPrecacheFrame)
				{
					float factor					= float(dwPrecacheFrame)/float(dwPrecacheTotal);
					float angle						= PI_MUL_2 * factor;
					vCameraDirection.set			(sinf(angle),0,cosf(angle));	vCameraDirection.normalize	();
					vCameraTop.set					(0,1,0);
					vCameraRight.crossproduct		(vCameraTop,vCameraDirection);

					mView.build_camera_dir			(vCameraPosition,vCameraDirection,vCameraTop);
				}

				// Matrices
				mFullTransform.mul			( mProject,mView	);
				Device.set_xform_view		( mView				);
				Device.set_xform_project	( mProject			);

				// *** Resume threads
				// Capture end point - thread must run only ONE cycle
				// Release start point - allow thread to run
				EnterCriticalSection		(&mt_csLeave);
				LeaveCriticalSection		(&mt_csEnter);

				Statistic.RenderTOTAL_Real.FrameStart	();
				Statistic.RenderTOTAL_Real.Begin		();
				Begin									();
				
				seqRender.Process						(rp_Render);
				Statistic.Show							();

				End										();
				Statistic.RenderTOTAL_Real.End			();
				Statistic.RenderTOTAL_Real.FrameEnd		();
				Statistic.RenderTOTAL.accum	= Statistic.RenderTOTAL_Real.accum;

				// *** Suspend threads
				// Capture startup point
				// Release end point - allow thread to wait for startup point
				EnterCriticalSection		(&mt_csEnter);
				LeaveCriticalSection		(&mt_csLeave);
			}
        }
    }

	// Stop Balance-Thread
	mt_bMustExit = TRUE;
	LeaveCriticalSection	(&mt_csEnter);
	while (mt_bMustExit) Sleep(0);
	DeleteCriticalSection	(&mt_csEnter);
	DeleteCriticalSection	(&mt_csLeave);

	Destroy		();
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
