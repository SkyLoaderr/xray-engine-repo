//----------------------------------------------------
// file: navpoint.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "navpoint.h"
#include "Log.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "Texture.h"
#include "Frustum.h"
#include "Scene.h"
#include "D3DUtils.h"
#include "EditorPref.h"
#include "Builder.h"

#define NAVPOINT_VERSION				0x0010
//----------------------------------------------------
#define NAVPOINT_CHUNK_VERSION          0xE501
#define NAVPOINT_CHUNK_POINTS			0xE502
//----------------------------------------------------

#define NAVPOINT_SIZE 1.5f
//------------------------------------------------------------------------------
// Naviagtion points
//------------------------------------------------------------------------------
CNavPoint::CNavPoint( char *name ):SceneObject(){
	Construct();
	strcpy( m_Name, name );
}

CNavPoint::CNavPoint():SceneObject(){
	Construct();
}

void CNavPoint::Construct(){
	m_ClassID   = OBJCLASS_NAVPOINT;
	m_Position.set(0,0,0);
}

CNavPoint::~CNavPoint(){
}

//----------------------------------------------------
bool CNavPoint::GetBox( Fbox& box ){
	box.set( m_Position, m_Position );
	box.min.x -= NAVPOINT_SIZE*0.5f;
	box.min.y -= 0;
	box.min.z -= NAVPOINT_SIZE*0.5f;
	box.max.x += NAVPOINT_SIZE*0.5f;
	box.max.y += NAVPOINT_SIZE;
	box.max.z += NAVPOINT_SIZE*0.5f;
	return true;
}

void CNavPoint::DrawNavPoint  (Fcolor& c){
//    DU::DrawFlag(m_Position,NAVPOINT_SIZE,NAVPOINT_SIZE*0.5f,.3f,c);
/*
	FLvertex v[7];
	WORD j[12];

	for(WORD i=0;i<6;i++){j[2*i] = 0;j[2*i+1] = i+1;}
	for(i=0;i<7;i++) v[i].set(m_Position,c,c,0,0);
	v[1].x += NAVPOINT_SIZE;
	v[2].x -= NAVPOINT_SIZE;
	v[3].y += NAVPOINT_SIZE;
	v[4].y -= NAVPOINT_SIZE;
	v[5].z += NAVPOINT_SIZE;
	v[6].z -= NAVPOINT_SIZE;

	CDX( UI->d3d()->DrawIndexedPrimitive(
		D3DPT_LINELIST,D3DFVF_LVERTEX,
		v, 7, j, 12, 0 ));
*/
}

void CNavPoint::Render( Fmatrix& parent, ERenderPriority flag ){
    if (flag==rpNormal){
        if (UI->Device.m_Frustum.testSphere(m_Position,NAVPOINT_SIZE)){
            Fcolor c;
            c.set(0.f,1.f,0.f,1.f);
            DrawNavPoint(c);
            if( Selected() ){
                Fbox bb; GetBox(bb);
                DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
                DU::DrawSelectionBox(bb,&clr);
            }
        }
    }
}

bool CNavPoint::FrustumPick(const CFrustum& frustum, const Fmatrix& parent){
    return (frustum.testSphere(m_Position,NAVPOINT_SIZE))?true:false;
}

bool CNavPoint::RTL_Pick(
	float *distance,
	Fvector& start,
	Fvector& direction,
	Fmatrix& parent, SPickInfo* pinf )
{
	Fvector transformed;
	parent.transform(transformed, m_Position);

	Fvector ray2;
	ray2.sub( transformed, start );

    float d = ray2.dotproduct(direction);
    if( d > 0  ){
        float d2 = ray2.magnitude();
        if( ((d2*d2-d*d) < (NAVPOINT_SIZE*NAVPOINT_SIZE)) && (d>NAVPOINT_SIZE) ){
            (*distance) = d;
            return true;
        }
    }
	return false;
}

void CNavPoint::Move( Fvector& amount ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI->UpdateScene();
	m_Position.add( amount );
}
//----------------------------------------------------

bool CNavPoint::Load(CStream& F){
	DWORD version = 0;

    R_ASSERT(F.ReadChunk(NAVPOINT_CHUNK_VERSION,&version));
    if( version!=NAVPOINT_VERSION ){
        Log->DlgMsg( mtError, "CNavPoint: Unsuported version.");
        return false;
    }
    
	SceneObject::Load(F);

    R_ASSERT(F.FindChunk(NAVPOINT_CHUNK_POINTS));
    m_Type 			= F.Rdword();
    F.Rvector		(m_Position);

    return true;
}

void CNavPoint::Save(CFS_Base& F){
	SceneObject::Save(F);

	F.open_chunk	(NAVPOINT_CHUNK_VERSION);
	F.Wword			(NAVPOINT_VERSION);
	F.close_chunk	();

    F.open_chunk	(NAVPOINT_CHUNK_POINTS);
    F.Wdword		(m_Type);
    F.Wvector		(m_Position);
	F.close_chunk	();
}

//----------------------------------------------------

