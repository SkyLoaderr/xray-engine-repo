#ifndef DeviceH
#define DeviceH

#include "ui_camera.h"
#include "frustum.h"
#include "ShaderManager.h"
#include "hw.h"
#include "ftimer.h"
#include "estats.h"
#include "primitivesR.h"
//---------------------------------------------------------------------------
// refs
class Shader;
class CFontHUD;

//------------------------------------------------------------------------------
class ENGINE_API CRenderDevice{
    friend class 			CUI_Camera;
    friend class 			TUI;

    float 					m_fNearer;

    Shader*					m_NullShader;

	DWORD					Timer_MM_Delta;
	CTimer					Timer;
	CTimer					TimerGlobal;
public:
    Shader*					m_WireShader;
    Shader*					m_SelectionShader;

    Fmaterial				m_DefaultMat;

    CFrustum    			m_Frustum;

	CShaderManager			Shader;
public:
    int 					dwWidth, dwHeight;
    int 					m_RenderWidth_2, m_RenderHeight_2;
    int 					m_RealWidth, m_RealHeight;
    float					m_RenderArea;
    float 					m_ScreenQuality;
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
public:
							CRenderDevice 	();
    virtual 				~CRenderDevice	();

	bool 					Create			(HANDLE handle);
	void 					Destroy			();
    void 					Resize			(int w, int h);
    void					ReloadShaders	();
	void 					RefreshTextures	(bool bOnlyNew);

    void 					RenderNearer	(float f_Near);
    void 					ResetNearer		();

	void 					Begin			();
	void 					End				();

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
    void					UpdateFog		();

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
};

extern ENGINE_API CRenderDevice Device;

// video
enum {
	rsFullscreen		= (1ul<<0ul),
	rsTriplebuffer		= (1ul<<1ul),
	rsClearBB			= (1ul<<2ul),
	rsNoVSync			= (1ul<<3ul),
	rsWireframe			= (1ul<<4ul),
	rsAntialias			= (1ul<<5ul),
	rsNormalize			= (1ul<<6ul),
	rsOverdrawView		= (1ul<<7ul),
	rsOcclusion			= (1ul<<8ul),
	rsDepthEnhance		= (1ul<<9ul),
	rsAnisotropic		= (1ul<<10ul),
	rsStatistic			= (1ul<<11ul),
	mtSound				= (1ul<<24ul),
	mtInput				= (1ul<<25ul)
};
extern DWORD psDeviceFlags;

#endif
