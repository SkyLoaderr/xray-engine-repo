//----------------------------------------------------
// file: EditObject.cpp
//----------------------------------------------------

#include "stdafx.h"       
#pragma hdrstop

#include "EditObject.h"
#include "EditMesh.h"
#ifdef _EDITOR
	#include "motion.h"
	#include "bone.h"
#endif

// mimimal bounding box size
float g_MinBoxSize 	= 0.05f;

CEditableObject::CEditableObject(LPCSTR name)
{
	m_LibName		= name;

	m_dwFlags 		= 0;
    m_ObjVer.reset	();

	m_Box.invalidate();

    m_LoadState 	= 0;

    m_ActiveSMotion = 0;

	t_vPosition.set	(0.f,0.f,0.f);
    t_vScale.set   	(1.f,1.f,1.f);
    t_vRotate.set  	(0.f,0.f,0.f);

	a_vPosition.set	(0.f,0.f,0.f);
    a_vRotate.set  	(0.f,0.f,0.f);

    bOnModified		= false;

    m_RefCount		= 0;

    m_LODShader		= 0;
}

CEditableObject::~CEditableObject(){
    ClearGeometry();
}
//----------------------------------------------------

void CEditableObject::VerifyMeshNames(){
	int idx=0;
    for(EditMeshIt m_def=m_Meshes.begin();m_def!=m_Meshes.end();m_def++){
    	AnsiString nm = (*m_def)->m_Name;
		while (FindMeshByName(nm.c_str(),*m_def)||(nm=="")){
	    	nm="Mesh_#";
			nm+=idx++;
		}
        strcpy((*m_def)->m_Name,nm.c_str());
    }
}

void CEditableObject::GenerateMeshNames(){
	int idx=0;
    for(EditMeshIt m_def=m_Meshes.begin();m_def!=m_Meshes.end();m_def++,idx++)
    	sprintf((*m_def)->m_Name,"Mesh#%d",idx);
}
bool CEditableObject::ContainsMesh(const CEditableMesh* m){
    VERIFY(m);
    for(EditMeshIt m_def=m_Meshes.begin();m_def!=m_Meshes.end();m_def++)
        if (m==(*m_def)) return true;
    return false;
}

CEditableMesh* CEditableObject::FindMeshByName	(const char* name, CEditableMesh* Ignore){
    for(EditMeshIt m=m_Meshes.begin();m!=m_Meshes.end();m++)
        if ((Ignore!=(*m))&&(stricmp((*m)->GetName(),name)==0)) return (*m);
    return 0;
}

void CEditableObject::ClearGeometry (){
    OnDeviceDestroy();
    if (!m_Meshes.empty())
        for(EditMeshIt 	m=m_Meshes.begin(); m!=m_Meshes.end();m++)_DELETE(*m);
    if (!m_Surfaces.empty())
        for(SurfaceIt 	s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
            _DELETE(*s_it);
    m_Meshes.clear();
    m_Surfaces.clear();
#ifdef _EDITOR
    // bones
    for(BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end();b_it++)_DELETE(*b_it);
    m_Bones.clear();
    // skeletal motions
    for(SMotionIt s_it=m_SMotions.begin(); s_it!=m_SMotions.end();s_it++) _DELETE(*s_it);
    m_SMotions.clear();
#endif
    m_ActiveSMotion = 0;
}

int CEditableObject::GetFaceCount(){
	int cnt=0;
    for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
        cnt+=(*m)->GetFaceCount();
	return cnt;
}

int CEditableObject::GetSurfFaceCount(const char* surf_name){
	int cnt=0;
    CSurface* surf = FindSurfaceByName(surf_name);
    for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
        cnt+=(*m)->GetSurfFaceCount(surf);
	return cnt;
}

int CEditableObject::GetVertexCount(){
	int cnt=0;
    for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
        cnt+=(*m)->GetVertexCount();
	return cnt;
}

void CEditableObject::UpdateBox(){
	VERIFY(!m_Meshes.empty());
    EditMeshIt m = m_Meshes.begin();
    m_Box.invalidate();
    for(;m!=m_Meshes.end();m++){
        Fbox meshbox;
        (*m)->GetBox(meshbox);
        for(int i=0; i<8; i++){
            Fvector pt;
            meshbox.getpoint(i, pt);
            m_Box.modify(pt);
        }
    }
}
//----------------------------------------------------
void CEditableObject::RemoveMesh(CEditableMesh* mesh){
	EditMeshIt m_it = std::find(m_Meshes.begin(),m_Meshes.end(),mesh);
    VERIFY(m_it!=m_Meshes.end());
	m_Meshes.erase(m_it);
    _DELETE(mesh);
}

void CEditableObject::TranslateToWorld(const Fmatrix& parent) 
{
	EditMeshIt m = m_Meshes.begin();
	for(;m!=m_Meshes.end();m++) (*m)->Transform( parent );
    OnDeviceDestroy();
	UpdateBox();
}

CSurface*	CEditableObject::FindSurfaceByName(const char* surf_name, int* s_id){
	for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
    	if (stricmp((*s_it)->_Name(),surf_name)==0){ if (s_id) *s_id=s_it-m_Surfaces.begin(); return *s_it;}
    return 0;
}

