//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop
#include "gamefont.h"
#include "dxerr8.h"
#include "ImageManager.h"
#include "ui_main.h"
#include "render.h"
#include "GameMtlLib.h"
#include "ResourceManager.h"

#pragma package(smart_init)

CRenderDevice 		Device;

extern int	rsDVB_Size;
extern int	rsDIB_Size;

//---------------------------------------------------------------------------
CRenderDevice::CRenderDevice()
{
	psDeviceFlags.assign(rsStatistic|rsFilterLinear|rsFog|rsDrawGrid);
// dynamic buffer size
	rsDVB_Size		= 2048;
	rsDIB_Size		= 2048;
// default initialization
    m_ScreenQuality = 1.f;
    dwWidth 		= dwHeight 	= 256;
    m_RealWidth 	= m_RealHeight 		= 256;
    m_RenderWidth_2	= m_RenderHeight_2 	= 128;
	mProjection.identity();
    mFullTransform.identity();
    mView.identity	();
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
    pSystemFont		= 0;

	fASPECT 		= 1.f;
	fFOV 			= 60.f;
}

CRenderDevice::~CRenderDevice(){
	VERIFY(!bReady);
}

extern void Surface_Init();
void CRenderDevice::Initialize()
{
//	m_Camera.Reset();

    m_DefaultMat.set(1,1,1);
	Surface_Init();

	// game materials
    GMLib.Load	();

	// compiler shader
	std::string fn;
    FS.update_path(fn,_game_data_,"shaders_xrlc.xr");
    if (FS.exist(fn.c_str())){
    	ShaderXRLC.Load(fn.c_str());
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find file '%s'",fn.c_str());
    }

	// Startup shaders
	Create				();

    ::Render->Initialize();
}

void CRenderDevice::ShutDown()
{
    ::Render->ShutDown	();

	ShaderXRLC.Unload	();
	GMLib.Unload		();

	// destroy context
	Destroy				();
	xr_delete			(pSystemFont);

	// destroy shaders
//	PSLib.xrShutDown	();
}

void CRenderDevice::InitTimer(){
	Timer_MM_Delta	= 0;
	{
		u32 time_mm			= timeGetTime	();
		while (timeGetTime()==time_mm);			// wait for next tick
		u32 time_system		= timeGetTime	();
		u32 time_local		= TimerAsync	();
		Timer_MM_Delta			= time_system-time_local;
	}
}
//---------------------------------------------------------------------------
void CRenderDevice::RenderNearer(float n){
    mProjection._43=m_fNearer-n;
    RCache.set_xform_project(mProjection);
}
void CRenderDevice::ResetNearer(){
    mProjection._43=m_fNearer;
    RCache.set_xform_project(mProjection);
}
//---------------------------------------------------------------------------
bool CRenderDevice::Create()
{
	if (bReady)	return false;
	ELog.Msg(mtInformation,"Starting RENDER device...");

	HW.CreateDevice		(m_hRenderWnd,dwWidth,dwHeight);

	// after creation
	dwFrame				= 0;

	std::string sh;
    FS.update_path		(sh,_game_data_,"shaders.xr");

    IReader* F			= 0;
	if (FS.exist(sh.c_str()))
		F				= FS.r_open(0,sh.c_str());
	Resources			= xr_new<CResourceManager>	();

    // if build options - load textures immediately
    if (strstr(Core.Params,"-build")||strstr(Core.Params,"-ebuild"))
        Device.Resources->DeferredLoad(FALSE);

    _Create				(F);
	FS.r_close			(F);

	ELog.Msg			(mtInformation, "D3D: initialized");

	return true;
}

//---------------------------------------------------------------------------
void CRenderDevice::Destroy(){
	if (!bReady) return;

	ELog.Msg( mtInformation, "Destroying Direct3D...");

	HW.Validate			();

	// before destroy
	_Destroy			(FALSE);
	xr_delete			(Resources);

	// real destroy
	HW.DestroyDevice	();

	ELog.Msg( mtInformation, "D3D: device cleared" );
}
//---------------------------------------------------------------------------
void CRenderDevice::_SetupStates()
{
	HW.Caps.Update();
	for (u32 i=0; i<HW.Caps.raster.dwStages; i++){
		float fBias = -1.f;
		CHK_DX(HW.pDevice->SetSamplerState( i, D3DSAMP_MIPMAPLODBIAS, *((LPDWORD) (&fBias))));
	}
	Device.SetRS(D3DRS_DITHERENABLE,	TRUE				);
    Device.SetRS(D3DRS_COLORVERTEX,		TRUE				);
    Device.SetRS(D3DRS_STENCILENABLE,	FALSE				);
    Device.SetRS(D3DRS_ZENABLE,			TRUE				);
    Device.SetRS(D3DRS_SHADEMODE,		D3DSHADE_GOURAUD	);
	Device.SetRS(D3DRS_CULLMODE,		D3DCULL_CCW			);
	Device.SetRS(D3DRS_ALPHAFUNC,		D3DCMP_GREATER		);
	Device.SetRS(D3DRS_LOCALVIEWER,		TRUE				);
    Device.SetRS(D3DRS_NORMALIZENORMALS,TRUE				);

	Device.SetRS(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	Device.SetRS(D3DRS_SPECULARMATERIALSOURCE,D3DMCS_MATERIAL);
	Device.SetRS(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
	Device.SetRS(D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1	);

    ResetMaterial();
}
//---------------------------------------------------------------------------
void CRenderDevice::_Create(IReader* F)
{
	bReady				= TRUE;

	// General Render States
    _SetupStates		();
    
    RCache.OnDeviceCreate		();
	Resources->OnDeviceCreate	(F);
    ::Render->OnDeviceCreate	();

    m_WireShader.create			("editor\\wire");
    m_SelectionShader.create	("editor\\selection");

	// signal another objects
    UI->OnDeviceCreate			();           
	seqDevCreate.Process		(rp_DeviceCreate);

	pSystemFont					= xr_new<CGameFont>("hud_font_small");
}

void CRenderDevice::_Destroy(BOOL	bKeepTextures)
{
	xr_delete					(pSystemFont);

	bReady 						= FALSE;
    m_CurrentShader				= 0;

    UI->OnDeviceDestroy			();

	m_WireShader.destroy		();
	m_SelectionShader.destroy	();

	seqDevDestroy.Process		(rp_DeviceDestroy);
	::Render->Models->OnDeviceDestroy	();

	Resources->OnDeviceDestroy	(bKeepTextures);

	RCache.OnDeviceDestroy		();
    ::Render->OnDeviceDestroy	();
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

    fASPECT 		= (float)dwHeight / (float)dwWidth;
    mProjection.build_projection( deg2rad(fFOV), fASPECT, m_Camera.m_Znear, m_Camera.m_Zfar );
    m_fNearer 		= mProjection._43;

    Reset			();

    RCache.set_xform_project(mProjection);
    RCache.set_xform_world	(Fidentity);

    UI->RedrawScene	();
}

void CRenderDevice::Reset  	()
{
    u32 tm_start			= TimerAsync();
    Resources->reset_begin	();
    Memory.mem_compact		();
    HW.DevPP.BackBufferWidth= dwWidth;
    HW.DevPP.BackBufferHeight= dwHeight;
    HW.Reset				();
    dwWidth					= HW.DevPP.BackBufferWidth;
    dwHeight				= HW.DevPP.BackBufferHeight;
    m_RenderWidth_2 		= dwWidth * 0.5f;
    m_RenderHeight_2		= dwHeight * 0.5f;
//		fWidth_2			= float(dwWidth/2);
//		fHeight_2			= float(dwHeight/2);
    Resources->reset_end	();
    _SetupStates			();
    u32 tm_end				= TimerAsync();
    Msg						("*** RESET [%d ms]",tm_end-tm_start);
}

BOOL CRenderDevice::Begin	()
{
	VERIFY(bReady);
	HW.Validate		();
	HRESULT	_hr		= HW.pDevice->TestCooperativeLevel();
    if (FAILED(_hr))
	{
		// If the device was lost, do not render until we get it back
		if		(D3DERR_DEVICELOST==_hr)		{
			Sleep	(33);
			return	FALSE;
		}

		// Check if the device is ready to be reset
		if		(D3DERR_DEVICENOTRESET==_hr)
		{
			Reset	();
		}
	}

	CHK_DX					(HW.pDevice->BeginScene());
	CHK_DX(HW.pDevice->Clear(0,0,
		D3DCLEAR_ZBUFFER|D3DCLEAR_TARGET|
		(HW.Caps.bStencil?D3DCLEAR_STENCIL:0),
		EPrefs.scene_clear_color,1,0
		));
	RCache.OnFrameBegin		();
	return		TRUE;
}

//---------------------------------------------------------------------------
void CRenderDevice::End()
{
	VERIFY(HW.pDevice);
	VERIFY(bReady);

    seqRender.Process						(rp_Render);
    
	Statistic.Show(pSystemFont);
	Device.SetRS	(D3DRS_FILLMODE,D3DFILL_SOLID);
	pSystemFont->OnRender();
    Device.SetRS	(D3DRS_FILLMODE,Device.dwFillMode);

	// end scene
	RCache.OnFrameEnd();
    CHK_DX(HW.pDevice->EndScene());

	CHK_DX(HW.pDevice->Present( NULL, NULL, NULL, NULL ));
}

void CRenderDevice::UpdateView()
{
// set camera matrix
	m_Camera.GetView(mView);

    RCache.set_xform_view(mView);
    mFullTransform.mul(mProjection,mView);

// frustum culling sets
    ::Render->ViewBase.CreateFromMatrix(mFullTransform,FRUSTUM_P_ALL);
}

void CRenderDevice::FrameMove()
{
	dwFrame++;

	// Timer
	float fPreviousFrameTime = Timer.GetElapsed_sec(); Timer.Start();	// previous frame
	fTimeDelta = 0.1f * fTimeDelta + 0.9f*fPreviousFrameTime;	// smooth random system activity - worst case ~7% error
	if (fTimeDelta>1.f) fTimeDelta=1.f;							// limit to 1 fps minimum

	u64	qTime		= TimerGlobal.GetElapsed_clk();
	fTimeGlobal		= float(qTime)*CPU::cycles2seconds;

	dwTimeGlobal	= u32((qTime*u64(1000))/CPU::cycles_per_second);
	dwTimeDelta		= iFloor(fTimeDelta*1000.f+0.5f);

    m_Camera.Update(fTimeDelta);

    // process objects
	seqFrame.Process(rp_Frame);
}

void CRenderDevice::DP(D3DPRIMITIVETYPE pt, ref_geom geom, u32 vBase, u32 pc)
{
	ref_shader S 			= m_CurrentShader?m_CurrentShader:m_WireShader;
    u32 dwRequired			= S->E[0]->passes.size();
    RCache.set_Geometry		(geom);
    for (u32 dwPass = 0; dwPass<dwRequired; dwPass++){
    	RCache.set_Shader	(S,dwPass);
		RCache.Render		(pt,vBase,pc);
    }
}

void CRenderDevice::DIP(D3DPRIMITIVETYPE pt, ref_geom geom, u32 baseV, u32 startV, u32 countV, u32 startI, u32 PC)
{
	ref_shader S 			= m_CurrentShader?m_CurrentShader:m_WireShader;
    u32 dwRequired			= S->E[0]->passes.size();
    RCache.set_Geometry		(geom);
    for (u32 dwPass = 0; dwPass<dwRequired; dwPass++){
    	RCache.set_Shader	(S,dwPass);
		RCache.Render		(pt,baseV,startV,countV,startI,PC);
    }
}

void CRenderDevice::ReloadTextures()
{
	UI->SetStatus("Reload textures...");
	Resources->ED_UpdateTextures(0);
	UI->SetStatus("");
}

void CRenderDevice::UnloadTextures()
{
    Resources->DeferredUnload();
}

bool CRenderDevice::MakeScreenshot(U32Vec& pixels, u32& width, u32& height)
{
	if (!bReady) return false;

    // free managed resource
    Resources->Evict();

    IDirect3DSurface9* 	poldZB=0;
    IDirect3DSurface9* 	pZB=0;
    IDirect3DSurface9* 	pRT=0;
    IDirect3DSurface9* 	poldRT=0;
    D3DVIEWPORT9		oldViewport;
    SetRS(D3DRS_COLORWRITEENABLE,D3DCOLORWRITEENABLE_ALPHA|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);
    CHK_DX(HW.pDevice->GetRenderTarget(0,&poldRT));
    CHK_DX(HW.pDevice->GetDepthStencilSurface(&poldZB));
    CHK_DX(HW.pDevice->GetViewport(&oldViewport));

	CHK_DX(HW.pDevice->CreateRenderTarget(width,height,D3DFMT_A8R8G8B8,D3DMULTISAMPLE_NONE,0,FALSE,&pRT,0));
	CHK_DX(HW.pDevice->CreateDepthStencilSurface(width,height,HW.Caps.bStencil?D3DFMT_D24S8:D3DFMT_D24X8,D3DMULTISAMPLE_NONE,0,FALSE,&pZB,0));
	CHK_DX(HW.pDevice->SetRenderTarget(0,pRT));
	CHK_DX(HW.pDevice->SetDepthStencilSurface(pZB));

	UI->Redraw();

	// Create temp-surface
	IDirect3DSurface9*	pFB;
	R_CHK(HW.pDevice->CreateOffscreenPlainSurface(
		width,height,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&pFB,NULL));
	R_CHK(HW.pDevice->GetRenderTargetData(pRT, pFB));

	D3DLOCKED_RECT	D;
	R_CHK(pFB->LockRect(&D,0,D3DLOCK_NOSYSLOCK));
	pixels.resize(width*height);
	// Image processing
	u32* pPixel	= (u32*)D.pBits;

	SPBItem* pb 	= UI->ProgressStart(height,"Screenshot making");
    U32It it 		= pixels.begin();
    for (int h=height-1; h>=0; h--,it+=width){
        LPDWORD dt 	= LPDWORD(u32(pPixel)+u32(D.Pitch*h));
        CopyMemory	(it,dt,sizeof(u32)*width);
	    pb->Inc		();
    }
    UI->ProgressEnd(pb);

    R_CHK(pFB->UnlockRect());

	CHK_DX(HW.pDevice->SetDepthStencilSurface(poldZB));
    CHK_DX(HW.pDevice->SetRenderTarget(0,poldRT));
    CHK_DX(HW.pDevice->SetViewport(&oldViewport));

    _RELEASE(pZB);
    _RELEASE(poldZB);
    _RELEASE(pFB);
    _RELEASE(pRT);
    _RELEASE(poldRT);

    return true;
}

void CRenderDevice::Reset(IReader* F, BOOL bKeepTextures)
{
	CTimer tm;
    tm.Start();
	_Destroy		(bKeepTextures);
	_Create			(F);
    tm.Stop();
	Msg				("*** RESET [%d ms]",tm.GetElapsed_ms());
}


