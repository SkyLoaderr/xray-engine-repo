//----------------------------------------------------
// file: UI_D3D.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Log.h"
#include "UI_Main.h"
#include "Texture.h"
#include "VisibilityTester.h"

//---------------------------------------------------------------------------
bool TUI::D3D_Create(){
    HRESULT hr;

    if( FAILED( InitD3DX(paRender->Handle, &m_DX))){
		Log.Msg( "D3D: DirectDrawCreateEx() failed" );
		return false;
    }
    D3D_CreateStateBlocks();
	Log.Msg( "D3D: initialized" );
	m_D3D_ok = true;
	return true;
}

void TUI::D3D_CreateStateBlocks()
{
    m_DX->pD3DDev->BeginStateBlock();
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_LIGHTING, TRUE));
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_ZBIAS, 0));
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW	));
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ALPHABLENDENABLE,FALSE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_SRCBLEND,D3DBLEND_ONE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_DESTBLEND,D3DBLEND_ZERO) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_COLORVERTEX,FALSE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ZENABLE, TRUE ) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ZWRITEENABLE, TRUE ) );
    CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL));
    CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_AMBIENTMATERIALSOURCE,  D3DMCS_MATERIAL));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1));
    m_DX->pD3DDev->EndStateBlock(&SB_DrawMeshLayer);

    m_DX->pD3DDev->BeginStateBlock();
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_LIGHTING, FALSE));
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_ZBIAS, 0));
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW	));
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ALPHABLENDENABLE,TRUE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_SRCBLEND,D3DBLEND_ONE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_COLORVERTEX,FALSE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ZENABLE, TRUE ) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ZWRITEENABLE, TRUE ) );
    CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL));
    CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_AMBIENTMATERIALSOURCE,  D3DMCS_MATERIAL));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1,   D3DTA_TEXTURE));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,     D3DTOP_SELECTARG1));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1,   D3DTA_TEXTURE));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP,     D3DTOP_SELECTARG1));
    m_DX->pD3DDev->EndStateBlock(&SB_DrawMeshLayerAdd);

    m_DX->pD3DDev->BeginStateBlock();
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_LIGHTING, FALSE));
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_ZBIAS, 0));
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW	));
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ALPHABLENDENABLE,TRUE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_SRCBLEND,D3DBLEND_ZERO) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_DESTBLEND,D3DBLEND_SRCCOLOR) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_COLORVERTEX,FALSE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ZENABLE, TRUE ) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ZWRITEENABLE, TRUE ) );
    CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL));
    CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_AMBIENTMATERIALSOURCE,  D3DMCS_MATERIAL));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1,   D3DTA_TEXTURE));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,     D3DTOP_SELECTARG1));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1,   D3DTA_TEXTURE));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP,     D3DTOP_SELECTARG1));
    m_DX->pD3DDev->EndStateBlock(&SB_DrawMeshLayerMul);

    m_DX->pD3DDev->BeginStateBlock();
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_LIGHTING, TRUE));
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_ZBIAS, 0));
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW ));
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ALPHABLENDENABLE,TRUE ));
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA ) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA ));
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_COLORVERTEX,FALSE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ZENABLE, TRUE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ZWRITEENABLE, FALSE ) );
    CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL));
    CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_AMBIENTMATERIALSOURCE,  D3DMCS_MATERIAL));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE));
    m_DX->pD3DDev->EndStateBlock(&SB_DrawMeshLayerAlpha);

    m_DX->pD3DDev->BeginStateBlock();
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_LIGHTING, FALSE));
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_ZBIAS, 1));
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE	));
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ALPHABLENDENABLE,FALSE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_SRCBLEND,D3DBLEND_ONE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_DESTBLEND,D3DBLEND_ZERO) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_COLORVERTEX,TRUE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ZENABLE, TRUE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ZWRITEENABLE, TRUE ) );
    CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_EMISSIVEMATERIALSOURCE, D3DMCS_COLOR1));
    CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_DIFFUSEMATERIALSOURCE,  D3DMCS_COLOR1));
    CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_AMBIENTMATERIALSOURCE,  D3DMCS_COLOR1));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE));
    m_DX->pD3DDev->EndStateBlock(&SB_DrawWire);

    m_DX->pD3DDev->BeginStateBlock();
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_LIGHTING, FALSE));
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_ZBIAS, 0));
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW	));
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ALPHABLENDENABLE,TRUE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_COLORVERTEX,TRUE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ZENABLE, FALSE ) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ZWRITEENABLE, FALSE ) );
    CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL));
    CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_AMBIENTMATERIALSOURCE,  D3DMCS_MATERIAL));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1,   D3DTA_DIFFUSE));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP,     D3DTOP_SELECTARG1));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1,   D3DTA_DIFFUSE));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP,     D3DTOP_SELECTARG1));
    m_DX->pD3DDev->EndStateBlock(&SB_DrawSelectionRect);

    m_DX->pD3DDev->BeginStateBlock();
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_LIGHTING, FALSE));
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_ZBIAS, 0));
    CDX( m_DX->pD3DDev->SetRenderState	(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE	));
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ALPHABLENDENABLE,TRUE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_COLORVERTEX,TRUE) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ZENABLE, TRUE ) );
	CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_ZWRITEENABLE, FALSE ) );
    CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL));
    CDX( m_DX->pD3DDev->SetRenderState  (D3DRENDERSTATE_AMBIENTMATERIALSOURCE,  D3DMCS_MATERIAL));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE));
    CDX( m_DX->pD3DDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1));
    m_DX->pD3DDev->EndStateBlock(&SB_DrawClipPlane);
}

void TUI::D3D_DestroyStateBlocks()
{
    m_DX->pD3DDev->DeleteStateBlock(SB_DrawMeshLayer);
    m_DX->pD3DDev->DeleteStateBlock(SB_DrawMeshLayerAdd);
    m_DX->pD3DDev->DeleteStateBlock(SB_DrawMeshLayerMul);
    m_DX->pD3DDev->DeleteStateBlock(SB_DrawMeshLayerAlpha);
    m_DX->pD3DDev->DeleteStateBlock(SB_DrawWire);
    m_DX->pD3DDev->DeleteStateBlock(SB_DrawSelectionRect);
    m_DX->pD3DDev->DeleteStateBlock(SB_DrawClipPlane);
    SB_DrawMeshLayer = 0;
    SB_DrawMeshLayerAdd = 0;
    SB_DrawMeshLayerMul = 0;
    SB_DrawMeshLayerAlpha = 0;
    SB_DrawWire = 0;
    SB_DrawSelectionRect = 0;
    SB_DrawClipPlane = 0;
}
//---------------------------------------------------------------------------
void TUI::D3D_Clear(){
    D3D_DestroyStateBlocks();
    ReleaseD3DX();
	Log.Msg( "D3D: device cleared" );
	m_D3D_ok = false;
}

//---------------------------------------------------------------------------
void __fastcall TUI::D3D_Resize()
{
    m_RealWidth = paRender->Width;
    m_RealHeight = paRender->Height;

    m_RenderWidth  = m_RealWidth * m_ScreenQuality;
    m_RenderHeight = m_RealHeight * m_ScreenQuality;

    D3D_DestroyStateBlocks();

    TM.ReleaseD3DSurfaces();

    if (FAILED(ResizeD3DX( m_RenderWidth, m_RenderHeight ))){
        MessageDlg("Don't create DirectX device. Editor halted!", mtError, TMsgDlgButtons() << mbOK, 0);
        Application->Terminate();
    }
    m_Aspect = (float)m_RenderHeight / (float)m_RenderWidth;
    m_Projection.build_projection( m_FOV, m_Aspect, m_Znear, m_Zfar );

    CDX( m_DX->pD3DDev->SetTransform(D3DTRANSFORMSTATE_PROJECTION,(LPD3DMATRIX)m_Projection.d3d()) );
    CDX( m_DX->pD3DDev->SetTransform(D3DTRANSFORMSTATE_WORLD,(LPD3DMATRIX)m_Identity.d3d()) );

    bResize = false;

    frustum->Projection(m_Aspect, m_FOV, m_Znear, m_Zfar);

    D3D_CreateStateBlocks();

    UpdateScene();
}
//---------------------------------------------------------------------------
void TUI::D3D_StartDraw( ){
	_ASSERTE( m_D3D_ok );
    _ASSERTE( m_DX->pD3DDev );
	m_StartTime = timeGetTime();

    BeginDraw();

    if (bResize) D3D_Resize();

	CDX( UI.d3d()->SetRenderState   (D3DRENDERSTATE_SPECULARENABLE,FALSE) );
	CDX( UI.d3d()->SetRenderState   (D3DRENDERSTATE_SHADEMODE,D3DSHADE_GOURAUD) );
/*
	CDX( UI.d3d()->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE) );
	CDX( UI.d3d()->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE) );
	CDX( UI.d3d()->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_CURRENT) );
	CDX( UI.d3d()->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1) );
	CDX( UI.d3d()->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE) );
	CDX( UI.d3d()->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_CURRENT) );
*/
}

//---------------------------------------------------------------------------
void TUI::D3D_EndDraw(){
	_ASSERTE( m_D3D_ok );
    EndDraw();
}

//----------------------------------------------------
void TUI::D3D_UpdateGrid(){

	_ASSERTE( !is_zero(m_GridStep.x) );
	_ASSERTE( !is_zero(m_GridStep.z) );
	_ASSERTE( m_GridSubDiv[0] && m_GridSubDiv[1] );

	m_GridPoints.clear();
	m_GridIndices.clear();

	FLvertex left,right;
	left.y = right.y = 0;
	left.specular.set(0); right.specular.set(0);
	left.tu = right.tu = 0;
	left.tv = right.tv = 0;

	for(int thin=0; thin<2; thin++){
		for(int i=-m_GridCounts[0]; i<m_GridCounts[0]; i++){
			if( (!!thin) != !!(i%m_GridSubDiv[0]) ){
				left.z = -m_GridCounts[1]*m_GridStep.z;
				right.z = m_GridCounts[1]*m_GridStep.z;
				left.x = i*m_GridStep.x;
				right.x = left.x;
				left.color.c = (i%m_GridSubDiv[0]) ? m_ColorGrid.c : m_ColorGridTh.c;
				right.color.c = left.color.c;
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
				left.color.c = (i%m_GridSubDiv[1]) ? m_ColorGrid.c : m_ColorGridTh.c;
				right.color = left.color;
				m_GridIndices.push_back( m_GridPoints.size() );
				m_GridPoints.push_back( left );
				m_GridIndices.push_back( m_GridPoints.size() );
				m_GridPoints.push_back( right );
			}
		}
	}
}

void TUI::D3D_DrawGrid(){

	_ASSERTE( m_D3D_ok );

	if( !bDrawGrid ) return;

	if( m_GridPoints.size() == 0 ||	m_GridIndices.size()==0 ) return;

    CDX( m_DX->pD3DDev->SetTransform(D3DTRANSFORMSTATE_WORLD,m_Identity.d3d()) );
	TM.BindNone();
    UI.D3D_ApplyRenderState(UI.SB_DrawWire);
	CDX( m_DX->pD3DDev->DrawIndexedPrimitive(
		D3DPT_LINELIST,D3DFVF_LVERTEX,
		m_GridPoints.begin(), m_GridPoints.size(),
		m_GridIndices.begin(), m_GridIndices.size(),
		0 ));
}

void TUI::D3D_DrawPivot(){

	if( !bDrawPivot ) return;

	FLvertex v[7];
	WORD j[12];

	for(int i=0;i<6;i++){
		j[2*i] = 0;
		j[2*i+1] = i+1;
	}

	FPcolor c;
	c.set( 127, 255, 127, 0 );

	for(i=0;i<7;i++){
		v[i].x = m_Pivot.x;
		v[i].y = m_Pivot.y;
		v[i].z = m_Pivot.z;
		v[i].color.c = c.c;
		v[i].specular.c = c.c;
		v[i].tu = 0;
		v[i].tv = 0;
	}

	v[1].x += 5.f;
	v[2].x -= 5.f;
	v[3].y += 5.f;
	v[4].y -= 5.f;
	v[5].z += 5.f;
	v[6].z -= 5.f;

	CDX( m_DX->pD3DDev->SetTransform(D3DTRANSFORMSTATE_WORLD,m_Identity.d3d()) );
	TM.BindNone();
    UI.D3D_ApplyRenderState(UI.SB_DrawWire);
	CDX( m_DX->pD3DDev->DrawIndexedPrimitive(
		D3DPT_LINELIST,D3DFVF_LVERTEX,
		v, 7, j, 12, 0 ));
}

void TUI::D3D_DrawSelectionRect(){

	if( !m_SelectionRect )
		return;

	FTLvertex v[4];
	WORD i[6];

	i[0] = 2;
	i[1] = 1;
	i[2] = 0;
	i[3] = 3;
	i[4] = 2;
	i[5] = 0;

	FPcolor c;
	c.set( 127, 255, 127, 64 );

	for( int k=0;k<4;k++){
		v[k].color.c = c.c;
		v[k].specular.c = c.c;
		v[k].rhw = 0;
		v[k].tu = 0;
		v[k].tv = 0;
		v[k].sz = 0;
	}

	clamp( (long)m_SelStart.x, (long)0, (long)m_RenderWidth-1 );
	clamp( (long)m_SelEnd.x, (long)0, (long)m_RenderWidth-1 );
	clamp( (long)m_SelStart.y, (long)0, (long)m_RenderHeight-1 );
	clamp( (long)m_SelEnd.y, (long)0, (long)m_RenderHeight-1 );

	v[0].sx = min( m_SelStart.x, m_SelEnd.x );
	v[1].sx = min( m_SelStart.x, m_SelEnd.x );
	v[2].sx = max( m_SelStart.x, m_SelEnd.x );
	v[3].sx = max( m_SelStart.x, m_SelEnd.x );

	v[0].sy = min( m_SelStart.y, m_SelEnd.y );
	v[1].sy = max( m_SelStart.y, m_SelEnd.y );
	v[2].sy = max( m_SelStart.y, m_SelEnd.y );
	v[3].sy = min( m_SelStart.y, m_SelEnd.y );

	TM.BindNone();
    UI.D3D_ApplyRenderState(UI.SB_DrawSelectionRect);
	CDX( m_DX->pD3DDev->DrawIndexedPrimitive(
		D3DPT_TRIANGLELIST,D3DFVF_TLVERTEX,
		v,4,i,6,0 ) );
}

//---------------------------------------------------------------------------

