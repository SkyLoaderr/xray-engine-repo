// xrOcclusion.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "xrOcclusion.h"

//-----------------------------------------------------------------------------
#pragma comment(lib,"d3dx.lib")
#pragma comment(lib,"ddraw.lib")
//-----------------------------------------------------------------------------
// Globals
HWND					g_hwnd		= 0;
LPD3DXCONTEXT			g_pContext	= 0;
LPDIRECT3D7				g_pD3D		= 0;
LPDIRECT3DDEVICE7		g_pDevice	= 0;
LPDIRECTDRAWSURFACE7	g_pBB		= 0;
float					g_viewfar	= 0;


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


//-----------------------------------------------------------------------------
// Name: WndProc()
// Desc: Static msg handler which passes messages to the application class.
//-----------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch(uMsg)
    {
	case WM_ACTIVATE:
		return 0;
	case WM_SETCURSOR:
		return 1;
	case WM_SYSCOMMAND:
		// Prevent moving/sizing and power loss in fullscreen mode
		switch( wParam ){
		case SC_MOVE:
		case SC_SIZE:
		case SC_MAXIMIZE:
		case SC_MONITORPOWER:
			return 1;
			break;
		}
		break;
	case WM_CLOSE:
		return 0;
    default:
        break;
    }
    return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

#define CHR(a) { hr = a; if(FAILED(hr)) return hr; }

ORM_API HRESULT __cdecl	ORM_Create	(BOOL bHW, float v_far)
{
    HRESULT hr;

	// Window
    // Register the windows class
	HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(0);
	WNDCLASS wndClass = { 0, WndProc, 0, 0, hInstance,
		0,0,0, NULL, "_OCCLUDER_" };
	CHR(RegisterClass( &wndClass ));
	
	// Set the window's initial style
	DWORD m_dwWindowStyle = WS_BORDER|WS_DLGFRAME;
	
	// Set the window's initial width
	RECT rc;
	SetRect( &rc, 0, 0, 256, 256 );
	AdjustWindowRect( &rc, m_dwWindowStyle, FALSE );
	
	// Create the render window
	g_hwnd = CreateWindow("_OCCLUDER_", "Occluder", m_dwWindowStyle,
		CW_USEDEFAULT, CW_USEDEFAULT,
		(rc.right-rc.left), (rc.bottom-rc.top), 0L,
		0, hInstance, 0L );
	
	
    CHR(D3DXInitialize());

    CHR(D3DXCreateContextEx(	bHW?D3DX_HWLEVEL_RASTER:D3DX_HWLEVEL_2D,		// 
                                0,						// flags
                                g_hwnd,					// window
                                0,						// focusWnd
                                16,			            // colorbits
                                0,						// alphabits
                                16,						// numdepthbits
                                0,                      // numstencilbits
                                D3DX_DEFAULT,           // numbackbuffers
                                256,					// width
                                256,					// height
                                D3DX_DEFAULT,           // refresh rate
                                &g_pContext             // returned D3DX interface
                       ));

    g_pDevice = g_pContext->GetD3DDevice();
    if( g_pDevice == NULL ) return E_FAIL;

	g_pBB = g_pContext->GetBackBuffer(0);
    if( g_pBB == NULL ) return E_FAIL;

	g_pD3D = g_pContext->GetD3D();
    if( g_pD3D == NULL ) return E_FAIL;

	// renderer
    g_pContext->SetClearColor	(D3DRGBA(0,0,0,0));
    g_pContext->Clear			(D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER);
    g_pDevice->SetRenderState	(D3DRENDERSTATE_DITHERENABLE,			FALSE			);
    g_pDevice->SetRenderState	(D3DRENDERSTATE_SPECULARENABLE,			FALSE			);
	g_pDevice->SetRenderState	(D3DRENDERSTATE_LIGHTING,				TRUE			);
	g_pDevice->SetRenderState   (D3DRENDERSTATE_COLORVERTEX,			FALSE			);
	g_pDevice->SetRenderState   (D3DRENDERSTATE_ALPHABLENDENABLE,		FALSE			);
    g_pDevice->SetRenderState   (D3DRENDERSTATE_ZENABLE,				TRUE			);
	g_pDevice->SetRenderState   (D3DRENDERSTATE_ZWRITEENABLE,			TRUE			);
	g_pDevice->SetRenderState   (D3DRENDERSTATE_FOGENABLE,				FALSE			);
	g_pDevice->SetRenderState   (D3DRENDERSTATE_NORMALIZENORMALS,		FALSE			);
	g_pDevice->SetRenderState	(D3DRENDERSTATE_SHADEMODE,				D3DSHADE_FLAT	);
	g_pDevice->SetRenderState   (D3DRENDERSTATE_CULLMODE,				D3DCULL_CCW		);
    g_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG1,				D3DTA_TEXTURE	);
    g_pDevice->SetTextureStageState( 0, D3DTSS_COLORARG2,				D3DTA_DIFFUSE	);
    g_pDevice->SetTextureStageState( 0, D3DTSS_COLOROP,					D3DTOP_SELECTARG2);
    g_pDevice->SetTextureStageState( 0, D3DTSS_MINFILTER,				D3DTFN_POINT	);
    g_pDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER,				D3DTFG_POINT	);
	g_pDevice->SetTexture		(0,0);

	// material
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
	g_pDevice->SetMaterial		(&M);

	// occlusion kernel
	g_viewfar = v_far;
	extern void KernelSetup();
	KernelSetup();

	return S_OK;
}

ORM_API HRESULT __cdecl	ORM_Destroy	()
{
	extern void KernelDestroy();
	KernelDestroy();

	g_pD3D->Release();
	g_pBB->Release();
	g_pDevice->Release();
	g_pContext->Release();
	DestroyWindow(g_hwnd);

	return S_OK;
}
