#ifndef DeviceH
#define DeviceH

#include "ui_camera.h"
#include "frustum.h"
#include "Statistic.h"
#include "ShaderManager.h"
#include "hw.h"
#include "ftimer.h"
#include "primitivesR.h"
//---------------------------------------------------------------------------
// refs
class Shader;

//------------------------------------------------------------------------------
class ENGINE_API CRenderDevice{
    friend class 			CUI_Camera;
    friend class 			TUI;

    float 					m_fNearer;

    int 					m_RenderWidth, m_RenderHeight;
    int 					m_RenderWidth_2, m_RenderHeight_2;
    int 					m_RealWidth, m_RealHeight;
    float					m_RenderArea;
    float 					m_ScreenQuality;

    Shader*					m_NullShader;
	CTimer					Timer;
	CTimer					TimerGlobal;
public:
    Shader*					m_WireShader;
    Shader*					m_SelectionShader;

    Fmaterial				m_DefaultMat;

    CFrustum    			m_Frustum;
    CStatistic     			m_Statistic;

	CShaderManager			Shader;
// new
public:
    HANDLE 					m_Handle;

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

	// Shared Streams
	CSharedStreams	 	  	Streams;
	IDirect3DIndexBuffer8*	Streams_QuadIB;
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
	void			   		DP				(D3DPRIMITIVETYPE pt, DWORD vtd, LPVOID v, DWORD vc);
//	void					DPS				(D3DPRIMITIVETYPE pt, DWORD vtd, LPD3DDRAWPRIMITIVESTRIDEDDATA va, DWORD vc);
	void					DIP				(D3DPRIMITIVETYPE pt, DWORD vtd, LPVOID v, DWORD vc, LPWORD i, DWORD ic);
//	void					DIPS			(D3DPRIMITIVETYPE pt, DWORD vtd, LPD3DDRAWPRIMITIVESTRIDEDDATA va, DWORD vc, LPWORD i, DWORD ic);
//	void					DPVB			(D3DPRIMITIVETYPE pt, LPDIRECT3DVERTEXBUFFER7 vb, DWORD sv, DWORD nv);

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
};

extern ENGINE_API CRenderDevice Device;

#endif
