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

/*
#define STRICT
#define D3D_OVERLOADS
#include <windows.h>
#include <tchar.h>
#include <math.h>
#include <stdio.h>
#include <D3DX8.h>
#include <DInput.h>
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "DXUtil.h"
#include "resource.h"
#include "commctrl.h"
#include <list>
*/

#include "object.h"

IDirectInput8*			g_pDI			= NULL;
IDirectInputDevice8*	g_pMouse		= NULL;
DIMOUSESTATE2			g_dims;      // DirectInput mouse state structure

//-----------------------------------------------------------------------------
// Function: SetAcquire
//
// Description: 
//      Acquire or unacquire the mouse, depending on if the app is active
//       Input device must be acquired before the GetDeviceState is called
//
//-----------------------------------------------------------------------------
HRESULT SetAcquire( HWND hWnd )
{
	// nothing to do if g_pMouse is NULL
	if (NULL == g_pMouse)
		return S_FALSE;

	if (g_bActive) 
	{
		// acquire the input device 
		g_pMouse->Acquire();
	} 
	else 
	{
		// unacquire the input device 
		g_pMouse->Unacquire();
	}

	return S_OK;
}




//-----------------------------------------------------------------------------
// Function: UpdateInputState
//
// Description: 
//      Get the input device's state and display it.
//
//-----------------------------------------------------------------------------
char g_szMouseText[256];    // current  mouse state text
HRESULT UpdateInputState( HWND hWnd )
{
	if (NULL != g_pMouse) 
	{
		HRESULT hr;

		hr = DIERR_INPUTLOST;

		// if input is lost then acquire and keep trying 
		while ( DIERR_INPUTLOST == hr ) 
		{
			// get the input's device state, and put the state in g_dims
			hr = g_pMouse->GetDeviceState( sizeof(DIMOUSESTATE2), &g_dims );

			if ( hr == DIERR_INPUTLOST )
			{
				// DirectInput is telling us that the input stream has
				// been interrupted.  We aren't tracking any state
				// between polls, so we don't have any special reset
				// that needs to be done.  We just re-acquire and
				// try again.
				hr = g_pMouse->Acquire();
				if ( FAILED(hr) )
				{
					return hr;
				}
			}
		}

		if ( FAILED(hr) )
		{
			return hr;
		}

	}
	else
	{
		// No mouse!
	}

	return S_OK;
}




//-----------------------------------------------------------------------------
// Function: InitDirectInput
//
// Description: 
//      Initialize the DirectInput variables.
//
//-----------------------------------------------------------------------------
#define DIRECTINPUT_VERSION 0x0700
#include <dinput.h>

HRESULT InitDirectInput( HWND hWnd, bool bExclusive )
{
	HRESULT hr;

	// Register with the DirectInput subsystem and get a pointer
	// to a IDirectInput interface we can use.
	hr = DirectInput8Create( g_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&g_pDI, NULL );
	if ( FAILED(hr) )
	{
//		ASSERT ( FALSE );
		return hr;
	}

	// Obtain an interface to the system mouse device.
	hr = g_pDI->CreateDevice( GUID_SysMouse, &g_pMouse, NULL );
	if ( FAILED(hr) )
	{
		ASSERT ( FALSE );
		return hr;
	}

	// Set the data format to "mouse format" - a predefined data format 
	//
	// A data format specifies which controls on a device we
	// are interested in, and how they should be reported.
	//
	// This tells DirectInput that we will be passing a
	// DIMOUSESTATE2 structure to IDirectInputDevice::GetDeviceState.
	hr = g_pMouse->SetDataFormat( &c_dfDIMouse2 );
	if ( FAILED(hr) )
	{
		ASSERT ( FALSE );
		return hr;
	}

	// Set the cooperativity level to let DirectInput know how
	// this device should interact with the system and with other
	// DirectInput applications.
	DWORD dwFlags = DISCL_FOREGROUND;


	// KLUDGE! Win2k really hates FOREGROUND+EXCLUSIVE devices - don't know why.
	dwFlags |= DISCL_NONEXCLUSIVE;

	hr = g_pMouse->SetCooperativeLevel( hWnd, dwFlags );
	if ( FAILED(hr) )
	{
		ASSERT ( FALSE );
		return hr;
	}

	// And acquire it the first time (the windows message has already missed us).
	SetAcquire( hWnd );

	return S_OK;
}





//-----------------------------------------------------------------------------
// Function: FreeDirectInput
//
// Description: 
//      Initialize the DirectInput variables.
//
//-----------------------------------------------------------------------------
HRESULT FreeDirectInput()
{
	// Unacquire and release any DirectInputDevice objects.
	if (NULL != g_pMouse) 
	{
		// Unacquire the device one last time just in case 
		// the app tried to exit while the device is still acquired.
		g_pMouse->Unacquire();

		g_pMouse->Release();
		g_pMouse = NULL;
	}

	// Release any DirectInput objects.
	if (NULL != g_pDI) 
	{
		g_pDI->Release();
		g_pDI = NULL;
	}

	return S_OK;
}



// Enter or leave exclusive mode.
HRESULT SetExclusiveMode ( bool bExclusive, HWND hWnd )
{
	if ( ( bExclusive && g_bExclusive ) || ( !bExclusive && !g_bExclusive ) )
	{
		return ( DI_OK );
	}


	// Need to rip it all down and recreate - what a hassle.
	FreeDirectInput();

	HRESULT hr = InitDirectInput ( hWnd, bExclusive );
	if ( FAILED ( hr ) )
	{
		return ( hr );
	}

	g_bExclusive = bExclusive;

	return ( hr );
}



