//----------------------------------------------------
// file: WayPoint.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "WayPoint.h"
#include "Scene.h"
#include "UI_Tools.h"
#include "FrameWayPoint.h"
#include "d3dutils.h"
#include "ui_main.h"

//----------------------------------------------------

#define WAYPOINT_SIZE 	1.5f
#define WAYPOINT_RADIUS WAYPOINT_SIZE*.5f

//------------------------------------------------------------------------------
// Way Point
//------------------------------------------------------------------------------
CWayPoint::CWayPoint()
{
	m_vPosition.set(0,0,0);
	m_dwFlags	= 0;
    m_bSelected	= false;
}
void CWayPoint::GetBox(Fbox& bb)
{
	bb.set(m_vPosition,m_vPosition);
    bb.max.y+=WAYPOINT_SIZE;
    bb.max.x+=WAYPOINT_RADIUS;
    bb.max.z+=WAYPOINT_RADIUS;
    bb.min.x-=WAYPOINT_RADIUS;
    bb.min.z-=WAYPOINT_RADIUS;
}
void CWayPoint::Render(bool bParentSelect)
{
	Fvector pos;
    pos.set	(m_vPosition.x,m_vPosition.y+WAYPOINT_SIZE*0.85f,m_vPosition.z);
    DU::DrawCross(pos,WAYPOINT_RADIUS,WAYPOINT_SIZE*0.85f,WAYPOINT_RADIUS,WAYPOINT_RADIUS,WAYPOINT_SIZE*0.15f,WAYPOINT_RADIUS,0x0000ff00);
	// draw links
	Fvector p1,p2;
    p1.set	(m_vPosition.x,m_vPosition.y+WAYPOINT_SIZE*0.85f,m_vPosition.z);
    for (WPIt it=m_Links.begin(); it!=m_Links.end(); it++){
    	CWayPoint* O = (CWayPoint*)(*it);
	    p2.set	(O->m_vPosition.x,O->m_vPosition.y+WAYPOINT_SIZE*0.85f,O->m_vPosition.z);
    	DU::DrawLink(p1,p2,0.25f,0xffffff00);
    }
	if (bParentSelect&&m_bSelected){
    	Fbox bb; GetBox(bb);
        DWORD clr = 0xffffffff;
		DU::DrawSelectionBox(bb,&clr);
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
	return find(m_Links.begin(),m_Links.end(),P);
}
void CWayPoint::InvertLink(CWayPoint* P)
{
	WPIt A=HasLink(P);
    WPIt B=P->HasLink(this);
    bool a=(A!=m_Links.end()), b=(B!=P->m_Links.end());
    if (a&&b) return;
	if (a) m_Links.erase(A);
	if (b) P->m_Links.erase(B);
    if (a) P->AppendLink(this);
	if (b) AppendLink(P);
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
CWayObject::CWayObject(LPVOID data, LPCSTR name):CCustomObject(data,name)
{
	Construct(data);
}

void CWayObject::Construct(LPVOID data)
{
	ClassID   	= OBJCLASS_WAY;
    m_Type		= wtPatrolPath;
	AppendWayPoint();
}

CWayObject::~CWayObject()
{
	Clear();
}

void CWayObject::Clear()
{
	for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++)
    	_DELETE(*it);
}

void CWayObject::InvertLink()
{
    WPVec objects;
    if (GetSelectedPoints(objects)){
        WPIt _A0=objects.begin();
        WPIt _A1=objects.end(); _A1--;
        WPIt _B1=objects.end();
        for (WPIt _A=_A0; _A!=_A1; _A++){
            CWayPoint* A = (CWayPoint*)(*_A);
            WPIt _B=_A; _B++;
            for (; _B!=_B1; _B++){
                CWayPoint* B = (CWayPoint*)(*_B);
                A->InvertLink(B);
            }
        }
    }
}

void CWayObject::Add1Link()
{
	RemoveLink();
    WPVec objects;
    if (GetSelectedPoints(objects)){
        WPIt _A0=objects.begin();
        WPIt _A1=objects.end(); _A1--;
        WPIt _B1=objects.end();
        for (WPIt _A=_A0; _A!=_A1; _A++){
            CWayPoint* A = (CWayPoint*)(*_A);
            WPIt _B=_A; _B++;
            for (; _B!=_B1; _B++){
                CWayPoint* B = (CWayPoint*)(*_B);
                A->AddSingleLink(B);
            }
        }
    }
}

void CWayObject::Add2Link()
{
    RemoveLink();
    WPVec objects;
    if (GetSelectedPoints(objects)){
        WPIt _A0=objects.begin();
        WPIt _A1=objects.end(); _A1--;
        WPIt _B1=objects.end();
        for (WPIt _A=_A0; _A!=_A1; _A++){
            CWayPoint* A = (CWayPoint*)(*_A);
            WPIt _B=_A; _B++;
            for (; _B!=_B1; _B++){
                CWayPoint* B = (CWayPoint*)(*_B);
                A->AddDoubleLink(B);
            }
        }
    }
}

void CWayObject::RemoveLink()
{
	WPVec objects;
    if (GetSelectedPoints(objects)){
        WPIt _A0=objects.begin();
        WPIt _A1=objects.end(); _A1--;
        WPIt _B1=objects.end();
        for (WPIt _A=_A0; _A!=_A1; _A++){
            CWayPoint* A = (CWayPoint*)(*_A);
            WPIt _B=_A; _B++;
            for (; _B!=_B1; _B++){ 
                CWayPoint* B = (CWayPoint*)(*_B);
                A->RemoveLink(B);
            }
        }
    }
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
        	WPIt it = m_WayPoints.begin()+i;
        	_DELETE(*it);
			m_WayPoints.erase(it);
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
    if ((flag==1)&&(m_WayPoints.size()==1)) for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++) (*it)->Select(1);
}

bool CWayObject::RaySelect(int flag, Fvector& start,Fvector& dir, bool bRayTest)
{
    if (IsPointMode()){
    	float dist = UI.ZFar();
    	if ((bRayTest&&RayPick(dist,start,dir))||!bRayTest) Select(1);
        CWayPoint* nearest=0;
        dist = UI.ZFar();
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
    if (IsPointMode()){
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
	if (IsPointMode()){
        for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++)
            if ((*it)->m_bSelected) (*it)->m_vPosition.set(pos);
    }else{
        for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++)
            (*it)->m_vPosition.set(pos);
    }
}

void CWayObject::Move(Fvector& amount)
{
	if (IsPointMode()){
        for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++)
            if ((*it)->m_bSelected) (*it)->m_vPosition.add(amount);
    }else{
        for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++)
            (*it)->m_vPosition.add(amount);
    }
}

void CWayObject::Render(int priority, bool strictB2F)
{
//	inherited::Render(priority, strictB2F);
    if ((1==priority)&&(false==strictB2F)){
		Fbox bb; GetBox(bb);
        Fvector C; float S; bb.getsphere(C,S);
        if (Device.m_Frustum.testSphere(C,S)){
			for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++) (*it)->Render(Selected());
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

bool CWayObject::IsPointMode(){
	if (Tools.GetTargetClassID()==OBJCLASS_WAY){
		TfraWayPoint* frame=(TfraWayPoint*)Tools.GetFrame(); R_ASSERT(frame);
        return frame->ebPointMode->Down;
    }
    return false;
}

bool CWayObject::Load(CStream& F)
{
	Clear();
    
	DWORD version = 0;
	char buf[1024];

    if (!F.FindChunk(WAYOBJECT_CHUNK_VERSION)) return false;
    R_ASSERT(F.ReadChunk(WAYOBJECT_CHUNK_VERSION,&version));
    if(version!=WAYOBJECT_VERSION){
        ELog.DlgMsg( mtError, "CWayPoint: Unsupported version.");
        return false;
    }

	CCustomObject::Load(F);

	R_ASSERT(F.FindChunk(WAYOBJECT_CHUNK_POINTS));
    m_WayPoints.resize(F.Rword());
	for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++){
    	CWayPoint* W 	= new CWayPoint(); *it = W;
    	F.Rvector		(W->m_vPosition);
    	W->m_dwFlags 	= F.Rdword();
        W->m_bSelected	= F.Rword();
    }

	R_ASSERT(F.FindChunk(WAYOBJECT_CHUNK_LINKS));
    int l_cnt = F.Rword();
    for (int k=0; k<l_cnt; k++){
    	int idx0 = F.Rword();
    	int idx1 = F.Rword();
        m_WayPoints[idx0]->AppendLink(m_WayPoints[idx1]);
    }

	R_ASSERT(F.FindChunk(WAYOBJECT_CHUNK_TYPE));
    m_Type			= EWayType(F.Rdword());

    return true;
}
//----------------------------------------------------

void CWayObject::Save(CFS_Base& F)
{
	CCustomObject::Save(F);

	F.open_chunk	(WAYOBJECT_CHUNK_VERSION);
	F.Wword			(WAYOBJECT_VERSION);
	F.close_chunk	();

    int l_cnt		= 0;
	F.open_chunk	(WAYOBJECT_CHUNK_POINTS);
    F.Wword			(m_WayPoints.size());
	for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++){
    	CWayPoint* W = *it;
		F.Wvector	(W->m_vPosition);
        F.Wdword	(W->m_dwFlags);
        F.Wword		(W->m_bSelected);
        l_cnt		+= W->m_Links.size();
    }
	F.close_chunk	();

	F.open_chunk	(WAYOBJECT_CHUNK_LINKS);
    F.Wword			(l_cnt);
	for (it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++){
    	CWayPoint* W= *it;
    	int from	= it-m_WayPoints.begin();
        for (WPIt l_it=W->m_Links.begin(); l_it!=W->m_Links.end(); l_it++){
        	WPIt to	= find(m_WayPoints.begin(),m_WayPoints.end(),*l_it); R_ASSERT(to!=W->m_Links.end());
	    	F.Wword	(from);
	    	F.Wword	(to-m_WayPoints.begin());
        }
    }
	F.close_chunk	();

    F.open_chunk	(WAYOBJECT_CHUNK_TYPE);
    F.Wdword		(m_Type);
    F.close_chunk	();
}

bool CWayObject::ExportGame(SExportStreams& F){
	F.patrolpath.stream.open_chunk		(F.patrolpath.chunk++);
	{
        F.patrolpath.stream.open_chunk	(WAYOBJECT_CHUNK_VERSION);
        F.patrolpath.stream.Wword		(WAYOBJECT_VERSION);
        F.patrolpath.stream.close_chunk	();

        F.patrolpath.stream.open_chunk	(WAYOBJECT_CHUNK_NAME);
        F.patrolpath.stream.WstringZ	(Name);
        F.patrolpath.stream.close_chunk	();

        int l_cnt		= 0;
        F.patrolpath.stream.open_chunk	(WAYOBJECT_CHUNK_POINTS);
        F.patrolpath.stream.Wword		(m_WayPoints.size());
        for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++){
            CWayPoint* W = *it;
            F.patrolpath.stream.Wvector	(W->m_vPosition);
            F.patrolpath.stream.Wdword	(W->m_dwFlags);
            l_cnt		+= W->m_Links.size();
        }
        F.patrolpath.stream.close_chunk	();

        F.patrolpath.stream.open_chunk	(WAYOBJECT_CHUNK_LINKS);
        F.patrolpath.stream.Wword		(l_cnt);
        for (it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++){
            CWayPoint* W= *it;
            int from	= it-m_WayPoints.begin();
            for (WPIt l_it=W->m_Links.begin(); l_it!=W->m_Links.end(); l_it++){
                WPIt to	= find(m_WayPoints.begin(),m_WayPoints.end(),*l_it); R_ASSERT(to!=W->m_Links.end());
                F.patrolpath.stream.Wword	(from);
                F.patrolpath.stream.Wword	(to-m_WayPoints.begin());
            }
        }
        F.patrolpath.stream.close_chunk	();
    }
    F.patrolpath.stream.close_chunk		();
    return true;
}
//----------------------------------------------------

