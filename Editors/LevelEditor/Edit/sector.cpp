//----------------------------------------------------
// file: Sector.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop                                      

#include "Log.h"
#include "Sector.h"
#include "Frustum.h"
#include "EditMesh.h"
#include "SceneObject.h"
#include "Scene.h"
#include "Texture.h"
#include "cl_intersect.h"
#include "bottombar.h"
#include "portal.h"
#include "portalutils.h"
#include "cl_collector.h"
#include "MgcConvexHull3D.h"
#include "ui_main.h"

#define SECTOR_VERSION   					0x0011
//----------------------------------------------------
#define SECTOR_CHUNK_VERSION				0xF010
#define SECTOR_CHUNK_COLOR					0xF020
#define SECTOR_CHUNK_PRIVATE				0xF025
#define SECTOR_CHUNK_ITEMS					0xF030
#define 	SECTOR_CHUNK_ONE_ITEM			0xF031
//----------------------------------------------------
CSectorItem::CSectorItem(){
	object=NULL;
    mesh=NULL;
}
CSectorItem::CSectorItem(CSceneObject* o, CEditableMesh* m){
	object=o;
    mesh=m;
}
void CSectorItem::GetTransform(Fmatrix& parent){
	object->GetFullTransformToWorld(parent);
}
bool CSectorItem::IsItem(const char* O, const char* M){
	return (0==stricmp(O,object->Name))&&(0==stricmp(M,mesh->GetName()));
}
//------------------------------------------------------------------------------

CSector::CSector(LPVOID data, LPCSTR name):CCustomObject(data,name){
	Construct(data);
}

void CSector::Construct(LPVOID data){
	ClassID			= OBJCLASS_SECTOR;
    sector_color.set(1,1,1,0);
	m_bDefault		= false;
	sector_num		= -1;
	m_bHasLoadError = false;
}

CSector::~CSector(){
	OnDestroy();
}

bool CSector::FindSectorItem(const char* O, const char* M, SItemIt& it){
	for (it=sector_items.begin();it!=sector_items.end();it++)
    	if ((*it).IsItem(O,M)) return true;
    return false;
}

bool CSector::FindSectorItem(CSceneObject* o, CEditableMesh* m, SItemIt& it){
	for (it=sector_items.begin();it!=sector_items.end();it++)
    	if ((*it).IsItem(o,m)) return true;
    return false;
}

bool CSector::AddMesh	(CSceneObject* O, CEditableMesh* M){
	SItemIt it;
	if (!PortalUtils.FindSector(O,M))
	    if (!FindSectorItem(O, M, it)){
    	 	sector_items.push_back(CSectorItem(O, M));
            UpdateVolume();
            return true;
        }
    return false;
}

bool CSector::DelMesh	(CSceneObject* O, CEditableMesh* M){
	SItemIt it;
    if (FindSectorItem(O, M, it)){
    	sector_items.erase(it);
		UpdateVolume();
    }
	if (sector_items.empty()){
    	ELog.Msg(mtInformation,"Last mesh deleted.\nSector has no meshes and will be removed.");
		Scene.RemoveObject(this,false);
        delete this;
        return false;
    }
    return true;
}

bool CSector::GetBox( Fbox& box ){
	box.set(m_Box);
	return true;
}

void CSector::Render(int priority, bool strictB2F){
    if ((1==priority)&&(true==strictB2F)){
        if (!fraBottomBar->miDrawSectorSolid->Checked){
            Fmatrix matrix;
            Fcolor color;
            float k = Selected()?0.4f:0.2f;
            color.set(sector_color.r,sector_color.g,sector_color.b,k);
            Device.SetRS(D3DRS_CULLMODE,D3DCULL_NONE);
            for (SItemIt it=sector_items.begin();it!=sector_items.end();it++){
                it->object->GetFullTransformToWorld(matrix);
                it->mesh->RenderSelection( matrix, color.get() );
            }
            Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);
        }
    }else if ((1==priority)&&(false==strictB2F)){
        Fmatrix matrix;
        Fcolor color;
        Fcolor color2;
        float k = Selected()?0.8f:0.5f;
        float k2 = Selected()?0.5f:0.2f;
		color.set(sector_color.r*k,sector_color.g*k,sector_color.b*k,1.f);
		color2.set(sector_color.r*k2,sector_color.g*k2,sector_color.b*k2,1.f);
        if (fraBottomBar->miDrawSectorSolid->Checked){
			Device.SetRS(D3DRS_CULLMODE,D3DCULL_NONE);
            for (SItemIt it=sector_items.begin();it!=sector_items.end();it++){
                it->object->GetFullTransformToWorld(matrix);
        	    it->mesh->RenderSelection( matrix, color.get() );
        	    it->mesh->RenderEdge( matrix, color2.get() );
            }
	        Device.SetRS(D3DRS_CULLMODE,D3DCULL_CCW);
        }

//        DU::DrawSelectionBox(m_Box);
    }
}

void CSector::Move( Fvector& amount ){
// internal use only!!!
    m_SectorCenter.add(amount);
}

bool CSector::FrustumPick(const CFrustum& frustum)
{
	if (!frustum.testSphere_dirty(m_SectorCenter,m_SectorRadius)) return false;
	for (SItemIt s_it=sector_items.begin();s_it!=sector_items.end();s_it++)
    	if (s_it->mesh->FrustumPick(frustum,s_it->object->_Transform())) return true;
	return false;
}

bool CSector::RayPick(float& distance, Fvector& start, Fvector& direction, SRayPickInfo* pinf){
    bool bPick=false;
	for (SItemIt s_it=sector_items.begin();s_it!=sector_items.end();s_it++)
    	if (s_it->mesh->RayPick(distance,start,direction,s_it->object->_ITransform(),pinf)) bPick=true;
	return bPick;
}
//----------------------------------------------------

void CSector::UpdateVolume(){
    Fbox bb;
    Fvector pt;
    m_Box.invalidate();
    for (SItemIt s_it=sector_items.begin();s_it!=sector_items.end();s_it++){
        s_it->mesh->GetBox(bb);
        bb.xform(s_it->object->_Transform());
        for(int i=0; i<8; i++){
            bb.getpoint(i, pt);
            m_Box.modify(pt);
        }
    }
    m_Box.getsphere(m_SectorCenter,m_SectorRadius);

    UI.RedrawScene();
}
//----------------------------------------------------
void CSector::OnDestroy( ){
    // remove existence sector portal
    ObjectList& lst = Scene.ListObj(OBJCLASS_PORTAL);
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
    	if (!(Scene.ContainsObject(it->object,OBJCLASS_SCENEOBJECT)&&it->object->GetReference()->ContainsMesh(it->mesh))){
            sector_items.erase(it); it--;
            bUpdate=true;
        }
    }
    if (bUpdate){
        PortalUtils.RemoveSectorPortal(this);
    }
	UpdateVolume();
}
//----------------------------------------------------

EVisible CSector::Intersect(const Fvector& center, float radius)
{
	float dist=m_SectorCenter.distance_to(center);

    Fvector R;
    m_Box.getradius(R);

	bool bInSphere = ((dist+radius)<m_SectorRadius)&&(radius>R.x)&&(radius>R.y)&&(radius>R.z);
	if (m_Box.contains(center)){
    	if (bInSphere) return fvFully;
        else return fvPartialInside;
    }else{
    	if (dist<(radius+m_SectorRadius)) return fvPartialOutside;
    }
	return fvNone;
}
//----------------------------------------------------

EVisible CSector::Intersect(const Fbox& box)
{
	if (m_Box.intersect(box)){
    	Fvector c; float r;
        box.getsphere(c,r);
    	return Intersect(c,r);
    }else return fvNone;
}
//----------------------------------------------------

void CSector::CaptureInsideVolume(){
	// test all mesh faces
	// fill object list (test bounding sphere intersection)
    ObjectList lst;
	if (Scene.SpherePick(m_SectorCenter, m_SectorRadius, OBJCLASS_SCENEOBJECT, lst)){
    // test all object meshes
        Fmatrix matrix;
	    CSceneObject *obj=NULL;
        // ignore dynamic objects
		for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
	        obj = (CSceneObject*)(*_F);
	        if (!obj->IsStatic()) continue;
	        EditMeshVec* M = obj->Meshes();
            R_ASSERT(M);
            for(EditMeshIt m_def = M->begin();m_def!=M->end();m_def++){
                obj->GetFullTransformToWorld(matrix);
                Fbox bb;
				(*m_def)->GetBox(bb);
                bb.xform(matrix);
                EVisible vis=Intersect(bb);
            	if ((fvFully==vis)||(fvPartialInside==vis))
					AddMesh(obj,*m_def);
            }
        }
        UpdateVolume();
		UI.RedrawScene();
    }
}
//----------------------------------------------------

void CSector::CaptureAllUnusedMeshes(){
    DWORDVec fl;
    CSceneObject *obj=NULL;
    ObjectList& lst=Scene.ListObj(OBJCLASS_SCENEOBJECT);
    // ignore dynamic objects
    UI.ProgressStart(lst.size(),"Capturing unused face...");
    for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
		UI.ProgressInc();
        obj = (CSceneObject*)(*_F);
        if (!obj->IsStatic()) continue;
        EditMeshVec* M = obj->Meshes();
        R_ASSERT(M);
        for(EditMeshIt m_def = M->begin(); m_def!=M->end();m_def++)
        	AddMesh(obj,*m_def);
    }
    UI.ProgressEnd();
    UI.RedrawScene();
}

//----------------------------------------------------
bool CSector::SpherePick(const Fvector& center, float radius){
	float R=radius+m_SectorRadius;
    float dist_sqr=center.distance_to_sqr(m_SectorCenter);
    if (dist_sqr<R*R) return true;
    return false;
}
//----------------------------------------------------

bool CSector::IsEmpty()
{
    int count=0;
    for (SItemIt it=sector_items.begin();it!=sector_items.end();it++)
        count+=it->mesh->GetFaceCount(true);
	return !count;
}
//----------------------------------------------------

void CSector::GetCounts(int* objects, int* meshes, int* faces)
{
	if (faces){
    	*faces=0;
	    for (SItemIt it=sector_items.begin();it!=sector_items.end();it++)
    	    *faces+=it->mesh->GetFaceCount(true);
    }
	if (meshes) *meshes=sector_items.size();
	if (objects){
        set<CSceneObject*> objs;
	    for (SItemIt it=sector_items.begin();it!=sector_items.end();it++)
        	objs.insert(it->object);
    	*objects=objs.size();
    }
}
//----------------------------------------------------

void CSector::LoadSectorDef( CStream* F ){
	char o_name[MAX_OBJ_NAME]="";
	char m_name[MAX_OBJ_NAME]="";

    CSectorItem sitem;

	// sector item
    R_ASSERT(F->FindChunk(SECTOR_CHUNK_ONE_ITEM));
	F->RstringZ(o_name);
	sitem.object=(CSceneObject*)Scene.FindObjectByName(o_name,OBJCLASS_SCENEOBJECT);
    if (sitem.object==0){
    	ELog.Msg(mtError,"Sector Item contains object '%s' - can't load.\nObject not found.",o_name);
        m_bHasLoadError = true;
        return;
    }
    if (!sitem.object->IsStatic()){
    	ELog.Msg(mtError,"Sector Item contains object '%s' - can't load.\nObject is dynamic.",o_name);
        m_bHasLoadError = true;
        return;
    }
//    if (!sitem.object->CheckVersion()){
//    	ELog.Msg(mtError,"Sector Item contains object '%s' - can't load.\nDifferent object version.",o_name);
//        m_bHasLoadError = true;
//     	return;
//    }
	F->RstringZ(m_name);
	sitem.mesh=sitem.object->GetReference()->FindMeshByName(m_name);
    if (sitem.mesh==0){
    	ELog.Msg(mtError,"Sector Item contains object '%s' mesh '%s' - can't load.\nMesh not found.",o_name,m_name);
        m_bHasLoadError = true;
        return;
    }

    sector_items.push_back(sitem);
}

bool CSector::Load(CStream& F){
	DWORD version = 0;

    char buf[1024];
    R_ASSERT(F.ReadChunk(SECTOR_CHUNK_VERSION,&version));
    if( version!=SECTOR_VERSION ){
        ELog.Msg( mtError, "CSector: Unsupported version.");
        return false;
    }

	CCustomObject::Load(F);

    R_ASSERT(F.ReadChunk(SECTOR_CHUNK_COLOR,&sector_color));

	R_ASSERT(F.FindChunk(SECTOR_CHUNK_PRIVATE));
    m_bDefault 		= F.Rbyte();

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

    UpdateVolume();
    return true;
}

void CSector::Save(CFS_Base& F){
	CCustomObject::Save(F);

	F.open_chunk	(SECTOR_CHUNK_VERSION);
	F.Wword			(SECTOR_VERSION);
	F.close_chunk	();

    F.write_chunk	(SECTOR_CHUNK_COLOR,&sector_color,sizeof(Fcolor));

	F.open_chunk	(SECTOR_CHUNK_PRIVATE);
	F.Wbyte			(m_bDefault);
	F.close_chunk	();

	F.open_chunk	(SECTOR_CHUNK_ITEMS);
    int count=0;
    for(SItemIt it=sector_items.begin(); it!=sector_items.end(); it++){
        F.open_chunk(count); count++;
            F.open_chunk(SECTOR_CHUNK_ONE_ITEM);
            F.WstringZ	(it->object->Name);
            F.WstringZ	(it->mesh->GetName());
	        F.close_chunk();
        F.close_chunk();
    }
	F.close_chunk	();
}

//----------------------------------------------------

void CSector::FillProp(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp(pref,items);
    PHelper.CreateFColor(items, PHelper.PrepareKey(pref,"Color"), &sector_color);
}
//----------------------------------------------------

