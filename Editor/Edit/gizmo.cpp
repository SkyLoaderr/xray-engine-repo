//----------------------------------------------------
// file: Gizmo.cpp
//----------------------------------------------------
#include "Pch.h"
#pragma hdrstop

#include "Gizmo.h"
#include "ui_main.h"
#include "texture.h"
#include "scene.h"
#include "statistic.h"

static Fvector rect[4] = {  {-.5f, 0, .5f},
                            { .5f, 0, .5f},
                            { .5f, 0,-.5f},
                            {-.5f, 0,-.5f}};

CGizmo::CGizmo(){
    m_RenderBuffer.resize(4);
    FPcolor c_rect;
    c_rect.set( 255, 255, 0, 0 );
	for(int i=0; i<4; i++){
		m_RenderBuffer[i].color.set(c_rect);
		m_RenderBuffer[i].specular.set(c_rect);
        m_RenderBuffer[i].tu = 0.f;
        m_RenderBuffer[i].tv = 0.f;
	}
    m_Center.set(0,0,0);
    m_Direction.set(0,0,1);
    m_Normal.set(0,1,0);
    m_Size.set(2,0,2);
    BuildPlane();
}
//----------------------------------------------------

CGizmo::~CGizmo(){
}
//----------------------------------------------------

void CGizmo::BuildPlane()
{
    Fmatrix rot, m;
    rot.rotation(m_Direction,m_Normal);
	m.scale( m_Size );
    for(int i=0; i<4; i++){
        m_RenderBuffer[i].p.transform(rect[i],rot);
		m.shorttransform( m_RenderBuffer[i].p );
		m_RenderBuffer[i].p.add( m_Center );
    }
}

static const int ind_cnt = 8;
static const WORD indices[ind_cnt] = {
	        0, 1, 1, 2, 2, 3, 3, 0 };

void CGizmo::Render(){
	CDX( UI.d3d()->SetTransform(D3DTRANSFORMSTATE_WORLD,UI.m_Identity.d3d()) );
	TM.BindNone();
    UI.D3D_ApplyRenderState(UI.SB_DrawWire);

    CDX( UI.d3d()->DrawIndexedPrimitive(
        D3DPT_LINELIST,D3DFVF_LVERTEX,
        &(m_RenderBuffer[0]), m_RenderBuffer.size(),
        (LPWORD)indices, ind_cnt, 0 ));
}
//----------------------------------------------------

void CGizmo::NormalAlign(Fvector& normal)
{
    m_Normal.set(normal);
    BuildPlane();
}
//----------------------------------------------------

void CGizmo::Move( Fvector& amount ){
    m_Center.add(amount);
}

void CGizmo::Scale( Fvector& amount ){
	Fmatrix m;
	m.scale( amount );
	m.transform( m_Size );
}

void CGizmo::Rotate( Fvector& axis, float angle ){
	Fmatrix m;
	m.rotation( axis, angle );
	m.transform( m_Normal );
}

