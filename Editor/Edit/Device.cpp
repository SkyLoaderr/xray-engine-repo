//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop
#include "Device.h"
#include "Scene.h"
#include "ui_main.h"
#include "library.h"

#pragma package(smart_init)

CRenderDevice Device;

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
DWORD psDeviceFlags 	= rsClearBB|rsNoVSync;
DWORD dwClearColor		= 0x00555555;

//---------------------------------------------------------------------------
CRenderDevice::CRenderDevice(){
    m_ScreenQuality = 1.f;
    m_RenderWidth 	= m_RenderHeight 	= 256;
    m_RealWidth 	= m_RealHeight 		= 256;
    m_RenderWidth_2	= m_RenderHeight_2 	= 128;
	mProjection.identity();
    mFullTransform.identity();
    mView.identity();
    m_DefaultMat.set(1,1,1);
	m_NullShader	= 0;
	m_WireShader	= 0;
	m_SelectionShader = 0;

    bReady 			= FALSE;
	bActive			= FALSE;

	// Engine flow-control
	fTimeDelta		= 0;
	fTimeGlobal		= 0;
	dwTimeDelta		= 0;
	dwTimeGlobal	= 0;
}

CRenderDevice::~CRenderDevice(){
	VERIFY(!bReady);
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
bool CRenderDevice::Create(HANDLE handle){
	if (bReady)	return false;
	ELog.Msg(mtInformation,"Starting RENDER device...");

    m_Handle			= handle;

	HW.CreateDevice		(handle,m_RenderWidth,m_RenderHeight);

	// after creation
	bReady				= TRUE;

	dwFrame				= 0;
	// Signal everyone - device created
    Shader.Initialize	();
    m_NullShader 		= Shader.Create();
    m_WireShader 		= Shader.Create("$ed_wire");
    m_SelectionShader 	= Shader.Create("$ed_selection");

    OnDeviceCreate();

	ELog.Msg( mtInformation, "D3D: initialized" );
	return true;
}

//---------------------------------------------------------------------------
void CRenderDevice::Destroy(){
	if (!bReady) return;

	ELog.Msg( mtInformation, "Destroying Direct3D...");

	HW.Validate					();

	if (m_NullShader) Shader.Delete(m_NullShader);
	if (m_WireShader) Shader.Delete(m_WireShader);
	if (m_SelectionShader) Shader.Delete(m_SelectionShader);

	// before destroy
	bReady 						= FALSE;
	OnDeviceDestroy();
    Shader.Clear();

	// real destroy
	_RELEASE					(Streams_QuadIB);
	HW.DestroyDevice			();
	ELog.Msg( mtInformation, "D3D: device cleared" );
}

void CRenderDevice::OnDeviceCreate(){
/*	SetRS(D3DRS_COLORVERTEX,TRUE);
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
*/
	// General Render States
	HW.Caps.Update();
	for (DWORD i=0; i<HW.Caps.dwNumBlendStages; i++)
	{
		if (psDeviceFlags&rsAnisotropic)	{
			CHK_DX(HW.pDevice->SetTextureStageState( i, D3DTSS_MINFILTER,	D3DTEXF_ANISOTROPIC ));
			CHK_DX(HW.pDevice->SetTextureStageState( i, D3DTSS_MAGFILTER,	D3DTEXF_ANISOTROPIC ));
			CHK_DX(HW.pDevice->SetTextureStageState( i, D3DTSS_MIPFILTER,	D3DTEXF_LINEAR		));
			CHK_DX(HW.pDevice->SetTextureStageState( i, D3DTSS_MAXANISOTROPY, 16				));
		} else {
			CHK_DX(HW.pDevice->SetTextureStageState( i, D3DTSS_MINFILTER,	D3DTEXF_LINEAR 		));
			CHK_DX(HW.pDevice->SetTextureStageState( i, D3DTSS_MAGFILTER,	D3DTEXF_LINEAR 		));
			CHK_DX(HW.pDevice->SetTextureStageState( i, D3DTSS_MIPFILTER,	D3DTEXF_LINEAR		));
		}
		float fBias = -1.f;
		CHK_DX(HW.pDevice->SetTextureStageState( i, D3DTSS_MIPMAPLODBIAS, *((LPDWORD) (&fBias))));
	}
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_DITHERENABLE,		TRUE				));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_COLORVERTEX,		TRUE				));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_STENCILENABLE,	FALSE				));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_ZENABLE,			TRUE				));
    CHK_DX(HW.pDevice->SetRenderState( D3DRS_SHADEMODE,		D3DSHADE_GOURAUD	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_CULLMODE,			D3DCULL_CCW			));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_ALPHAFUNC,		D3DCMP_GREATER		));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_LOCALVIEWER,		FALSE				));

	CHK_DX(HW.pDevice->SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_SPECULARMATERIALSOURCE,D3DMCS_MATERIAL	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL	));
	CHK_DX(HW.pDevice->SetRenderState( D3DRS_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1	));

	if (psDeviceFlags&rsWireframe)	{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_FILLMODE,			D3DFILL_WIREFRAME	)); }
	else							{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_FILLMODE,			D3DFILL_SOLID		)); }
	if (psDeviceFlags&rsAntialias)	{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS,TRUE				));	}
	else							{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS,FALSE				)); }
	if (psDeviceFlags&rsNormalize)	{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_NORMALIZENORMALS,	TRUE				)); }
	else							{ CHK_DX(HW.pDevice->SetRenderState( D3DRS_NORMALIZENORMALS,	FALSE				)); }

    ResetMaterial();
	// signal another objects
    Shader.OnDeviceCreate		();
	Primitive.OnDeviceCreate	();
    Scene->OnDeviceCreate		();

    UpdateFog();

	// Create TL-primitive
	{
		const DWORD dwTriCount = 1024;
		const DWORD dwIdxCount = dwTriCount*2*3;
		WORD	*Indices = 0;
		DWORD	dwUsage=D3DUSAGE_WRITEONLY;
		if (HW.Caps.bSoftware)	dwUsage|=D3DUSAGE_SOFTWAREPROCESSING;
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
    Scene->OnDeviceDestroy		();
    Shader.OnDeviceDestroy		();
	Primitive.OnDeviceDestroy	();
	Streams.OnDeviceDestroy		();
}

void CRenderDevice::UpdateFog(){
	//Fog parameters
    st_Environment& E	= Scene->m_LevelOp.m_Envs[Scene->m_LevelOp.m_CurEnv];
    Fcolor& FogColor 	= E.m_FogColor;
    float Fogness		= (UI->bRenderFog)?E.m_Fogness:0;
    float view_dist		= (UI->bRenderFog)?E.m_ViewDist:UI->ZFar();
	SetRS( D3DRS_FOGCOLOR,	FogColor.get());
	float start	= (1.0f - Fogness)* 0.85f * view_dist;
	float end	= 0.91f * view_dist;
	if (HW.Caps.bTableFog)	{
		ELog.Msg(mtInformation,"* Using hardware fog...");
		SetRS( D3DRS_RANGEFOGENABLE,FALSE				);
		SetRS( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR		);
		SetRS( D3DRS_FOGVERTEXMODE,D3DFOG_NONE			);
		if (!HW.Caps.bWFog) {
			start/=view_dist;
			end  /=view_dist;
		}
	} else {
		ELog.Msg(mtInformation,"* Fog is emulated...");
		SetRS( D3DRS_FOGTABLEMODE,	D3DFOG_NONE			);
		SetRS( D3DRS_FOGVERTEXMODE,D3DFOG_LINEAR		);
		SetRS( D3DRS_RANGEFOGENABLE,FALSE				);
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

    m_RenderWidth  	= m_RealWidth * m_ScreenQuality;
    m_RenderHeight 	= m_RealHeight * m_ScreenQuality;
    m_RenderWidth_2 = m_RenderWidth * 0.5f;
    m_RenderHeight_2= m_RenderHeight * 0.5f;

    Destroy			();

    m_Camera.m_Aspect = (float)m_RenderHeight / (float)m_RenderWidth;
    mProjection.build_projection( m_Camera.m_FOV, m_Camera.m_Aspect, m_Camera.m_Znear, m_Camera.m_Zfar );
    m_fNearer = mProjection._43;

    Create			(m_Handle);

    SetTransform	(D3DTS_PROJECTION,mProjection);
    SetTransform	(D3DTS_WORLD,precalc_identity);

    UI->RedrawScene();
}

void CRenderDevice::Begin( ){
	VERIFY(bReady);
	HW.Validate	();
    if (HW.pDevice->TestCooperativeLevel()!=D3D_OK){
		Sleep	(500);
		Destroy	();
		Create	(m_Handle);
	}

	CHK_DX(HW.pDevice->BeginScene());
	CHK_DX(HW.pDevice->Clear(0,0,
		D3DCLEAR_ZBUFFER|
		((psDeviceFlags&rsClearBB)?D3DCLEAR_TARGET:0)|
		(HW.Caps.bStencil?D3DCLEAR_STENCIL:0),
		dwClearColor,1,0
		));
	Streams.BeginFrame();
}

//---------------------------------------------------------------------------
void CRenderDevice::End(){
	VERIFY(HW.pDevice);
	VERIFY(bReady);

	// end scene
	Shader.SetNULL	();
//	Primitive.Reset	();
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
	if (fTimeDelta>.06666f) fTimeDelta=.06666f;					// limit to 15fps minimum

	u64	qTime		= TimerGlobal.GetElapsed();
	fTimeGlobal		= float(qTime)*CPU::cycles2seconds;

	dwTimeGlobal	= DWORD((qTime*u64(1000))/CPU::cycles_per_second);
	dwTimeDelta		= iFloor(fTimeDelta*1000.f+0.5f);

    m_Camera.Update(fTimeDelta);
}

void CRenderDevice::DP(D3DPRIMITIVETYPE pt, CVertexStream* vs, DWORD vBase, DWORD pc){
    DWORD dwRequired	= Device.Shader.dwPassesRequired;
	Primitive.setVertices(vs->getFVF(),vs->getStride(),vs->getBuffer());
    for (DWORD dwPass = 0; dwPass<dwRequired; dwPass++){
        Shader.SetupPass(dwPass);
		Primitive.Render(pt,vBase,pc);
		m_Statistic.dwRenderPolyCount += pc/3;
    }
}

void CRenderDevice::DIP(D3DPRIMITIVETYPE pt, CVertexStream* vs, DWORD vBase, DWORD vc, CIndexStream* is, DWORD iBase, DWORD pc){
    DWORD dwRequired	= Device.Shader.dwPassesRequired;
    Primitive.setIndicesUC(vBase, is->getBuffer());
    Primitive.setVertices(vs->getFVF(),vs->getStride(),vs->getBuffer());
    for (DWORD dwPass = 0; dwPass<dwRequired; dwPass++){
        Shader.SetupPass(dwPass);
		Primitive.Render(pt,vBase,vc,iBase,pc);
		m_Statistic.dwRenderPolyCount += pc/3;
    }
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
	if (m_NullShader) Shader.Delete(m_NullShader);
	if (m_WireShader) Shader.Delete(m_WireShader);
	if (m_SelectionShader) Shader.Delete(m_SelectionShader);

    UI->Command(COMMAND_UNLOAD_LIBMESHES);
    Shader.Reload();

    m_NullShader 		= Shader.Create();
    m_WireShader 		= Shader.Create("$ed_wire");
    m_SelectionShader 	= Shader.Create("$ed_selection");
}

void CRenderDevice::RefreshTextures(bool bOnlyNew){
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

bool CRenderDevice::MakeScreenshot(DWORDVec& pixels, DWORD& width, DWORD& height){
/*	if (!m_bReady) return false;
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
*/
    return false;
}


