//-----------------------------------------------------------------------------
// File: gravity.cpp
//
// Copyright (c) 1999 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "tester.h"
#include "math.h"
#include "resource.h"
#include "ftimer.h"

//-----------------------------------------------------------------------------
#define TSHOW	1.f
_stats	STAT;
float	tmToShow=TSHOW;
CTimer	T;
extern HWND logWindow;
//-----------------------------------------------------------------------------
_options MODE;
static BOOL CALLBACK startDlgProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
	case WM_INITDIALOG:
		SetWindowText	(GetDlgItem(hw,IDC_LEVEL),"d:\\projects\\xr_3da\\data\\try_level");
		SetWindowText	(GetDlgItem(hw,IDC_VISLEVELS),"1");
		CheckRadioButton(hw, IDC_VA1, IDC_VA4, IDC_VA1);
		break;
	case WM_DESTROY:
		break;
	case WM_CLOSE:
		EndDialog(hw, IDCANCEL);
		break;
	case WM_COMMAND:
		switch (LOWORD(wp)) {
		case IDCANCEL:
			EndDialog(hw, IDCANCEL);
			break;
		case IDOK:
			{
				GetWindowText(GetDlgItem(hw,IDC_LEVEL),MODE.folder,_MAX_PATH);
				strcat(MODE.folder,"\\");
				MODE.bFullVolume = (BST_CHECKED==IsDlgButtonChecked(hw, IDC_FULLVOLUME));
				MODE.bDoubleResolution = (BST_CHECKED==IsDlgButtonChecked(hw, IDC_INCRESOLUTION));
				if (BST_CHECKED==IsDlgButtonChecked(hw, IDC_VA1)) MODE.dwVisSize=64;
				if (BST_CHECKED==IsDlgButtonChecked(hw, IDC_VA2)) MODE.dwVisSize=128;
				if (BST_CHECKED==IsDlgButtonChecked(hw, IDC_VA4)) MODE.dwVisSize=256;
				char nLevels[32];
				GetWindowText(GetDlgItem(hw,IDC_VISLEVELS),nLevels,32);
				MODE.dwVisLevels = atoi(nLevels);
				EndDialog(hw, IDOK);
			}
			break;
		case IDC_LEVELSELECT:
			{
				char folder[_MAX_PATH];
				BROWSEINFO F;
				F.hwndOwner		= hw;
				F.pidlRoot		= NULL;
				F.pszDisplayName= folder;
				F.lpszTitle		= "Select XR-Level folder";
				F.ulFlags		= BIF_RETURNONLYFSDIRS;
				F.lpfn			= NULL;
				F.lParam		= NULL;
				F.iImage		= 0;
				LPITEMIDLIST	p = SHBrowseForFolder(&F);
				if (p) {
					SHGetPathFromIDList(p, folder);
					SetWindowText(GetDlgItem(hw,IDC_LEVEL),folder);
				}
			}
			break;
		case IDC_BDEBUG:
			CheckDlgButton  (hw, IDC_FULLVOLUME, BST_UNCHECKED);
			CheckDlgButton  (hw, IDC_INCRESOLUTION, BST_UNCHECKED);
			CheckRadioButton(hw, IDC_VA1, IDC_VA4, IDC_VA1);
			SetWindowText	(GetDlgItem(hw,IDC_VISLEVELS),"1");
			break;
		case IDC_BRELEASE:
			CheckDlgButton  (hw, IDC_FULLVOLUME,	BST_CHECKED);
			CheckDlgButton  (hw, IDC_INCRESOLUTION, BST_CHECKED);
			CheckRadioButton(hw, IDC_VA1, IDC_VA4, IDC_VA4);
			SetWindowText	(GetDlgItem(hw,IDC_VISLEVELS),"3");
			break;
		}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
//-----------------------------------------------------------------------------

class CGravity
{
public:
    CGravity();
    ~CGravity();
    void                    PauseDrawing();
    void                    RestartDrawing();
    void                    UnInit();
    HRESULT                 InitD3DX();
    HRESULT                 InitRenderer();
    HRESULT                 HandleModeChanges();
    HRESULT                 Draw();
    BOOL                    m_bD3DXReady;
    BOOL                    m_bActive;

    LPDIRECT3DDEVICE7       m_pD3DDev;
    LPDIRECT3D7             m_pD3D;
    LPDIRECTDRAW7           m_pDD;

    ID3DXContext*           m_pD3DX;
};
CGravity* g_pGravity;


LPDIRECT3DDEVICE7	Device()
{return g_pGravity->m_pD3DDev;}
LPDIRECT3D7			D3D()
{return g_pGravity->m_pD3D;}
LPDIRECTDRAWSURFACE7 BackBuffer()
{
	LPDIRECTDRAWSURFACE7 pBB = g_pGravity->m_pD3DX->GetBackBuffer(0);
	pBB->Release();
	return pBB;
}

CGravity::CGravity()
{  
    m_bD3DXReady        = FALSE;
    m_pD3DDev           = NULL;
    m_pD3D              = NULL;
    m_pDD               = NULL;
    m_pD3DX             = NULL;
    m_bActive           = TRUE;
	ZeroMemory			(&STAT,sizeof(STAT));
}

CGravity::~CGravity()
{
	delete pTester;
    g_pGravity->UnInit();
}

void InterpretError(HRESULT hr)
{
    char errStr[100];
    D3DXGetErrorString(hr, 100, errStr );
    MessageBox(NULL,errStr,"D3DX Error",MB_OK);
}

void CGravity::PauseDrawing()
{
    g_pGravity->m_bActive = FALSE;
}

void CGravity::RestartDrawing()
{
    g_pGravity->m_bActive = TRUE;
}

//*****************************************************************************
// Renderer Initialization Code
//*****************************************************************************

HRESULT CGravity::InitD3DX()
{
    HRESULT hr;

    if( FAILED( hr = D3DXInitialize() ) )
        return hr;



    // D3DX Initialization
    hr = D3DXCreateContextEx(   D3DX_HWLEVEL_2D,		// 
                                0,						// flags
                                GetDlgItem(logWindow,IDC_VIEW),
                                0,						// focusWnd
                                16,			            // colorbits
                                0,						// alphabits
                                16,						// numdepthbits
                                0,                      // numstencilbits
                                D3DX_DEFAULT,           // numbackbuffers
                                MODE.dwVisSize,			// width
                                MODE.dwVisSize,			// height
                                D3DX_DEFAULT,           // refresh rate
                                &m_pD3DX                // returned D3DX interface
                       );
    if( FAILED(hr) )
        return hr;

    m_pD3DDev = m_pD3DX->GetD3DDevice();
    if( m_pD3DDev == NULL )
        return E_FAIL;

    m_pD3D = m_pD3DX->GetD3D();
    if( m_pD3D == NULL )
        return E_FAIL;

    m_pDD = m_pD3DX->GetDD();
    if( m_pDD == NULL )
        return E_FAIL;

    m_bD3DXReady = TRUE;

	pTester				= new CTester();

    return InitRenderer();
}

// ***************************************************************************
// Renderer Initialization Code
// ***************************************************************************

HRESULT CGravity::InitRenderer()
{
    if( !m_bD3DXReady )
        return E_FAIL;

    m_pD3DX->SetClearColor		(D3DRGBA(0,0,0,0));
    m_pD3DX->Clear				(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER);
    m_pD3DDev->SetRenderState	(D3DRENDERSTATE_DITHERENABLE,			FALSE			);
    m_pD3DDev->SetRenderState	(D3DRENDERSTATE_SPECULARENABLE,			FALSE			);
	m_pD3DDev->SetRenderState	(D3DRENDERSTATE_LIGHTING,				TRUE			);
	m_pD3DDev->SetRenderState   (D3DRENDERSTATE_COLORVERTEX,			FALSE			);
	m_pD3DDev->SetRenderState   (D3DRENDERSTATE_ALPHABLENDENABLE,		FALSE			);
    m_pD3DDev->SetRenderState   (D3DRENDERSTATE_ZENABLE,				TRUE			);
	m_pD3DDev->SetRenderState   (D3DRENDERSTATE_ZWRITEENABLE,			TRUE			);
	m_pD3DDev->SetRenderState   (D3DRENDERSTATE_FOGENABLE,				FALSE			);
	m_pD3DDev->SetRenderState   (D3DRENDERSTATE_NORMALIZENORMALS,		FALSE			);
	m_pD3DDev->SetRenderState	(D3DRENDERSTATE_SHADEMODE,				D3DSHADE_FLAT	);
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG1,				D3DTA_TEXTURE	);
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLORARG2,				D3DTA_DIFFUSE	);
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_COLOROP,					D3DTOP_SELECTARG2);
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MINFILTER,				D3DTFN_POINT	);
    m_pD3DDev->SetTextureStageState( 0, D3DTSS_MAGFILTER,				D3DTFG_POINT	);
	m_pD3DDev->SetTexture(0,0);

	D3DMATERIAL7	M;
    memset(&M,0,sizeof(D3DMATERIAL7));
    M.diffuse.r  = 1.f;
    M.diffuse.g  = 1.f;
    M.diffuse.b  = 1.f;
    M.diffuse.a  = 1.f;
    M.ambient.r  = 1.f;
    M.ambient.g  = 1.f;
    M.ambient.b  = 1.f;
    M.ambient.a  = 1.f;
    M.specular.r = .0f;
    M.specular.g = .0f;
    M.specular.b = .0f;
    M.specular.a = .0f;
    M.power		 = .0f;
	m_pD3DDev->SetMaterial		(&M);

    return S_OK;
}


void CGravity::UnInit()
{
    _RELEASE(m_pD3DDev);
    _RELEASE(m_pD3D);
    _RELEASE(m_pDD);
    _RELEASE(m_pD3DX);
    m_bD3DXReady = FALSE;
    D3DXUninitialize();
}

// *********************************************************************************
// Rendering Code
// *********************************************************************************
void ShowStats()
{
	DWORD dwTime = timeGetTime()-STAT.dwStartTime;
	float fSPS = 1000.f*float(STAT.dwSlots)/float(dwTime);
	char buf[100];
	sprintf(buf,"%2.1f", fSPS);
	SetWindowText(GetDlgItem(logWindow,IDC_STAT_SPS),buf);
	sprintf(buf,"%d",STAT.dwClipped);
	SetWindowText(GetDlgItem(logWindow,IDC_STAT_VC),buf);
	float progr = pTester->GetProgress();
	DWORD sec   = DWORD(float(dwTime)/float(progr*1000.f))-(dwTime/1000);
	sprintf(buf,"%2.0d:%2.0d",sec/60,sec%60);
	SetWindowText(GetDlgItem(logWindow,IDC_STAT_TIME),buf);
	DWORD pos = DWORD(progr*100.f);
	SendMessage(GetDlgItem(logWindow,IDC_PB), PBM_SETPOS, pos, 0); 
}

HRESULT CGravity::Draw()
{

    HRESULT		hr;
		
    if( !m_bD3DXReady ) return E_FAIL;
    if( !m_bActive )    return S_OK;

    hr = m_pD3DDev->BeginScene();
    if ( SUCCEEDED(hr) )
    {
        hr = m_pD3DX->Clear(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER);
        if ( FAILED(hr) ) return hr;

		if (pTester) pTester->OnRender		();
        m_pD3DDev->EndScene		();
		if (pTester) pTester->OnSurfaceReady	();
		if (pTester && pTester->bDone) {
			ShowStats();
			_DELETE(pTester);
		}
    }
/*
    hr = m_pD3DX->UpdateFrame( 0 );
    if ( hr == DDERR_SURFACELOST || hr == DDERR_SURFACEBUSY )
        hr = HandleModeChanges();
*/
	if (pTester) {
		tmToShow-=T.GetAsync(); T.Start();
		if (tmToShow<0) {
			tmToShow+=TSHOW;
			ShowStats();
		}
	}

    return hr;
}

HRESULT CGravity::HandleModeChanges()
{
    HRESULT hr;
    hr = m_pDD->TestCooperativeLevel();

    if( SUCCEEDED( hr ) || hr == DDERR_WRONGMODE )
    {
        UnInit();

        if(FAILED(hr = InitD3DX()))
            return hr;
    }
    else if( hr != DDERR_EXCLUSIVEMODEALREADYSET &&
             hr != DDERR_NOEXCLUSIVEMODE )
    {
        // Busted!!
        return hr;
    }
    return S_OK;
}

// ****************************************************************************
// Windowing Code...
// ****************************************************************************

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg)
    {
    case WM_ACTIVATEAPP:
        break;
    case WM_CREATE:
        break;
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        if( g_pGravity && g_pGravity->m_bD3DXReady )
        {
            HRESULT hr;
            
            if( wParam == SIZE_MINIMIZED )
            {
                g_pGravity->m_bActive = FALSE;
                break;
            }
            else if( LOWORD(lParam)>0 && HIWORD(lParam)>0 )
            {
                if( FAILED(hr = g_pGravity->m_pD3DX->Resize(LOWORD(lParam),HIWORD(lParam))))
                {
                    InterpretError(hr);
                    g_pGravity->m_bD3DXReady = FALSE;
                    PostQuitMessage(0);
                }
            }
            g_pGravity->m_bActive = TRUE;
            
        }
        break;
    case WM_KEYDOWN:
        switch( wParam )
        {
        case VK_ESCAPE:
        {    
            PostQuitMessage(0);
            break;
        }
        }
        break;
   case WM_COMMAND:
        break;
    default:
        break;
    }

    return DefWindowProc(hwnd,uMsg,wParam,lParam);

}

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    LPSTR lpszCmdLine, int nCmdShow) 
{
    HRESULT     hr;    
    MSG         msg; 
    HACCEL      hAccelApp;
    HCURSOR     hcur = NULL;
    int         ret = 0;

    // Startup options
    int res = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_START), 0, startDlgProc );
    if (res!=IDOK) return 0;

   	CreateLog	();
	InitMath	();
	InitCommonControls();

	g_pGravity = new CGravity; // set up our data AFTER starting up d3dx!
    if( !g_pGravity )
    {
        ret = -1;
        goto Exit;
    }
    
    hAccelApp = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_APP_ACCELERATOR));
    if ( !hAccelApp )
    {
        ret = -1;
        goto Exit;
    }

    // Initialize D3DX
    hr = g_pGravity->InitD3DX();
    if ( FAILED(hr) )
    {
        InterpretError(hr);
        ret = -1;
        goto Exit;
    }

	// Starting
	T.Start();
	STAT.dwClipped=0;
	STAT.dwSlots  =0;
	STAT.dwStartTime = timeGetTime();
	STAT.fProgress=0;
    SendMessage(GetDlgItem(logWindow,IDC_PB), PBM_SETRANGE, 0, MAKELPARAM(0, 100)); 
    SendMessage(GetDlgItem(logWindow,IDC_PB), PBM_SETPOS, 0, 0); 

    BOOL bGotMsg;
    PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );
    while( WM_QUIT != msg.message  )
    {
        bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
 
        if( bGotMsg )
        {
            if ( !TranslateAccelerator( logWindow, hAccelApp, &msg ) )
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
         }
        else
        {
            if( g_pGravity && g_pGravity->m_bActive )
            {
                hr = g_pGravity->Draw();
                if( FAILED(hr) )
                {
                    InterpretError(hr);
                    g_pGravity->m_bD3DXReady = FALSE;
                    PostQuitMessage(-1);
                }
            }
            else
            {
                WaitMessage();
            }
        }
    }
    delete g_pGravity; // clean up our data BEFORE shutting down d3dx!

Exit:
    if(hcur)
        DestroyCursor(hcur);
    
    return ret;
}
