#ifndef DeviceH
#define DeviceH

#include "ui_camera.h"
#include "frustum.h"
#include "Statistic.h"
#include "ShaderManager.h"
//---------------------------------------------------------------------------
struct strDX{
	LPDIRECT3D7      		pD3D;
	LPDIRECT3DDEVICE7		pD3DDev;
	LPDIRECTDRAW7			pDD;
	LPDIRECTDRAWSURFACE7	pBackBuffer;
	DDSURFACEDESC2			BackDesc;
};

extern "C" __declspec(dllimport) HRESULT CreateTexture(LPDIRECTDRAWSURFACE7& pTexture, LPSTR pSrcName, LPSTR pAlternateSrcName, int* w, int* h, BOOL* bAlpha);
extern "C" __declspec(dllexport) void ReleaseTexture(LPDIRECTDRAWSURFACE7& pTexture);
extern "C" __declspec(dllimport) HRESULT ReleaseD3DX();
extern "C" __declspec(dllimport) HRESULT InitD3DX(HWND hWnd, strDX** dx, DWORD mode);
extern "C" __declspec(dllimport) HRESULT BeginDraw();
extern "C" __declspec(dllimport) HRESULT EndDraw();
extern "C" __declspec(dllimport) HRESULT ResizeD3DX(int X, int Y);

// refs
class Shader;

//------------------------------------------------------------------------------
class CDevice{
    friend class 	CUI_Camera;
    friend class 	TUI;
    strDX*  		m_DX;

	long 			m_FramePrevTime;

    float 			m_fNearer;

    int 			m_RenderWidth, m_RenderHeight;
    int 			m_RealWidth, m_RealHeight;
    float			m_RenderArea;
    float 			m_ScreenQuality;

    Shader*			m_NullShader;
public:
    Shader*			m_WireShader;
    Shader*			m_SelectionShader;

    D3DDEVICEDESC7 	m_Caps;

	float 			m_FrameDTime;
    float			m_fTimeGlobal;

	Fmatrix			m_View;
	Fmatrix 		m_Projection;
	Fmatrix 		m_FullTransform;

    Fmaterial		m_DefaultMat;
    
	CUI_Camera 		m_Camera;
    CFrustum    	m_Frustum;
    CStatistic     	m_Statistic;

    bool 			m_bReady;

	CShaderManager	Shader;
public:
					CDevice		();
    virtual 		~CDevice	();

	bool 			Create		(HANDLE handle);
	void 			Destroy		();
    void 			Resize		(int w, int h);
    void			ReloadShaders();
	void 			RefreshTextures(bool bOnlyNew);

    void 			RenderNearer(float f_Near);
    void 			ResetNearer	();

	void 			Begin		();
	void 			End			();

    IC float		GetRenderArea(){return m_RenderArea;}

    IC void			SetTexture	(DWORD dwStage, LPDIRECTDRAWSURFACE7 lpTexture){
    								VERIFY(m_bReady);
                                    CDX(m_DX->pD3DDev->SetTexture( dwStage, lpTexture ));
							    }
    IC void			SetTSS		(DWORD dwStage, D3DTEXTURESTAGESTATETYPE dwState, DWORD dwValue){
    								VERIFY(m_bReady);
                                    CDX(m_DX->pD3DDev->SetTextureStageState(dwStage,dwState,dwValue));
								}
    IC void			SetRS		(D3DRENDERSTATETYPE p1, DWORD p2){
    								VERIFY(m_bReady);
                                    CDX(m_DX->pD3DDev->SetRenderState(p1,p2));
								}
    IC void			SetTransform(D3DTRANSFORMSTATETYPE p1, const Fmatrix& m){
    								VERIFY(m_bReady);
                                    CDX(m_DX->pD3DDev->SetTransform(p1,m.d3d()));
							    } 
	IC LPDIRECT3DDEVICE7 GetDevice(){return m_DX->pD3DDev;}
	IC LPDIRECT3D7  GetD3D      (){return m_DX->pD3D;}

	// draw
    void			DP			(D3DPRIMITIVETYPE pt, DWORD vtd, LPVOID v, DWORD vc);
    void			DPS			(D3DPRIMITIVETYPE pt, DWORD vtd, LPD3DDRAWPRIMITIVESTRIDEDDATA va, DWORD vc);
    void			DIP			(D3DPRIMITIVETYPE pt, DWORD vtd, LPVOID v, DWORD vc, LPWORD i, DWORD ic);
	void			DIPS		(D3DPRIMITIVETYPE pt, DWORD vtd, LPD3DDRAWPRIMITIVESTRIDEDDATA va, DWORD vc, LPWORD i, DWORD ic);
    void			DPVB		(D3DPRIMITIVETYPE pt, LPDIRECT3DVERTEXBUFFER7 vb, DWORD sv, DWORD nv);

    // light&material
    IC void			LightEnable	(DWORD dwLightIndex, BOOL bEnable){
                                CDX(m_DX->pD3DDev->LightEnable(dwLightIndex, bEnable));}
    IC void			SetLight	(DWORD dwLightIndex, Flight& lpLight){
                                CDX(m_DX->pD3DDev->SetLight(dwLightIndex, lpLight.d3d()));}
	IC void			SetMaterial	(Fmaterial& mat){
                                CDX(m_DX->pD3DDev->SetMaterial(mat.d3d()));}
	IC void			ResetMaterial(){
                                CDX(m_DX->pD3DDev->SetMaterial(m_DefaultMat.d3d()));}

	// state blocks
    void			Validate	();
	IC void			DeleteSB	(DWORD dwBlockHandle){ 
                                CDX(m_DX->pD3DDev->DeleteStateBlock(dwBlockHandle));}	
	IC void			EndSB		(LPDWORD lpdwBlockHandle){
                                CDX(m_DX->pD3DDev->EndStateBlock(lpdwBlockHandle));}	
	IC void			BeginSB		(){	
                                CDX(m_DX->pD3DDev->BeginStateBlock());}
	IC void			ApplySB		(DWORD dwBlockHandle){
                                CDX(m_DX->pD3DDev->ApplyStateBlock(dwBlockHandle));}	

    void			UpdateView	();
	void			UpdateTimer	();
    void			UpdateFog	();

    bool			MakeScreenshot(DWORDVec& pixels, DWORD& width, DWORD& height);
    
    void			OnDeviceCreate();
    void			OnDeviceDestroy();
};

#endif
