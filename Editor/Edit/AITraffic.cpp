//----------------------------------------------------
// file: AITraffic.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "AITraffic.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "D3DUtils.h"
#include "Scene.h"

#define AITPOINT_VERSION				0x0010
//----------------------------------------------------
#define AITPOINT_CHUNK_VERSION			0xE501
#define AITPOINT_CHUNK_POINT			0xE502
#define AITPOINT_CHUNK_LINKS			0xE503
//----------------------------------------------------

#define AITPOINT_SIZE 	1.5f
#define AITPOINT_RADIUS AITPOINT_SIZE*.5f
//------------------------------------------------------------------------------
// AI Traffic points
//------------------------------------------------------------------------------
CAITPoint::CAITPoint(char *name):SceneObject(){
	Construct();
	strcpy( m_Name, name );
}

CAITPoint::CAITPoint():SceneObject(){
	Construct();
}

void CAITPoint::Construct(){
	m_ClassID   = OBJCLASS_AITPOINT;
	m_Position.set(0,0,0);
}

CAITPoint::~CAITPoint(){
}
//----------------------------------------------------

void CAITPoint::OnDestroy(){
	// удалить у всех линков себя
    for (ObjectIt o_it=m_Links.begin(); o_it!=m_Links.end(); o_it++)
    	((CAITPoint*)(*o_it))->DeleteLink(this);
}
//----------------------------------------------------

bool CAITPoint::GetBox( Fbox& box ){
	box.set( m_Position, m_Position );
	box.min.x -= AITPOINT_RADIUS;
	box.min.y -= 0;
	box.min.z -= AITPOINT_RADIUS;
	box.max.x += AITPOINT_RADIUS;
	box.max.y += AITPOINT_SIZE;
	box.max.z += AITPOINT_RADIUS;
	return true;
}

void CAITPoint::DrawPoint(Fcolor& c){
	Fvector pos;
    pos.set	(m_Position.x,m_Position.y+AITPOINT_SIZE*0.85f,m_Position.z);
    DU::DrawCross(pos,AITPOINT_RADIUS,AITPOINT_SIZE*0.85f,AITPOINT_RADIUS,AITPOINT_RADIUS,AITPOINT_SIZE*0.15f,AITPOINT_RADIUS,c.get());
}
//----------------------------------------------------

void CAITPoint::DrawLinks(Fcolor& c){
	Fvector p1,p2;
    p1.set	(m_Position.x,m_Position.y+AITPOINT_SIZE*0.85f,m_Position.z);
    for (ObjectIt o_it=m_Links.begin(); o_it!=m_Links.end(); o_it++){
    	CAITPoint* O = (CAITPoint*)(*o_it);
	    p2.set	(O->m_Position.x,O->m_Position.y+AITPOINT_SIZE*0.85f,O->m_Position.z);
    	DU::DrawLine(p1,p2,c.get());
    }
}
//----------------------------------------------------

void CAITPoint::Render( Fmatrix& parent, ERenderPriority flag ){
    if (flag==rpNormal){
        if (UI->Device.m_Frustum.testSphere(m_Position,AITPOINT_SIZE)){
            Fcolor c1,c2;
            c1.set(0.f,1.f,0.f,1.f);
            c2.set(1.f,1.f,0.f,1.f);
            DrawPoint(c1);
            DrawLinks(c2);
            if( Selected() ){
                Fbox bb; GetBox(bb);
                DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
                DU::DrawSelectionBox(bb,&clr);
            }
        }
    }
}
//----------------------------------------------------

bool CAITPoint::FrustumPick(const CFrustum& frustum, const Fmatrix& parent){
    return (frustum.testSphere(m_Position,AITPOINT_SIZE))?true:false;
}
//----------------------------------------------------

bool CAITPoint::RayPick(float& distance, Fvector& S, Fvector& D, Fmatrix& parent, SRayPickInfo* pinf){
	Fvector transformed;
	parent.transform_tiny(transformed, m_Position);

	Fvector ray2;
	ray2.sub( transformed, S );

    float d = ray2.dotproduct(D);
    if( d > 0  ){
        float d2 = ray2.magnitude();
        if( ((d2*d2-d*d) < (AITPOINT_SIZE*AITPOINT_SIZE)) && (d>AITPOINT_SIZE) ){
        	if (d<distance){
	            distance = d;
    	        return true;
            }
        }
    }
	return false;
}
//----------------------------------------------------

void CAITPoint::Move( Fvector& amount ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI->UpdateScene();
	m_Position.add( amount );
}
//----------------------------------------------------

void CAITPoint::Rotate( Fvector& center, Fvector& axis, float angle ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
	Fmatrix m;
	m.rotation			(axis, -angle);
	m_Position.sub		(center);
	m.transform_tiny	(m_Position);
	m_Position.add		(center);
    UI->UpdateScene		();
}
//----------------------------------------------------


bool CAITPoint::Load(CStream& F){
	DWORD version = 0;
	char buf[1024];

    R_ASSERT(F.ReadChunk(AITPOINT_CHUNK_VERSION,&version));
    if( version!=AITPOINT_VERSION ){
        Log->DlgMsg( mtError, "CAITPoint: Unsupported version.");
        return false;
    }
    
	SceneObject::Load(F);

    R_ASSERT(F.FindChunk(AITPOINT_CHUNK_POINT));
    F.Rvector		(m_Position);

    R_ASSERT(F.FindChunk(AITPOINT_CHUNK_LINKS));
    m_NameLinks.resize(F.Rdword());
    for (AStringIt s_it=m_NameLinks.begin(); s_it!=m_NameLinks.end(); s_it++){
		F.RstringZ	(buf); *s_it = buf;
    }

    return true;
}
//----------------------------------------------------

void CAITPoint::Save(CFS_Base& F){
	SceneObject::Save(F);

	F.open_chunk	(AITPOINT_CHUNK_VERSION);
	F.Wword			(AITPOINT_VERSION);
	F.close_chunk	();

    F.open_chunk	(AITPOINT_CHUNK_POINT);
    F.Wvector		(m_Position);
	F.close_chunk	();

    F.open_chunk	(AITPOINT_CHUNK_LINKS);
    F.Wdword		(m_Links.size());
    for (ObjectIt o_it=m_Links.begin(); o_it!=m_Links.end(); o_it++)
    	F.WstringZ	((*o_it)->GetName());
	F.close_chunk	();
}
//----------------------------------------------------

void CAITPoint::OnSynchronize(){
	m_Links.resize(m_NameLinks.size());
    ObjectIt o_it = m_Links.begin();
	for (AStringIt s_it=m_NameLinks.begin(); s_it!=m_NameLinks.end(); s_it++,o_it++){
    	*o_it = Scene->FindObjectByName(s_it->c_str(),OBJCLASS_AITPOINT);
        R_ASSERT(*o_it);
    }
    m_NameLinks.clear();
}
//----------------------------------------------------

void CAITPoint::AppendLink(CAITPoint* P){
	m_Links.push_back(P);
}
//----------------------------------------------------

bool CAITPoint::AddLink(CAITPoint* P){
	if (find(m_Links.begin(),m_Links.end(),P)==m_Links.end()){
    	AppendLink(P);
        P->AppendLink(this);
        UI->RedrawScene();
    	return true;
    }
	return false;
}
//----------------------------------------------------

bool CAITPoint::DeleteLink(CAITPoint* P){
	ObjectIt it = find(m_Links.begin(),m_Links.end(),P);
	if (it!=m_Links.end()){
		m_Links.erase(it);
        UI->RedrawScene();
        P->RemoveLink(this);
    	return true;
    }
    return false;
}
//----------------------------------------------------

bool CAITPoint::RemoveLink(CAITPoint* P){
	if (DeleteLink(P)){
    	P->DeleteLink(this);
        return true;
    }
	return false;
}
//----------------------------------------------------

