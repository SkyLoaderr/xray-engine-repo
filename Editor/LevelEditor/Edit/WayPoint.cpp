//----------------------------------------------------
// file: WayPoint.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "WayPoint.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "D3DUtils.h"
#include "Scene.h"

#define WAYPOINT_VERSION				0x0012
//----------------------------------------------------
#define WAYPOINT_CHUNK_VERSION			0xE501
#define WAYPOINT_CHUNK_POINT			0xE502
#define WAYPOINT_CHUNK_LINK_FROM		0xE503
#define WAYPOINT_CHUNK_LINK_TO			0xE504
#define WAYPOINT_CHUNK_PATHNAME			0xE510
//----------------------------------------------------

#define WAYPOINT_SIZE 	1.5f
#define WAYPOINT_RADIUS WAYPOINT_SIZE*.5f

#define WP_FROM 0
#define WP_TO	1
//------------------------------------------------------------------------------
// AI Traffic points
//------------------------------------------------------------------------------
CWayPoint::CWayPoint(char *name):CCustomObject()
{
	Construct();
	Name		= name;
}

CWayPoint::CWayPoint():CCustomObject()
{
	Construct();
}

void CWayPoint::Construct()
{
	ClassID   	= OBJCLASS_WAYPOINT;
    m_PathName	= "";
}

CWayPoint::~CWayPoint()
{
	OnDestroy();
}
//----------------------------------------------------

void CWayPoint::OnDestroy()
{
	inherited::OnDestroy();
	// удалить у всех линков себя
    for (ObjectIt o_it=m_Links.begin(); o_it!=m_Links.end(); o_it++)
    	((CWayPoint*)(*o_it))->DeleteLink(this);
}
//----------------------------------------------------

bool CWayPoint::GetBox( Fbox& box )
{
	box.set( PPosition, PPosition );
	box.min.x -= WAYPOINT_RADIUS;
	box.min.y -= 0;
	box.min.z -= WAYPOINT_RADIUS;
	box.max.x += WAYPOINT_RADIUS;
	box.max.y += WAYPOINT_SIZE;
	box.max.z += WAYPOINT_RADIUS;
	return true;
}

void CWayPoint::DrawPoint(Fcolor& c)
{
	Fvector pos;
    pos.set	(PPosition.x,PPosition.y+WAYPOINT_SIZE*0.85f,PPosition.z);
    DU::DrawCross(pos,WAYPOINT_RADIUS,WAYPOINT_SIZE*0.85f,WAYPOINT_RADIUS,WAYPOINT_RADIUS,WAYPOINT_SIZE*0.15f,WAYPOINT_RADIUS,c.get());
}
//----------------------------------------------------

void CWayPoint::DrawLinks(Fcolor& c)
{
	Fvector p1,p2;
    p1.set	(PPosition.x,PPosition.y+WAYPOINT_SIZE*0.85f,PPosition.z);
    for (ObjectIt o_it=m_Links.begin(); o_it!=m_Links.end(); o_it++){
    	CWayPoint* O = (CWayPoint*)(*o_it);
	    p2.set	(O->PPosition.x,O->PPosition.y+WAYPOINT_SIZE*0.85f,O->PPosition.z);
    	DU::DrawLink(p1,p2,0.25f,c.get());
    }
}
//----------------------------------------------------

void CWayPoint::Render(int priority, bool strictB2F)
{
	inherited::Render(priority, strictB2F);
    if ((1==priority)&&(false==strictB2F)){
        Fcolor c1,c2;
        c1.set(0.f,1.f,0.f,1.f);
        c2.set(1.f,1.f,0.f,1.f);
		DrawLinks(c2);
        if (Device.m_Frustum.testSphere(PPosition,WAYPOINT_SIZE)){
            DrawPoint(c1);
            if( Selected() ){
                Fbox bb; GetBox(bb);
                DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
                DU::DrawSelectionBox(bb,&clr);
            }
        }
    }
}
//----------------------------------------------------

bool CWayPoint::FrustumPick(const CFrustum& frustum)
{
	Fvector P=PPosition; P.y+=WAYPOINT_RADIUS;
    return (frustum.testSphere(P,WAYPOINT_RADIUS))?true:false;
}
//----------------------------------------------------

bool CWayPoint::RayPick(float& distance, Fvector& S, Fvector& D, SRayPickInfo* pinf)
{
	Fvector ray2;
	ray2.sub( PPosition, S ); ray2.y+=WAYPOINT_RADIUS;

    float d = ray2.dotproduct(D);
    if( d > 0  ){
        float d2 = ray2.magnitude();
        if( ((d2*d2-d*d) < (WAYPOINT_RADIUS*WAYPOINT_RADIUS)) && (d>WAYPOINT_RADIUS) ){
        	if (d<distance){
	            distance = d;
    	        return true;
            }
        }
    }
	return false;
}
//----------------------------------------------------

bool CWayPoint::Load(CStream& F)
{
	DWORD version = 0;
	char buf[1024];

    R_ASSERT(F.ReadChunk(WAYPOINT_CHUNK_VERSION,&version));
    if((version!=0x0010)&&(version!=WAYPOINT_VERSION)){
        ELog.DlgMsg( mtError, "CWayPoint: Unsupported version.");
        return false;
    }

	CCustomObject::Load(F);

    if (version==0x0010){
	    R_ASSERT(F.FindChunk(WAYPOINT_CHUNK_POINT));
    	F.Rvector	(PPosition);
        UpdateTransform();
    }

    R_ASSERT(F.FindChunk(WAYPOINT_CHUNK_LINKS));
    m_NameLinks.resize(F.Rdword());
    for (AStringIt s_it=m_NameLinks.begin(); s_it!=m_NameLinks.end(); s_it++){
		F.RstringZ	(buf); *s_it = buf;
    }

    if (F.FindChunk(WAYPOINT_CHUNK_PATHNAME)){
		F.RstringZ	(buf); m_PathName=buf;
    }

    return true;
}
//----------------------------------------------------

void CWayPoint::Save(CFS_Base& F)
{
	CCustomObject::Save(F);

	F.open_chunk	(WAYPOINT_CHUNK_VERSION);
	F.Wword			(WAYPOINT_VERSION);
	F.close_chunk	();

    F.open_chunk	(WAYPOINT_CHUNK_LINK_FROM);
    F.Wdword		(m_Links.size());
    for (ObjectIt o_it=m_Links[WP_FROM].begin(); o_it!=m_Links[WP_FROM].end(); o_it++)
    	F.WstringZ	((*o_it)->Name);
	F.close_chunk	();

    F.open_chunk	(WAYPOINT_CHUNK_LINK_TO);
    F.Wdword		(m_Links.size());
    for (ObjectIt o_it=m_Links[WP_TO].begin(); o_it!=m_Links[WP_TO].end(); o_it++)
    	F.WstringZ	((*o_it)->Name);
	F.close_chunk	();

    F.open_chunk	(WAYPOINT_CHUNK_PATHNAME);
	F.WstringZ		(m_PathName.c_str());
	F.close_chunk	();
}
//----------------------------------------------------

void CWayPoint::OnSynchronize()
{
	m_Links.resize(m_NameLinks.size());
    ObjectIt o_it = m_Links.begin();
	for (AStringIt s_it=m_NameLinks.begin(); s_it!=m_NameLinks.end(); s_it++,o_it++){
    	*o_it = Scene.FindObjectByName(s_it->c_str(),OBJCLASS_WAYPOINT);
        R_ASSERT(*o_it);
    }
    m_NameLinks.clear();
}
//----------------------------------------------------

void CWayPoint::AppendLink(CWayPoint* P)
{
	m_Links.push_back(P);
}
//----------------------------------------------------

bool CWayPoint::AddSingleLink(CWayPoint* P)
{
	if (find(m_Links.begin(),m_Links.end(),P)==m_Links.end()){
    	AppendLink(P);
        SetPathName(P->GetPathName(),true);
        UI.RedrawScene();
    	return true;
    }
	return false;
}
//----------------------------------------------------

bool CWayPoint::AddDoubleLink(CWayPoint* P)
{
	if (find(m_Links.begin(),m_Links.end(),P)==m_Links.end()){
    	AppendLink(P);
        P->AppendLink(this);
        SetPathName(P->GetPathName(),true);
        UI.RedrawScene();
    	return true;
    }
	return false;
}
//----------------------------------------------------

bool CWayPoint::DeleteLink(CWayPoint* P)
{
	ObjectIt it = find(m_Links.begin(),m_Links.end(),P);
	if (it!=m_Links.end()){
		m_Links.erase(it);
        UI.RedrawScene();
//S        P->RemoveLink(this);
    	return true;
    }
    return false;
}
//----------------------------------------------------

bool CWayPoint::RemoveLink(CWayPoint* P)
{
	if (DeleteLink(P)){
    	P->DeleteLink(this);
        return true;
    }
	return false;
}
//----------------------------------------------------

void CWayPoint::GetLinkedObjects(ObjectList& lst)
{
	for (ObjectIt it=m_Links.begin(); it!=m_Links.end(); it++){
    	if (find(lst.begin(),lst.end(),*it)!=lst.end()) continue;
    	lst.push_back(*it);
    	((CWayPoint*)(*it))->GetLinkedObjects(lst);
    }
}
//----------------------------------------------------

void CWayPoint::SetPathName(LPCSTR name, bool bOnlyThis)
{
	if (bOnlyThis){
    	m_PathName=name;
    }else{
		ObjectList objects;
        GetLinkedObjects(objects);
		for (ObjectIt it=objects.begin(); it!=objects.end(); it++)
			((CWayPoint*)(*it))->SetPathName(name,true);
    }
}
//----------------------------------------------------

