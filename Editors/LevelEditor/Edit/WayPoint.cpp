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
#include "render.h"
#include "PropertiesListHelper.h"

//----------------------------------------------------

#define WAYPOINT_SIZE 	1.5f
#define WAYPOINT_RADIUS WAYPOINT_SIZE*.5f

//------------------------------------------------------------------------------
// Way Point
//------------------------------------------------------------------------------
CWayPoint::CWayPoint(LPCSTR name)
{
	m_Name			= name;
	m_vPosition.set	(0,0,0);
	m_Flags.zero	();
    m_bSelected		= false;
}

CWayPoint::~CWayPoint()
{
	for (WPLIt it=m_Links.begin(); it!=m_Links.end(); it++)
    	xr_delete	(*it);
    m_Links.clear	();
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
    DU.DrawCross(pos,WAYPOINT_RADIUS,WAYPOINT_SIZE*0.85f,WAYPOINT_RADIUS,WAYPOINT_RADIUS,WAYPOINT_SIZE*0.15f,WAYPOINT_RADIUS,0x0000ff00);
	// draw links
	Fvector p1,p2;
    p1.set	(m_vPosition.x,m_vPosition.y+WAYPOINT_SIZE*0.85f,m_vPosition.z);
    for (WPLIt it=m_Links.begin(); it!=m_Links.end(); it++){
    	SWPLink* O = (SWPLink*)(*it);
	    p2.set	(O->way_point->m_vPosition.x,O->way_point->m_vPosition.y+WAYPOINT_SIZE*0.85f,O->way_point->m_vPosition.z);
    	DU.DrawLink(p1,p2,0.25f,0xffffff00);
        Fvector xx;
        xx.sub(p2,p1);
        xx.mul(0.9f);
        xx.add(p1);
    	DU.DrawText(xx,AnsiString().sprintf("P: %1.2f",O->probability).c_str(),0xffffffff,0xff000000);
    }
	if (bParentSelect&&m_bSelected){
    	Fbox bb; GetBox(bb);
        u32 clr = 0xffffffff;
		DU.DrawSelectionBox(bb,&clr);
	}
}
bool CWayPoint::RayPick(float& distance, const Fvector& S, const Fvector& D)
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
    return (frustum.testSphere_dirty(P,WAYPOINT_RADIUS))?true:false;
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
    UI.RedrawScene();
    UI.Command	(COMMAND_UPDATE_PROPERTIES);
}
WPLIt CWayPoint::FindLink(CWayPoint* P)
{
	for (WPLIt it=m_Links.begin(); it!=m_Links.end(); it++)
    	if ((*it)->way_point==P) return it;
	return m_Links.end();
}
void CWayPoint::InvertLink(CWayPoint* P)
{
	WPLIt A=FindLink(P);
    WPLIt B=P->FindLink(this);
    bool a=(A!=m_Links.end()), b=(B!=P->m_Links.end());
    if (a&&b) return;
	if (a) m_Links.erase(A);
	if (b) P->m_Links.erase(B);
    if (a) P->AppendLink(this, 1.f);	//. link pb?
	if (b) AppendLink(P, 1.f);          //. link pb?
}
void CWayPoint::AppendLink(CWayPoint* P, float pb)
{
	m_Links.push_back(xr_new<SWPLink>(P,pb));
}
bool CWayPoint::DeleteLink(CWayPoint* P)
{
	WPLIt it = FindLink(P);
	if (it!=m_Links.end()){
    	xr_delete		(*it);
		m_Links.erase	(it);
        UI.RedrawScene	();
    	return true;
    }
    return false;
}
bool CWayPoint::AddSingleLink(CWayPoint* P)
{
	if (std::find(m_Links.begin(),m_Links.end(),P)==m_Links.end()){
    	AppendLink(P,1.f);
        UI.RedrawScene();
    	return true;
    }
	return false;
}
bool CWayPoint::AddDoubleLink(CWayPoint* P)
{
	if (std::find(m_Links.begin(),m_Links.end(),P)==m_Links.end()){
    	AppendLink(P,1.f);
        P->AppendLink(this,1.f);
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
    	xr_delete(*it);
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
        	xr_delete(*it);
			m_WayPoints.erase(it);
            i--;
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
    m_WayPoints.push_back(xr_new<CWayPoint>(FHelper.GenerateName("wp",2,FindWPByName).c_str()));
    m_WayPoints.back()->m_bSelected=true;
    return m_WayPoints.back();
}

void CWayObject::Select(int flag)
{
    if (IsPointMode()){
	    if (Selected()){
        	for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++) (*it)->Select(flag);
        }
    }else{
		inherited::Select(flag);
    }
/*

    if (flag==0) for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++) (*it)->Select(0);
    if ((flag==1)&&(m_WayPoints.size()==1)) for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++) (*it)->Select(1);
*/
}

bool CWayObject::RaySelect(int flag, const Fvector& start, const Fvector& dir, bool bRayTest)
{
    if (IsPointMode()){
    	float dist = UI.ZFar();
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
	    if (Selected()){
            bool bRes=false;
            for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++)
                bRes|=(*it)->FrustumSelect(flag,frustum);
            return true;
        }
        return false;
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
        RCache.set_xform_world(Fidentity);
        Device.SetShader		(Device.m_WireShader);
        for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++) (*it)->Render(Selected());
        if( Selected() ){
            u32 clr = Locked()?0xFFFF0000:0xFFFFFFFF;
            Fbox bb; GetBox(bb);
            DU.DrawSelectionBox(bb,&clr);
        }
    }
}

bool CWayObject::RayPick(float& distance, const Fvector& S, const Fvector& D, SRayPickInfo* pinf)
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

bool CWayObject::IsPointMode()
{
	if (Tools.GetTarget()==OBJCLASS_WAY){
	    TfraWayPoint* frame=(TfraWayPoint*)Tools.GetFrame(); R_ASSERT(frame);
    	return frame->ebModePoint->Down;
    }else return false;
}

bool CWayObject::Load(IReader& F)
{
	Clear();

	u32 version = 0;
	AnsiString buf;

    if (!F.find_chunk(WAYOBJECT_CHUNK_VERSION)) return false;
    R_ASSERT(F.r_chunk(WAYOBJECT_CHUNK_VERSION,&version));
    if(version!=WAYOBJECT_VERSION){
        ELog.DlgMsg( mtError, "CWayPoint: Unsupported version.");
        return false;
    }

	CCustomObject::Load(F);

	R_ASSERT(F.find_chunk(WAYOBJECT_CHUNK_POINTS));
    m_WayPoints.resize(F.r_u16());
	for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++){
    	CWayPoint* W 	= xr_new<CWayPoint>(""); *it = W;
    	F.r_fvector3	(W->m_vPosition);
    	W->m_Flags.set	(F.r_u32());
        W->m_bSelected	= F.r_u16();
        F.r_stringZ		(buf);
        W->m_Name		= buf.c_str();
    }

	R_ASSERT(F.find_chunk(WAYOBJECT_CHUNK_LINKS));
    int l_cnt = F.r_u16();
    for (int k=0; k<l_cnt; k++){
    	int idx0 	= F.r_u16();
    	int idx1 	= F.r_u16();
        float pb	= F.r_float();
        m_WayPoints[idx0]->AppendLink(m_WayPoints[idx1],pb);
    }

	R_ASSERT(F.find_chunk(WAYOBJECT_CHUNK_TYPE));
    m_Type			= EWayType(F.r_u32());

    return true;
}
//----------------------------------------------------

void CWayObject::Save(IWriter& F)
{
	CCustomObject::Save(F);

	F.open_chunk	(WAYOBJECT_CHUNK_VERSION);
	F.w_u16			(WAYOBJECT_VERSION);
	F.close_chunk	();

    int l_cnt		= 0;
	F.open_chunk	(WAYOBJECT_CHUNK_POINTS);
    F.w_u16			(m_WayPoints.size());
	for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++){
    	CWayPoint* W = *it;
		F.w_fvector3(W->m_vPosition);
        F.w_u32		(W->m_Flags.get());
        F.w_u16		(W->m_bSelected);
        F.w_stringZ	(*W->m_Name?*W->m_Name:"");
        l_cnt		+= W->m_Links.size();
    }
	F.close_chunk	();

	F.open_chunk	(WAYOBJECT_CHUNK_LINKS);
    F.w_u16			(l_cnt);
	for (it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++){
    	CWayPoint* W= *it;
    	int from	= it-m_WayPoints.begin();
        for (WPLIt l_it=W->m_Links.begin(); l_it!=W->m_Links.end(); l_it++){
        	WPIt to	= std::find(m_WayPoints.begin(),m_WayPoints.end(),(*l_it)->way_point); R_ASSERT(to!=m_WayPoints.end());
	    	F.w_u16	(from);
	    	F.w_u16	(to-m_WayPoints.begin());
            F.w_float((*l_it)->probability);
        }
    }
	F.close_chunk	();

    F.open_chunk	(WAYOBJECT_CHUNK_TYPE);
    F.w_u32		(m_Type);
    F.close_chunk	();
}

bool CWayObject::ExportGame(SExportStreams& F){
	F.patrolpath.stream.open_chunk		(F.patrolpath.chunk++);
	{
        F.patrolpath.stream.open_chunk	(WAYOBJECT_CHUNK_VERSION);
        F.patrolpath.stream.w_u16		(WAYOBJECT_VERSION);
        F.patrolpath.stream.close_chunk	();

        F.patrolpath.stream.open_chunk	(WAYOBJECT_CHUNK_NAME);
        F.patrolpath.stream.w_stringZ	(Name);
        F.patrolpath.stream.close_chunk	();

        int l_cnt		= 0;
        F.patrolpath.stream.open_chunk	(WAYOBJECT_CHUNK_POINTS);
        F.patrolpath.stream.w_u16		(m_WayPoints.size());
        for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++){
            CWayPoint* W = *it;
            F.patrolpath.stream.w_fvector3	(W->m_vPosition);
            F.patrolpath.stream.w_u32		(W->m_Flags.get());
	        F.patrolpath.stream.w_stringZ	(*W->m_Name?*W->m_Name:"");
            l_cnt		+= W->m_Links.size();
        }
        F.patrolpath.stream.close_chunk	();

        F.patrolpath.stream.open_chunk	(WAYOBJECT_CHUNK_LINKS);
        F.patrolpath.stream.w_u16		(l_cnt);
        for (it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++){
            CWayPoint* W= *it;
            int from	= it-m_WayPoints.begin();
            for (WPLIt l_it=W->m_Links.begin(); l_it!=W->m_Links.end(); l_it++){
                WPIt to= std::find(m_WayPoints.begin(),m_WayPoints.end(),(*l_it)->way_point); R_ASSERT(to!=m_WayPoints.end());
                F.patrolpath.stream.w_u16	(from);
                F.patrolpath.stream.w_u16	(to-m_WayPoints.begin());
	            F.patrolpath.stream.w_float	((*l_it)->probability);
            }
        }
        F.patrolpath.stream.close_chunk	();
    }
    F.patrolpath.stream.close_chunk		();
    return true;
}
//----------------------------------------------------

CWayPoint* CWayObject::FindWayPoint(LPCSTR nm)
{
	ref_str N 		= nm;
    for (WPIt it=m_WayPoints.begin(); it!=m_WayPoints.end(); it++)
    	if ((*it)->m_Name.equal(N)) return *it;
    return 0;
}

void __fastcall CWayObject::OnWayPointNameAfterEdit(PropItem* sender, LPVOID edit_val)
{
	RTextValue* V 	= (RTextValue*)sender->GetFrontValue();
	AnsiString* new_name = (AnsiString*)edit_val;
    if (FindWayPoint(new_name->c_str())){
        *new_name 	= *V->GetValue();
    }else{
	    *new_name 	= new_name->LowerCase();
    }
}

void CWayObject::FillProp(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp(pref,items);

	if (IsPointMode()){
        for(WPIt it=m_WayPoints.begin();it!=m_WayPoints.end();it++){
        	CWayPoint* W = *it;
            if ((*it)->m_bSelected){
            	PropValue* V 					= PHelper.CreateRText(items, FHelper.PrepareKey(pref,"Name"),&W->m_Name);
                V->Owner()->OnAfterEditEvent 	= OnWayPointNameAfterEdit;
                for (int k=0; k<32; k++)
                    PHelper.CreateFlag32(items,	FHelper.PrepareKey(pref,"Flags",AnsiString(k).c_str()),	&W->m_Flags,	1<<k);
                for (WPLIt l_it=W->m_Links.begin(); l_it!=W->m_Links.end(); l_it++)
                    PHelper.CreateFloat	(items,	FHelper.PrepareKey(pref,"Links",*(*l_it)->way_point->m_Name),&(*l_it)->probability);
            }
    	}
    }
}
//----------------------------------------------------

bool CWayObject::OnSelectionRemove()
{
	if (IsPointMode()){
    	RemoveSelectedPoints();
	    return m_WayPoints.empty();
    }else return true;
}
//----------------------------------------------------

