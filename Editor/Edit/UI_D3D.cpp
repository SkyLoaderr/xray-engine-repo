//----------------------------------------------------
// file: UI_D3D.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "UI_Main.h"
#include "Texture.h"
#include "Scene.h"
//---------------------------------------------------------------------------
void TUI::D3D_UpdateGrid(){
// grid
	Fvector2 m_GridStep;
	DWORD m_ColorGrid;
	DWORD m_ColorGridTh;
	int m_GridSubDiv[2];
	int m_GridCounts[2];

	m_GridStep.set( 2.f, 2.f );
	m_GridSubDiv[0] = 8;
	m_GridSubDiv[1] = 8;
	m_GridCounts[0] = 127;
	m_GridCounts[1] = 127;
	m_ColorGrid = RGBA_MAKE(150, 150, 150, 0);
	m_ColorGridTh = RGBA_MAKE( 180, 180, 180, 0 );

	VERIFY( !fis_zero(m_GridStep.x) );
	VERIFY( !fis_zero(m_GridStep.y) );
	VERIFY( m_GridSubDiv[0] && m_GridSubDiv[1] );

	m_GridPoints.clear();
	m_GridIndices.clear();

	FVF::L left,right;
	left.p.y = right.p.y = 0;

	for(int thin=0; thin<2; thin++){
		for(int i=-m_GridCounts[0]; i<m_GridCounts[0]; i++){
			if( (!!thin) != !!(i%m_GridSubDiv[0]) ){
				left.p.z = -m_GridCounts[1]*m_GridStep.y;
				right.p.z = m_GridCounts[1]*m_GridStep.y;
				left.p.x = i*m_GridStep.x;
				right.p.x = left.p.x;
				left.color = (i%m_GridSubDiv[0]) ? m_ColorGrid : m_ColorGridTh;
				right.color = left.color;
				m_GridIndices.push_back( m_GridPoints.size() );
				m_GridPoints.push_back( left );
				m_GridIndices.push_back( m_GridPoints.size() );
				m_GridPoints.push_back( right );
			}
		}
		for(i=-m_GridCounts[1]; i<m_GridCounts[1]; i++){
			if( (!!thin) != !!(i%m_GridSubDiv[1]) ){
				left.p.x = -m_GridCounts[0]*m_GridStep.x;
				right.p.x = m_GridCounts[0]*m_GridStep.x;
				left.p.z = i*m_GridStep.y;
				right.p.z = left.p.z;
				left.color = (i%m_GridSubDiv[1]) ? m_ColorGrid : m_ColorGridTh;
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
	VERIFY( Device.m_bReady );
	if( !bDrawGrid ) return;
	if( m_GridPoints.size() == 0 ||	m_GridIndices.size()==0 ) return;
    Device.SetTransform(D3DTRANSFORMSTATE_WORLD,precalc_identity);
	Device.Shader.Set(Device.m_WireShader);
    Device.DIP(	D3DPT_LINELIST,FVF::F_L,
	   	        m_GridPoints.begin(), m_GridPoints.size(),
        	    m_GridIndices.begin(), m_GridIndices.size());
}

void TUI::D3D_DrawPivot(){
	VERIFY( Device.m_bReady );
	if( !bDrawGrid ) return;
	FVF::L v[6];
	WORD j[12];
    j[0]=0; j[1]=1; j[2]=2; j[3]=3; j[4]=4; j[5]=5;
	DWORD c=RGBA_MAKE( 127, 255, 127, 0 );
	for(int i=0;i<6;i++) v[i].set(m_Pivot.x,m_Pivot.y,m_Pivot.z,c);
	v[0].p.x+=5.f;	v[1].p.x-=5.f;
    v[2].p.y+=5.f;	v[3].p.y-=5.f;
    v[4].p.z+=5.f;	v[5].p.z-=5.f;
	Device.SetTransform(D3DTRANSFORMSTATE_WORLD,precalc_identity);
	Device.Shader.Set(Device.m_WireShader);
	Device.DIP(D3DPT_LINELIST,FVF::F_L, v, 6, j, 6);
}

void TUI::D3D_DrawAxis(){
	VERIFY( Device.m_bReady );
    WORD i[6];
    Fvector p[4];
    FVF::TL v[4];
    DWORD c[4];

    // indices
    i[0]=0; i[1]=1; i[2]=0; i[3]=2; i[4]=0; i[5]=3;
    // colors
    c[0]=0x00222222; c[1]=0x00FF0000; c[2]=0x0000FF00; c[3]=0x000000FF;

    // position
    p[0].direct(Device.m_Camera.GetPosition(),Device.m_Camera.GetDirection(),0.25f);
    p[1].set(p[0]); p[1].x+=.015f;
    p[2].set(p[0]); p[2].y+=.015f;
    p[3].set(p[0]); p[3].z+=.015f;

    // transform to screen
    float dx=-Device.m_RenderWidth/2.2f;
    float dy=Device.m_RenderHeight/2.25f;
    for (int k=0; k<4; k++){
	    v[k].transform(p[k],Device.m_FullTransform); v[k].color=c[k];v[k].tu=v[k].tv=0;
	    v[k].p.x	= X_TO_REAL(v[k].p.x)+dx;   v[k].p.y	= Y_TO_REAL(v[k].p.y)+dy;
    }

    // render
	Device.SetRS(D3DRENDERSTATE_SHADEMODE,D3DSHADE_GOURAUD);
	Device.Shader.Set(Device.m_WireShader);
    Device.DIP(D3DPT_LINELIST,FVF::F_TL, v, 4, i, 6);
	Device.SetRS(D3DRENDERSTATE_SHADEMODE,UI->dwRenderShadeMode);
}

void TUI::D3D_DrawSelectionRect(){
	VERIFY( Device.m_bReady );
	if( !m_SelectionRect ) return;
	FVF::TL v[4];
	DWORD c=RGBA_MAKE(127,255,127,64);
    UI->Device.SetRS(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
    v[0].set(m_SelStart.x*Device.m_ScreenQuality, m_SelStart.y*Device.m_ScreenQuality, 0,0,c,0,0);
    v[1].set(m_SelStart.x*Device.m_ScreenQuality, m_SelEnd.y*Device.m_ScreenQuality,   0,0,c,0,0);
    v[2].set(m_SelEnd.x*Device.m_ScreenQuality,   m_SelEnd.y*Device.m_ScreenQuality,   0,0,c,0,0);
    v[3].set(m_SelEnd.x*Device.m_ScreenQuality,   m_SelStart.y*Device.m_ScreenQuality, 0,0,c,0,0);

	Device.Shader.Set(Device.m_SelectionShader);
    Device.DP(D3DPT_TRIANGLEFAN,FVF::F_TL,v,4);
    UI->Device.SetRS(D3DRENDERSTATE_CULLMODE,D3DCULL_CCW);
}

//---------------------------------------------------------------------------

