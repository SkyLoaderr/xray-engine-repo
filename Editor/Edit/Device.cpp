//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop
#include "Device.h"
#include "ui_main.h"
#include "main.h"
#include "xr_hudfont.h"
#include "dxerr8.h"

#pragma package(smart_init)

CRenderDevice Device;

int psTextureLOD	= 0;
DWORD psDeviceFlags 	= rsStatistic|rsFilterLinear|rsFog|rsDrawGrid;
DWORD dwClearColor		= 0x00555555;

//---------------------------------------------------------------------------
void CRenderDevice::Error(HRESULT hr, const char *file, int line)
{
	char errmsg_buf[1024];

	const char *errStr = DXGetErrorString8A(hr);
	if (errStr==0) {
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,hr,0,errmsg_buf,1024,0);
		errStr = errmsg_buf;
	}
	_verify(errStr,(char *)file,line);
}

void __cdecl CRenderDevice::Fatal(const char* F,...)
{
	char errmsg_buf[1024];

	va_list		p;
	va_start	(p,F);
	vsprintf	(errmsg_buf,F,p);
	va_end		(p);
	_verify		(errmsg_buf,"<unknown>",0);
}

//---------------------------------------------------------------------------
CRenderDevice::CRenderDevice(){
    m_ScreenQuality = 1.f;
    dwWidth 		= dwHeight 	= 256;
    m_RealWidth 	= m_RealHeight 		= 256;
    m_RenderWidth_2	= m_RenderHeight_2 	= 128;
	mProjection.identity();
    mFullTransform.identity();
    mView.identity();
    m_DefaultMat.set(1,1,1);
	m_WireShader	= 0;
	m_SelectionShader = 0;

    bReady 			= FALSE;
	bActive			= FALSE;

	// Engine flow-control
	fTimeDelta		= 0;
	fTimeGlobal		= 0;
	dwTimeDelta		= 0;
	dwTimeGlobal	= 0;

	dwFillMode		= D3DFILL_SOLID;
    dwShadeMode		= D3DSHADE_GOURAUD;

    m_CurrentShader	= 0;
}

CRenderDevice::~CRenderDevice(){
	VERIFY(!bReady);
}

extern void Surface_Init();
void CRenderDevice::Initialize()
{
	Surface_Init();

	AnsiString fn = "shaders_xrlc.xr";
    FS.m_GameRoot.Update(fn);
    if (FS.Exist(fn.c_str())){
    	ShaderXRLC.Load(fn.c_str());
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find file '%s'",fn.c_str());
    }

	// load blenders
	Shader.xrStartUp	();
	// Startup shaders
	Create				();
//	PSLib.xrStartUp		();
	pHUDFont 			= new CFontHUD();
}

void CRenderDevice::ShutDown()
{
	ShaderXRLC.Unload	();

	// destroy context
	Destroy				();
	_DELETE				(pHUDFont);

	// destroy shaders
//	PSLib.xrShutDown	();
	Shader.xrShutDown	();
}

void CRenderDevice::InitTimer(){
	Timer_MM_Delta	= 0;
	{
		DWORD time_mm			= timeGetTime	();
		while (timeGetTime()==time_mm);			// wait for next tick
		DWORD time_system		= timeGetTime	();
		DWORD time_local		= TimerAsync	();
		Timer_MM_Delta			= time_system-time_local;
	}
}
//---------------------------------------------------------------------------
void CRenderDevice::RenderNearer(float n){
    mProjection._43=m_fNearer-n;
    SetTransform(D3DTS_PROJECTION,mProjection);
}
void CRenderDevice::ResetNearer(){
    mProjection._43=m_fNearer;
    SetTransform(D3DTS_PROJECTION,mProjection);
}
//---------------------------------------------------------------------------
bool CRenderDevice::Create(){
	if (bReady)	return false;
	ELog.Msg(mtInformation,"Starting RENDER device...");

    m_hWnd				= frmMain->Handle;
    m_hRenderWnd		= frmMain->D3DWindow->Handle;

	HW.CreateDevice		(frmMain->D3DWindow->Handle,dwWidth,dwHeight);

	// after creation
	bReady				= TRUE;

	dwFrame				= 0;

    OnDeviceCreate();

	ELog.Msg( mtInformation, "D3D: initialized" );
	return true;
}

//---------------------------------------------------------------------------
void CRenderDevice::Destroy(){
	if (!bReady) return;

	ELog.Msg( mtInformation, "Destroying Direct3D...");

	HW.Validate					();

	// before destroy
	bReady 						= FALSE;
	OnDeviceDestroy				();

	// real destroy
	HW.DestroyDevice			();

	ELog.Msg( mtInformation, "D3D: device cleared" );
}

void CRenderDevice::OnDeviceCreate(){
	// Shaders part
	Shader.OnDeviceCreate();
    m_WireShader 		= Shader.Create("editor\\wire");
    m_SelectionShader 	= Shader.Create("editor\\selection");

	// General Render States
	HW.Caps.Update();
	for (DWORD i=0; i<HW.Caps.pixel.dwStages; i++)
	{
//		if (psDeviceFlags&rsAnisotropic)	{
//			Device.SetTSS(i,D3DTSS_MINFILTER,	D3DTEXF_ANISOTROPIC	);
//			Device.SetTSS(i, D3DTSS_MAGFILTER,	D3DTEXF_ANISOTROPIC );
//			Device.SetTSS(i, D3DTSS_MIPFILTER,	D3DTEXF_LINEAR		);
//			Device.SetTSS(i, D3DTSS_MAXANISOTROPY, 16				);
//		} else
//		{
//			Device.SetTSS(i, D3DTSS_MINFILTER,	D3DTEXF_LINEAR 		);
//			Device.SetTSS(i, D3DTSS_MAGFILTER,	D3DTEXF_LINEAR 		);
//			Device.SetTSS(i, D3DTSS_MIPFILTER,	D3DTEXF_LINEAR		);
//		}
		float fBias = -1.f;
		CHK_DX(HW.pDevice->SetTextureStageState( i, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias))));
	}
	Device.SetRS(D3DRS_DITHERENABLE,	TRUE				);
    Device.SetRS(D3DRS_COLORVERTEX,		TRUE				);
    Device.SetRS(D3DRS_STENCILENABLE,	FALSE				);
    Device.SetRS(D3DRS_ZENABLE,			TRUE				);
    Device.SetRS(D3DRS_SHADEMODE,		D3DSHADE_GOURAUD	);
	Device.SetRS(D3DRS_CULLMODE,		D3DCULL_CCW			);
	Device.SetRS(D3DRS_ALPHAFUNC,		D3DCMP_GREATER		);
	Device.SetRS(D3DRS_LOCALVIEWER,		FALSE				);

	Device.SetRS(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	Device.SetRS(D3DRS_SPECULARMATERIALSOURCE,D3DMCS_MATERIAL);
	Device.SetRS(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
	Device.SetRS(D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1	);

    ResetMaterial();
	// signal another objects
	seqDevCreate.Process		(rp_DeviceCreate);
	Primitive.OnDeviceCreate	();

#ifdef _LEVEL_EDITOR
    UpdateFog();
#else
    UpdateFog(dwClearColor,0.f,m_Camera.m_Zfar);
#endif

	// Create TL-primitive
	{
		const DWORD dwTriCount = 1024;
		const DWORD dwIdxCount = dwTriCount*2*3;
		WORD	*Indices = 0;
		DWORD	dwUsage=D3DUSAGE_WRITEONLY;
		if (HW.Caps.vertex.bSoftware)	dwUsage|=D3DUSAGE_SOFTWAREPROCESSING;
		R_CHK(HW.pDevice->CreateIndexBuffer(dwIdxCount*2,dwUsage,D3DFMT_INDEX16,D3DPOOL_DEFAULT,&Streams_QuadIB));
		R_CHK(Streams_QuadIB->Lock(0,0,(BYTE**)&Indices,D3DLOCK_NOSYSLOCK));
		{
			int		Cnt = 0;
			int		ICnt= 0;
			for (int i=0; i<dwTriCount; i++)
			{
				Indices[ICnt++]=Cnt+0;
				Indices[ICnt++]=Cnt+1;
				Indices[ICnt++]=Cnt+2;

				Indices[ICnt++]=Cnt+3;
				Indices[ICnt++]=Cnt+2;
				Indices[ICnt++]=Cnt+1;

				Cnt+=4;
			}
		}
		R_CHK(Streams_QuadIB->Unlock());
	}
}

void CRenderDevice::OnDeviceDestroy(){
    m_CurrentShader		= 0;

	if (m_WireShader) Shader.Delete(m_WireShader);
	if (m_SelectionShader) Shader.Delete(m_SelectionShader);

	seqDevDestroy.Process		(rp_DeviceDestroy);

	Shader.OnDeviceDestroy		();

	Primitive.OnDeviceDestroy	();
	Streams.OnDeviceDestroy		();

	_RELEASE					(Streams_QuadIB);
}

#ifdef _LEVEL_EDITOR
#include "scene.h"
void CRenderDevice::UpdateFog(){
	st_Environment& E	= Scene->m_LevelOp.m_Envs[Scene->m_LevelOp.m_CurEnv];
    UpdateFog (E.m_FogColor.get(),(psDeviceFlags&rsFog)?E.m_Fogness:0,(psDeviceFlags&rsFog)?E.m_ViewDist:UI.ZFar());
}
#endif

void CRenderDevice::UpdateFog(DWORD color, float fogness, float view_dist){
	//Fog parameters
	float start	= (1.0f - fogness)* 0.85f * view_dist;
	float end	= 0.91f * view_dist;
	SetRS( D3DRS_FOGCOLOR,	color);
	SetRS( D3DRS_RANGEFOGENABLE,	FALSE				);
	if (HW.Caps.bTableFog)	{
		ELog.Msg(mtInformation,"* Using hardware fog...");
		SetRS( D3DRS_FOGTABLEMODE,	D3DFOG_LINEAR		);
		SetRS( D3DRS_FOGVERTEXMODE,	D3DFOG_NONE			);
	} else {
		ELog.Msg(mtInformation,"* Fog is emulated...");
		SetRS( D3DRS_FOGTABLEMODE,	D3DFOG_NONE			);
		SetRS( D3DRS_FOGVERTEXMODE,	D3DFOG_LINEAR		);
	}
	SetRS( D3DRS_FOGSTART,	*(DWORD *)(&start)	);
	SetRS( D3DRS_FOGEND,	*(DWORD *)(&end)	);
}

//---------------------------------------------------------------------------
void __fastcall CRenderDevice::Resize(int w, int h)
{
    m_RealWidth 	= w;
    m_RealHeight 	= h;
    m_RenderArea	= w*h;

    dwWidth  		= m_RealWidth * m_ScreenQuality;
    dwHeight 		= m_RealHeight * m_ScreenQuality;
    m_RenderWidth_2 = dwWidth * 0.5f;
    m_RenderHeight_2= dwHeight * 0.5f;

    Destroy			();

    m_Camera.m_Aspect = (float)dwHeight / (float)dwWidth;
    mProjection.build_projection( m_Camera.m_FOV, m_Camera.m_Aspect, m_Camera.m_Znear, m_Camera.m_Zfar );
    m_fNearer 		= mProjection._43;

    Create			();

    SetTransform	(D3DTS_PROJECTION,mProjection);
    SetTransform	(D3DTS_WORLD,precalc_identity);

    UI.RedrawScene();
}

void CRenderDevice::Begin( ){
	VERIFY(bReady);
	HW.Validate	();
    if (HW.pDevice->TestCooperativeLevel()!=D3D_OK){
		Sleep	(500);
		Destroy	();
		Create	();
	}

	CHK_DX(HW.pDevice->BeginScene());
	CHK_DX(HW.pDevice->Clear(0,0,
		D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET|
		(HW.Caps.bStencil?D3DCLEAR_STENCIL:0),
		dwClearColor,1,0
		));
	Streams.BeginFrame();
}

//---------------------------------------------------------------------------
void CRenderDevice::End(){
	VERIFY(HW.pDevice);
	VERIFY(bReady);

	Statistic.Show(pHUDFont);
	pHUDFont->OnRender();

	// end scene
	Shader.OnFrameEnd();
	Primitive.Reset	();
    CHK_DX(HW.pDevice->EndScene());

	CHK_DX(HW.pDevice->Present( NULL, NULL, NULL, NULL ));
}

void CRenderDevice::UpdateView(){
// set camera matrix
	m_Camera.GetView(mView);

    SetTransform(D3DTS_VIEW,mView);
    mFullTransform.mul(mProjection,mView);

// frustum culling sets
    m_Frustum.CreateFromViewMatrix();
}

void CRenderDevice::UpdateTimer(){
	dwFrame++;

	// Timer
	float fPreviousFrameTime = Timer.GetAsync(); Timer.Start();	// previous frame
	fTimeDelta = 0.1f * fTimeDelta + 0.9f*fPreviousFrameTime;	// smooth random system activity - worst case ~7% error
	if (fTimeDelta>1.f) fTimeDelta=1.f;							// limit to 1 fps minimum

	u64	qTime		= TimerGlobal.GetElapsed();
	fTimeGlobal		= float(qTime)*CPU::cycles2seconds;

	dwTimeGlobal	= DWORD((qTime*u64(1000))/CPU::cycles_per_second);
	dwTimeDelta		= iFloor(fTimeDelta*1000.f+0.5f);

    m_Camera.Update(fTimeDelta);
}

void CRenderDevice::DP(D3DPRIMITIVETYPE pt, CVertexStream* vs, DWORD vBase, DWORD pc){
	if (!m_CurrentShader) return;
    DWORD dwRequired	= m_CurrentShader->Passes.size();
	Primitive.setVertices(vs->getFVF(),vs->getStride(),vs->getBuffer());
    for (DWORD dwPass = 0; dwPass<dwRequired; dwPass++){
        Shader.set_Shader(m_CurrentShader,dwPass);
		Primitive.Render(pt,vBase,pc);
    }
    UPDATEC(pc*3,pc,dwRequired);
}

void CRenderDevice::DIP(D3DPRIMITIVETYPE pt, CVertexStream* vs, DWORD vBase, DWORD vc, CIndexStream* is, DWORD iBase, DWORD pc){
	if (!m_CurrentShader) return;
    DWORD dwRequired	= m_CurrentShader->Passes.size();
    Primitive.setIndicesUC(vBase, is->getBuffer());
    Primitive.setVertices(vs->getFVF(),vs->getStride(),vs->getBuffer());
    for (DWORD dwPass = 0; dwPass<dwRequired; dwPass++){
        Shader.set_Shader(m_CurrentShader,dwPass);
		Primitive.Render(pt,vBase,vc,iBase,pc);
    }
    UPDATEC(vc,pc,dwRequired);
}

void CRenderDevice::Validate()
{
/*    DWORD Pass,Res;
    Res =  HW.pDevice->ValidateDevice(&Pass);
    char* E = 0;
    switch (Res)
    {
    	case D3DERR_CONFLICTINGTEXTUREFILTER
		case D3DERR_CONFLICTINGTEXTUREPALETTE
		case D3DERR_DEVICELOST
		case D3DERR_TOOMANYOPERATIONS
		case D3DERR_UNSUPPORTEDALPHAARG
		case D3DERR_UNSUPPORTEDALPHAOPERATION
		case D3DERR_UNSUPPORTEDCOLORARG
		case D3DERR_UNSUPPORTEDCOLOROPERATION
		case D3DERR_UNSUPPORTEDFACTORVALUE
		case D3DERR_UNSUPPORTEDTEXTUREFILTER
		case D3DERR_WRONGTEXTUREFORMAT
    	case
		case D3DERR_INVALIDOBJECT:				E = "DDERR_INVALIDOBJECT"; break;
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
    	ELog.DlgMsg(mtError, E);
*/
}

void CRenderDevice::ReloadShaders(){
	OnDeviceDestroy();
	OnDeviceCreate();
//S
/*    Lib->OnDeviceDestroy();

	if (m_WireShader) Shader.Delete(m_WireShader);
	if (m_SelectionShader) Shader.Delete(m_SelectionShader);

    Shader.Reload();

    m_WireShader 		= Shader.Create("$ed_wire");
    m_SelectionShader 	= Shader.Create("$ed_selection");

    Lib->OnDeviceCreate();
*/
}

void CRenderDevice::RefreshTextures(bool bOnlyNew){
	UI.SetStatus("Reload textures...");
//S	Shader.RefreshTextures(bOnlyNew);
	UI.SetStatus("");
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

bool CRenderDevice::MakeScreenshot(DWORDVec& pixels, DWORD& width, DWORD& height){
	if (!bReady) return false;

    width 	= 256;
    height 	= 256;

    IDirect3DSurface8* pZB=0;
    IDirect3DSurface8* pRT=0;
    IDirect3DSurface8* poldRT=0;
    HW.pDevice->GetRenderTarget(&poldRT);
	HW.pDevice->GetDepthStencilSurface(&pZB);
	HW.pDevice->CreateRenderTarget(width,height,D3DFMT_A8R8G8B8,D3DMULTISAMPLE_NONE,TRUE,&pRT);
    HW.pDevice->SetRenderTarget(pRT,pZB);

    DWORD old_flag = psDeviceFlags;
	psDeviceFlags &=~rsStatistic;
	UI.Redraw();
    psDeviceFlags = old_flag;

	D3DLOCKED_RECT	D;
	R_CHK(pRT->LockRect(&D,0,D3DLOCK_NOSYSLOCK));
	pixels.resize(width*height);
	// Image processing
	DWORD* pPixel	= (DWORD*)D.pBits;

    UI.ProgressStart(height,"Screenshot making");
    DWORDIt it 		= pixels.begin();
    for (int h=height-1; h>=0; h--,it+=width){
        LPDWORD dt 	= LPDWORD(DWORD(pPixel)+DWORD(D.Pitch*h));
        CopyMemory	(it,dt,sizeof(DWORD)*width);
	    UI.ProgressInc();
    }
    UI.ProgressEnd();

    R_CHK(pRT->UnlockRect());
    HW.pDevice->SetRenderTarget(poldRT,pZB);

    _RELEASE(pZB);
    _RELEASE(pRT);
    _RELEASE(poldRT);

    return true;
}


