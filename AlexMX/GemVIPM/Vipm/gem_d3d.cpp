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

			m_pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );
			m_pd3dDevice->SetRenderState ( D3DRS_ZENABLE, TRUE );
			m_pd3dDevice->SetRenderState ( D3DRS_TEXTUREFACTOR, 0xffff0000 );
			m_pd3dDevice->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );

			// Draw the current state of the object.
			// Render twice.
			// Current level.
			int r_tmp=0, r_tris=0, r_edges=0;
			RenderCurrentObject				(m_pObject->iCurSlidingWindowLevel,r_tris);
			// Next level.
			m_pd3dDevice->SetRenderState	(D3DRS_TEXTUREFACTOR, 0xff008000);
			RenderCurrentObject				(m_pObject->iCurSlidingWindowLevel + 1,r_tmp);
			r_tris+=r_tmp;

			// Tweak the projection matrix so that things are a bit more visible.
			m_pd3dDevice->SetTransform		(D3DTS_PROJECTION, &m_matProjCloseZbias);

			// Render visible edges again in a different colour.
			m_pd3dDevice->SetRenderState	(D3DRS_TEXTUREFACTOR, 0xffffff00);
			m_pd3dDevice->SetRenderState	(D3DRS_ZENABLE, TRUE);
			RenderCurrentEdges				(r_edges);

			// Output statistics
			m_pFont->DrawText( 2,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );
			m_pFont->DrawText( 2, 20, D3DCOLOR_ARGB(255,255,255,0), m_strDeviceStats );

			char strTemp[1000];
			sprintf ( strTemp, "Sliding level: %i. Error: %f%%. Collapses: %i.", m_pObject->iCurSlidingWindowLevel, m_fSlidingWindowErrorTolerance * 100.0f, m_pObject->iNumCollapses);
			m_pFont->DrawText( 2, 40, D3DCOLOR_ARGB(255,255,255,0), strTemp );

			sprintf ( strTemp, "Faces: %i. Edges: %i. Collapse: %i.", r_tris, r_edges,0);
			m_pFont->DrawText( 2, 60, D3DCOLOR_ARGB(255,255,255,0), strTemp );

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
	if (!bAlreadyDone){
		// Only needs doing once, but annoyingly requires a D3D device to do the init.
		bAlreadyDone = TRUE;
		CreateTestObject					();
//.		m_pObject->MakeCurrentObjectFromPerm();
	}


	// And make a few lights.
	D3DLIGHT8		light;
	light.Type		= D3DLIGHT_DIRECTIONAL;
	D3DXCOLOR col	= D3DXCOLOR ( 1.0f, 1.0f, 1.0f, 0.0f );
	light.Diffuse	= D3DXCOLOR ( 1.0f, 1.0f, 1.0f, 0.0f );
	light.Ambient	= D3DXCOLOR ( 0.0f, 0.0f, 0.0f, 0.0f );
	light.Specular	= D3DXCOLOR ( 0.0f, 0.0f, 0.0f, 0.0f );
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

void CMyD3DApplication::CreateTestObject ()
{
	ASSERT ( m_pObject ); 

	m_pObject->BinCurrentObject();

	ASSERT ( m_pObject->CurPtRoot.ListNext() == NULL );
	ASSERT ( m_pObject->CurTriRoot.ListNext() == NULL );
	ASSERT ( m_pObject->CurEdgeRoot.ListNext() == NULL );

#if 0
	// Make a cube.
	MeshPt *ppt000 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt001 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt010 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt011 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt100 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt101 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt110 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt111 = new MeshPt ( &PermPtRoot );

	ppt000->mypt.vPos = D3DXVECTOR3 ( -1.0f,  1.0f, -1.0f );
	ppt001->mypt.vPos = D3DXVECTOR3 (  1.0f,  1.0f, -1.0f );
	ppt010->mypt.vPos = D3DXVECTOR3 ( -1.0f,  1.0f,  1.0f );
	ppt011->mypt.vPos = D3DXVECTOR3 (  1.0f,  1.0f,  1.0f );
	ppt100->mypt.vPos = D3DXVECTOR3 ( -1.0f, -1.0f, -1.0f );
	ppt101->mypt.vPos = D3DXVECTOR3 (  1.0f, -1.0f, -1.0f );
	ppt110->mypt.vPos = D3DXVECTOR3 ( -1.0f, -1.0f,  1.0f );
	ppt111->mypt.vPos = D3DXVECTOR3 (  1.0f, -1.0f,  1.0f );

	// Dodgy normals.
	MeshPt *pt = PermPtRoot.ListNext();
	DWORD dwIndex = 0;
	while ( pt != NULL )
	{
		//pt->mypt.dwIndex = dwIndex++;
		D3DXVec3Normalize ( &(pt->mypt.vNorm), &(pt->mypt.vPos) );

		pt = pt->ListNext();
	}


	MeshTri *ptri;
	// Top.
	ptri = new MeshTri ( ppt000, ppt010, ppt011, &PermTriRoot, &PermEdgeRoot );
	ptri = new MeshTri ( ppt000, ppt011, ppt001, &PermTriRoot, &PermEdgeRoot );
	// Bottom.
	ptri = new MeshTri ( ppt100, ppt111, ppt110, &PermTriRoot, &PermEdgeRoot );
	ptri = new MeshTri ( ppt100, ppt101, ppt111, &PermTriRoot, &PermEdgeRoot );
	// Left.
	ptri = new MeshTri ( ppt000, ppt110, ppt010, &PermTriRoot, &PermEdgeRoot );
	ptri = new MeshTri ( ppt000, ppt100, ppt110, &PermTriRoot, &PermEdgeRoot );
	// Right.
	ptri = new MeshTri ( ppt001, ppt011, ppt111, &PermTriRoot, &PermEdgeRoot );
	ptri = new MeshTri ( ppt001, ppt111, ppt101, &PermTriRoot, &PermEdgeRoot );
	// Front
	ptri = new MeshTri ( ppt000, ppt001, ppt101, &PermTriRoot, &PermEdgeRoot );
	ptri = new MeshTri ( ppt000, ppt101, ppt100, &PermTriRoot, &PermEdgeRoot );
	// Back
	ptri = new MeshTri ( ppt010, ppt111, ppt011, &PermTriRoot, &PermEdgeRoot );
	ptri = new MeshTri ( ppt010, ppt110, ppt111, &PermTriRoot, &PermEdgeRoot );
#else

	HRESULT hres;

	// Make a teapotahedron.
	LPD3DXMESH pmeshTeapot;
	ASSERT ( g_pd3dDevice != NULL );		// Slight fudge - shame we need a D3D device.
	//hres = D3DXCreateTeapot ( g_pd3dDevice, &pmeshTeapot, NULL );
	// These are just some simpler test meshes
	//hres = D3DXCreatePolygon ( g_pd3dDevice, 1.0f, 6, &pmeshTeapot, NULL );
	//hres = D3DXCreateSphere ( g_pd3dDevice, 1.0f, 30, 15, &pmeshTeapot, NULL );
	hres = D3DXCreateSphere ( g_pd3dDevice, 1.0f, 10, 10, &pmeshTeapot, NULL );

	// OK, now extract the data.
	int iNumVerts = pmeshTeapot->GetNumVertices();
	int iNumFaces = pmeshTeapot->GetNumFaces();

	LPDIRECT3DVERTEXBUFFER8 pVertexBuffer;
	hres = pmeshTeapot->GetVertexBuffer ( &pVertexBuffer );
	D3DVERTEXBUFFER_DESC vbdesc;
	hres = pVertexBuffer->GetDesc ( &vbdesc );
	// Create my "smart" pointer.
	MyFVFPointer pFVF ( vbdesc.FVF );
	BYTE *pbData;
	hres = pVertexBuffer->Lock ( 0, pFVF.GetFVFSize() * iNumVerts, &pbData, D3DLOCK_READONLY );
	pFVF.SetCurVertex ( pbData );

	// The de-index list.
	MeshPt **ppPts = new MeshPt*[iNumVerts];

//	srand( (unsigned)time( NULL ) );

	for ( int i = 0; i < iNumVerts; i++ )
	{
		ppPts[i] = new MeshPt ( &m_pObject->CurPtRoot );
		ppPts[i]->mypt.vPos		= pFVF.Position();
		ppPts[i]->mypt.vNorm	= pFVF.Normal();
		ppPts[i]->mypt.fU		= 1.f;//float(rand())/32767.f;
		ppPts[i]->mypt.fV		= 1.f;//float(rand())/32767.f;

		ppPts[i]->mypt.dwIndex	= i;

		++pFVF;
	}

	hres = pVertexBuffer->Unlock();
	pVertexBuffer->Release();


	// And now the index buffer.
	LPDIRECT3DINDEXBUFFER8 pIndexBuffer;
	hres = pmeshTeapot->GetIndexBuffer ( &pIndexBuffer );
	D3DINDEXBUFFER_DESC ibdesc;
	hres = pIndexBuffer->GetDesc ( &ibdesc );
	// Unlikely to get any 32bpp indices, but check, just in case.
	// If you do - well, I leave that as an exercise for the reader :-)

	// Oh - just found this comment in the docs:
	//
	// D3DXMESH_32BIT 
	//   The mesh has 32-bit indices instead of 16-bit indices.
	//   A 32-bit mesh can support up to 2^32-1 faces and vertices.
	//   This flag is not supported and should not be used. 
	//
	// So, that answers that question!
	ASSERT ( ibdesc.Format == D3DFMT_INDEX16 );

	// Also, assume that this defines a trilist. Not sure if the mesh tells us
	// what the primitive type is anywhere.
	WORD *pIndex;
	ASSERT ( sizeof (*pIndex) * iNumFaces * 3 == ibdesc.Size );
	hres = pIndexBuffer->Lock ( 0, ibdesc.Size, (BYTE**)&pIndex, D3DLOCK_READONLY );

	for ( int j = 0; j < iNumFaces; j++ )
	{
		MeshPt *ppt[3];
		for ( int i = 0; i < 3; i++ )
		{
			ASSERT ( *pIndex < iNumVerts );
			ppt[i] = ppPts[*pIndex];
			pIndex++;
		}

		MeshTri *ptri = new MeshTri ( ppt[0], ppt[1], ppt[2], &m_pObject->CurTriRoot, &m_pObject->CurEdgeRoot );
	}

	hres = pIndexBuffer->Unlock();
	pIndexBuffer->Release();

	delete[] ppPts;

	// And finally bin the thing.
	pmeshTeapot->Release();
#endif

	m_pObject->iFullNumTris = 0;
	m_pObject->iFullNumPts	= 0;
	MeshPt *pt = m_pObject->CurPtRoot.ListNext();
	while ( pt != NULL )
	{
		// All the pts had better be the same material.
		pt = pt->ListNext();
		m_pObject->iFullNumPts++;
	}
	MeshTri *tri = m_pObject->CurTriRoot.ListNext();
	while ( tri != NULL )
	{
		// All the pts had better be the same material.
		tri = tri->ListNext();
		m_pObject->iFullNumTris++;
	}

	MeshEdge *edge = m_pObject->CurEdgeRoot.ListNext();
	while ( edge != NULL )
	{
		edge = edge->ListNext();
	}

	m_pObject->iNumCollapses = 0;
	m_pObject->iCurSlidingWindowLevel = 0;
	m_pObject->SetNewLevel ( m_pObject->iCurSlidingWindowLevel );
}


struct STDVERTEX
{
	D3DXVECTOR3 v;
	D3DXVECTOR3 norm;
	FLOAT       tu, tv;
};

#define STDVERTEX_FVF (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

// Renders the given material of the current state of the object.
// Set iSlidingWindowLevel to -1 if you don't care about level numbers.
void CMyD3DApplication::RenderCurrentObject (int iSlidingWindowLevel, int& r_tris)
{
	ASSERT ( m_pObject ); 

	STDVERTEX vert[3];

	HRESULT hres;

	hres = g_pd3dDevice->SetVertexShader ( STDVERTEX_FVF );

	MeshTri *tri = m_pObject->CurTriRoot.ListNext();
	while ( tri != NULL )
	{
		if ( ( iSlidingWindowLevel == -1 ) ||
				( iSlidingWindowLevel == tri->mytri.iSlidingWindowLevel ) )
		{
			// Draw this one.
			vert[0].v		= tri->pPt1->mypt.vPos;
			vert[0].norm	= tri->pPt1->mypt.vNorm;
			vert[0].tu		= tri->pPt1->mypt.fU;
			vert[0].tv		= tri->pPt1->mypt.fV;

			vert[1].v		= tri->pPt2->mypt.vPos;
			vert[1].norm	= tri->pPt2->mypt.vNorm;
			vert[1].tu		= tri->pPt2->mypt.fU;
			vert[1].tv		= tri->pPt2->mypt.fV;

			vert[2].v		= tri->pPt3->mypt.vPos;
			vert[2].norm	= tri->pPt3->mypt.vNorm;
			vert[2].tu		= tri->pPt3->mypt.fU;
			vert[2].tv		= tri->pPt3->mypt.fV;

			hres			= g_pd3dDevice->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, 1, vert, sizeof(vert[0]) );
			r_tris++;
		}
		tri = tri->ListNext();
	}
}

void CMyD3DApplication::RenderCurrentEdges (int& r_edges)
{
	ASSERT ( m_pObject ); 

	STDVERTEX vert[2];

	HRESULT hres;

	hres = g_pd3dDevice->SetVertexShader ( STDVERTEX_FVF );

	MeshEdge *edge = m_pObject->CurEdgeRoot.ListNext();
	while ( edge != NULL )
	{
		// Draw this one.
		vert[0].v		= edge->pPt1->mypt.vPos;
		vert[0].norm	= edge->pPt1->mypt.vNorm;
		vert[0].tu		= edge->pPt1->mypt.fU;
		vert[0].tv		= edge->pPt1->mypt.fV;

		vert[1].v		= edge->pPt2->mypt.vPos;
		vert[1].norm	= edge->pPt2->mypt.vNorm;
		vert[1].tu		= edge->pPt2->mypt.fU;
		vert[1].tv		= edge->pPt2->mypt.fV;

		hres			= g_pd3dDevice->DrawPrimitiveUP ( D3DPT_LINELIST, 1, vert, sizeof(vert[0]) );
		edge			= edge->ListNext();

		r_edges++;
	}
}

