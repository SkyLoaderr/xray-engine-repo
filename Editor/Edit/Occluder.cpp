//----------------------------------------------------
// file: Occluder.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Occluder.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "frustum.h"
#include "cl_intersect.h"
#include "texture.h"
#include "D3DUtils.h"
#include "MgcConvexHull2D.h"
#include "MgcAppr3DPlaneFit.h"
#include "FrameOccluder.h"

#define OCCLUDER_POINT_SEL_DIST				0.1f

#define OCCLUDER_VERSION	   				0x0011
//----------------------------------------------------
#define OCCLUDER_CHUNK_VERSION				0x0110
#define OCCLUDER_CHUNK_TRANSFORM			0x0111
#define OCCLUDER_CHUNK_VERTICES				0x0112
#define OCCLUDER_CHUNK_SEL_VERTICES			0x0113
//----------------------------------------------------

COccluder::COccluder( char *name ):SceneObject(){
	Construct();
	strcpy( m_Name, name );
}

COccluder::COccluder():SceneObject(){
	Construct();
}

COccluder::~COccluder(){
}

void COccluder::Construct(){
	m_ClassID = OBJCLASS_OCCLUDER;
    m_vPlaneSize.set(1.f,1.f);
    m_vRotate.set(PI_DIV_2,0,0);
    m_vCenter.set(0,0,0);

    m_Points.resize(4);
    m_Points[0].set(-1.f,-1.f);
    m_Points[1].set(+1.f,-1.f);
    m_Points[2].set(+1.f,+1.f);
    m_Points[3].set(-1.f,+1.f);
}

bool COccluder::ResizePoints(int new_size){
	if ((m_Points.size()==new_size)||(new_size>MAX_OCCLUDER_POINTS)||(new_size<MIN_OCCLUDER_POINTS)) return false;
    int old_size = m_Points.size();
	m_Points.resize(new_size);
	if (new_size>old_size){
    	Fvector2 d;
        d.sub(m_Points[0],m_Points[old_size-1]);
		float d_len = d.magnitude();
        if (d_len){
	        d.mul(1/d_len);
            d_len /= (new_size-old_size+1);
            float sz = d_len;
        	for (int i=0; i<(new_size-old_size); i++,sz+=d_len)
             	m_Points[old_size+i].direct(m_Points[old_size-1],d,sz);
        }else{
        	for (int i=0; i<(new_size-old_size); i++) m_Points[old_size+i].set(m_Points[0]);
        }
    }
    m_SelPoints.clear();
	return true;
}

void COccluder::MovePoints(Fvector& val){
	Fvector2 amount;
    amount.x=val.x/m_vPlaneSize.x;
    amount.y=val.z/m_vPlaneSize.y;
    for (INTIt it=m_SelPoints.begin(); it!=m_SelPoints.end(); it++){
		Fvector2& p = m_Points[*it];
		p.add(amount);
    	clamp(p.x,-1.f,1.f);
    	clamp(p.y,-1.f,1.f);
    }
}

void COccluder::Optimize(){
	Fvector2Vec vertices;
    // compute 2D Convex Hull
    Mgc::ConvexHull2D Hull(m_Points.size(),(const Mgc::Vector2*)m_Points.begin());
	int Count   = Hull.GetEdgeQuantity();
	if (Count==0) return;
    int CurVert = Hull.GetEdge(0).m_aiVertex[0];
	vertices.push_back(m_Points[CurVert]);
    CurVert = Hull.GetEdge(0).m_aiVertex[1];
    vertices.push_back(m_Points[CurVert]);

    vector<bool>    marks(Count,false);
    marks[0]=true;
    for (int A=0; A<Count; A++) {
        for (int i=1; i<Count; i++){
            if (marks[i]) continue;
            const Mgc::ConvexHull2D::Edge &E = Hull.GetEdge(i);
            if (E.m_aiVertex[0]==CurVert) {
                CurVert = E.m_aiVertex[1];
                vertices.push_back(m_Points[CurVert]);
                marks[i]=true;
                break;
            } else if (E.m_aiVertex[1]==CurVert) {
                CurVert = E.m_aiVertex[0];
                vertices.push_back(m_Points[CurVert]);
                marks[i]=true;
                break;
            }
        }
    }
    vertices.pop_back();

    // edge-collapse
    for (int k=0; k<vertices.size(); k++){
        int k1=k-1, k2=k+1;
        if (k==0) k1=vertices.size()-1;
        if (k==vertices.size()-1) k2=0;
        if (vertices[k].similar(vertices[k1])){ vertices.erase(vertices.begin()+k); k--; continue; }
        Fvector2 e1,e2;
        e1.sub(vertices[k1],vertices[k]); e1.norm();
        e2.sub(vertices[k2],vertices[k]); e2.norm();
        if ((1-fabsf(e1.dot(e2)))<=EPS){ vertices.erase(vertices.begin()+k); k--; continue; }
    }

    if (vertices.size()>2){
		m_Points = vertices;
        m_SelPoints.clear();
    }
}

bool COccluder::GetBox( Fbox& box ){
	UpdatePoints3D();
	box.invalidate();
    for(FvectorIt it=m_3DPoints.m_Points.begin(); it!=m_3DPoints.m_Points.end(); it++)
		box.modify(*it);
	return true;
}

void COccluder::Render( Fmatrix& parent, ERenderPriority flag ){
	FLvertexVec V;
	UI->Device.SetRS(D3DRENDERSTATE_FILLMODE,D3DFILL_SOLID);
	if (flag==rpAlphaNormal){
		DWORD C=RGBA_MAKE ( 128, 255, 128, Selected()?BYTE(255*0.3f):BYTE(255*0.15f) );
        // draw plane
        DU::DrawPlane(m_vCenter,m_vPlaneSize,m_vRotate,RGBA_MAKE(128,255,128,Selected()?BYTE(255*0.15f):BYTE(255*0.07f)),false,false,0);
        UpdatePoints3D();
        // draw convex plane
        V.resize(m_3DPoints.m_Points.size());
        FLvertexIt l_it=V.begin();
        for(FvectorIt it=m_3DPoints.m_Points.begin(); it!=m_3DPoints.m_Points.end(); it++,l_it++) l_it->set(*it,C);
        UI->Device.SetRS(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
        UI->Device.DP( D3DPT_TRIANGLEFAN,FVF::F_L, V.begin(), V.size());
        UI->Device.SetRS(D3DRENDERSTATE_CULLMODE,D3DCULL_CCW);
    }
    if (flag==rpNormal){
        if(Selected()){
            Fbox bb;
            if (GetBox(bb)){
                DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
                DU::DrawSelectionBox(bb,&clr);
            }
        }
        UpdatePoints3D();
        DWORD CP=RGBA_MAKE( 255, 0,   0, 255 );
        DWORD CB=RGBA_MAKE( 0,   196, 0, Selected()?BYTE(255*1.0f):BYTE(255*0.6f) );
        // edges
        V.resize(m_3DPoints.m_Points.size());
        FLvertexIt l_it=V.begin();
        for(FvectorIt it=m_3DPoints.m_Points.begin(); it!=m_3DPoints.m_Points.end(); it++,l_it++) l_it->set(*it,CB);
        V.push_back(V.front());
        UI->Device.DP( D3DPT_LINESTRIP,FVF::F_L, V.begin(), V.size());
        // points
        for(it=m_3DPoints.m_Points.begin(); it!=m_3DPoints.m_Points.end(); it++)
            DU::DrawCross(*it,0.025f,0.025f,0.025f,0.025f,0.025f,0.025f,CP,false);
        // sel points
        if (Selected()){
            Fvector S;
            S.set(OCCLUDER_POINT_SEL_DIST,OCCLUDER_POINT_SEL_DIST,OCCLUDER_POINT_SEL_DIST);
            for (INTIt it=m_SelPoints.begin(); it!=m_SelPoints.end(); it++)
                DU::DrawSelectionBox(m_3DPoints.m_Points[*it],S);
        }
    }
    UI->Device.SetRS(D3DRENDERSTATE_FILLMODE,UI->dwRenderFillMode);
}

void COccluder::UpdatePoints3D(){
	if (m_3DPoints.dwFrame==UI->Device.m_Statistic.dwTotalFrame) return;
    VERIFY(m_Points.size()>2);
    // update transform matrix
    Fmatrix	                M,mScale,mTranslate,mRotate;
    mRotate.setHPB			(m_vRotate.y, m_vRotate.x, m_vRotate.z);
    mScale.scale			(m_vPlaneSize.x,1.f,m_vPlaneSize.y);
    mTranslate.translate	(m_vCenter);
    M.mul			        (mTranslate,mRotate);
    M.mul			        (mScale);
    m_3DPoints.dwFrame		= UI->Device.m_Statistic.dwTotalFrame;
    m_3DPoints.m_Points.resize(m_Points.size());
    FvectorIt lst_it		= m_3DPoints.m_Points.begin();
    Fvector					p;
    for (Fvector2It it=m_Points.begin(); it!=m_Points.end(); it++,lst_it++){
        p.set				(it->x,0.f,it->y);
        M.transform			(*lst_it,p);
    }
}

bool COccluder::FrustumPick(const CFrustum& frustum, const Fmatrix& parent){
	UpdatePoints3D();
	sPoly s(m_3DPoints.m_Points.begin(),m_3DPoints.m_Points.size());
	if (frustum.testPoly(s)) return true;
    return false;
}

bool COccluder::RayPick(float& distance, Fvector& start, Fvector& direction, Fmatrix& parent, SRayPickInfo* pinf){
    float range;
    bool bPick=false;

    UpdatePoints3D();
    Fvector p[3];
    p[0].set(m_3DPoints.m_Points.front());
    for(FvectorIt it=m_3DPoints.m_Points.begin()+1; it!=m_3DPoints.m_Points.end()-1; it++){
        p[1].set(*it);
        p[2].set(*(it+1));
        range=UI->ZFar();
        if (RAPID::TestRayTri2(start,direction,p,range)){
            if ((range>=0)&&(range<distance)){
                distance=range;
                bPick=true;
            }
        }
    }
    return bPick;
}

bool COccluder::SelectPoint(Fvector& start, Fvector& direction, bool bLeaveSel){
	float distance=flt_max;
    UpdatePoints3D();
	if (!bLeaveSel) m_SelPoints.clear();
    if(RayPick(distance,start, direction,precalc_identity,0)){
        Fvector I;
        I.direct(start,direction,distance);
        TfraOccluder* F = (TfraOccluder*)UI->m_Tools->GetFrame();
        VERIFY(F);
        // select point
        float dist=flt_max;
        int pt_i=-1;
        for(FvectorIt it=m_3DPoints.m_Points.begin(); it!=m_3DPoints.m_Points.end(); it++){
            float d = it->distance_to(I);
            if ((d<=OCCLUDER_POINT_SEL_DIST)&&(d<dist)){
                dist = d;
                pt_i = it-m_3DPoints.m_Points.begin();
            }
        }
        if (pt_i!=-1){
        	m_SelPoints.push_back(pt_i);
            return true;
        }
    }
	return false;
}

bool COccluder::SelectPoint(const CFrustum& frustum, bool bLeaveSel){
	bool bRes = false;
    UpdatePoints3D();
	if (!bLeaveSel) m_SelPoints.clear();
	for(FvectorIt it=m_3DPoints.m_Points.begin(); it!=m_3DPoints.m_Points.end(); it++){
    	if (frustum.testSphere(*it,OCCLUDER_POINT_SEL_DIST)){
			m_SelPoints.push_back(it-m_3DPoints.m_Points.begin());
            bRes = true;
        }
    }
    return bRes;
}

void COccluder::Move( Fvector& amount ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    m_vCenter.add(amount);
}

void COccluder::Rotate( Fvector& center, Fvector& axis, float angle ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
	Fmatrix m;
	m.rotation(axis, -angle);

	m_vCenter.sub( center );
    m.transform_tiny(m_vCenter);
	m_vCenter.add( center );

    m_vRotate.direct(m_vRotate,axis,axis.z?-angle:angle);
}

void COccluder::LocalRotate( Fvector& axis, float angle ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    m_vRotate.direct(m_vRotate,axis,angle);
}

void COccluder::Scale( Fvector& center, Fvector& amount ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
	m_vPlaneSize.x+=amount.x;
	m_vPlaneSize.y+=amount.z;
	if (m_vPlaneSize.x<EPS) m_vPlaneSize.x=EPS;
	if (m_vPlaneSize.y<EPS) m_vPlaneSize.y=EPS;

	Fmatrix m;
    Fvector S;
    S.add(amount,1.f);
	m.scale( S );
	m_vCenter.sub( center );
	m.transform_tiny(m_vCenter);
	m_vCenter.add( center );
}

void COccluder::LocalScale( Fvector& amount ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
	m_vPlaneSize.x+=amount.x;
	m_vPlaneSize.y+=amount.z;
	if (m_vPlaneSize.x<EPS) m_vPlaneSize.x=EPS;
	if (m_vPlaneSize.y<EPS) m_vPlaneSize.y=EPS;
}

//----------------------------------------------------

bool COccluder::Load(CStream& F){
	DWORD version = 0;

    R_ASSERT(F.ReadChunk(OCCLUDER_CHUNK_VERSION,&version));
    if( version!=OCCLUDER_VERSION ){
        Log->DlgMsg( mtError, "Occluder: Unsupported version.");
        return false;
    }

	SceneObject::Load(F);

    R_ASSERT(F.FindChunk(OCCLUDER_CHUNK_TRANSFORM));
    F.Rvector		(m_vRotate);
    F.Rvector		(m_vCenter);
    F.Read			(&m_vPlaneSize,sizeof(m_vPlaneSize));

    R_ASSERT(F.FindChunk(OCCLUDER_CHUNK_VERTICES));
    m_Points.resize	(F.Rdword());
	F.Read			(m_Points.begin(), m_Points.size()*sizeof(Fvector2) );

    R_ASSERT(F.FindChunk(OCCLUDER_CHUNK_SEL_VERTICES));
    m_SelPoints.resize(F.Rdword());
	F.Read			(m_SelPoints.begin(), m_SelPoints.size()*sizeof(int) );

    return true;
}

void COccluder::Save(CFS_Base& F){
	SceneObject::Save(F);

	F.open_chunk	(OCCLUDER_CHUNK_VERSION);
	F.Wword			(OCCLUDER_VERSION);
	F.close_chunk	();

	F.open_chunk	(OCCLUDER_CHUNK_TRANSFORM);
    F.Wvector		(m_vRotate);
    F.Wvector		(m_vCenter);
    F.write			(&m_vPlaneSize,sizeof(m_vPlaneSize));
	F.close_chunk	();

	F.open_chunk	(OCCLUDER_CHUNK_VERTICES);
	F.Wdword		(m_Points.size());
	F.write			(m_Points.begin(),m_Points.size()*sizeof(Fvector2));
	F.close_chunk	();

	F.open_chunk	(OCCLUDER_CHUNK_SEL_VERTICES);
	F.Wdword		(m_SelPoints.size());
	F.write			(m_SelPoints.begin(),m_SelPoints.size()*sizeof(int));
	F.close_chunk	();
}
//----------------------------------------------------


