//----------------------------------------------------
// file: CEditObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "SceneClassList.h"
#include "..\FS.h"
//#include "Shader.h"
//#include "xrShader.h"

#define EOBJ_CURRENT_VERSION		0x0010
//----------------------------------------------------
#define EOBJ_CHUNK_VERSION		  	0x0900
#define EOBJ_CHUNK_REFERENCE     	0x0902
#define EOBJ_CHUNK_FLAG           	0x0903
#define EOBJ_CHUNK_PLACEMENT     	0x0904
#define EOBJ_CHUNK_SURFACES			0x0905
#define EOBJ_CHUNK_EDITMESHES      	0x0910
#define EOBJ_CHUNK_LIB_VERSION     	0x0911
#define EOBJ_CHUNK_CLASSSCRIPT     	0x0912
#define EOBJ_CHUNK_BONES			0x0913 
#define EOBJ_CHUNK_OMOTIONS			0x0914 
#define EOBJ_CHUNK_OACTIVE_MOTION	0x0915
//----------------------------------------------------

// mimimal bounding box size
float g_MinBoxSize = 0.05f;

CEditObject::CEditObject( char *name, bool bLib ):SceneObject(){
	Construct();
	strcpy( m_Name, name );
    bLibItem = bLib;
}

CEditObject::CEditObject( bool bLib ):SceneObject(){
	Construct();
    bLibItem = bLib;
}

void CEditObject::Construct(){
	m_ClassID = OBJCLASS_EDITOBJECT;

	mTransform.identity();
    vScale.set(1,1,1);
    vRotate.set(0,0,0);
    vPosition.set(0,0,0);

	m_DynamicObject = false;

	m_LibRef = 0;

	m_Box.set( 0,0,0, 0,0,0 );
    m_Center.set(0,0,0);
    m_fRadius = 0;
}

CEditObject::~CEditObject(){
    ClearGeometry();
}

//----------------------------------------------------
/*
void CEditObject::CloneFrom(CEditObject* source, bool bSetPlacement){
    VERIFY(source);
	m_DynamicObject= source->m_DynamicObject;
    if (bSetPlacement){
	    vPosition.set(source->vPosition);
    	vRotate.set(source->vRotate);
	    vScale.set(source->vScale);
		mTransform.set(source->mTransform);
    }
	m_Box.set(source->m_Box);
    m_fRadius = source->m_fRadius;
    m_Center.set(source->m_Center);
	m_LibRef = source->m_LibRef;
    bLibItem = source->bLibItem;

    ClearGeometry();
    CopyGeometry(source);
}
*/

void CEditObject::VerifyMeshNames(){
    VERIFY(m_LibRef==0);
	int idx=0;
	char nm[MAX_OBJ_NAME];
    for(EditMeshIt m_def=m_Meshes.begin();m_def!=m_Meshes.end();m_def++){
    	strcpy(nm,(*m_def)->m_Name);
    	while (FindMeshByName(nm,*m_def)||(strlen(nm)==0))
	    	sprintf(nm,"Mesh_#%d",idx++);
        strcpy((*m_def)->m_Name,nm);
    }
}

bool CEditObject::ContainsMesh(const CEditMesh* m){
    VERIFY(m);
    CEditObject* _O = m_LibRef?m_LibRef:this;
    for(EditMeshIt m_def=_O->m_Meshes.begin();m_def!=_O->m_Meshes.end();m_def++)
        if (m==(*m_def)) return true;
    return false;
}

CEditMesh* CEditObject::FindMeshByName	(const char* name, CEditMesh* Ignore){
    CEditObject* _O = m_LibRef?m_LibRef:this;
    for(EditMeshIt m=_O->m_Meshes.begin();m!=_O->m_Meshes.end();m++)
        if ((Ignore!=(*m))&&(strcmp((*m)->GetName(),name)==0)) return (*m);
    return 0;
}

void CEditObject::ClearGeometry (){
	if (!m_LibRef){
//        ClearRenderBuffers();
		// meshes
        if (!m_Meshes.empty())
        	for(EditMeshIt 	m=m_Meshes.begin(); m!=m_Meshes.end();m++)_DELETE(*m);
        if (!m_Surfaces.empty())
            for(SurfaceIt 	s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
                _DELETE(*s_it);
        m_Meshes.clear();
        m_Surfaces.clear();
		// bones
		m_Bones.clear();
    }
}

int CEditObject::GetFaceCount(){
	float cnt=0;
    CEditObject* _O=IsReference()?m_LibRef:this;
    for(EditMeshIt m = _O->m_Meshes.begin();m!=_O->m_Meshes.end();m++)
        cnt+=(*m)->GetFaceCount();
	return cnt;
}

int CEditObject::GetVertexCount(){
	float cnt=0;
    CEditObject* _O=IsReference()?m_LibRef:this;
    for(EditMeshIt m = _O->m_Meshes.begin();m!=_O->m_Meshes.end();m++)
        cnt+=(*m)->GetVertexCount();
	return cnt;
}

void CEditObject::UpdateTransform( ){
    // update transform matrix
	Fmatrix	mScale,mTranslate,mRotate;

    Fmatrix rX, rY, rZ; 	rX.rotateX(vRotate.x); rY.rotateY(vRotate.y); rZ.rotateZ(vRotate.z);
    mRotate.mul(rY,rX); 	mRotate.mul(rZ);
	mScale.scale			(vScale);
	mTranslate.translate	(vPosition);
	mTransform.mul			(mTranslate,mRotate);
	mTransform.mul			(mScale);

    // update bounding volume
    Fbox BB; GetBox			(BB);
    BB.getsphere			(m_Center,m_fRadius);
}

void CEditObject::UpdateBox(){
	if(m_Meshes.empty()){
		m_Box.set( vPosition, vPosition );
		m_Box.grow( g_MinBoxSize );
	}else{
		Fvector pt;
		bool boxdefined = false;
		EditMeshIt m = m_Meshes.begin();
		for(;m!=m_Meshes.end();m++){
			Fbox meshbox;
			(*m)->GetBox(meshbox);
			if(boxdefined){
				for(int i=0; i<8; i++){
					Fvector pt;
					meshbox.getpoint(i, pt);
					m_Box.modify(pt);
				}
			}else{
				meshbox.getpoint( 0, pt );
				m_Box.set(pt,pt);
				boxdefined = true;
				for(int i=1; i<8; i++){
					meshbox.getpoint( i, pt );
					m_Box.modify(pt);
				}
			}
		}
	}
}
//----------------------------------------------------

bool CEditObject::GetBox( Fbox& box ){
	Fvector pt;
	m_Box.getpoint( 0, pt );
	mTransform.transform(pt);
	box.set(pt,pt);
	for( int i=1; i<8; i++){
		m_Box.getpoint( i, pt );
		mTransform.transform(pt);
		box.modify( pt );
	}
	return true;
}

void CEditObject::Move(Fvector& amount){
	vPosition.add( amount );
    UpdateTransform();
}

void CEditObject::Rotate(Fvector& center, Fvector& axis, float angle){
    vRotate.direct(vRotate,axis,angle);
    UpdateTransform();
}

void CEditObject::LocalRotate(Fvector& axis, float angle){
    vRotate.direct(vRotate,axis,angle);
    UpdateTransform();
}

void CEditObject::Scale( Fvector& center, Fvector& amount ){
	vScale.add(amount);
    if (vScale.x<EPS) vScale.x=EPS;
    if (vScale.y<EPS) vScale.y=EPS;
    if (vScale.y<EPS) vScale.y=EPS;
    UpdateBox();
}

void CEditObject::LocalScale( Fvector& amount ){
	vScale.add(amount);
    if (vScale.x<EPS) vScale.x=EPS;
    if (vScale.y<EPS) vScale.y=EPS;
    if (vScale.z<EPS) vScale.z=EPS;
    UpdateBox();
}

void CEditObject::SaveObject(const char* fname){
    CFS_Memory F;
    F.open_chunk(CHUNK_OBJECT_BODY);
    Save(F);
    F.close_chunk();
    F.SaveTo(fname,0);//"OBJECT");
}

void CEditObject::Save(CFS_Base& F){
	F.open_chunk	(SCENEOBJECT_CHUNK_PARAMS);
	F.Wword			(m_Selected);
	F.Wword			(m_Visible);
	F.WstringZ		(m_Name );
	F.close_chunk	();

	F.open_chunk	(EOBJ_CHUNK_VERSION);
	F.Wword			(EOBJ_CURRENT_VERSION);
	F.close_chunk	();

	F.open_chunk	(EOBJ_CHUNK_PLACEMENT);
    F.Wvector		(vPosition);
    F.Wvector		(vRotate);
    F.Wvector		(vScale);
	F.close_chunk	();

    F.write_chunk	(EOBJ_CHUNK_FLAG,&m_DynamicObject,1);

    if( IsReference() ){
    	// reference object version
        F.open_chunk	(EOBJ_CHUNK_REFERENCE);
        F.write			(&m_LibRef->m_LibRefVer,m_LibRef->m_LibRefVer.size());
        F.WstringZ		(m_LibRef->m_Name);
        F.close_chunk	();
    } else {
    	// object version
        F.write_chunk	(EOBJ_CHUNK_LIB_VERSION,&m_LibRefVer,m_LibRefVer.size());
        // surfaces
        F.open_chunk	(EOBJ_CHUNK_SURFACES);
        F.Wdword		(m_Surfaces.size());
        for (SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++){
            F.WstringZ	((*s_it)->name);
            F.WstringZ	(!(*s_it)->shader.empty()?(*s_it)->shader.c_str():"");
            F.Wbyte		((*s_it)->sideflag);
            F.Wdword	((*s_it)->dwFVF);
            F.Wdword	((*s_it)->textures.size());
            for (AStringIt n_it=(*s_it)->textures.begin(); n_it!=(*s_it)->textures.end(); n_it++)
                F.WstringZ(n_it->c_str());
            for (AStringIt v_it=(*s_it)->vmaps.begin(); v_it!=(*s_it)->vmaps.end(); v_it++)
                F.WstringZ(v_it->c_str());
        }
        F.close_chunk	();

        // meshes
        F.open_chunk	(EOBJ_CHUNK_EDITMESHES);
        int count = 0;
        for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++){
			F.open_chunk(count); count++;
            (*m)->SaveMesh(F);
			F.close_chunk();
        }
		F.close_chunk	();

		// bones
        F.open_chunk	(EOBJ_CHUNK_BONES);
		F.Wdword		(m_Bones.size());
		for (BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++) (*b_it)->Save(F);
		F.close_chunk	();
    }
}
//------------------------------------------------------------------------------
void CEditObject::RemoveMesh(CEditMesh* mesh){
	EditMeshIt m_it = std::find(m_Meshes.begin(),m_Meshes.end(),mesh);
    VERIFY(m_it!=m_Meshes.end());
	m_Meshes.erase(m_it);
    _DELETE(mesh);
}
/*
void CEditObject::TranslateToWorld() {
	VERIFY( !IsReference() );
	EditMeshIt m = m_Meshes.begin();
	for(;m!=m_Meshes.end();m++) (*m)->Transform( mTransform );

    // reset placement
    vPosition.set(0,0,0);
    vScale.set(1,1,1);
    vRotate.set(0,0,0);
	UpdateBox();
}
*/
void CEditObject::GetFullTransformToWorld( Fmatrix& m ){
    m.set( mTransform );
}

void CEditObject::GetFullTransformToLocal( Fmatrix& m ){
    m.set( mTransform );
    m.invert();
}

st_Surface*	CEditObject::FindSurfaceByName(const char* surf_name, int* s_id){
	for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
    	if (strcmp((*s_it)->name,surf_name)==0){ 
			if (s_id) *s_id=s_it-m_Surfaces.begin(); 
			return *s_it;
		}
    return 0;
}

