#include "stdafx.h"
#include "frustum.h"

ENGINE_API CRenderDevice Device;

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
		CHK_DX			(HW.pDevice->Clear(0,0,D3DCLEAR_TARGET,D3DCOLOR_ARGB(0,0,0,0),1,0));
		if (0==dwPrecacheFrame)
		{
			Engine.mem_Compact	();
			Msg					("* MEMORY USAGE: %d K",Engine.mem_Usage()/1024);
		}
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

void CRenderDevice::PreCache	(u32 amount)
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
	CloseLogWindow				();
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
				} else {
					// Frustum geom
/*
					CFrustum	draw;
					Fplane		P;
					Fvector		p	= Device.vCameraPosition;
					mFullTransform.mul				( mProject,mView	);
					draw.CreateFromMatrix			(mFullTransform,FRUSTUM_P_ALL);
					for (int _it=0; _it<6; _it++)
					{
						P.n.invert	(draw.planes[_it].n);
						P.d			= -draw.planes[_it].d;
						R_CHK		(HW.pDevice->SetClipPlane(_it,(float*)&P));
					}
					R_CHK(HW.pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 63));
*/

/*
					// Shadow-test
					Fmatrix		mCam;		mCam.invert	(mView);
					Fmatrix		L_view;
					Fmatrix		L_project;
					float		p_FOV				= fFOV;
					float		p_A					= fASPECT;
					float		p_FAR				= 20.f;
					{
						// calc window extents in camera coords
						float YFov			=	deg2rad(p_FOV*p_A);
						float XFov			=	deg2rad(p_FOV);
						float wR			=	tanf(XFov*0.5f);
						float wL			=	-wR;
						float wT			=	tanf(YFov*0.5f);
						float wB			=	-wT;

						// calculate the corner vertices of the window
						// R,N,D,P = i,j,k,c
						Fvector	sPts	[4];  // silhouette points (corners of window)
						Fvector	Offset,	T;
						Offset.add			(mCam.k,mCam.c);
						sPts[0].mul			(mCam.i,wR);		T.mad(Offset,mCam.j,wT);	sPts[0].add(T);
						sPts[1].mul			(mCam.i,wL);		T.mad(Offset,mCam.j,wT);	sPts[1].add(T);
						sPts[2].mul			(mCam.i,wL);		T.mad(Offset,mCam.j,wB);	sPts[2].add(T);
						sPts[3].mul			(mCam.i,wR);		T.mad(Offset,mCam.j,wB);	sPts[3].add(T);

						// find projector direction vectors (from cop through silhouette pts)
						Fvector ProjDirs[4];
						ProjDirs[0].sub		(sPts[0],mCam.c);	//ProjDirs[0].normalize();
						ProjDirs[1].sub		(sPts[1],mCam.c);	//ProjDirs[1].normalize();
						ProjDirs[2].sub		(sPts[2],mCam.c);	//ProjDirs[2].normalize();
						ProjDirs[3].sub		(sPts[3],mCam.c);	//ProjDirs[3].normalize();

						// that's all 5 corner points, excluding "near" plane
						Fvector _F[5];
						_F[0].mad			(mCam.c, ProjDirs[0], p_FAR);
						_F[1].mad			(mCam.c, ProjDirs[1], p_FAR);
						_F[2].mad			(mCam.c, ProjDirs[2], p_FAR);
						_F[3].mad			(mCam.c, ProjDirs[3], p_FAR);
						_F[4].set			(mCam.c);

						// Build L-view vectors
						Fvector				L_dir,L_up,L_right,L_pos;
						float				cs	= 1000;
						L_dir.set			(0.100f, -0.540f, -0.836f);		L_dir.normalize	();
						L_up.set			(0,1,0);						L_up.normalize	();
						L_right.crossproduct(L_up,L_dir);
						L_up.crossproduct	(L_dir,L_right);
						L_pos.set			(0,0,0);
						L_view.build_camera_dir	(L_pos,L_dir,L_up);

						//
						Fbox bb;
						Fvector bbc,bbd;

						// L-view corner points and box
						bb.invalidate			();
						for (int i=0; i<5; i++)
						{
							L_view.transform_tiny	(T,_F[i]);
							bb.modify				(T);
						}
						bb.get_CD				(bbc,bbd);

						// Back project center
						Fmatrix inv;
						inv.invert				(L_view);
						inv.transform_tiny		(L_pos,bbc);

						// L-view matrix
						L_pos.mad				(L_dir, -cs);
						L_view.build_camera_dir	(L_pos,L_dir,L_up);

						// L-view corner points and box
						bb.invalidate			();
						for (int i=0; i<5; i++)
						{
							L_view.transform_tiny	(T,_F[i]);
							bb.modify				(T);
						}
						bb.get_CD				(bbc,bbd);

						// L_project
						// D3DXMatrixPerspectiveOffCenterLH	((D3DXMATRIX*)&L_project,bb.min.x,bb.max.x,bb.min.y,bb.max.y,cs-50.f,cs+50.f);
						// L_project.identity			();
						// D3DXMatrixOrthoLH				((D3DXMATRIX*)&L_project,d,d,cs-1050.f,cs+50.f);
						float				d			= 2*_max(bbd.x,bbd.y);
						D3DXMatrixOrthoLH				((D3DXMATRIX*)&L_project,2*bbd.x,2*bbd.y,cs-50.f,cs+50.f);
						// L_project.build_projection	(deg2rad(90.f),1.f,0.2f,100.f);
					}

					mView	= L_view;
					mProject= L_project;
*/
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

	seqAppCycleEnd.Process	(rp_AppCycleEnd);

	// Stop Balance-Thread
	mt_bMustExit = TRUE;
	LeaveCriticalSection	(&mt_csEnter);
	while (mt_bMustExit)	Sleep(0);
	DeleteCriticalSection	(&mt_csEnter);
	DeleteCriticalSection	(&mt_csLeave);

	Destroy					();
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

	dwTimeGlobal	= u32((qTime*u64(1000))/CPU::cycles_per_second);
	dwTimeDelta		= iFloor(fTimeDelta*1000.f+0.5f);

	// Frame move
	Statistic.EngineTOTAL.Begin	();
	seqFrame.Process(rp_Frame);
	Statistic.EngineTOTAL.End	();
}
