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
        DWORD 				i;
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
    sPoly s(m_Vertices.begin(),m_Vertices.size());
	if (frustum.testPoly(s)) return true;
    return false;
}
//------------------------------------------------------------------------------

bool CPortal::RayPick(float& distance, Fvector& start, Fvector& direction, SRayPickInfo* pinf){
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

    for (DWORD i=0; i<m_Vertices.size()-1; i++){
    	Fvector temp;
        temp.mknormal(m_Center,m_Vertices[i],m_Vertices[i+1]);
    	m_Normal.add(temp);
    }
    float m=m_Normal.magnitude();
    if (fabsf(m)<=EPS){
    	ELog.DlgMsg(mtError,"Portal: Some error found at pos: [%3.2f,%3.2f,%3.2f].",m_Vertices[0].x,m_Vertices[0].x,m_Vertices[0].x);
        SetValid(false);
		return false;
    }
    m_Normal.div(m);

    if (!bLoadMode){
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
    }

	Simplify();
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
    for (DWORD I=0; I<vertices.size(); I++){
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

void CPortal::Simplify(){
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
    for (DWORD k=0; k<m_Vertices.size(); k++){
     	mView.transform_tiny(p,m_Vertices[k]);
        points[k].set(p.x,p.y);
    }
    // compute 2D Convex Hull
    Mgc::ConvexHull2D Hull(points.size(),(const Mgc::Vector2*)points.begin());
	int Count   = Hull.GetEdgeQuantity();
	if (Count==0) return;
    int CurVert = Hull.GetEdge(0).m_aiVertex[0];
	vertices.push_back(points[CurVert]);
    CurVert = Hull.GetEdge(0).m_aiVertex[1];
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

bool CPortal::Load(CStream& F){
	DWORD version = 0;

    char buf[64];
    R_ASSERT(F.ReadChunk(PORTAL_CHUNK_VERSION,&version));
    if( version!=PORTAL_VERSION ){
        ELog.Msg( mtError, "CPortal: Unsupported version.");
        return false;
    }

	CCustomObject::Load(F);

	if (F.FindChunk	(PORTAL_CHUNK_SECTOR_FRONT)){
        F.RstringZ	(buf);
        m_SectorFront=(CSector*)Scene.FindObjectByName(buf,OBJCLASS_SECTOR);
    }
	if (F.FindChunk	(PORTAL_CHUNK_SECTOR_BACK)){
        F.RstringZ	(buf);
		m_SectorBack=(CSector*)Scene.FindObjectByName(buf,OBJCLASS_SECTOR);
    }

    if (!m_SectorBack||!m_SectorFront){
        ELog.Msg( mtError, "Portal: Can't find required sectors.\nObject '%s' can't load.", Name);
    	return false;
    }

    R_ASSERT(F.FindChunk(PORTAL_CHUNK_VERTICES));
	m_Vertices.resize(F.Rword());
	F.Read			(m_Vertices.begin(), m_Vertices.size()*sizeof(Fvector));

    if (m_Vertices.size()<3){
        ELog.Msg( mtError, "Portal: '%s' can't create.\nInvalid portal. (m_Vertices.size()<3)", Name);
    	return false;
    }

    Update(true);
    return true;
}
//------------------------------------------------------------------------------

void CPortal::Save(CFS_Base& F){
	CCustomObject::Save(F);

	F.open_chunk	(PORTAL_CHUNK_VERSION);
	F.Wword			(PORTAL_VERSION);
	F.close_chunk	();

    if (m_SectorFront){
		F.open_chunk	(PORTAL_CHUNK_SECTOR_FRONT);
        F.WstringZ		(m_SectorFront->Name);
		F.close_chunk	();
    }

	if (m_SectorBack){
		F.open_chunk	(PORTAL_CHUNK_SECTOR_BACK);
        F.WstringZ		(m_SectorBack->Name);
		F.close_chunk	();
    }

	F.open_chunk	(PORTAL_CHUNK_VERTICES);
    F.Wword			(m_Vertices.size());
    F.write			(m_Vertices.begin(),m_Vertices.size()*sizeof(Fvector));
	F.close_chunk	();
}
//------------------------------------------------------------------------------

