#ifndef DeviceH
#define DeviceH

#include "ui_camera.h"
#include "hwcaps.h"
#include "hw.h"
#include "texturemanager.h"
#include "pure.h"
#include "ftimer.h"
#include "estats.h"
#include "primitivesR.h"
#include "shader_xrlc.h"
#include "ModelPool.h"
//---------------------------------------------------------------------------
// refs
class CGameFont;
class CInifile;

//------------------------------------------------------------------------------
class ENGINE_API CRenderDevice{
    friend class 			CUI_Camera;
    friend class 			TUI;

    float 					m_fNearer;

	DWORD					Timer_MM_Delta;
	CTimer					Timer;
	CTimer					TimerGlobal;

    Shader*					m_CurrentShader;

	void					_Create		(CStream* F);
	void					_Destroy	(BOOL	bKeepTextures);
public:
    Shader*					m_WireShader;
    Shader*					m_SelectionShader;

    Fmaterial				m_DefaultMat;           

    CModelPool				Models;
public:
    u32 					dwWidth, dwHeight;
    u32 					m_RenderWidth_2, m_RenderHeight_2;
    u32 					m_RealWidth, m_RealHeight;
    float					m_RenderArea;
    float 					m_ScreenQuality;

	DWORD 					dwFillMode;
    DWORD					dwShadeMode;
public:
    HANDLE 					m_hWnd;
    HANDLE 					m_hRenderWnd;

    IC void					SetHandle(HANDLE main_hwnd, HANDLE render_hwnd){m_hWnd=main_hwnd; m_hRenderWnd=render_hwnd;}

	DWORD					dwFrame;

	BOOL					bReady;
	BOOL					bActive;

	// Engine flow-control
	float					fTimeDelta;
	float					fTimeGlobal;
	DWORD					dwTimeDelta;
	DWORD					dwTimeGlobal;

    // camera
	CUI_Camera 				m_Camera;
	Fmatrix					mView;
	Fmatrix 				mProjection;
	Fmatrix					mFullTransform;

	CDraw					Primitive;
	// Shared Streams
	CSharedStreams	 	  	Streams;

	CStats					Statistic;

	CGameFont* 				pSystemFont;

	// registrators
	CRegistrator <pureDeviceDestroy>	seqDevDestroy;
	CRegistrator <pureDeviceCreate>		seqDevCreate;
	CRegistrator <pureFrame>			seqFrame;
public:
							CRenderDevice 	();
    virtual 				~CRenderDevice	();

	bool 					Create			();
	void 					Destroy			();
    void 					Resize			(int w, int h, bool bRefreshDevice=true);
	void 					RefreshTextures	(LPSTRVec* modif);
	void 					ReloadTextures	();
	void 					UnloadTextures	();

    void 					RenderNearer	(float f_Near);
    void 					ResetNearer		();

	void 					Begin			();
	void 					End				();

	void 					Initialize		(void);
	void 					ShutDown		(void);
	void 					Reset			(CStream* F, BOOL bKeepTextures);

    IC float				GetRenderArea	(){return m_RenderArea;}
	// Sprite rendering
	IC float 				_x2real			(float x)	
    { return (x+1)*m_RenderWidth_2;	}
	IC float 				_y2real			(float y)	
    { return (y+1)*m_RenderHeight_2;}

    IC void					SetViewport		(u32 x, u32 y, u32 width, u32 height, float minZ=0.f, float maxZ=1.f)
    { VERIFY(bReady); D3DVIEWPORT8 VP={x,y,width,height,minZ,maxZ}; CHK_DX(HW.pDevice->SetViewport(&VP)); }
    IC void					ResetViewport	()
    { VERIFY(bReady); D3DVIEWPORT8 VP={0,0,dwWidth,dwHeight,0.f,1.f}; CHK_DX(HW.pDevice->SetViewport(&VP)); }

    IC void					SetTexture		(DWORD dwStage, IDirect3DTexture8* lpTexture)
    { VERIFY(bReady); CHK_DX(HW.pDevice->SetTexture( dwStage, lpTexture )); }
    IC void					SetTSS			(DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwState, DWORD dwValue)
    { VERIFY(bReady); CHK_DX(HW.pDevice->SetTextureStageState(dwStage,dwState,dwValue)); }
    IC void					SetRS			(D3DRENDERSTATETYPE p1, DWORD p2)
    { VERIFY(bReady); CHK_DX(HW.pDevice->SetRenderState(p1,p2)); }
    IC void					SetTransform	(D3DTRANSFORMSTATETYPE p1, const Fmatrix& m)
    { VERIFY(bReady); CHK_DX(HW.pDevice->SetTransform(p1,(D3DMATRIX*)&m)); }

	IC void					set_xform		(DWORD ID, const Fmatrix& m)
    { SetTransform((D3DTRANSFORMSTATETYPE)ID,m); }
	IC void					set_xform_world	(const Fmatrix& M)	
    { set_xform(D3DTS_WORLD,M);			}
	IC void					set_xform_view	(const Fmatrix& M)	
    { set_xform(D3DTS_VIEW,M);			}
	IC void					set_xform_project(const Fmatrix& M)	
    { set_xform(D3DTS_PROJECTION,M);	}

	// draw
	void			   		SetShader		(Shader* sh){m_CurrentShader = sh;}
	void			   		DP				(D3DPRIMITIVETYPE pt, CVS* vs, IDirect3DVertexBuffer8* vb, DWORD vBase, DWORD pc);
	IC void			   		DP				(D3DPRIMITIVETYPE pt, CVS* vs, DWORD vBase, DWORD pc)
    { DP(pt,vs,Streams.Vertex.Buffer(),vBase,pc); }
	void 					DIP				(D3DPRIMITIVETYPE pt, CVS* vs, IDirect3DVertexBuffer8* vb, DWORD vBase, DWORD vc, IDirect3DIndexBuffer8* ib, DWORD iBase, DWORD pc);
	IC void 				DIP				(D3DPRIMITIVETYPE pt, CVS* vs, DWORD vBase, DWORD vc, DWORD iBase, DWORD pc)
    { DIP(pt,vs,Streams.Vertex.Buffer(),vBase,vc,Streams.Index.Buffer(),iBase,pc); }

    // light&material
    IC void					LightEnable		(DWORD dwLightIndex, BOOL bEnable)
    { CHK_DX(HW.pDevice->LightEnable(dwLightIndex, bEnable));}
    IC void					SetLight		(DWORD dwLightIndex, Flight& lpLight)
    { CHK_DX(HW.pDevice->SetLight(dwLightIndex, lpLight.d3d()));}
	IC void					SetMaterial		(Fmaterial& mat)
    { CHK_DX(HW.pDevice->SetMaterial(mat.d3d()));}
	IC void					ResetMaterial	()
    { CHK_DX(HW.pDevice->SetMaterial(m_DefaultMat.d3d()));}

	// update
    void					UpdateView		();
	void					UpdateTimer		();

    bool					MakeScreenshot	(U32Vec& pixels, u32& width, u32& height);

	void 					InitTimer		();
	// Mode control
	IC u32	 				TimerAsync		(void)
    { u64 qTime = TimerGlobal.GetElapsed(); return u32((qTime*u64(1000))/CPU::cycles_per_second); }
	IC u32	 				TimerAsyncMM	(void)
    { return TimerAsync()+Timer_MM_Delta; }
public:
	CShaderManager			Shader;
    Shader_xrLC_LIB			ShaderXRLC;
};

extern ENGINE_API CRenderDevice Device;

// video
enum {
	rsFilterLinear		= (1ul<<20ul),
    rsEdgedFaces		= (1ul<<21ul),
	rsRenderTextures	= (1ul<<22ul),
	rsLighting			= (1ul<<23ul),
    rsFog				= (1ul<<24ul),
    rsRenderRealTime	= (1ul<<25ul),
    rsDrawGrid			= (1ul<<26ul),
    rsDrawSafeRect		= (1ul<<27ul),
};

#define DEFAULT_CLEARCOLOR 0x00555555
extern DWORD dwClearColor;

// textures
#include "TextureManager_Runtime.h"
#include "PrimitivesR_Runtime.h"

#define		REQ_CREATE()	if (!Device.bReady)	return;
#define		REQ_DESTROY()	if (Device.bReady)	return;

#include "xrCPU_Pipe.h"
ENGINE_API extern xrDispatchTable	PSGP;

#endif
