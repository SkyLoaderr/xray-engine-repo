/* Copyright (C) Tom Forsyth, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Tom Forsyth, 2001"
 */

#include "gemvipm.h"

#define STRICT
#define D3D_OVERLOADS
#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <stdio.h>
#include <D3DX8.h>
#include <DInput.h>
#include "resource.h"
#include "commctrl.h"
#include <list>


// Fudge - exposes m_pd3dDevice to the outside world.
HINSTANCE				g_hInst			= NULL;
BOOL					g_bActive		= TRUE;
BOOL					g_bExclusive	= FALSE;
BOOL					g_bKeyDownCtrl	= FALSE;
BOOL					g_bKeyDownShift	= FALSE;
BOOL					m_bOrbitStrafe	= TRUE;

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CMyD3DApplication d3dApp;

	// DInput needs the instance.
    g_hInst = hInst;


    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}

//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
	m_CameraHPB.set		(0,0,0);
	m_CameraDist		= 5.f;

    m_strWindowTitle	= _T("VIPM tester");
    m_bUseDepthBuffer	= TRUE;

    m_pFont				= new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );

	m_fSlidingWindowErrorTolerance = 0.1f;	// 10%

	g_iMaxNumTrisDrawn = -1;	// No limit.
	g_bShowVIPMInfo = FALSE;
}




//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
	m_pObject				= xr_new<Object>();
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
void CMyD3DApplication::BuildCamera()
{
	// Set the transform matrices
	D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f, 5.0f );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	Fvector3 D;
	D.setHP	(m_CameraHPB.x,m_CameraHPB.y);
	Fvector3 P;
	P.mul	(D,-m_CameraDist);

	vEyePt	= D3DXVECTOR3(P.x,P.y,P.z);
	D3DXMatrixLookAtLH( &m_matView, &vEyePt, &vLookatPt, &vUpVec );
}

HRESULT CMyD3DApplication::FrameMove()
{
	Object *pFirstObj = m_pObject;

	// camera Move/Rotate
	HRESULT hr;
	// Where's the mouse?
	hr = UpdateInputState( m_hWnd );

	DWORD dwButtons = 0;
#define BUTTON_LEFT		0x01
#define BUTTON_RIGHT	0x02
#define BUTTON_MID		0x04

	if ( g_dims.rgbButtons[0] & 0x80 )
	{
		dwButtons |= BUTTON_LEFT;
	}
	if ( g_dims.rgbButtons[1] & 0x80 )
	{
		dwButtons |= BUTTON_RIGHT;
	}
	if ( g_dims.rgbButtons[2] & 0x80 )
	{
		dwButtons |= BUTTON_MID;
	}



	// Spot going-down and going-up buttons.
	static DWORD dwPreviousButtons = 0;
	DWORD dwButtonsGoneDown = dwButtons & ~dwPreviousButtons;
	DWORD dwButtonsGoneUp = ~dwButtons & dwPreviousButtons;
	dwPreviousButtons = dwButtons;

	// If shift is not down, but some mouse buttons are, this may be a movement of some sort.
	if ( ( dwButtons != 0 ) && !g_bKeyDownShift ){
		if ( !g_bExclusive ) SetExclusiveMode ( TRUE, m_hWnd );

		{
			if ( dwButtons == BUTTON_LEFT ){
				m_CameraHPB.x	-= ((float)g_dims.lX * 0.007f);
				m_CameraHPB.y	-= ((float)g_dims.lY * 0.007f);;
			}else if ( dwButtons == BUTTON_RIGHT ){
				m_CameraDist	-= (float)g_dims.lY * 0.01f;
			}
		}
	}else{
		// Not moving.
		// Make sure we are out of exclusive mode.
		SetExclusiveMode( FALSE, m_hWnd );
	}

	BuildCamera			();

    return S_OK;
}

// Sets menu items up correctly.
void CMyD3DApplication::SetMenuItems()
{
	CheckMenuItem( GetMenu(m_hWnd), IDM_SHOW_VIPM_INFO,
		   g_bShowVIPMInfo ? MF_CHECKED : MF_UNCHECKED );

	CheckMenuItem( GetMenu(m_hWnd), IDM_CACHE_DISPLAY_ENABLE,
		   ( g_iMaxNumTrisDrawn > 0 ) ? MF_CHECKED : MF_UNCHECKED );
	CheckMenuItem( GetMenu(m_hWnd), IDM_CACHE_OPTIMISE_CHEAP,
		   g_bUseFastButBadOptimise ? MF_CHECKED : MF_UNCHECKED );

}

//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
	xr_delete(m_pObject);

    SAFE_DELETE( m_pFont );
    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam )
{

	if( WM_ACTIVATE == uMsg )
	{
		// Free the mouse.
		SetExclusiveMode ( FALSE, hWnd );

		// sent when window changes active state
		if ( WA_INACTIVE == wParam )
		{
			g_bActive = FALSE;
		}
		else
		{
			g_bActive = TRUE;
		}

		// Set exclusive mode access to the mouse based on active state
		SetAcquire( hWnd );

		return TRUE;
	}
	else if ( ( WM_KEYDOWN == uMsg ) || ( WM_KEYUP == uMsg ) ||
			  ( WM_SYSKEYDOWN == uMsg ) || ( WM_SYSKEYUP == uMsg ) )
	{
		// Top bit (bit31) is current key status.
		if ( wParam == VK_CONTROL )
		{
			g_bKeyDownCtrl = ( ( lParam & 0x80000000 ) == 0 );
		}
		else if ( wParam == VK_SHIFT )
		{
			g_bKeyDownShift = ( ( lParam & 0x80000000 ) == 0 );
		}
	}
    if( uMsg == WM_COMMAND )
    {
        switch( LOWORD(wParam) )
        {
		case IDM_VIPM_COLLAPSE_NEXT:
			m_pObject->DoCollapse();
			break;
		case IDM_VIPM_COLLAPSE_PREV:
			m_pObject->UndoCollapse();
			break;
		case IDM_VIPM_COLLAPSE_NEXT_10:
			{
				for ( int i = 10; i > 0; i-- )
				{
					m_pObject->DoCollapse();
				}
			}
			break;
		case IDM_VIPM_COLLAPSE_PREV_10:
			{
				for ( int i = 10; i > 0; i-- )
				{
					m_pObject->UndoCollapse();
				}
			}
			break;
		case IDM_VIPM_COLLAPSE_ALL:
			while ( m_pObject->DoCollapse() ){}
			break;
		case IDM_VIPM_COLLAPSE_NONE:
			while ( m_pObject->UndoCollapse() ){}
			break;
		case IDM_VIPM_COLLAPSE_DEL:
			// Do all collapses.
			while ( m_pObject->DoCollapse() ){}
			// Delete the last collapse.
			m_pObject->BinEdgeCollapse();
			break;

		case IDM_VIPM_COLLAPSE_DO_BEST_ALL:
			CollapseAll();
			break;

		case IDM_SLIDING_WINDOW_INC:
			m_fSlidingWindowErrorTolerance *= 1.1f;
			break;
		case IDM_SLIDING_WINDOW_DEC:
			m_fSlidingWindowErrorTolerance /= 1.1f;
			break;

		case IDM_CACHE_DISPLAY_ENABLE:
			g_iMaxNumTrisDrawn = -g_iMaxNumTrisDrawn;
			break;

		case IDM_CACHE_OPTIMISE_CHEAP:
			g_bUseFastButBadOptimise = !g_bUseFastButBadOptimise;
			break;

		case IDM_CACHE_DISPLAY_INC:
			if ( g_iMaxNumTrisDrawn > 0 )
			{
				g_iMaxNumTrisDrawn++;
			}
			break;
		case IDM_CACHE_DISPLAY_DEC:
			if ( g_iMaxNumTrisDrawn > 1 )
			{
				g_iMaxNumTrisDrawn--;
			}
			break;
		case IDM_CACHE_DISPLAY_INC_10:
			if ( g_iMaxNumTrisDrawn > 0 )
			{
				g_iMaxNumTrisDrawn += 10;
			}
			break;
		case IDM_CACHE_DISPLAY_DEC_10:
			if ( g_iMaxNumTrisDrawn > 1 )
			{
				g_iMaxNumTrisDrawn -= 10;
				if ( g_iMaxNumTrisDrawn < 1 )
				{
					g_iMaxNumTrisDrawn = 1;
				}
			}
			break;

        }

		SetMenuItems();

    }


	switch ( uMsg )
	{
	case WM_MOUSELEAVE:
	case WM_MOUSEMOVE:
	case WM_MOUSEHOVER:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_LBUTTONDOWN  :
	case WM_LBUTTONUP    :
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN  :
	case WM_RBUTTONUP    :
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN  :
	case WM_MBUTTONUP    :
	case WM_MBUTTONDBLCLK:
	case WM_NCHITTEST:
	case WM_CONTEXTMENU:
	case WM_SETCURSOR:
		// Don't free the mouse - these happen when moving.
		break;
	default:
		// Free the mouse, just in case.
		SetExclusiveMode ( FALSE, hWnd );
		break;
	}


    return CD3DApplication::MsgProc( hWnd, uMsg, wParam, lParam );
}
