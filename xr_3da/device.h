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
	
	void									_Create		(LPCSTR shName);
	void									_Destroy	(BOOL	bKeepTextures);
public:
    HWND									m_hWnd;
	LRESULT									MsgProc	(HWND,UINT,WPARAM,LPARAM);

	DWORD									dwFrame;

	DWORD									dwWidth, dwHeight;
	float									fWidth_2, fHeight_2;
	BOOL									bReady;
	BOOL									bActive;

	void __cdecl							Fatal	(const char* F,...);
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
	float									fASPECT;
	IC void									set_xform			(DWORD ID, const Fmatrix& M);
	IC void									set_xform_world		(const Fmatrix& M)	{ set_xform(D3DTS_WORLD,M);			}
	IC void									set_xform_view		(const Fmatrix& M)	{ set_xform(D3DTS_VIEW,M);			}
	IC void									set_xform_project	(const Fmatrix& M)	{ set_xform(D3DTS_PROJECTION,M);	}
	
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
	void Reset			(LPCSTR shName="shaders.xr", BOOL bKeepTextures=FALSE);

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

IC void	CRenderDevice::set_xform	(DWORD ID, const Fmatrix& M)
{
	Statistic.dwXFORMs++;
	CHK_DX(HW.pDevice->SetTransform((D3DTRANSFORMSTATETYPE)ID,(D3DMATRIX*)&M));
}

#include "TextureManager_Runtime.h"
#include "PrimitivesR_Runtime.h"

#define		REQ_CREATE()	if (!Device.bReady)	return;
#define		REQ_DESTROY()	if (Device.bReady)	return;

#endif
