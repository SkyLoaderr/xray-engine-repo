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
LPDIRECT3DDEVICE8		g_pd3dDevice	= NULL;

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
	ObjectInstance *pFirstObjInst = m_ObjectInstRoot.ListNext();
	ASSERT ( pFirstObjInst != NULL );
	Object *pFirstObj = pFirstObjInst->pObj;

	{
		MeshPt *ppt;
		MeshEdge *pedge;
		MeshTri *ptri;

		// Move the viewer here. FrameMove() doesn't get called during pause mode,
		// and we still want to move around then.

		static DWORD dwLastTime = timeGetTime();

		// Get the relative time, in seconds
		DWORD dwThisTime = timeGetTime();
		float fTimeKey = (float)( dwThisTime - dwLastTime ) * 0.001f;
		if ( fTimeKey > 0.1f )
		{
			// A hiccup in framerate - clamp it.
			fTimeKey = 0.1f;
		}
		dwLastTime = dwThisTime;

		// Clear the render target
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00000000f, 1.0f, 0L );

		// Begin the scene
		if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
		{
			m_pd3dDevice->SetTransform( D3DTS_VIEW,       &m_matView );
			m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProjClose );

			ObjectInstance *pObjInst = pFirstObjInst;
			Object *pObj = pObjInst->pObj;

			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG2 );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2 );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );


			m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

			m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
			m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );


			m_pd3dDevice->SetTexture( 0, NULL );

			m_pd3dDevice->SetTransform( D3DTS_WORLD,      &pObjInst->matOrn );

			m_pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );
			m_pd3dDevice->SetRenderState ( D3DRS_ZENABLE, TRUE );
			m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0xffff0000 );
			m_pd3dDevice->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );

			// Draw the current state of the object.
			// Render twice.
			// Current level.
			pObjInst->RenderCurrentObject ( m_pd3dDevice, pObj->iCurSlidingWindowLevel );
			// Next level.
			m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0xff800000 );
			pObjInst->RenderCurrentObject ( m_pd3dDevice, pObj->iCurSlidingWindowLevel + 1 );

			// Tweak the projection matrix so that things are a bit more visible.
			m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProjCloseZbias );

			// Render visible edges again in a different colour.
			m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0xffffff00 );
			m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
			pObjInst->RenderCurrentEdges ( m_pd3dDevice );

			// Output statistics
			m_pFont->DrawText( 2,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );
			m_pFont->DrawText( 2, 20, D3DCOLOR_ARGB(255,255,255,0), m_strDeviceStats );

			char strTemp[1000];
			sprintf ( strTemp, "Sliding window level %i, error tolerance %f%%", pFirstObj->iCurSlidingWindowLevel, m_fSlidingWindowErrorTolerance * 100.0f );
			m_pFont->DrawText( 2, 40, D3DCOLOR_ARGB(255,255,255,0), strTemp );

			// End the scene.
			m_pd3dDevice->EndScene();
		}
	}

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
	m_pFont->InitDeviceObjects( m_pd3dDevice );

	SetMenuItems();

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{

	// Let the outside world know what device we're using.
	g_pd3dDevice = m_pd3dDevice;


	m_pFont->RestoreDeviceObjects();


	// Start up DInput.
	HRESULT hr;
	hr = InitDirectInput( m_hWnd );



	// Set the transform matrices
	D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f, 5.0f );
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXMATRIX  matWorld;

	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixLookAtLH( &m_matView, &vEyePt, &vLookatPt, &vUpVec );

	// Three different projection matrices for different purposes.
	FLOAT fAspect = m_d3dsdBackBuffer.Width / (FLOAT)m_d3dsdBackBuffer.Height;
	// For close-in work editing collapse orders.
	D3DXMatrixPerspectiveFovLH( &m_matProjClose, D3DX_PI/4, fAspect, 0.1f, 50.0f );
	// For close-in work plus a Zbias for the lines.
	D3DXMatrixPerspectiveFovLH( &m_matProjCloseZbias, D3DX_PI/4, fAspect, 0.1f + 0.001f, 50.0f + 0.001f );
	// For more distant viewing.
	D3DXMatrixPerspectiveFovLH( &m_matProjFar, D3DX_PI/4, fAspect, 1.0f, 500.0f );


	m_pd3dDevice->SetTransform( D3DTS_WORLD,      &matWorld );
	m_pd3dDevice->SetTransform( D3DTS_VIEW,       &m_matView );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProjClose );

	// Set misc render states
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );


	// Make an example object.
	static BOOL bAlreadyDone = FALSE;
	if ( !bAlreadyDone )
	{
		// Only needs doing once, but annoyingly requires a D3D device to do the init.
		bAlreadyDone = TRUE;
		m_pObject->CreateTestObject(m_pd3dDevice);
		m_pObject->MakeCurrentObjectFromPerm();
	}


	// And make a few lights.
	D3DLIGHT8 light;
	light.Type = D3DLIGHT_DIRECTIONAL;
	D3DXCOLOR col = D3DXCOLOR ( 1.0f, 1.0f, 1.0f, 0.0f );
	light.Diffuse  = D3DXCOLOR ( 1.0f, 1.0f, 1.0f, 0.0f );
	light.Ambient  = D3DXCOLOR ( 0.0f, 0.0f, 0.0f, 0.0f );
	light.Specular = D3DXCOLOR ( 0.0f, 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vec ( -1.0f, -1.0f, -1.0f );
	D3DXVec3Normalize ( &vec, &vec );
	light.Direction = vec;
	light.Position = D3DXVECTOR3 ( 0.0f, 0.0f, 0.0f );
	light.Falloff = 1.0f;
	light.Attenuation0 = 0.0f;
	light.Attenuation1 = 1.0f;
	light.Attenuation2 = 0.0f;
	light.Phi = 0.0f;
	light.Theta = 0.0f;

	m_pd3dDevice->SetLight ( 0, &light );
	m_pd3dDevice->LightEnable ( 0, TRUE );

	m_pd3dDevice->SetRenderState ( D3DRS_AMBIENT, 0x00101010 );

	D3DMATERIAL8 mat;
	mat.Ambient = D3DXCOLOR ( 1.0f, 1.0f, 1.0f, 0.0f );
	mat.Diffuse = D3DXCOLOR ( 1.0f, 1.0f, 1.0f, 0.0f );
	mat.Specular = D3DXCOLOR ( 0.5f, 0.5f, 0.5f, 0.0f );
	mat.Emissive= D3DXCOLOR ( 0.0f, 0.0f, 0.0f, 0.0f );
	mat.Power = 20.f;

	m_pd3dDevice->SetMaterial ( &mat );


	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
    m_pFont->InvalidateDeviceObjects();


    FreeDirectInput();

	// Warn things that D3D is leaving now.
	for ( ObjectInstance *pOI = m_ObjectInstRoot.ListNext(); pOI != NULL; pOI = pOI->ListNext() )
	{
		pOI->AboutToChangeDevice();
	}

	// Let the outside world know.
	g_pd3dDevice = NULL;


    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    m_pFont->DeleteDeviceObjects();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device intialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS8* pCaps, DWORD dwBehavior, 
                                          D3DFORMAT Format )
{
	// Whatever - no special properties required.
	return S_OK;
}
