#ifndef DeviceH
#define DeviceH

#include "ui_camera.h"
#include "frustum.h"
#include "texturemanager.h"
#include "hw.h"
#include "pure.h"
#include "ftimer.h"
#include "estats.h"
#include "primitivesR.h"
//---------------------------------------------------------------------------
// refs
class CFontHUD;

//------------------------------------------------------------------------------
class ENGINE_API CRenderDevice{
    friend class 			CUI_Camera;
    friend class 			TUI;

    float 					m_fNearer;

	DWORD					Timer_MM_Delta;
	CTimer					Timer;
	CTimer					TimerGlobal;

    Shader*					m_CurrentShader;
public:
    Shader*					m_WireShader;
    Shader*					m_SelectionShader;

    Fmaterial				m_DefaultMat;

    CFrustum    			m_Frustum;
public:
    int 					dwWidth, dwHeight;
    int 					m_RenderWidth_2, m_RenderHeight_2;
    int 					m_RealWidth, m_RealHeight;
    float					m_RenderArea;
    float 					m_ScreenQuality;

	DWORD 					dwFillMode;
    DWORD					dwShadeMode;
public:
    HANDLE 					m_hWnd;
    HANDLE 					m_hRenderWnd;

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
	IDirect3DIndexBuffer8*	Streams_QuadIB;

	CStats					Statistic;

	CFontHUD* 				pHUDFont;

	// registrators
	CRegistrator <pureDeviceDestroy>	seqDevDestroy;
	CRegistrator <pureDeviceCreate>		seqDevCreate;
	CRegistrator <pureFrame>			seqFrame;
public:
							CRenderDevice 	();
    virtual 				~CRenderDevice	();

	bool 					Create			();
	void 					Destroy			();
    void 					Resize			(int w, int h);
    void					ReloadShaders	();
	void 					RefreshTextures	(bool bOnlyNew);

    void 					RenderNearer	(float f_Near);
    void 					ResetNearer		();

	void 					Begin			();
	void 					End				();

	void 					Initialize		(void);
	void 					ShutDown		(void);

    IC float				GetRenderArea	(){return m_RenderArea;}
	// Sprite rendering
	IC float 				_x2real			(float x)	{ return (x+1)*m_RenderWidth_2;	}
	IC float 				_y2real			(float y)	{ return (y+1)*m_RenderHeight_2;}

    IC void					SetTexture		(DWORD dwStage, IDirect3DTexture8* lpTexture){
    											VERIFY(bReady);
	                          		      	    CHK_DX(HW.pDevice->SetTexture( dwStage, lpTexture ));
									   	 	}
    IC void					SetTSS			(DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwState, DWORD dwValue){
    											VERIFY(bReady);
                        		            	CHK_DX(HW.pDevice->SetTextureStageState(dwStage,dwState,dwValue));
											}
    IC void					SetRS			(D3DRENDERSTATETYPE p1, DWORD p2){
    											VERIFY(bReady);
                   		                	 	CHK_DX(HW.pDevice->SetRenderState(p1,p2));
											}
    IC void					SetTransform	(D3DTRANSFORMSTATETYPE p1, const Fmatrix& m){
    											VERIFY(bReady);
                               		  		   	CHK_DX(HW.pDevice->SetTransform(p1,m.d3d()));
							   			 	}
	// draw
	void			   		SetShader		(Shader* sh){m_CurrentShader = sh;}
	void			   		DP				(D3DPRIMITIVETYPE pt, CVertexStream* VS, DWORD vBase, DWORD pc);
	void 					DIP				(D3DPRIMITIVETYPE pt, CVertexStream* vs, DWORD vBase, DWORD vc, CIndexStream* is, DWORD iBase, DWORD pc);

    // light&material
    IC void					LightEnable		(DWORD dwLightIndex, BOOL bEnable){
                                			CHK_DX(HW.pDevice->LightEnable(dwLightIndex, bEnable));}
    IC void					SetLight		(DWORD dwLightIndex, Flight& lpLight){
                                			CHK_DX(HW.pDevice->SetLight(dwLightIndex, lpLight.d3d()));}
	IC void					SetMaterial		(Fmaterial& mat){
                         			       	CHK_DX(HW.pDevice->SetMaterial(mat.d3d()));}
	IC void					ResetMaterial	(){
                                			CHK_DX(HW.pDevice->SetMaterial(m_DefaultMat.d3d()));}

	// update
    void					Validate		();
    void					UpdateView		();
	void					UpdateTimer		();
    void					UpdateFog		(DWORD color, float fogness, float view_dist);
#ifdef _LEVEL_EDITOR
	void 					UpdateFog		();
#endif

    bool					MakeScreenshot	(DWORDVec& pixels, DWORD& width, DWORD& height);

    void					OnDeviceCreate	();
    void					OnDeviceDestroy	();

	void 					InitTimer		();
	// Mode control
	IC u32	 				TimerAsync		(void){
		u64	qTime		= TimerGlobal.GetElapsed();
		return u32((qTime*u64(1000))/CPU::cycles_per_second);
	}
	IC u32	 				TimerAsyncMM	(void){
		return TimerAsync()+Timer_MM_Delta;
	}

	__declspec(noreturn) void __cdecl		Fatal	(const char* F,...);
	void									Error	(HRESULT,LPCSTR,int);
public:
	CShaderManager			Shader;
};

extern ENGINE_API CRenderDevice Device;

// video
enum {
    rsFilterLinear		= (1ul<<0ul),
    rsEdgedFaces		= (1ul<<1ul),
	rsRenderTextures	= (1ul<<2ul),
	rsLighting			= (1ul<<3ul),
    rsFog				= (1ul<<4ul),
    rsRenderRealTime	= (1ul<<5ul),
    rsDrawGrid			= (1ul<<6ul),
	rsStatistic			= (1ul<<7ul),
    rsForceHWTransform	= (1ul<<8ul),

	mtSound				= (1ul<<24ul),
	mtInput				= (1ul<<25ul)
};
extern DWORD psDeviceFlags;
// textures
extern int psTextureLOD;

#include "TextureManager_Runtime.h"
#include "PrimitivesR_Runtime.h"

#define		REQ_CREATE()	if (!Device.bReady)	return;
#define		REQ_DESTROY()	if (Device.bReady)	return;

#endif
