//----------------------------------------------------
// file: UI_D3D.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "Texture.h"


//----------------------------------------------------

bool XRayEditorUI::D3D_Create(){

	if( FAILED( DirectDrawCreateEx(0, (VOID**)&m_DDraw, IID_IDirectDraw7, 0) ) ){
		NConsole.print( "D3D: DirectDrawCreateEx() failed" );
		return false; }

	if( FAILED( m_DDraw->SetCooperativeLevel( m_VisWindow,DDSCL_NORMAL) ) ){
		NConsole.print( "D3D: SetCooperativeLevel() failed..." );
		return false; }

	memset( &m_FrontDesc, 0, sizeof(m_FrontDesc) );
	m_FrontDesc.dwSize = sizeof(m_FrontDesc);
	m_FrontDesc.dwFlags = DDSD_CAPS;
	m_FrontDesc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	if( FAILED( m_DDraw->CreateSurface( &m_FrontDesc, &m_FrontBuffer, 0) ) ){
		NConsole.print( "D3D: CreateSurface() failed..." );
		return false; }

	D3D_CreateClipper();
	D3D_CreateBackBuffers();
	D3D_CreateDevice();

	NConsole.print( "D3D: initialized" );
	m_D3D_ok = true;
	return true;
}

bool XRayEditorUI::D3D_CreateDevice(){
	
	if( FAILED(m_DDraw->QueryInterface(IID_IDirect3D7,(VOID **)&m_D3D) ) ){
		NConsole.print( "D3D: QueryInterface() failed..." );
		return false; }

	if( FAILED(m_D3D->CreateDevice(IID_IDirect3DHALDevice,m_BackBuffer,&m_D3Dev ) ) ){
		NConsole.print( "D3D: CreateDevice() failed..." );
		return false; }

	return true;
}

bool XRayEditorUI::D3D_CreateBackBuffers(){

	_ASSERTE( m_RenderWidth );
	_ASSERTE( m_RenderHeight );

	// back buffer 

	memset( &m_BackDesc, 0, sizeof(m_BackDesc) );
	m_BackDesc.dwSize = sizeof(m_BackDesc);

	m_BackDesc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	m_BackDesc.dwHeight = m_RenderHeight;
	m_BackDesc.dwWidth = m_RenderWidth;

	m_BackDesc.ddsCaps.dwCaps =
		DDSCAPS_OFFSCREENPLAIN |
		DDSCAPS_3DDEVICE;

	if( FAILED(m_DDraw->CreateSurface(&m_BackDesc, &m_BackBuffer, 0) ) ){
		NConsole.print( "D3D: CreateSurface() failed..." );
		return false; }

	// Z-buffer

	memset( &m_ZDesc, 0, sizeof(m_ZDesc) );
	m_ZDesc.dwSize = sizeof(m_ZDesc);

	m_ZDesc.dwFlags = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
	m_ZDesc.dwHeight = m_RenderHeight;
	m_ZDesc.dwWidth = m_RenderWidth;

	m_ZDesc.ddpfPixelFormat.dwSize = sizeof(m_ZDesc.ddpfPixelFormat);
	m_ZDesc.ddpfPixelFormat.dwFlags = DDPF_ZBUFFER;
	m_ZDesc.ddpfPixelFormat.dwZBufferBitDepth = 16;
	m_ZDesc.ddpfPixelFormat.dwZBitMask = 0x0000ffff;

	m_ZDesc.ddsCaps.dwCaps =
		DDSCAPS_ZBUFFER |
		DDSCAPS_VIDEOMEMORY;

	if( FAILED(m_DDraw->CreateSurface(&m_ZDesc, &m_ZBuffer, 0) ) ){
		NConsole.print( "D3D: CreateSurface() failed..." );
		return false; }

	// attach buffers

	if( FAILED(m_BackBuffer->AddAttachedSurface(m_ZBuffer) ) ){
		NConsole.print( "D3D: AddAttachedSurface() failed..." );
		return false; }

	_ASSERTE( m_BackBuffer );

	memset( &m_BackDesc, 0, sizeof(m_BackDesc) );
	m_BackDesc.dwSize = sizeof(m_BackDesc);
	m_BackBuffer->GetSurfaceDesc( &m_BackDesc );

	return true;
}

bool XRayEditorUI::D3D_CreateClipper(){
	
	LPDIRECTDRAWCLIPPER _Clipper;

	if( FAILED( m_DDraw->CreateClipper( 0, &_Clipper, 0) ) ){
		NConsole.print( "D3D: CreateClipper() failed..." );
		return false; }

	if( FAILED( _Clipper->SetHWnd( 0, m_VisWindow ) ) ){
		NConsole.print( "D3D: SetHWnd() failed..." );
		return false; }

	if( FAILED( m_FrontBuffer->SetClipper( _Clipper ) ) ){
		NConsole.print( "D3D: SetClipper() failed..." );
		return false; }

	SAFE_RELEASE( _Clipper );
	return true;
}

void XRayEditorUI::D3D_Clear(){

	if( !m_D3D_ok ){
		NConsole.print( "D3D: device has not been initialized" );
		return; }

	SAFE_RELEASE( m_D3Dev );
	SAFE_RELEASE( m_D3D );  
	SAFE_RELEASE( m_ZBuffer );

	SAFE_RELEASE( m_BackBuffer );
	SAFE_RELEASE( m_FrontBuffer );

	if( m_DDraw ){
		m_DDraw->SetCooperativeLevel( m_VisWindow,DDSCL_NORMAL);
		SAFE_RELEASE( m_DDraw ); };

	NConsole.print( "D3D: device cleared" );
	m_D3D_ok = false;
}

void XRayEditorUI::D3D_StartDraw( RECT *_Rect ){

	_ASSERTE( m_D3D_ok );
	m_StartTime = timeGetTime();
	m_D3Dev->BeginScene();
	
	_ASSERTE( m_RenderWidth );
	_ASSERTE( m_RenderHeight );

	m_Aspect = (float)m_RenderHeight / (float)m_RenderWidth;
	m_Projection.projection( m_FOV, m_Aspect, m_Znear, m_Zfar );

	D3DVIEWPORT7 vp;
	vp.dwX = 0;
	vp.dwY = 0;
	vp.dwWidth = m_RenderWidth;
	vp.dwHeight = m_RenderHeight;
	vp.dvMinZ = 0.f;
	vp.dvMaxZ = 1.f;
	CDX( m_D3Dev->SetViewport( &vp ) );

	CDX( m_D3Dev->Clear(0,0,
		D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		m_BackgroundColor.value,1,0));

	CDX( m_D3Dev->SetTransform(D3DTRANSFORMSTATE_PROJECTION,(LPD3DMATRIX)m_Projection.a) );
	CDX( m_D3Dev->SetTransform(D3DTRANSFORMSTATE_WORLD,(LPD3DMATRIX)m_Identity.a) );

	IMatrix viewmatrix;
	viewmatrix.invert( m_Camera );
	CDX( m_D3Dev->SetTransform(D3DTRANSFORMSTATE_VIEW,(LPD3DMATRIX)viewmatrix.a) );

	if( g_LinearTFilter ){
		CDX( m_D3Dev->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTFG_LINEAR));
		CDX( m_D3Dev->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTFG_LINEAR));
	} else {
		CDX( m_D3Dev->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTFG_POINT));
		CDX( m_D3Dev->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTFG_POINT));
	}

	/*CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_ZENABLE, FALSE ) );
	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_COLORVERTEX,TRUE) );*/
	
	/*CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE ) );
	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_SHADEMODE,D3DSHADE_GOURAUD) );
	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_EMISSIVEMATERIALSOURCE,D3DMCS_MATERIAL) );
	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE) );
	CDX( m_D3Dev->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE) );
	CDX( m_D3Dev->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE) );
	CDX( m_D3Dev->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_CURRENT) );
	CDX( m_D3Dev->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1) );
	CDX( m_D3Dev->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE) );
	CDX( m_D3Dev->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_CURRENT) );*/

}

void XRayEditorUI::D3D_EndDraw(){

	_ASSERTE( m_D3D_ok );
	m_D3Dev->EndScene();

	HRESULT hr;

	RECT rcSrc;
	rcSrc.left = 0;
	rcSrc.top = 0;
	rcSrc.right = m_RenderWidth;
	rcSrc.bottom = m_RenderHeight;

	RECT rcClient;
	GetClientRect(m_VisWindow,&rcClient);
	MapWindowPoints(m_VisWindow,0,(LPPOINT)&rcClient,2);

/*
	HDC hdc;
	char t[256];

	
	if( !FAILED(m_BackBuffer->GetDC(&hdc)) ){
		
		SetBkMode( hdc, TRANSPARENT );
		SetTextColor( hdc, RGB(255,255,255) );

		sprintf( t, "MS:%03d", dt );
		TextOut( hdc, 0, 0, t, strlen(t) );

		sprintf( t, "R: %.3f, %.3f, %.3f", m_LastDir.x, m_LastDir.y, m_LastDir.z );
		TextOut( hdc, 0, 16, t, strlen(t) );

		sprintf( t, "S: %.3f, %.3f, %.3f", m_LastStart.x, m_LastStart.y, m_LastStart.z );
		TextOut( hdc, 0, 32, t, strlen(t) );

		sprintf( t, "Pick: %.3f, %.3f, %.3f", m_LastPick.x, m_LastPick.y, m_LastPick.z );
		TextOut( hdc, 0, 48, t, strlen(t) );

		m_BackBuffer->ReleaseDC( hdc );
	}
*/
	CDX(m_FrontBuffer->Blt(&rcClient,m_BackBuffer,0,DDBLT_WAIT,0));
	long dt = timeGetTime() - m_StartTime;
	if (dt<17) Sleep(17-dt);
/*
	HDC hdc;
	char t[32];

	long dt = timeGetTime() - m_StartTime;

	if( !FAILED(m_FrontBuffer->GetDC(&hdc)) ){
		sprintf( t, "MS:%03d", dt );
		SetBkMode( hdc, TRANSPARENT );
		SetTextColor( hdc, RGB(255,255,255) );
		TextOut( hdc, rcClient.left, rcClient.top, t, strlen(t) );
		m_FrontBuffer->ReleaseDC( hdc );
	}*/
}

//----------------------------------------------------

void XRayEditorUI::D3D_UpdateGrid(){

	_ASSERTE( !is_zero(m_GridStep.x) );
	_ASSERTE( !is_zero(m_GridStep.z) );
	_ASSERTE( m_GridSubDiv[0] && m_GridSubDiv[1] );

	m_GridPoints.clear();
	m_GridIndices.clear();

	D3DLVERTEX left,right;
	left.y = right.y = 0;
	left.specular = right.specular = 0;
	left.tu = right.tu = 0;
	left.tv = right.tv = 0;

	for(int thin=0; thin<2; thin++){
		for(int i=-m_GridCounts[0]; i<m_GridCounts[0]; i++){
			if( (!!thin) != !!(i%m_GridSubDiv[0]) ){
				left.z = -m_GridCounts[1]*m_GridStep.z;
				right.z = m_GridCounts[1]*m_GridStep.z;
				left.x = i*m_GridStep.x;
				right.x = left.x;
				left.color = (i%m_GridSubDiv[0]) ? m_ColorGrid.value : m_ColorGridTh.value;
				right.color = left.color;
				m_GridIndices.push_back( m_GridPoints.size() );
				m_GridPoints.push_back( left );
				m_GridIndices.push_back( m_GridPoints.size() );
				m_GridPoints.push_back( right );
			}
		}
		for(i=-m_GridCounts[1]; i<m_GridCounts[1]; i++){
			if( (!!thin) != !!(i%m_GridSubDiv[1]) ){
				left.x = -m_GridCounts[0]*m_GridStep.x;
				right.x = m_GridCounts[0]*m_GridStep.x;
				left.z = i*m_GridStep.z;
				right.z = left.z;
				left.color = (i%m_GridSubDiv[1]) ? m_ColorGrid.value : m_ColorGridTh.value;
				right.color = left.color;
				m_GridIndices.push_back( m_GridPoints.size() );
				m_GridPoints.push_back( left );
				m_GridIndices.push_back( m_GridPoints.size() );
				m_GridPoints.push_back( right );
			}
		}
	}
}

void XRayEditorUI::D3D_DrawGrid(){

	_ASSERTE( m_D3D_ok );

	if( !m_GridEnabled )
		return;

	if( m_GridPoints.size() == 0 ||
		m_GridIndices.size()==0 )
			return;

	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1) );
	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE) );
	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE ) );
	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_COLORVERTEX,TRUE) );

	TM.BindNone();

	CDX( m_D3Dev->DrawIndexedPrimitive(
		D3DPT_LINELIST,D3DFVF_LVERTEX,
		m_GridPoints.begin(), m_GridPoints.size(),
		m_GridIndices.begin(), m_GridIndices.size(),
		0 ));
}

void XRayEditorUI::D3D_DrawPivot(){

	if( !m_FltPivots )
		return;

	D3DLVERTEX v[7];
	WORD j[12];

	for(int i=0;i<6;i++){
		j[2*i] = 0;
		j[2*i+1] = i+1;
	}

	IColor c;
	c.set( 127, 255, 127 );

	for(i=0;i<7;i++){
		v[i].x = m_Pivot.x;
		v[i].y = m_Pivot.y;
		v[i].z = m_Pivot.z;
		v[i].color = c.value;
		v[i].specular = c.value;
		v[i].tu = 0;
		v[i].tv = 0;
	}

	v[1].x += 5.f;
	v[2].x -= 5.f;
	v[3].y += 5.f;
	v[4].y -= 5.f;
	v[5].z += 5.f;
	v[6].z -= 5.f;

	CDX( m_D3Dev->SetTransform(D3DTRANSFORMSTATE_WORLD,(LPD3DMATRIX)m_Identity.a) );

	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1) );
	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE) );
	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE ) );
	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_COLORVERTEX,TRUE) );

	//CDX( m_D3Dev->SetTexture(0,0) );
	TM.BindNone();

	CDX( m_D3Dev->DrawIndexedPrimitive(
		D3DPT_LINELIST,D3DFVF_LVERTEX,
		v, 7, j, 12, 0 ));
}

void XRayEditorUI::D3D_DrawSelectionRect(){

	if( !m_SelectionRect )
		return;
	
	D3DTLVERTEX v[4];
	WORD i[6];

	i[0] = 2;
	i[1] = 1;
	i[2] = 0;
	i[3] = 3;
	i[4] = 2;
	i[5] = 0;

	IColor c;
	c.set( 127, 255, 127, 64 );

	for( int k=0;k<4;k++){
		v[k].color = c.value;
		v[k].specular = c.value;
		v[k].rhw = 0;
		v[k].tu = 0;
		v[k].tv = 0;
		v[k].sz = 0;
	}

	clamp( m_SelStart.x, 0, m_RenderWidth-1 );
	clamp( m_SelEnd.x, 0, m_RenderWidth-1 );
	clamp( m_SelStart.y, 0, m_RenderHeight-1 );
	clamp( m_SelEnd.y, 0, m_RenderHeight-1 );

	v[0].sx = min( m_SelStart.x, m_SelEnd.x );
	v[1].sx = min( m_SelStart.x, m_SelEnd.x );
	v[2].sx = max( m_SelStart.x, m_SelEnd.x );
	v[3].sx = max( m_SelStart.x, m_SelEnd.x );

	v[0].sy = min( m_SelStart.y, m_SelEnd.y );
	v[1].sy = max( m_SelStart.y, m_SelEnd.y );
	v[2].sy = max( m_SelStart.y, m_SelEnd.y );
	v[3].sy = min( m_SelStart.y, m_SelEnd.y );

	//CDX( m_D3Dev->SetTexture(0,0) );
	TM.BindNone();

	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE) );
	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA) );
	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA) );

	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_COLORVERTEX,TRUE) );
	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_ZENABLE, FALSE ) );

	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_DIFFUSEMATERIALSOURCE,D3DMCS_COLOR1) );
	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_AMBIENTMATERIALSOURCE,D3DMCS_COLOR1) );
	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_SPECULARMATERIALSOURCE,D3DMCS_COLOR1) );
	CDX( m_D3Dev->SetRenderState(D3DRENDERSTATE_EMISSIVEMATERIALSOURCE,D3DMCS_COLOR1) );

	CDX( m_D3Dev->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,
		v,4,i,6,0 ) );
}

