//----------------------------------------------------
// file: Sector.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "Sector.h"
#include "SceneClassList.h"
#include "UI_Main.h"
#include "Frustum.h"
#include "EditMesh.h"
#include "D3DUtils.h"
#include "EditObject.h"
#include "Scene.h"
#include "Texture.h"
#include "Statistic.h"
#include "cl_intersect.h"
#include "bottombar.h"
#include "portal.h"
#include "portalutils.h"
#include "cl_collector.h"
#include "MgcConvexHull3D.h"

#define SECTOR_VERSION   					0x0010
//----------------------------------------------------
#define SECTOR_CHUNK_VERSION				0xF010
#define SECTOR_CHUNK_COLOR					0xF020
#define SECTOR_CHUNK_PRIVATE				0xF025
#define SECTOR_CHUNK_ITEMS					0xF030
#define 	SECTOR_CHUNK_ITEM_OBJECT		0xF031
#define 	SECTOR_CHUNK_ITEM_FACES			0xF032
#define SECTOR_CHUNK_CHULL_DATA				0xF040
#define SECTOR_CHUNK_CHULL_DATA2			0xF041
//----------------------------------------------------
CSectorItem::CSectorItem(){
	object=NULL;
    mesh=NULL;
}
CSectorItem::CSectorItem(CEditObject* o, CEditMesh* m){
	object=o;
    mesh=m;
    mask.resize(m->m_Faces.size());
    fill(mask.begin(),mask.end(),false);
}
void CSectorItem::GetTransform(Fmatrix& parent){
	object->GetFullTransformToWorld(parent);
}
bool CSectorItem::IsItem(const char* O, const char* M){
	return (0==strcmp(O,object->GetName()))&&(0==strcmp(M,mesh->GetName()));
}
//------------------------------------------------------------------------------
bool CSector::IsFaceUsed(CEditObject* O, CEditMesh* M, int f_id){
	SItemIt s_it;
	if (FindSectorItem(O, M, s_it)){
    	return s_it->mask[f_id];
    }
    return false;
}
//----------------------------------------------------
class FindMaskFaces{   
	BOOLVec& ref;
public:
    FindMaskFaces(BOOLVec& a):ref(a) {};
    bool operator() (DWORD &f) { return ref[f]; }
};
void CSector::TestUsedFaces(CEditObject* O, CEditMesh* M, DWORDVec& fl){
	SItemIt s_it;
	if (FindSectorItem(O, M, s_it)){
    	DWORDIt r_end = remove_if(fl.begin(), fl.end(), FindMaskFaces(s_it->mask));
        fl.erase(r_end,fl.end());
    }
}
//----------------------------------------------------
CSector::CSector( char *name ):SceneObject(){
	Construct();
	strcpy( m_Name, name );
}

CSector::CSector():SceneObject(){
	Construct();
}

void CSector::Construct(){
	m_ClassID = OBJCLASS_SECTOR;
    sector_color.set(1,1,1,0);
	m_bDefault=false;
    m_bNeedUpdateCHull=false;
	sector_num=-1;
	m_bHasLoadError = false;
}

CSector::~CSector(){
//	OnDestroy();
}

bool CSector::FindSectorItem(const char* O, const char* M, SItemIt& it){
	for (it=sector_items.begin();it!=sector_items.end();it++)
    	if ((*it).IsItem(O,M)) return true;
    return false;
}

bool CSector::FindSectorItem(CEditObject* o, CEditMesh* m, SItemIt& it){
	for (it=sector_items.begin();it!=sector_items.end();it++)
    	if ((*it).IsItem(o,m)) return true;
    return false;
}

void CSector::AddFace	(CEditObject* O, CEditMesh* M, int f_id){
	SItemIt it;
    if (FindSectorItem(O, M, it)) (*it).Add(f_id);
    else{
     	sector_items.push_back(CSectorItem(O, M));
        sector_items.back().Add(f_id);
    }
}

void CSector::AddFaces	(CEditObject* O, CEditMesh* M, DWORDVec& fl){
	SItemIt it;
    if (FindSectorItem(O, M, it)) (*it).Add(fl);
    else{
     	sector_items.push_back(CSectorItem(O, M));
        sector_items.back().Add(fl);
    }
}

void CSector::DelFace	(CEditObject* O, CEditMesh* M, int f_id){
	SItemIt it;
    if (FindSectorItem(O, M, it)) (*it).Del(f_id);
}

void CSector::DelFaces	(CEditObject* O, CEditMesh* M, DWORDVec& fl){
	SItemIt it;
    if (FindSectorItem(O, M, it)) (*it).Del(fl);
}

bool CSector::GetBox( Fbox& box ){
	return false;
}
/*
void CSector::PrepareVertices(){
    Fmatrix parent;
    DWORDList idx;
    m_SectorVertices.clear();
	for (SItemIt s_it=sector_items.begin();s_it!=sector_items.end();s_it++){
    	s_it->GetTransform(parent);
		FvectorList& m_vert=s_it->mesh->m_Points;
        idx.clear();
        for (DWORDIt f_it=s_it->Face_IDs.begin(); f_it!=s_it->Face_IDs.end(); f_it++){
        	st_Face& P=s_it->mesh->m_Faces[*f_it];
            idx.push_back(P.pv[0].pindex);
            idx.push_back(P.pv[1].pindex);
            idx.push_back(P.pv[2].pindex);
        }
        sort(idx.begin(),idx.end());
        DWORDIt idx_end=unique(idx.begin(),idx.end());
        idx.erase(idx_end,idx.end());
        for (DWORDIt i_it=idx.begin(); i_it!=idx.end(); i_it++){
			m_SectorVertices.push_back(m_vert[*i_it]);
            parent.transform_tiny(m_SectorVertices.back());
        }
    }
}
*/
void OptimizeVertices(CHFaceVec& Faces, FvectorVec& vert, FvectorVec& vert_new){
	int iVertexCount = vert.size();

	//caches oldIndex --> newIndex conversion
	int *indexCache = new int[iVertexCount];
	memset(indexCache, -1, sizeof(int)*iVertexCount);
	
	for(DWORD i = 0; i < Faces.size(); i++){
		int v0 = Faces[i].p0;
		int v1 = Faces[i].p1;
		int v2 = Faces[i].p2;
		
		//v0
		int index = indexCache[v0];
		if(index == -1){
        	vert_new.push_back(vert[v0]);
			indexCache[v0] = Faces[i].p0 = vert_new.size() - 1;
		}else{
			Faces[i].p0 = index;
		}
		
		//v1
		index = indexCache[v1];
		if(index == -1){
        	vert_new.push_back(vert[v1]);
			indexCache[v1] = Faces[i].p1 = vert_new.size() - 1;
		}else{
			Faces[i].p1 = index;
		}
		
		//v2
		index = indexCache[v2];
		if(index == -1){
        	vert_new.push_back(vert[v2]);
			indexCache[v2] = Faces[i].p2 = vert_new.size() - 1;
		}else{
			Faces[i].p2 = index;
		}
	}

	_DELETEARRAY(indexCache);
}

void CSector::UpdatePlanes(){
	m_CHSectorPlanes.clear();
    for (DWORD i=0; i<m_CHSectorFaces.size(); i++ ){
        CHFace&		T	= m_CHSectorFaces[i];
        Fvector& 	v0	= m_CHSectorVertices[T.p[0]];
        Fvector& 	v1	= m_CHSectorVertices[T.p[1]];
        Fvector&	v2	= m_CHSectorVertices[T.p[2]];
                
        Fvector t1,t2,n;
        t1.sub(v0,v1);
        t2.sub(v0,v2);
        n.crossproduct(t1,t2);
        if(n.square_magnitude()<=EPS) continue;

        Fplane	P; P.build(v0,v1,v2);
        float R	= P.classify(m_SectorCenter);
        if(R>0) {
            P.build(v2,v1,v0);
            swap(T.p[0],T.p[2]);
        }
                
        // search plane in list
        bool bNeedAppend=true;
        for(PlaneIt p_it=m_CHSectorPlanes.begin(); p_it!=m_CHSectorPlanes.end(); p_it++)
            if (p_it->similar(P,EPS,EPS_L)) { bNeedAppend = false; break; }
                    
        if (bNeedAppend) m_CHSectorPlanes.push_back(P);
    }
}

void CSector::MakeCHull	(){
    m_CHSectorFaces.clear();

    UI->ProgressStart(5,"Making convex hull...");
    //1. (BB)
    Fmatrix parent;
    Fbox bbox; bbox.invalidate();
    for (SItemIt s_it=sector_items.begin();s_it!=sector_items.end();s_it++){
        Fbox bb;
        s_it->GetTransform(parent);
        s_it->mesh->GetBox(bb);
        bb.transform(parent);
        bbox.merge(bb);
    }
	UI->ProgressInc();

    //2. XForm+Pack
    RAPID::CollectorPacked cp(bbox);
    for (s_it=sector_items.begin();s_it!=sector_items.end();s_it++){
        s_it->GetTransform(parent);
        FvectorVec& m_vert=s_it->mesh->m_Points;
        for (DWORDIt f_it=s_it->Face_IDs.begin(); f_it!=s_it->Face_IDs.end(); f_it++){
            Fvector v0, v1, v2;
            st_Face& P=s_it->mesh->m_Faces[*f_it];
            parent.transform_tiny(v0,m_vert[P.pv[0].pindex]);
            parent.transform_tiny(v1,m_vert[P.pv[1].pindex]);
            parent.transform_tiny(v2,m_vert[P.pv[2].pindex]);
            cp.add_face(v0,v1,v2,0,0,0,0,0,0);
        }
    }
	UI->ProgressInc();
    if (cp.getVS()<4){
		UI->ProgressEnd();
    	return;
    }

	Mgc::ConvexHull3D* chull=0;
    try{
	    Mgc::ConvexHull3D::Epsilon()		=0.001f;
    	Mgc::ConvexHull3D::TSize()			=128;
//		Mgc::ConvexHull3D::QuantityFactor()	=64;

        //3. Hull
        chull=new Mgc::ConvexHull3D(cp.getVS(), (const Mgc::Vector3*)cp.getV());
        int err_code=chull->Compute();
        if (err_code==CHULL3D_OK){
	        m_CHSectorFaces.assign((CHFace*)chull->GetTriangles(),(CHFace*)(chull->GetTriangles()+chull->GetTriangleQuantity()));
    	    bbox.set(chull->GetXMin(),chull->GetYMin(),chull->GetZMin(), chull->GetXMax(),chull->GetYMax(),chull->GetZMax());
        	bbox.getcenter(m_SectorCenter);
	        m_SectorRadius=bbox.getradius();
        	_DELETE(chull);
        	UI->ProgressInc();

            //4. Vertices+Faces remapping
            OptimizeVertices(m_CHSectorFaces,cp.getV_Vec(),m_CHSectorVertices);
			UI->ProgressInc();

            //5. Planes
            UpdatePlanes();
			UI->ProgressInc();

            for(FvectorIt it=m_CHSectorVertices.begin(); it!=m_CHSectorVertices.end(); it++)
                for(PlaneIt p_it=m_CHSectorPlanes.begin(); p_it!=m_CHSectorPlanes.end(); p_it++){
                    float D= p_it->classify(*it);
                    if (D>0) p_it->d-=D;
                }
            // CHull updated
            m_bNeedUpdateCHull = false;
        }else{
        	switch(err_code){
			case CHULL3D_INC_TSIZE: Log->Msg(mtError,"Temporary storage exceeded.\n Increase ms_iTSize and call the constructor again."); break;
			case CHULL3D_FTMP_ZERO: Log->Msg(mtError,"'fTmp' is zero. May be flat convex."); break;
			default: Log->Msg(mtError,"Unknown error."); break;
            }
        	_DELETE(chull);
        }
    } catch (...) {
        _DELETE(chull);
    	UI->ProgressEnd();
        return;
    }
	UI->ProgressEnd();
}

bool CSector::RenderCHull(DWORD color, bool bSolid, bool bEdge, bool bCull){
	if (m_CHSectorFaces.size()==0) return true;
    if (m_bNeedUpdateCHull) return false;
	if (m_CHSectorVertices.size()>0xf000) return false;
	if (!bCull) UI->Device.SetRS(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
    if (bEdge||bSolid){
        DWORDVec colors;
        colors.resize(m_CHSectorVertices.size(),color);
        D3DDRAWPRIMITIVESTRIDEDDATA dt;
        dt.position.lpvData = m_CHSectorVertices.begin();
        dt.position.dwStride = sizeof(Fvector);
        dt.diffuse.lpvData = colors.begin();
        dt.diffuse.dwStride = sizeof(DWORD);

		UI->Device.RenderNearer(-0.0003);
        static WORDVec indices;
        indices.resize(m_CHSectorFaces.size()*3);
        for (DWORD i=0; i<m_CHSectorFaces.size(); i++){ 
        	indices[i*3+0]=m_CHSectorFaces[i].p0;
			indices[i*3+1]=m_CHSectorFaces[i].p1;
			indices[i*3+2]=m_CHSectorFaces[i].p2;
        }
        if (bEdge){
	        UI->Device.SetRS(D3DRENDERSTATE_FILLMODE,D3DFILL_WIREFRAME);
	        UI->Device.DIPS(D3DPT_TRIANGLELIST, FVF::F_L,
    				        &dt, m_CHSectorVertices.size(), 
                            indices.begin(),indices.size());
	        UI->Device.SetRS(D3DRENDERSTATE_FILLMODE,UI->dwRenderFillMode);
        }else if (bSolid){
	        UI->Device.DIPS(D3DPT_TRIANGLELIST, FVF::F_L,
    				        &dt, m_CHSectorVertices.size(), 
                            indices.begin(),indices.size());
        }
		UI->Device.ResetNearer();
    }
	if (!bCull) UI->Device.SetRS(D3DRENDERSTATE_CULLMODE,D3DCULL_CCW);
	return true;
}

void CSector::Render( Fmatrix& parent, ERenderPriority flag ){
    if (flag==rpAlphaNormal){
        Fmatrix matrix;
        Fcolor color;
        float k = Selected()?0.4f:0.2f;
		color.set(sector_color.r,sector_color.g,sector_color.b,k);
        if (fraBottomBar->miDrawSectorFaces->Checked){
			UI->Device.SetRS(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
            for (SItemIt it=sector_items.begin();it!=sector_items.end();it++){
                it->object->GetFullTransformToWorld(matrix);
                it->mesh->RenderList( matrix, color.get(), false, it->Face_IDs );
            }
			UI->Device.SetRS(D3DRENDERSTATE_CULLMODE,D3DCULL_CCW);
        }
        if (fraBottomBar->miDrawSectorCHull->Checked) 
        	RenderCHull(color.get(),true,false,true);
    }else if (flag==rpNormal){
        Fmatrix matrix;
        Fcolor color;
        float k = Selected()?0.4f:0.2f;
		color.set(sector_color.r*k,sector_color.g*k,sector_color.b*k,1.f);
        if (fraBottomBar->miDrawSectorCHull->Checked||fraBottomBar->miDrawSectorEdgedSFaces->Checked){
			UI->Device.SetRS(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
            for (SItemIt it=sector_items.begin();it!=sector_items.end();it++){
                it->object->GetFullTransformToWorld(matrix);
                it->mesh->RenderList( matrix, color.get(), true, it->Face_IDs );
            }
			UI->Device.SetRS(D3DRENDERSTATE_CULLMODE,D3DCULL_CCW);
        }
        if (fraBottomBar->miDrawSectorCHull->Checked&&fraBottomBar->miDrawSectorEdgedCHull->Checked) 
        	RenderCHull(color.get(),false,true,false);
    }
}

void CSector::Move( Fvector& amount ){
// internal use only!!!
	for(FvectorIt v_it=m_CHSectorVertices.begin(); v_it!=m_CHSectorVertices.end(); v_it++)
    	v_it->add(amount);
    m_SectorCenter.add(amount);
    UpdatePlanes();
}

bool CSector::FrustumPick(const CFrustum& frustum, const Fmatrix& parent){
	if (!frustum.testSphere(m_SectorCenter,m_SectorRadius)) return false;
	Fvector p[3];
    for(CHFaceIt it=m_CHSectorFaces.begin(); it!=m_CHSectorFaces.end(); it++){
    	for(int k=0; k<3; k++) p[k].set(m_CHSectorVertices[it->p[k]]);
	    sPoly s(p,3);
		if (frustum.testPoly(s)) return true;
    }
	return false;
}

bool CSector::RayPick(float& distance, Fvector& start, Fvector& direction, Fmatrix& parent, SRayPickInfo* pinf){
	Fvector p[3];
    float u,v;
    float range;
    bool bPick=false;
    for(CHFaceIt it=m_CHSectorFaces.begin(); it!=m_CHSectorFaces.end(); it++){
    	for(int k=0; k<3; k++) p[k].set(m_CHSectorVertices[it->p[k]]);
        range=UI->ZFar();
		if (RAPID::TestRayTri(start,direction,p,u,v,range,false)){
        	if ((range>=0)&&(range<distance)){
            	distance=range;
	            bPick=true;
            }
        }
    }
	return bPick;
}
//----------------------------------------------------

void CSector::SectorChanged(bool bNeedCreateCHull){
	m_bNeedUpdateCHull = true;
    Update(bNeedCreateCHull);
}
//----------------------------------------------------

void CSector::Update(bool bNeedCreateCHull){
	SItemIt it = sector_items.begin();
    for(;it!=sector_items.end();it++){
		if (it->mask.size()==0){
	    	it->mask.resize(it->mesh->m_Faces.size());
		    fill(it->mask.begin(),it->mask.end(),false);
            for(DWORDIt f_it=it->Face_IDs.begin(); f_it!=it->Face_IDs.end(); f_it++)
            	it->mask[*f_it]=true;
    	}
    }
	if (bNeedCreateCHull){
    	UI->SetStatus("Sector updating...");
    	MakeCHull();
    	UI->SetStatus("...");
    }
    UI->RedrawScene();
}
//----------------------------------------------------
void CSector::OnDestroy( ){
    // remove existence sector portal
    ObjectList& lst = Scene->ListObj(OBJCLASS_PORTAL);
    ObjectIt _F = lst.begin();
    while(_F!=lst.end()){
    	CPortal* P=(CPortal*)(*_F);
        if((P->m_SectorFront==this)||(P->m_SectorBack==this)){
            _DELETE((*_F));
            ObjectIt _D = _F; _F++;
            lst.remove((*_D));
        }else{
            _F++;
        }
    }
}

void CSector::OnSceneUpdate(){
	bool bUpdate=false;
    for(SItemIt it = sector_items.begin();it!=sector_items.end();it++){
    	if (!(Scene->ContainsObject(it->object,OBJCLASS_EDITOBJECT)&&it->object->ContainsMesh(it->mesh))){
            sector_items.erase(it); it--;
            bUpdate=true;
        }
    }
    if (bUpdate){
    	Update(true);
        PortalUtils.RemoveSectorPortal(this);
    }
}
//----------------------------------------------------

void CSector::CaptureInsideVolume(){
	// test all mesh faces
	// fill object list (test bounding sphere intersection)
    ObjectList lst;
	if (m_bNeedUpdateCHull) MakeCHull();
	if (Scene->SpherePick(m_SectorCenter, m_SectorRadius, OBJCLASS_EDITOBJECT, lst)){
    // test all object meshes
        DWORDVec fl;
        Fmatrix matrix;
        CEditObject *O_ref=NULL, *O_lib=NULL;
        // ignore dynamic objects
		for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
            O_ref = (CEditObject*)(*_F); O_lib=(O_ref->IsReference())?O_ref->GetRef():O_ref;
	        if (O_lib->IsDynamic()) continue;
            for(EditMeshIt m_def = O_lib->m_Meshes.begin();m_def!=O_lib->m_Meshes.end();m_def++){
                fl.clear();
                O_ref->GetFullTransformToWorld(matrix);
                (*m_def)->CHullPickFaces(m_CHSectorPlanes,matrix,fl);
                AddFaces(O_ref,*m_def,fl);
            }
        }
        MakeCHull();
		UI->RedrawScene();
    }
}
//----------------------------------------------------

void CSector::CaptureAllUnusedFaces(){
    DWORDVec fl;
    CEditObject *O_ref=NULL, *O_lib=NULL;
    ObjectList& lst=Scene->ListObj(OBJCLASS_EDITOBJECT);
    // ignore dynamic objects
    UI->ProgressStart(lst.size(),"Capturing unused face...");
    for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
		UI->ProgressInc();
        O_ref = (CEditObject*)(*_F); O_lib=(O_ref->IsReference())?O_ref->GetRef():O_ref;
        if (O_lib->IsDynamic()) continue;
        for(EditMeshIt m_def = O_lib->m_Meshes.begin();m_def!=O_lib->m_Meshes.end();m_def++){
            fl.clear();
            fl.resize((*m_def)->m_Faces.size());
            for(DWORD i=0; i<fl.size(); i++) fl[i]=i;
            AddFaces(O_ref,*m_def,fl);
        }
    }
    UI->ProgressEnd();
    MakeCHull();
    UI->RedrawScene();
}

//----------------------------------------------------
bool CSector::SpherePick(const Fvector& center, float radius, const Fmatrix& parent){
	float R=radius+m_SectorRadius;
    float dist_sqr=center.distance_to_sqr(m_SectorCenter);
    if (dist_sqr<R*R) return true;
    return false;
}
//----------------------------------------------------

EVisible CSector::TestCHullSphereIntersection(const Fvector&P, float R){
	bool bPartialInside=false;
	bool bPartialOutside=false;
	if (SpherePick(P,R,precalc_identity)){
        for(PlaneIt pl_it=m_CHSectorPlanes.begin(); pl_it!=m_CHSectorPlanes.end(); pl_it++){
        	float dist=pl_it->classify(P);
            if (dist>R) return fvNone;
            if (dist>EPS_L) bPartialOutside=true;
            else if (-dist<R) bPartialInside=true;
        }
    }else return fvNone;
    if (bPartialOutside) return fvPartialOutside;
    if (bPartialInside)  return fvPartialInside;
	return fvFully;
}
//----------------------------------------------------

int CSector::GetSectorFacesCount(){
	int count=0;
    for (SItemIt it=sector_items.begin();it!=sector_items.end();it++)
        count+=it->Face_IDs.size();
    return count;
}
//----------------------------------------------------

void CSector::LoadSectorDef( CStream* F ){
	char o_name[MAX_OBJ_NAME]="";
	char m_name[MAX_OBJ_NAME]="";

    CSectorItem sitem;

	// sector item
    R_ASSERT(F->FindChunk(SECTOR_CHUNK_ITEM_OBJECT));
	F->RstringZ(o_name);
	sitem.object=(CEditObject*)Scene->FindObjectByName(o_name,OBJCLASS_EDITOBJECT); 
    if (sitem.object==0){
    	Log->Msg(mtError,"Sector Item contains object '%s' - can't load.\nObject not found.",o_name);
        m_bHasLoadError = true;
        return;
    }
    if (!sitem.object->CheckVersion()){
    	Log->Msg(mtError,"Sector Item contains object '%s' - can't load.\nDifferent object version.",o_name);
        m_bHasLoadError = true;
     	return;
    }
	F->RstringZ(m_name);
	sitem.mesh=sitem.object->FindMeshByName(m_name);
    if (sitem.mesh==0){
    	Log->Msg(mtError,"Sector Item contains object '%s' mesh '%s' - can't load.\nMesh not found.",o_name,m_name);
        m_bHasLoadError = true;
        return;
    }

    R_ASSERT(F->FindChunk(SECTOR_CHUNK_ITEM_FACES));
	sitem.Face_IDs.resize(F->Rdword());
	F->Read(sitem.Face_IDs.begin(), sitem.Face_IDs.size()*sizeof(DWORD));

    sector_items.push_back(sitem);
}

bool CSector::Load(CStream& F){
	DWORD version = 0;

    char buf[1024];
    R_ASSERT(F.ReadChunk(SECTOR_CHUNK_VERSION,&version));
    if( version!=SECTOR_VERSION ){
        Log->Msg( mtError, "CSector: Unsupported version.");
        return false;
    }
    
	SceneObject::Load(F);

    R_ASSERT(F.ReadChunk(SECTOR_CHUNK_COLOR,&sector_color));

	R_ASSERT(F.FindChunk(SECTOR_CHUNK_PRIVATE));
    m_bDefault 		= F.Rbyte();
	m_bNeedUpdateCHull= F.Rbyte();

	if (F.FindChunk(SECTOR_CHUNK_CHULL_DATA2)){
        int cnt;
        cnt		   		= F.Rdword();
        m_CHSectorFaces.resize(cnt);
        F.Read			(m_CHSectorFaces.begin(),cnt*sizeof(CHFace));
        cnt		   		= F.Rdword();
        m_CHSectorVertices.resize(cnt);
        F.Read			(m_CHSectorVertices.begin(),cnt*sizeof(Fvector));
        cnt		   		= F.Rdword();
        m_CHSectorPlanes.resize(cnt);
        F.Read			(m_CHSectorPlanes.begin(),cnt*sizeof(Fplane));
        F.Rvector		(m_SectorCenter);
        m_SectorRadius	= F.Rfloat();
    }

    // Objects
    CStream* OBJ 	= F.OpenChunk(SECTOR_CHUNK_ITEMS);
    if(OBJ){
        CStream* O   	= OBJ->OpenChunk(0);
        for (int count=1; O; count++) {
            LoadSectorDef(O);
            O->Close	();
            O 			= OBJ->OpenChunk(count);
        }
        OBJ->Close();
    }

    if (sector_items.empty()) return false;
    
    Update((m_CHSectorFaces.size()==0)||(m_CHSectorPlanes.size()==0)||m_bNeedUpdateCHull||m_bHasLoadError);
    return true;
}

void CSector::Save(CFS_Base& F){
	SceneObject::Save(F);

	F.open_chunk	(SECTOR_CHUNK_VERSION);
	F.Wword			(SECTOR_VERSION);
	F.close_chunk	();

    F.write_chunk	(SECTOR_CHUNK_COLOR,&sector_color,sizeof(Fcolor));

	F.open_chunk	(SECTOR_CHUNK_PRIVATE);
	F.Wbyte			(m_bDefault);
	F.Wbyte			(m_bNeedUpdateCHull);
	F.close_chunk	();

	F.open_chunk	(SECTOR_CHUNK_CHULL_DATA2);
    F.Wdword		(m_CHSectorFaces.size());
    F.write			(m_CHSectorFaces.begin(),m_CHSectorFaces.size()*sizeof(CHFace));
    F.Wdword		(m_CHSectorVertices.size());
    F.write			(m_CHSectorVertices.begin(),m_CHSectorVertices.size()*sizeof(Fvector));
	F.Wdword		(m_CHSectorPlanes.size() );
    F.write			(m_CHSectorPlanes.begin(),m_CHSectorPlanes.size()*sizeof(Fplane));
	F.Wvector		(m_SectorCenter);
	F.Wfloat		(m_SectorRadius);
	F.close_chunk	();

	F.open_chunk	(SECTOR_CHUNK_ITEMS);
    int count=0;
    for(SItemIt it=sector_items.begin(); it!=sector_items.end(); it++){
        F.open_chunk(count); count++;
            F.open_chunk(SECTOR_CHUNK_ITEM_OBJECT);
            F.WstringZ	(it->object->GetName());
            F.WstringZ	(it->mesh->GetName());
	        F.close_chunk();

            F.open_chunk(SECTOR_CHUNK_ITEM_FACES);
            F.Wdword	(it->Face_IDs.size());
            F.write		(it->Face_IDs.begin(),it->Face_IDs.size()*sizeof(DWORD));
	        F.close_chunk();
        F.close_chunk();
    }
	F.close_chunk	();
}

//----------------------------------------------------

