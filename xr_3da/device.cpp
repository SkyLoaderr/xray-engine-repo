#include "stdafx.h"
#include "frustum.h"

#pragma warning(disable:4995)
// mmsystem.h
#define MMNOSOUND
#define MMNOMIDI
#define MMNOAUX
#define MMNOMIXER
#define MMNOJOY
#include <mmsystem.h>
// d3dx9.h
#include <d3dx9.h>
#pragma warning(default:4995)

ENGINE_API CRenderDevice Device;

BOOL CRenderDevice::Begin	()
{
	HW.Validate		();
	HRESULT	_hr		= HW.pDevice->TestCooperativeLevel();
    if (FAILED(_hr))
	{
		// If the device was lost, do not render until we get it back
		if		(D3DERR_DEVICELOST==_hr)		{
			Sleep	(33);
			return	FALSE;
		}

		// Check if the device is ready to be reset
		if		(D3DERR_DEVICENOTRESET==_hr)
		{
			Reset	();
		}
	}

	CHK_DX					(HW.pDevice->BeginScene());
	RCache.OnFrameBegin		();
	if (HW.Caps.SceneMode)	overdrawBegin	();
	FPU::m24r	();
	return		TRUE;
}

void CRenderDevice::Clear	()
{
	CHK_DX(HW.pDevice->Clear(0,0,
		D3DCLEAR_ZBUFFER|
		(psDeviceFlags.test(rsClearBB)?D3DCLEAR_TARGET:0)|
		(HW.Caps.bStencil?D3DCLEAR_STENCIL:0),
		D3DCOLOR_XRGB(0,0,0),1,0
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
		CHK_DX			(HW.pDevice->Clear(0,0,D3DCLEAR_TARGET,D3DCOLOR_ARGB(0,0,0,0),1,0));
		if (0==dwPrecacheFrame)
		{
			Memory.mem_compact		();
			Msg						("* MEMORY USAGE: %d K",Memory.mem_usage()/1024);
		}
	}

	// end scene
	RCache.OnFrameEnd	();
    CHK_DX(HW.pDevice->EndScene());

	HRESULT _hr		= HW.pDevice->Present( NULL, NULL, NULL, NULL );
	if				(D3DERR_DEVICELOST==_hr)	return;			// we will handle this later
	R_ASSERT2		(SUCCEEDED(_hr),	"Presentation failed. Driver upgrade needed?");
}

#pragma pack(push,8)
struct THREAD_NAME
{
	DWORD	dwType;
	LPCSTR	szName;
	DWORD	dwThreadID;
	DWORD	dwFlags;
};

void	SetThreadName(LPCSTR name)
{
	THREAD_NAME		tn;
	tn.dwType		= 0x1000;
	tn.szName		= name;
	tn.dwThreadID	= DWORD(-1);
	tn.dwFlags		= 0;
	__try
	{
		RaiseException(0x406D1388,0,sizeof(tn)/sizeof(DWORD),(DWORD*)&tn);
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}

#pragma pack(pop)


volatile u32	mt_Thread_marker		= 0x12345678;
void __cdecl	mt_Thread	(void *ptr)	{
	SetThreadName			("X-RAY Secondary thread");

	while (true) {
		// waiting for Device permission to execute
		EnterCriticalSection	(&Device.mt_csEnter);

		if (Device.mt_bMustExit) {
			Device.mt_bMustExit = FALSE;				// Important!!!
			LeaveCriticalSection(&Device.mt_csEnter);	// Important!!!
			return;
		}
		// we has granted permission to execute
		mt_Thread_marker			= Device.dwFrame;
		Device.seqFrameMT.Process	(rp_Frame);

		// now we give control to device - signals that we are ended our work
		LeaveCriticalSection	(&Device.mt_csEnter);
		// waits for device signal to continue - to start again
		EnterCriticalSection	(&Device.mt_csLeave);
		// returns sync signal to device
		LeaveCriticalSection	(&Device.mt_csLeave);
	}
}

void CRenderDevice::PreCache	(u32 amount)
{
	if (HW.Caps.bForceGPU_REF)	amount=0;
	// Msg			("* PCACHE: start for %d...",amount);
	dwPrecacheFrame	= dwPrecacheTotal = amount;
}

void CRenderDevice::Run			()
{
    MSG         msg;
    BOOL		bGotMsg;

	Log				("Starting engine...");
	SetThreadName	("X-RAY Primary thread");

	// Startup timers and calculate timer delta
	dwTimeGlobal				= 0;
	Timer_MM_Delta				= 0;
	{
		u32 time_mm			= timeGetTime	();
		while (timeGetTime()==time_mm);			// wait for next tick
		u32 time_system		= timeGetTime	();
		u32 time_local		= TimerAsync	();
		Timer_MM_Delta		= time_system-time_local;
	}

	// Start all threads
	InitializeCriticalSection	(&mt_csEnter);
	InitializeCriticalSection	(&mt_csLeave);
	EnterCriticalSection		(&mt_csEnter);
	mt_bMustExit				= FALSE;
    _beginthread				( mt_Thread, 0, (void *) 0  );

	// Message cycle
    PeekMessage					( &msg, NULL, 0U, 0U, PM_NOREMOVE );

	seqAppCycleStart.Process	(rp_AppCycleStart);

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
			if (bReady) {
				FrameMove					( );

				// Precache
				if (dwPrecacheFrame)
				{
					float factor					= float(dwPrecacheFrame)/float(dwPrecacheTotal);
					float angle						= PI_MUL_2 * factor;
					vCameraDirection.set			(_sin(angle),0,_cos(angle));	vCameraDirection.normalize	();
					vCameraTop.set					(0,1,0);
					vCameraRight.crossproduct		(vCameraTop,vCameraDirection);

					mView.build_camera_dir			(vCameraPosition,vCameraDirection,vCameraTop);
				}

				// Matrices
				mFullTransform.mul			( mProject,mView	);
				RCache.set_xform_view		( mView				);
				RCache.set_xform_project	( mProject			);
				D3DXMatrixInverse			( (D3DXMATRIX*)&mInvFullTransform, 0, (D3DXMATRIX*)&mFullTransform);

				// *** Resume threads
				// Capture end point - thread must run only ONE cycle
				// Release start point - allow thread to run
				EnterCriticalSection		(&mt_csLeave);
				LeaveCriticalSection		(&mt_csEnter);

				Statistic.RenderTOTAL_Real.FrameStart	();
				Statistic.RenderTOTAL_Real.Begin		();
				if (bActive)							{
					if (Begin())				{
						seqRender.Process					(rp_Render);
						Statistic.Show						();
						End									();
					}
				}
				Statistic.RenderTOTAL_Real.End			();
				Statistic.RenderTOTAL_Real.FrameEnd		();
				Statistic.RenderTOTAL.accum	= Statistic.RenderTOTAL_Real.accum;

				// *** Suspend threads
				// Capture startup point
				// Release end point - allow thread to wait for startup point
				EnterCriticalSection					(&mt_csEnter);
				LeaveCriticalSection					(&mt_csLeave);

				// Ensure, that second thread gets chance to execute anyway
				if (dwFrame!=mt_Thread_marker)			seqFrameMT.Process	(rp_Frame);
			} else {
				Sleep	(100);
			}
        }
    }

	seqAppCycleEnd.Process	(rp_AppCycleEnd);

	// Stop Balance-Thread
	mt_bMustExit = TRUE;
	LeaveCriticalSection	(&mt_csEnter);
	while (mt_bMustExit)	Sleep(0);
	DeleteCriticalSection	(&mt_csEnter);
	DeleteCriticalSection	(&mt_csLeave);
}

void CRenderDevice::FrameMove()
{
	dwFrame++;

	// Timer
	float fPreviousFrameTime = Timer.GetElapsed_sec(); Timer.Start();	// previous frame
	fTimeDelta = 0.1f * fTimeDelta + 0.9f*fPreviousFrameTime;	// smooth random system activity - worst case ~7% error
	if (fTimeDelta>.06666f) fTimeDelta=.06666f;					// limit to 15fps minimum

	u64	qTime		= TimerGlobal.GetElapsed_clk();
	fTimeGlobal		= float(qTime)*CPU::cycles2seconds;

	dwTimeGlobal	= u32((qTime*u64(1000))/CPU::cycles_per_second);
	dwTimeDelta		= iFloor(fTimeDelta*1000.f+0.5f);

	// Frame move
	Statistic.EngineTOTAL.Begin	();
	seqFrame.Process(rp_Frame);
	Statistic.EngineTOTAL.End	();
}
