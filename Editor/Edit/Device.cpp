//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop
#include "Device.h"
#include "Scene.h"
#include "ui_main.h"
#include "library.h"

#pragma package(smart_init)

//---------------------------------------------------------------------------
CDevice::CDevice(){
    m_DX 			= 0;
    m_ScreenQuality = 1.f;
    m_RenderWidth 	= m_RenderHeight = 0;
    m_RealWidth 	= m_RealHeight = 0;
    m_bReady 		= false;
	m_Projection.identity();
    m_FullTransform.identity();
	m_FramePrevTime	= timeGetTime();
	m_FrameDTime   	= 0;
    m_fTimeGlobal	= 0;
    m_DefaultMat.set(1,1,1);
	m_NullShader	= 0;
	m_WireShader	= 0;
	m_SelectionShader = 0;
	ZeroMemory		(&m_Caps,sizeof(m_Caps));
}

CDevice::~CDevice(){
	VERIFY(!m_bReady);
}
//---------------------------------------------------------------------------
void CDevice::RenderNearer(float n){
    m_Projection._43=m_fNearer-n;
    SetTransform(D3DTRANSFORMSTATE_PROJECTION,m_Projection);
}
void CDevice::ResetNearer(){
    m_Projection._43=m_fNearer;
    SetTransform(D3DTRANSFORMSTATE_PROJECTION,m_Projection);
}
//---------------------------------------------------------------------------
bool CDevice::Create(HANDLE handle){
    if( FAILED( InitD3DX(handle, &m_DX, UI->dwRenderHWTransform))){
		Log->DlgMsg( mtError, "D3D: DirectDrawCreateEx() failed" );
		return false;
    }
    m_DX->pD3DDev->GetCaps(&m_Caps);
	m_bReady = true;
// shaders
    Shader.Initialize();
    m_NullShader = Shader.Create();
    m_WireShader = Shader.Create("$ed_wire");
    m_SelectionShader = Shader.Create("$ed_selection");

    OnDeviceCreate();

	Log->Msg( mtInformation, "D3D: initialized" );
	return true;
}

//---------------------------------------------------------------------------
void CDevice::Destroy(){
	if (m_NullShader) Shader.Delete(m_NullShader);
	if (m_WireShader) Shader.Delete(m_WireShader);
	if (m_SelectionShader) Shader.Delete(m_SelectionShader);

//    D3D_DestroyStateBlocks();
	m_bReady = false;
	OnDeviceDestroy();
    Shader.Clear();
    ReleaseD3DX();
	Log->Msg( mtInformation, "D3D: device cleared" );
}

void CDevice::OnDeviceCreate(){
	SetRS(D3DRS_COLORVERTEX,TRUE);
	SetRS(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
	SetRS(D3DRS_DITHERENABLE,		TRUE				);
	SetRS(D3DRS_STENCILENABLE,		FALSE				);
    SetRS(D3DRS_ZENABLE,			TRUE				);
	SetRS(D3DRS_CULLMODE,			D3DCULL_CCW			);
	SetRS(D3DRS_LOCALVIEWER,		FALSE				);

	SetRS(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL	);
	SetRS(D3DRS_SPECULARMATERIALSOURCE,D3DMCS_MATERIAL	);
	SetRS(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1	);
	SetRS(D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1	);

	SetRS(D3DRS_NORMALIZENORMALS,TRUE);

	float fBias = -1.f;
    for (int k=0; k<m_Caps.wMaxSimultaneousTextures; k++)
		SetTSS( k, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias)));
/*
	if (psDeviceFlags&rsWireframe)	{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_FILLMODE,			D3DFILL_WIREFRAME	)); }
	else							{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_FILLMODE,			D3DFILL_SOLID		)); }
	if (psDeviceFlags&rsAntialias)	{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS,TRUE				));	}
	else							{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS,FALSE				)); }
	if (psDeviceFlags&rsNormalize)	{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_NORMALIZENORMALS,	TRUE				)); }
	else							{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_NORMALIZENORMALS,	FALSE				)); }
*/
    ResetMaterial();
	// signal another objects
    Shader.OnDeviceCreate();
    Scene->OnDeviceCreate();

    UpdateFog();
}

void CDevice::OnDeviceDestroy(){
    Scene->OnDeviceDestroy();
    Shader.OnDeviceDestroy();
}

void CDevice::UpdateFog(){
	//Fog parameters
    st_Environment& E	= Scene->m_LevelOp.m_Envs[Scene->m_LevelOp.m_CurEnv];
    Fcolor& FogColor 	= E.m_FogColor;
    float Fogness		= (UI->bRenderFog)?E.m_Fogness:0;
    float view_dist		= (UI->bRenderFog)?E.m_ViewDist:UI->ZFar();
	SetRS( D3DRS_FOGCOLOR,	FogColor.get());
	float start	= (1.0f - Fogness)* 0.85f * view_dist;
	float end	= 0.91f * view_dist;
	if (m_Caps.dpcTriCaps.dwRasterCaps&D3DPRASTERCAPS_FOGTABLE)	{
		Log->Msg(mtInformation,"* Using hardware fog...");
		SetRS( D3DRS_RANGEFOGENABLE,FALSE				);
		SetRS( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR		);
		SetRS( D3DRS_FOGVERTEXMODE,D3DFOG_NONE			);
		if (!m_Caps.dpcTriCaps.dwRasterCaps&D3DPRASTERCAPS_WFOG) {
			start/=view_dist;
			end  /=view_dist;
		}
	} else {
		Log->Msg(mtInformation,"* Fog is emulated...");
		SetRS( D3DRS_FOGTABLEMODE,	D3DFOG_NONE			);
		SetRS( D3DRS_FOGVERTEXMODE,D3DFOG_LINEAR		);
		SetRS( D3DRS_RANGEFOGENABLE,FALSE				);
	}
	SetRS( D3DRS_FOGSTART,	*(DWORD *)(&start)	);
	SetRS( D3DRS_FOGEND,	*(DWORD *)(&end)	);
}

//---------------------------------------------------------------------------
void __fastcall CDevice::Resize(int w, int h)
{
    m_bReady = false;
	
    m_RealWidth 	= w;
    m_RealHeight 	= h;
    m_RenderArea	= w*h;

    m_RenderWidth  = m_RealWidth * m_ScreenQuality;
    m_RenderHeight = m_RealHeight * m_ScreenQuality;

    OnDeviceDestroy();
    
    if (FAILED(ResizeD3DX(m_RenderWidth, m_RenderHeight))){
        Log->DlgMsg(mtError,"Don't create DirectX device. Editor halted!");
        THROW;
    }
    m_bReady = true;

    m_Camera.m_Aspect = (float)m_RenderHeight / (float)m_RenderWidth;
    m_Projection.build_projection( m_Camera.m_FOV, m_Camera.m_Aspect, m_Camera.m_Znear, m_Camera.m_Zfar );
    m_fNearer = m_Projection._43;

    SetTransform(D3DTRANSFORMSTATE_PROJECTION,m_Projection);
    SetTransform(D3DTRANSFORMSTATE_WORLD,precalc_identity);

	OnDeviceCreate();

    UI->RedrawScene();
}

void CDevice::Begin( ){
	VERIFY(m_bReady);
    VERIFY(m_DX->pD3DDev);

    BeginDraw();
}

//---------------------------------------------------------------------------
void CDevice::End(){
	VERIFY(m_bReady);
    Shader.SetNULL();
    EndDraw();
}

void CDevice::UpdateView(){
// set camera matrix
	m_Camera.GetView(m_View);

    SetTransform(D3DTRANSFORMSTATE_VIEW,m_View);
    m_FullTransform.mul(m_Projection,m_View);

// frustum culling sets
    m_Frustum.CreateFromViewMatrix();
}

void CDevice::UpdateTimer(){
    float t = timeGetTime();
    m_FrameDTime = 0.001f * ( t - m_FramePrevTime );
	if (m_FrameDTime>0.06666f) m_FrameDTime=.06666f; // limit to 15fps minimum
    m_fTimeGlobal += m_FrameDTime;
    m_FramePrevTime = t;

    m_Camera.Update(m_FrameDTime);
}

void CDevice::DP(D3DPRIMITIVETYPE pt, DWORD vtd, LPVOID v, DWORD vc){
    DWORD dwRequired	= UI->Device.Shader.dwPassesRequired;
    for (DWORD dwPass = 0; dwPass<dwRequired; dwPass++){
        Shader.SetupPass(dwPass);
        CDX(m_DX->pD3DDev->DrawPrimitive(pt,vtd,v,vc,0));
		m_Statistic.dwRenderPolyCount += vc/3;
    }
}

void CDevice::DPS(D3DPRIMITIVETYPE pt, DWORD vtd, LPD3DDRAWPRIMITIVESTRIDEDDATA va, DWORD vc){
    DWORD dwRequired	= UI->Device.Shader.dwPassesRequired;
    for (DWORD dwPass = 0; dwPass<dwRequired; dwPass++){
        Shader.SetupPass(dwPass);
        CDX(m_DX->pD3DDev->DrawPrimitiveStrided(pt,vtd,va,vc,0));
		m_Statistic.dwRenderPolyCount += vc/3;
    }
}

void CDevice::DIP(D3DPRIMITIVETYPE pt, DWORD vtd, LPVOID v, DWORD vc, LPWORD i, DWORD ic){
    DWORD dwRequired	= UI->Device.Shader.dwPassesRequired;
    for (DWORD dwPass = 0; dwPass<dwRequired; dwPass++){
        Shader.SetupPass(dwPass);
        CDX(m_DX->pD3DDev->DrawIndexedPrimitive(pt,vtd,v,vc,i,ic,0));
		m_Statistic.dwRenderPolyCount += ic;
    }
}

void CDevice::DIPS(D3DPRIMITIVETYPE pt, DWORD vtd, LPD3DDRAWPRIMITIVESTRIDEDDATA va, DWORD vc, LPWORD i, DWORD ic){
    DWORD dwRequired	= UI->Device.Shader.dwPassesRequired;
    for (DWORD dwPass = 0; dwPass<dwRequired; dwPass++){
        Shader.SetupPass(dwPass);
        CDX(m_DX->pD3DDev->DrawIndexedPrimitiveStrided(pt,vtd,va,vc,i,ic,0));
		m_Statistic.dwRenderPolyCount += ic;
    }
}

void CDevice::DPVB(D3DPRIMITIVETYPE pt, LPDIRECT3DVERTEXBUFFER7 vb, DWORD sv, DWORD nv){
    DWORD dwRequired	= UI->Device.Shader.dwPassesRequired;
    for (DWORD dwPass = 0; dwPass<dwRequired; dwPass++){
        Shader.SetupPass(dwPass);
        CDX(m_DX->pD3DDev->DrawPrimitiveVB(pt,vb,sv,nv,0));
		m_Statistic.dwRenderPolyCount += nv/3;
    }
}

void CDevice::Validate()
{
    DWORD Pass,Res;
    Res =  GetDevice()->ValidateDevice(&Pass);
    char* E = 0;
    switch (Res)
    {
		case DDERR_INVALIDOBJECT:				E = "DDERR_INVALIDOBJECT"; break;
		case DDERR_INVALIDPARAMS:               E = "DDERR_INVALIDPARAMS"; break;
		case D3DERR_CONFLICTINGTEXTUREFILTER:   E = "D3DERR_CONFLICTINGTEXTUREFILTER"; break;
		case D3DERR_CONFLICTINGTEXTUREPALETTE:  E = "D3DERR_CONFLICTINGTEXTUREPALETTE"; break;
		case D3DERR_TOOMANYOPERATIONS:          E = "D3DERR_TOOMANYOPERATIONS"; break;
		case D3DERR_UNSUPPORTEDALPHAARG:        E = "D3DERR_UNSUPPORTEDALPHAARG"; break;
		case D3DERR_UNSUPPORTEDALPHAOPERATION:  E = "D3DERR_UNSUPPORTEDALPHAOPERATION"; break;
		case D3DERR_UNSUPPORTEDCOLORARG:        E = "D3DERR_UNSUPPORTEDCOLORARG"; break;
		case D3DERR_UNSUPPORTEDCOLOROPERATION:  E = "D3DERR_UNSUPPORTEDCOLOROPERATION"; break;
		case D3DERR_UNSUPPORTEDFACTORVALUE:     E = "D3DERR_UNSUPPORTEDFACTORVALUE"; break;
		case D3DERR_UNSUPPORTEDTEXTUREFILTER:   E = "D3DERR_UNSUPPORTEDTEXTUREFILTER"; break;
		case D3DERR_WRONGTEXTUREFORMAT:         E = "D3DERR_WRONGTEXTUREFORMAT"; break;
    }
    if (E)
    	Log->DlgMsg(mtError, E);
}

void CDevice::ReloadShaders(){
	if (m_NullShader) Shader.Delete(m_NullShader);
	if (m_WireShader) Shader.Delete(m_WireShader);
	if (m_SelectionShader) Shader.Delete(m_SelectionShader);

    UI->Command(COMMAND_UNLOAD_LIBMESHES);
    Shader.Reload();

    m_NullShader 		= Shader.Create();
    m_WireShader 		= Shader.Create("$ed_wire");
    m_SelectionShader 	= Shader.Create("$ed_selection");
}

void CDevice::RefreshTextures(bool bOnlyNew){
	UI->SetStatus("Reload textures...");
	Shader.RefreshTextures(bOnlyNew);
	UI->SetStatus("");
}

struct clr_16{
	unsigned b	: 5;
	unsigned g	: 6;
	unsigned r	: 5;
    void set(WORD val){CopyMemory(this,&val,2);}
    DWORD get(){
    	Fcolor C; C.set(float(r)/31.f,float(g)/63.f,float(b)/31.f,0);
    	return C.get();
    }
};

bool CDevice::MakeScreenshot(DWORDVec& pixels, DWORD& width, DWORD& height){
	if (!m_bReady) return false;
	if ((m_DX->BackDesc.ddpfPixelFormat.dwRGBBitCount!=32)&&
	    (m_DX->BackDesc.ddpfPixelFormat.dwRGBBitCount!=16)) return false;

    DDSURFACEDESC2 desc;
    memset(&desc, 0, sizeof(desc));
    desc.dwSize = sizeof(desc);
    if (DD_OK==m_DX->pBackBuffer->Lock(0,&desc,DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT|DDLOCK_READONLY,0)){
        width 	= desc.dwWidth;
        height 	= desc.dwHeight;
        pixels.resize(width*height);
		UI->ProgressStart(height,"Making screenshot");
        if (desc.ddpfPixelFormat.dwRGBBitCount==32){
            for (DWORD h=0; h<height; h++){
                DWORD* dt 	= LPDWORD(DWORD(desc.lpSurface)+DWORD(desc.lPitch*h));
                CopyMemory	(pixels.begin()+h*width,dt,sizeof(DWORD)*width);
            }
        }else{
	        if (desc.ddpfPixelFormat.dwRGBBitCount==16){
            	clr_16 C;
	            for (DWORD h=0; h<height; h++){
    	            WORD* dt 	= LPWORD(DWORD(desc.lpSurface)+DWORD(desc.lPitch*h));
        	        for (DWORD w=0; w<width; w++){
                    	C.set(*dt++);
            	        pixels[h*width+w] = C.get();
                    }
	            }
            }
        }
        m_DX->pBackBuffer->Unlock(0);
        return true;
    }
    return false;
}

