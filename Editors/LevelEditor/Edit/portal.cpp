//----------------------------------------------------
// file: Portal.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "Portal.h"
#include "Frustum.h"
#include "Scene.h"
#include "cl_intersect.h"
#include "sector.h"
#include "MgcConvexHull2D.h"
#include "MgcAppr3DPlaneFit.h"
#include "bottombar.h"
#include "d3dutils.h"
#include "ui_main.h"
#include "SceneObject.h"

#define PORTAL_VERSION   					0x0010
//------------------------------------------------------------------------------
#define PORTAL_CHUNK_VERSION				0xFA10
#define PORTAL_CHUNK_COLOR					0xFA20
#define PORTAL_CHUNK_SECTOR_FRONT			0xFA30
#define PORTAL_CHUNK_SECTOR_BACK			0xFA40
#define PORTAL_CHUNK_VERTICES				0xFA50
//------------------------------------------------------------------------------

CPortal::CPortal(LPVOID data, LPCSTR name):CCustomObject(data,name){
	Construct(data);
}
//------------------------------------------------------------------------------

void CPortal::Construct(LPVOID data){
	ClassID		= OBJCLASS_PORTAL;
	m_Center.set(0,0,0);
	m_SectorFront=0;
	m_SectorBack=0;
}
//------------------------------------------------------------------------------

CPortal::~CPortal(){
}
//------------------------------------------------------------------------------

bool CPortal::GetBox( Fbox& box ){
	if( m_Vertices.empty() ){
		box.set(0,0,0, 0,0,0);
		return false;
    }
	box.set( m_Vertices[0], m_Vertices[0] );
	for( FvectorIt pt=m_Vertices.begin()+1; pt!=m_Vertices.end(); pt++)
		box.modify(*pt);
	return true;
}
//------------------------------------------------------------------------------

void CPortal::Render(int priority, bool strictB2F){
	if ((1==priority)&&(false==strictB2F)){
        FvectorVec& src 	= m_SimplifyVertices;//(fraBottomBar->miDrawPortalSimpleModel->Checked)?m_SimplifyVertices:m_Vertices;
        if (src.size()<2) 	return;
        u32 				i;
        FvectorVec	 		V;
        V.resize			(src.size()+2);
        V[0].set			(m_Center);
        for(i=0; i<src.size(); i++) V[i+1].set(src[i]);
        V[V.size()-1].set	(src[0]);
		// render portal tris
        Fcolor 				col;
        Device.SetShader	(Device.m_WireShader);
        // front
		if (m_SectorFront){
			col.set			(m_SectorFront->sector_color);
	        if (!Selected())col.mul_rgb(0.7f);
		    Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);
    	    DU::DrawPrimitiveL	(D3DPT_TRIANGLEFAN, V.size()-2, V.begin(), V.size(), col.get(), true, false);
		    Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);
        }
        // back
		if (m_SectorBack){
			col.set			(m_SectorBack->sector_color);
	        if (!Selected())col.mul_rgb(0.7f);
		    Device.SetRS(D3DRS_CULLMODE,D3DCULL_CW);
    	    DU::DrawPrimitiveL	(D3DPT_TRIANGLEFAN, V.size()-2, V.begin(), V.size(), col.get(), true, false);
		    Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);
        }
		col.set				(1.f,1.f,1.f,1.f);
		Device.RenderNearer(0.0002);
        if (!Selected())	col.mul_rgb(0.5f);
    	// render portal edges
        FvectorVec& src_ln 	= (fraBottomBar->miDrawPortalSimpleModel->Checked)?m_SimplifyVertices:m_Vertices;
        Device.SetShader	(Device.m_WireShader);
        DU::DrawPrimitiveL	(D3DPT_LINESTRIP, src_ln.size(), src_ln.begin(), src_ln.size(), col.get(), true, true);
        Device.ResetNearer	();
        DU::DrawFaceNormal	(m_Center,m_Normal,1,0xFFFFFFFF);
        DU::DrawFaceNormal	(m_Center,m_Normal,1,0x00000000);
/*		for (int k=0; k<1000; k++){
        	Fvector dir;
            dir.random_dir(m_Normal,deg2rad(45.f));
	        DU::DrawFaceNormal	(m_Center,dir,1,0x00FF0000);
    	}
*/
   	}
}
//------------------------------------------------------------------------------

void CPortal::Move( Fvector& amount ){
// internal use only!!!
	FvectorIt v_it;
	for(v_it=m_Vertices.begin(); v_it!=m_Vertices.end(); v_it++)
    	v_it->add(amount);
	for(v_it=m_SimplifyVertices.begin(); v_it!=m_SimplifyVertices.end(); v_it++)
    	v_it->add(amount);
    m_Center.add(amount);
}
//------------------------------------------------------------------------------

bool CPortal::FrustumPick(const CFrustum& frustum){
	if (frustum.testPolyInside(m_Vertices.begin(),m_Vertices.size())) return true;
    return false;
}
//------------------------------------------------------------------------------

bool CPortal::RayPick(float& distance, const Fvector& start, const Fvector& direction, SRayPickInfo* pinf){
	Fvector p[3];
    float range;
    bool bPick=false;
	p[0].set(m_Center);
	FvectorVec& src=(fraBottomBar->miDrawPortalSimpleModel->Checked)?m_SimplifyVertices:m_Vertices;
    for(FvectorIt it=src.begin(); it!=src.end(); it++){
		p[1].set(*it);
		p[2].set(((it+1)==src.end())?src.front():*(it+1));
        range=UI.ZFar();
		if (CDB::TestRayTri2(start,direction,p,range)){
        	if ((range>=0)&&(range<distance)){
            	distance=range;
	            bPick=true;
            }
        }
    }
    return bPick;
}
//------------------------------------------------------------------------------

bool CPortal::Update(bool bLoadMode){
	Fbox box;
    GetBox(box);
    box.getsphere(m_Center,m_Radius);
// check normal
//------------------------------------------------------------------------------
    m_Normal.set(0,0,0);

    R_ASSERT(!m_Vertices.empty());
    R_ASSERT(m_Vertices.size()>=3);

    for (u32 i=0; i<m_Vertices.size()-1; i++){
    	Fvector temp;
        temp.mknormal(m_Center,m_Vertices[i],m_Vertices[i+1]);
    	m_Normal.add(temp);
    }
    float m=m_Normal.magnitude();
    if (fabsf(m)<=EPS_S){
		Scene.m_CompilerErrors.AppendFace(m_Vertices[0],m_Vertices[1],m_Vertices[2]);
    	ELog.Msg(mtError,"Portal: Degenerate portal found.");
        SetValid(false);
		return false;
    }
    m_Normal.div(m);

    // simplify portal
	Simplify();

    if (!bLoadMode){
        map<CSector*,int> A,B;
        Fvector SF_dir, SB_dir;
        Fvector InvNorm;
        InvNorm.invert(m_Normal);
        for (int k=0; k<100; k++){
	        SF_dir.random_dir(m_Normal,PI_DIV_4);
	        SB_dir.random_dir(InvNorm,PI_DIV_4);
	        if (m_SectorFront->RayPick(1000,m_Center,SF_dir)) 	A[m_SectorFront] += 1;
	        if (m_SectorBack->RayPick(1000,m_Center,SB_dir))	A[m_SectorBack] += 1;
	        if (m_SectorFront->RayPick(1000,m_Center,SB_dir)) 	B[m_SectorFront] += 1;
	        if (m_SectorBack->RayPick(1000,m_Center,SF_dir))	B[m_SectorBack] += 1;
        }
/*
        int a_f = A[m_SectorFront];
        int a_b = A[m_SectorBack];
        int b_f = B[m_SectorFront];
        int b_b = B[m_SectorBack];
        Log("----------",Name);
        Log("a_f ",a_f);
        Log("a_b ",a_b);
        Log("b_f ",b_f);
        Log("b_b ",b_b);
*/
        int a = A[m_SectorFront]+A[m_SectorBack];
        int b = B[m_SectorFront]+B[m_SectorBack];
        if (a>b);
        else if (a<b) InvertOrientation();
        else ELog.Msg(mtError, "Check portal orientation: '%s'",Name);
/*
        map<CSector*,int> counters;
        for (u32 i=0; i<m_Vertices.size()-1; i++){
            Fbox bb; bb.set(m_Vertices[i],m_Vertices[i]); bb.grow(0.01f);
            SBoxPickInfoVec result;
	        if (Scene.BoxPick(bb,result)!=2) continue;
            CSector* s1 = PortalUtils.FindSector(result[0].s_obj,result[0].e_mesh);
            CSector* s2 = PortalUtils.FindSector(result[1].s_obj,result[1].e_mesh);
            if (s1==s2) continue;
            if (((s1==m_SectorFront)&&(s2==m_SectorBack))||((s2==m_SectorFront)&&(s1==m_SectorBack))){
                for (SBoxPickInfoIt it=result.begin(); it!=result.end(); it++){
                    for (int k=0; k<it->inf.size(); k++){
                        CDB::RESULT& r = it->inf[k];
                        Fvector v[3];
                        it->s_obj->GetFaceWorld(it->e_mesh,r.id,v);
                        for (int i=0; i<3; i++){
                        	for (int y=0; y<m_Vertices.size(); y++)
                            	if (m_Vertices[y].similar(v[i])) continue;
                        	if (P.classify(v[i])>0.f)	counters[m_SectorFront] += 1;
                        	else						counters[m_SectorBack] += 1;
                        }
                    }
                }
            }
        }
        if (counters[m_SectorFront]<=counters[m_SectorBack])
        	InvertOrientation();
//    	m_Normal
//    	m_Center
/*
        float step=0.05f;
        float delta_step=0.05f;

        Fvector SF_point, SB_point;
        vector<CSector*> front;
        vector<CSector*> back;

        while(step<=1.f){
            // test inverted
            SF_point.mad(m_Center,m_Normal,step);
            SB_point.mad(m_Center,m_Normal,-step);

            front.clear();
            back.clear();

            if (m_SectorFront->Intersect	(SF_point,0)==fvFully) front.push_back(m_SectorFront);
            if (m_SectorBack->Intersect		(SF_point,0)==fvFully) front.push_back(m_SectorBack);
            if (m_SectorFront->Intersect	(SB_point,0)==fvFully) back.push_back(m_SectorFront);
            if (m_SectorBack->Intersect		(SB_point,0)==fvFully) back.push_back(m_SectorBack);

            if ((front.empty()||back.empty()) || ((front.size()==2)&&(back.size()==2)))
                step+=delta_step;
            else break;
        }
        CSector *SF=0,*SB=0;
        if ((front.size()==1)&&(back.size()==1)) 		{ SF = front[0]; SB = back[0]; }
        else  if ((front.size()==2)&&(back.size()==1)) 	{ SB = back[0];  SF = front[(SB==front[0])?1:0]; }
        else  if ((front.size()==1)&&(back.size()==2)) 	{ SF = front[0]; SB = back [(SF==back[0])?1:0]; }

        if (!(SF && (SF!=SB))){
            ELog.Msg(mtError, "Check portal orientation: '%s'",Name);
        }else{
            if (SF!=m_SectorFront) InvertOrientation();
        }
*/
    }

    return true;
}
//------------------------------------------------------------------------------

void CPortal::InvertOrientation(){
    reverse(m_Vertices.begin(),m_Vertices.end());
    reverse(m_SimplifyVertices.begin(),m_SimplifyVertices.end());
    m_Normal.invert();
    UI.RedrawScene();
    Scene.UndoSave();
}
//------------------------------------------------------------------------------

double tri_area(Fvector2 P0, Fvector2 P1, Fvector2 P2)
{
	double	e1 = P0.distance_to(P1);
	double	e2 = P1.distance_to(P2);
	double	e3 = P2.distance_to(P0);

	double	p  = (e1+e2+e3)/2.0;
	return	sqrt(p*(p-e1)*(p-e2)*(p-e3));
}
//------------------------------------------------------------------------------

bool Isect2DLvs2DL (Fvector2& P1, Fvector2& P2, Fvector2& P3, Fvector2& P4, Fvector2& P)
{
    double a1, a2, b1, b2, c1, c2;	/* Coefficients of line eqns. */
    double r1, r2, r3, r4;			/* 'Sign' values */
    double denom, num;				/* Intermediate values */

									/* Compute a1, b1, c1, where line joining points 1 and 2
									* is "a1 x  +  b1 y  +  c1  =  0".	*/

    a1 = P2.y - P1.y;
    b1 = P1.x - P2.x;
    c1 = P2.x * P1.y - P1.x * P2.y;

    /* Compute a2, b2, c2 */

    a2 = P4.y - P3.y;
    b2 = P3.x - P4.x;
    c2 = P4.x * P3.y - P3.x * P4.y;

    /* Lines intersect: compute intersection point.
	*/

    denom = a1 * b2 - a2 * b1;
    if ( fabs(denom) < EPS ) return false;

    num = b1 * c2 - b2 * c1;
    P.x = float(num / denom);

    num = a2 * c1 - a1 * c2;
    P.y = float(num / denom);

    return true;
}
//------------------------------------------------------------------------------

void SimplifyVertices(Fvector2Vec& vertices)
{
    // 1. Select smallest edge
    double  min_size = dbl_max;
    int     edge_id  = -1;
    Fvector2  P;
    for (u32 I=0; I<vertices.size(); I++){
        // 2. Build two lines (1[L1a,L1b], 2[L2a,L2b]) and search point of intersection
        int L1a = I-1; if (L1a<0) L1a = vertices.size()-1;
        int L1b = I;
        int L2a = I+1; if (L2a>=int(vertices.size())) L2a -= vertices.size();
        int L2b = I+2; if (L2b>=int(vertices.size())) L2b -= vertices.size();

        Fvector2 P_Test;
        if (!Isect2DLvs2DL(vertices[L1a],vertices[L1b],vertices[L2b],vertices[L2a],P_Test)) continue;

        double   area     = tri_area(vertices[L1b],P_Test,vertices[L2a]); //*(vertices[L1b].dist(vertices[L2a]));

        if (area<min_size) {
            edge_id     = I;
            min_size    = area;
            P           = P_Test;
        }
    }

    // 3. Remove L1b & L2b - insert P
    if (edge_id<0) return;

    int L1b = edge_id;
    int L2a = edge_id+1; if (L2a>=int(vertices.size())) L2a -= vertices.size();
    vertices[L1b] = P;
    vertices.erase(vertices.begin()+L2a);
}
//------------------------------------------------------------------------------

void CPortal::Simplify()
{
	Fvector rkOffset;
    Fvector rkNormal;
    // compute plane
    Fplane P;
	Mgc::OrthogonalPlaneFit(m_Vertices.size(), (Mgc::Vector3*)m_Vertices.begin(), (Mgc::Vector3&)rkOffset, (Mgc::Vector3&)rkNormal);
    P.build(rkOffset, rkNormal);
    // project points
	Fmatrix		mView;
    Fvector		at,from,up,right,y;
	from.set	(rkOffset);
	at.sub		(from,rkNormal);
	y.set		(0,1,0);
	if (fabsf(rkNormal.y)>0.99f) y.set(1,0,0);
	right.crossproduct(y,rkNormal); right.normalize();
	up.crossproduct(rkNormal,right);up.normalize();
	mView.build_camera(from,at,up);

    Fvector2Vec points, vertices;
    Fvector p;
    points.resize(m_Vertices.size());
    for (u32 k=0; k<m_Vertices.size(); k++){
     	mView.transform_tiny(p,m_Vertices[k]);
        points[k].set(p.x,p.y);
    }
    // compute 2D Convex Hull
    Mgc::ConvexHull2D Hull(points.size(),(const Mgc::Vector2*)points.begin());
//    Hull.ByDivideAndConquer();
    Hull.ByIncremental();
    Hull.RemoveCollinear();
	int Count   	= Hull.GetQuantity();
	if (Count<=0) return;
    const int* indices 	= Hull.GetIndices();
    for (int ind_i=0; ind_i<Count; ind_i++){
    	vertices.push_back(points[indices[ind_i]]);
    }
//    R_ASSERT2(0,"Go to ALEXMX and say: ''Test portal simplifier. Please!''");
/*
    int* indices 	= Hull.GetIndices();
    int CurVert 	= indices[0];
	vertices.push_back(points[CurVert]);
    CurVert 		= indices[1];
    vertices.push_back(points[CurVert]);

    vector<bool>    marks(Count,false);
    marks[0]=true;
    for (int A=0; A<Count; A++) {
        for (int i=1; i<Count; i++){
            if (marks[i]) continue;
            const Mgc::ConvexHull2D::Edge &E = Hull.GetEdge(i);
            if (E.m_aiVertex[0]==CurVert) {
                CurVert = E.m_aiVertex[1];
                vertices.push_back(points[CurVert]);
                marks[i]=true;
                break;
            } else if (E.m_aiVertex[1]==CurVert) {
                CurVert = E.m_aiVertex[0];
                vertices.push_back(points[CurVert]);
                marks[i]=true;
                break;
            }
        }
    }
    vertices.pop_back();

    // edge-collapse
    for (k=0; k<vertices.size(); k++){
        int k1=k-1, k2=k+1;
        if (k==0) k1=vertices.size()-1;
        if (k==vertices.size()-1) k2=0;
        if (vertices[k].similar(vertices[k1])){ vertices.erase(vertices.begin()+k); k--; continue; }
        Fvector2 e1,e2;
        e1.sub(vertices[k1],vertices[k]); e1.norm();
        e2.sub(vertices[k2],vertices[k]); e2.norm();
        if ((1-fabsf(e1.dot(e2)))<=EPS){ vertices.erase(vertices.begin()+k); k--; continue; }
    }
*/
    // simplify
    while(vertices.size()>XR_MAX_PORTAL_VERTS){
    	int f_cnt=vertices.size();
    	SimplifyVertices(vertices);
        if (f_cnt==int(vertices.size())){ SetValid(false); return; }
    }

    if (vertices.size()<=2){ SetValid(false); return; }
    else					SetValid(true);

    // convert to 3D & check portal normal
    Fvector center, temp, norm;
    center.set(0,0,0);
    mView.invert();
    m_SimplifyVertices.resize(vertices.size());
    for (k=0; k<vertices.size(); k++){
    	p.set(vertices[k].x,vertices[k].y,0.f);
     	mView.transform_tiny(m_SimplifyVertices[k],p);
        center.add(m_SimplifyVertices[k]);
    }
    center.div(vertices.size());

    norm.set(0,0,0);
    for (k=0; k<m_SimplifyVertices.size()-1; k++){
        temp.mknormal(center,m_SimplifyVertices[k],m_SimplifyVertices[k+1]);
    	norm.add(temp);
    }
    norm.div(m_SimplifyVertices.size());
    float m=norm.magnitude();    VERIFY(fabsf(m)>EPS);    norm.div(m);

    if (norm.dotproduct(m_Normal)<0){
    	reverse(m_SimplifyVertices.begin(), m_SimplifyVertices.end());
	    m_Normal.invert(norm);
    }else{
	    m_Normal.set(norm);
    }
}
//------------------------------------------------------------------------------

bool CPortal::Load(IReader& F){
	u32 version = 0;

    char buf[64];
    R_ASSERT(F.r_chunk(PORTAL_CHUNK_VERSION,&version));
    if( version!=PORTAL_VERSION ){
        ELog.Msg( mtError, "CPortal: Unsupported version.");
        return false;
    }

	CCustomObject::Load(F);

	if (F.find_chunk	(PORTAL_CHUNK_SECTOR_FRONT)){
        F.r_stringZ	(buf);
        m_SectorFront=(CSector*)Scene.FindObjectByName(buf,OBJCLASS_SECTOR);
    }
	if (F.find_chunk	(PORTAL_CHUNK_SECTOR_BACK)){
        F.r_stringZ	(buf);
		m_SectorBack=(CSector*)Scene.FindObjectByName(buf,OBJCLASS_SECTOR);
    }

    if (!m_SectorBack||!m_SectorFront){
        ELog.Msg( mtError, "Portal: Can't find required sectors.\nObject '%s' can't load.", Name);
    	return false;
    }

    R_ASSERT(F.find_chunk(PORTAL_CHUNK_VERTICES));
	m_Vertices.resize(F.r_u16());
	F.r				(m_Vertices.begin(), m_Vertices.size()*sizeof(Fvector));

    if (m_Vertices.size()<3){
        ELog.Msg( mtError, "Portal: '%s' can't create.\nInvalid portal. (m_Vertices.size()<3)", Name);
    	return false;
    }

    Update(true);
    return true;
}
//------------------------------------------------------------------------------

void CPortal::Save(IWriter& F){
	CCustomObject::Save(F);

	F.open_chunk	(PORTAL_CHUNK_VERSION);
	F.w_u16			(PORTAL_VERSION);
	F.close_chunk	();

    if (m_SectorFront){
		F.open_chunk	(PORTAL_CHUNK_SECTOR_FRONT);
        F.w_stringZ		(m_SectorFront->Name);
		F.close_chunk	();
    }

	if (m_SectorBack){
		F.open_chunk	(PORTAL_CHUNK_SECTOR_BACK);
        F.w_stringZ		(m_SectorBack->Name);
		F.close_chunk	();
    }

	F.open_chunk	(PORTAL_CHUNK_VERTICES);
    F.w_u16			(m_Vertices.size());
    F.w				(m_Vertices.begin(),m_Vertices.size()*sizeof(Fvector));
	F.close_chunk	();
}
//------------------------------------------------------------------------------

bool CPortal::GetSummaryInfo(SSceneSummary* inf)
{
	inf->portal_cnt++;
	return true;
}

