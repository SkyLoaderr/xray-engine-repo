#include "stdafx.h"
#include "resource.h"

extern LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

void CRenderDevice::Initialize()
{
	Log("Initializing Engine...");
	TimerGlobal.Start			();

	// Unless a substitute hWnd has been specified, create a window to render into
    if( m_hWnd == NULL)
    {
		const char*	wndclass ="_XRAY_";

        // Register the windows class
		HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(0);
        WNDCLASS wndClass = { 0, WndProc, 0, 0, hInstance,
                              LoadIcon( hInstance, MAKEINTRESOURCE(IDI_XRAY) ),
                              LoadCursor( NULL, IDC_ARROW ),
                              (HBRUSH)GetStockObject(GRAY_BRUSH),
                              NULL, wndclass };
        RegisterClass( &wndClass );

        // Set the window's initial style
        m_dwWindowStyle = WS_BORDER|WS_DLGFRAME;

        // Set the window's initial width
        RECT rc;
        SetRect( &rc, 0, 0, 640, 480 );
        AdjustWindowRect( &rc, m_dwWindowStyle, FALSE );

        // Create the render window
        m_hWnd = CreateWindow( wndclass, "XRAY Engine", m_dwWindowStyle,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               (rc.right-rc.left), (rc.bottom-rc.top), 0L,
                               0, hInstance, 0L );
    }

    // Save window properties
    m_dwWindowStyle = GetWindowLong( m_hWnd, GWL_STYLE );
    GetWindowRect( m_hWnd, &m_rcWindowBounds );
    GetClientRect( m_hWnd, &m_rcWindowClient );

	// Command line
	char *lpCmdLine		= Core.Params;
	if (strstr(lpCmdLine,"-multipass")!=NULL)	HW.Caps.bForceMultipass		= TRUE;
	else										HW.Caps.bForceMultipass		= FALSE;
	if (strstr(lpCmdLine,"-emu_fog")!=NULL)		HW.Caps.bForceVertexFog		= TRUE;
	else										HW.Caps.bForceVertexFog		= FALSE;
	if (strstr(lpCmdLine,"-gpu_sw")!=NULL)		HW.Caps.bForceGPU_SW		= TRUE;
	else										HW.Caps.bForceGPU_SW		= FALSE;
	if (strstr(lpCmdLine,"-gpu_nopure")!=NULL)	HW.Caps.bForceGPU_NonPure	= TRUE;
	else										HW.Caps.bForceGPU_NonPure	= FALSE;

	// Startup shaders
	Shader.xrStartUp	();
}

void CRenderDevice::ShutDown()
{
	// destroy context
	Destroy				();

	// destroy shaders
	Shader.xrShutDown	();

	// destroy streams
}
