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
// Way Point
//------------------------------------------------------------------------------
CWayPoint::CWayPoint()
{
	m_vPosition.set(0,0,0);
	m_dwFlag	= 0;
    m_bSelected	= false;
}
void CWayPoint::Render()
{
	Fvector pos;
    pos.set	(m_vPosition.x,m_vPosition.y+WAYPOINT_SIZE*0.85f,m_vPosition.z);
    DU::DrawCross(pos,WAYPOINT_RADIUS,WAYPOINT_SIZE*0.85f,WAYPOINT_RADIUS,WAYPOINT_RADIUS,WAYPOINT_SIZE*0.15f,WAYPOINT_RADIUS,m_bSelected?0x00ffffff:0x0000ff00);
	// draw links
	Fvector p1,p2;
    p1.set	(m_vPosition.x,m_vPosition.y+WAYPOINT_SIZE*0.85f,m_vPosition.z);
    for (WPIt it=m_Links.begin(); it!=m_Links.end(); it++){
    	CWayPoint* O = (CWayPoint*)(*it);
	    p2.set	(O->m_vPosition.x,O->m_vPosition.y+WAYPOINT_SIZE*0.85f,O->m_vPosition.z);
    	DU::DrawLink(p1,p2,0.25f,0xffffff00);
    }
	if (m_bSelected){
	}
}
bool CWayPoint::RayPick(float& distance, Fvector& S, Fvector& D)
{
	Fvector ray2;
	ray2.sub( m_vPosition, S ); ray2.y+=WAYPOINT_RADIUS;

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
bool CWayPoint::FrustumPick(const CFrustum& frustum)
{
	Fvector P=m_vPosition; P.y+=WAYPOINT_RADIUS;
    return (frustum.testSphere(P,WAYPOINT_RADIUS))?true:false;
}
bool CWayPoint::FrustumSelect(int flag, const CFrustum& frustum)
{
	if (FrustumPick(frustum)){
    	Select(flag);
    	return true;
    }
	return false;
}
void CWayPoint::Select( int flag )
{
	m_bSelected = (flag==-1)?(m_bSelected?false:true):flag;
}
WPIt CWayPoint::HasLink(CWayPoint* P)
{
	find(m_Links.begin(),m_Links.end(),P);
}
void CWayPoint::InvertLink(CWayPoint* P)
{
	WPIt it=HasLink(P);
	if (it!=m_Links.end()){
    	m_Links.erase(it);
        P->AppendLink(this);
    }else{
		WPIt it=P->HasLink(this);
    	if (it!=P->m_Links.end()){
	    	P->m_Links.erase(it);
    	    AppendLink(P);
        }
    }
} 
void CWayPoint::AppendLink(CWayPoint* P)
{
	m_Links.push_back(P);
}
bool CWayPoint::DeleteLink(CWayPoint* P)
{
	WPIt it = find(m_Links.begin(),m_Links.end(),P);
	if (it!=m_Links.end()){
		m_Links.erase(it);
        UI.RedrawScene();
    	return true;
    }
    return false;
}
bool CWayPoint::AddSingleLink(CWayPoint* P)
{
	if (find(m_Links.begin(),m_Links.end(),P)==m_Links.end()){
    	AppendLink(P);
        UI.RedrawScene();
    	return true;
    }
	return false;
}
bool CWayPoint::AddDoubleLink(CWayPoint* P)
{
	if (find(m_Links.begin(),m_Links.end(),P)==m_Links.end()){
    	AppendLink(P);
        P->AppendLink(this);
        UI.RedrawScene();
    	return true;
    }
	return false;
}
bool CWayPoint::RemoveLink(CWayPoint* P)
{
	if (DeleteLink(P)){
    	P->DeleteLink(this);
        return true;
    }
	return false;
}
//------------------------------------------------------------------------------
// Way Object
//------------------------------------------------------------------------------
CWayObject::CWayObject(char *name):CCustomObject()
{
	Construct();
	Name		= name;
}

CWayObject::CWayObject():CCustomObject()
{
	Construct();
}

void CWayObject::Construct()
{
	ClassID   	= OBJCLASS_WAY;
    AppendWayPoint();
}

CWayObject::~CWayObject()
{
	for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++)
    	_DELETE(*it);
}

void CWayObject::RemoveSelectedPoints()
{
	for (WPIt f_it=m_WayPoints.begin(); f_it!=m_WayPoints.end(); f_it++){
    	if ((*f_it)->m_bSelected){
			for (WPIt l_it=m_WayPoints.begin(); l_it!=m_WayPoints.end(); l_it++){
            	if (l_it==f_it) continue;
    			(*l_it)->DeleteLink(*f_it);
            }
        }
    }
	for (int i=0; i<(int)m_WayPoints.size(); i++)
    	if (m_WayPoints[i]->m_bSelected){
			m_WayPoints.erase(m_WayPoints.begin()+i);
            i--;
        }
    // remove object from scene if empty?
    if (m_WayPoints.empty()){
    	Scene.RemoveObject(this,true);
    	delete this;
    }
}

int CWayObject::GetSelectedPoints(WPVec& lst)
{
	for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++)
    	if ((*it)->m_bSelected) lst.push_back(*it);
    return lst.size();
}

CWayPoint* CWayObject::GetFirstSelected()
{
	for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++)
    	if ((*it)->m_bSelected) return *it;
    return 0;
}

CWayPoint* CWayObject::AppendWayPoint()
{
	for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++)
    	(*it)->Select(0);
    m_WayPoints.push_back(new CWayPoint());
    m_WayPoints.back()->m_bSelected=true;
    return m_WayPoints.back();
}

void CWayObject::Select(int flag)
{
	inherited::Select(flag);
    if (flag==0) for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++) (*it)->Select(0);
}

bool CWayObject::RaySelect(int flag, Fvector& start,Fvector& dir, bool bRayTest)
{
    if (UI.GetShiftState().Contains(ssAlt)){
    	if ((bRayTest&&RayPick(UI.ZFar(),start,dir))||!bRayTest) Select(1);
        CWayPoint* nearest=0;
        float dist = UI.ZFar();
		for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++)
			if ((*it)->RayPick(dist,start,dir)) nearest=*it;
        if (nearest!=0){
        	nearest->Select(flag);
            return true;
        }
    }else 	return inherited::RaySelect(flag,start,dir,bRayTest);
    return false;
}

bool CWayObject::FrustumSelect(int flag, const CFrustum& frustum)
{
    if (UI.GetShiftState().Contains(ssAlt)){
    	Select(1);
		for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++)
        	(*it)->FrustumSelect(flag,frustum);
        return true;
    }else 	return inherited::FrustumSelect(flag,frustum);
}

bool CWayObject::GetBox( Fbox& box )
{
	box.invalidate();
	for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++)
    	box.modify((*it)->m_vPosition);
    box.max.x+=WAYPOINT_RADIUS;
    box.max.z+=WAYPOINT_RADIUS;
    box.max.y+=WAYPOINT_SIZE;
    box.min.x-=WAYPOINT_RADIUS;
    box.min.z-=WAYPOINT_RADIUS;
	return true;
}

void CWayObject::MoveTo(const Fvector& pos, const Fvector& up)
{
	Fvector P=PPosition;
	inherited::MoveTo(pos,up);
    P.add(PPosition);
	for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++)
    	(*it)->m_vPosition.add(P);
}

void CWayObject::Render(int priority, bool strictB2F)
{
//	inherited::Render(priority, strictB2F);
    if ((1==priority)&&(false==strictB2F)){
		Fbox bb; GetBox(bb);
        Fvector C; float S; bb.getsphere(C,S);
        if (Device.m_Frustum.testSphere(C,S)){
			for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++) (*it)->Render();
            if( Selected() ){
                DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
                DU::DrawSelectionBox(bb,&clr);
            }
        }
    }
}

bool CWayObject::RayPick(float& distance, Fvector& S, Fvector& D, SRayPickInfo* pinf)
{
    bool bPick=false;
	for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++)
    	if ((*it)->RayPick(distance,S,D)) bPick=true;
    return bPick;
}

bool CWayObject::FrustumPick(const CFrustum& frustum)
{
	for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++)
    	if ((*it)->FrustumPick(frustum)) return true;
    return false;
}
//----------------------------------------------------


/*
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
*/
