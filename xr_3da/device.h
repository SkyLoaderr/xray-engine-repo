#ifndef xr_device
#define xr_device
#pragma once

// Note:
// ZNear - always 0.0f
// ZFar  - always 1.0f

#include "pure.h"
#include "hwcaps.h"
#include "hw.h"
#include "texturemanager.h"
#include "ftimer.h"
#include "stats.h"
#include "primitivesR.h"
#include "xr_effgamma.h"
#include "PSLibrary.h"

#define VIEWPORT_NEAR 0.2f

// refs
class ENGINE_API CRenderDevice 
{
private:
    // Main objects used for creating and rendering the 3D scene
    DWORD									m_dwWindowStyle;
    RECT									m_rcWindowBounds;
    RECT									m_rcWindowClient;

	DWORD									Timer_MM_Delta;
	CTimer									Timer;
	CTimer									TimerGlobal;
public:
    HWND									m_hWnd;
	LRESULT									MsgProc	(HWND,UINT,WPARAM,LPARAM);

	DWORD									dwFrame;

	DWORD									dwWidth, dwHeight;
	float									fWidth_2, fHeight_2;
	BOOL									bReady;
	BOOL									bActive;

	__declspec(noreturn) void __cdecl		Fatal	(const char* F,...);
	void									Error	(HRESULT,LPCSTR,int);
public:
	// Registrators
	CRegistrator	<pureDeviceDestroy	 >	seqDevDestroy;
	CRegistrator	<pureDeviceCreate	 >	seqDevCreate;
	CRegistrator	<pureFrame			 >	seqFrame;
	CRegistrator	<pureFrame			 >	seqFrameMT;
	CRegistrator	<pureRender			 >	seqRender;
	CRegistrator	<pureAppActivate	 >	seqAppActivate;
	CRegistrator	<pureAppDeactivate	 >	seqAppDeactivate;

	// Dependent classes
	CShaderManager							Shader;
	CStats									Statistic;
	CDraw									Primitive;
	CGammaControl							Gamma;
	CPSLibrary								PSLib;

	// Shared Streams
	CSharedStreams							Streams;
	IDirect3DIndexBuffer8*					Streams_QuadIB;

	// Engine flow-control
	float									fTimeDelta;
	float									fTimeGlobal;
	DWORD									dwTimeDelta;
	DWORD									dwTimeGlobal;

	// Cameras & projection
	Fvector									vCameraPosition;
	Fvector									vCameraDirection;
	Fvector									vCameraTop;
	Fvector									vCameraRight;
	Fmatrix									mView;
	Fmatrix									mProject;
	Fmatrix									mFullTransform;
	float									fFOV;


	CRenderDevice() {
	    m_hWnd              = NULL;
		bActive				= FALSE;
		bReady				= FALSE;
		Timer.Start			();
	};

	// Scene control
	void Begin			(void);
	void End			(void);
	void FrameMove		(void);
	
	void overdrawBegin	(void);
	void overdrawEnd	(void);

	// Mode control
	void DumpFlags		(void);
	u32	 TimerAsync		(void)
	{
		u64	qTime		= TimerGlobal.GetElapsed();
		return u32((qTime*u64(1000))/CPU::cycles_per_second);
	}
	u32	 TimerAsyncMM	(void)
	{
		return TimerAsync()+Timer_MM_Delta;
	}

	// Creation & Destroying
	void Create			(void);
	void Run			(void);
	void Destroy		(void);

	void Initialize		(void);
	void ShutDown		(void);

	// Sprite rendering
	IC float _x2real	(float x)	{ return (x+1)*fWidth_2;	}
	IC float _y2real	(float y)	{ return (y+1)*fHeight_2;	}

	// Multi-threading
	CRITICAL_SECTION	mt_csEnter;
	CRITICAL_SECTION	mt_csLeave;
	volatile BOOL		mt_bMustExit;
};

extern ENGINE_API CRenderDevice Device;

#endif
