//----------------------------------------------------
// file: EditObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "fmesh.h"
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

	m_Flags.zero	();
    m_Version		= 0;

#ifdef _EDITOR
    m_Visual		= 0;
    vs_SkeletonGeom	= 0;
#endif
	m_Box.invalidate();

    m_LoadState.zero();

    m_ActiveSMotion = 0;

	t_vPosition.set	(0.f,0.f,0.f);
    t_vScale.set   	(1.f,1.f,1.f);
    t_vRotate.set  	(0.f,0.f,0.f);

	a_vPosition.set	(0.f,0.f,0.f);
    a_vRotate.set  	(0.f,0.f,0.f);

    bOnModified		= false;

    m_RefCount		= 0;

    m_LODShader		= 0;
    
    m_CreateName	= "unknown";
    m_CreateTime	= 0;
	m_ModifName		= "unknown";
    m_ModifTime		= 0;
}

CEditableObject::~CEditableObject()
{
    ClearGeometry();
}
//----------------------------------------------------

void CEditableObject::VerifyMeshNames()
{
	int idx=0;
	string256 nm; 
    for(EditMeshIt m_def=m_Meshes.begin();m_def!=m_Meshes.end();m_def++){
		strcpy(nm,(*m_def)->m_Name[0]?(*m_def)->m_Name:"mesh");
		while (FindMeshByName(nm,*m_def))
			sprintf(nm,"%s_%d",(*m_def)->m_Name[0]?(*m_def)->m_Name:"mesh",idx++);
    }
	VERIFY(xr_strlen(nm)<sizeof((*m_def)->m_Name));
	strcpy((*m_def)->m_Name,nm);
}

bool CEditableObject::ContainsMesh(const CEditableMesh* m)
{
    VERIFY(m);
    for(EditMeshIt m_def=m_Meshes.begin();m_def!=m_Meshes.end();m_def++)
        if (m==(*m_def)) return true;
    return false;
}

CEditableMesh* CEditableObject::FindMeshByName	(const char* name, CEditableMesh* Ignore)
{
    for(EditMeshIt m=m_Meshes.begin();m!=m_Meshes.end();m++)
        if ((Ignore!=(*m))&&(stricmp((*m)->GetName(),name)==0)) return (*m);
    return 0;
}

void CEditableObject::ClearGeometry ()
{
#ifdef _EDITOR
    OnDeviceDestroy();
#endif
    if (!m_Meshes.empty())
        for(EditMeshIt 	m=m_Meshes.begin(); m!=m_Meshes.end();m++)xr_delete(*m);
    if (!m_Surfaces.empty())
        for(SurfaceIt 	s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
            xr_delete(*s_it);
    m_Meshes.clear();
    m_Surfaces.clear();
#ifdef _EDITOR
    // bones
    for(BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end();b_it++)xr_delete(*b_it);
    m_Bones.clear();
    // skeletal motions
    for(SMotionIt s_it=m_SMotions.begin(); s_it!=m_SMotions.end();s_it++) xr_delete(*s_it);
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
    xr_delete(mesh);
}

void CEditableObject::TranslateToWorld(const Fmatrix& parent)
{
	EditMeshIt m = m_Meshes.begin();
	for(;m!=m_Meshes.end();m++) (*m)->Transform( parent );
#ifdef _EDITOR
	OnDeviceDestroy();
#endif
	UpdateBox();
}

CSurface*	CEditableObject::FindSurfaceByName(const char* surf_name, int* s_id){
	for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
    	if (stricmp((*s_it)->_Name(),surf_name)==0){ if (s_id) *s_id=s_it-m_Surfaces.begin(); return *s_it;}
    return 0;
}

LPCSTR CEditableObject::GenerateSurfaceName(const char* base_name)
{
	static char nm[128];
	strcpy(nm, base_name);
	if (FindSurfaceByName(nm)){
		DWORD idx=0;
		do{
			sprintf(nm,"%s_%d",base_name,idx);
			idx++;
		}while(FindSurfaceByName(nm));
	}
	return nm;
}

bool CEditableObject::VerifyBoneParts()
{
	U8Vec b_use(BoneCount(),0);
    for (BPIt bp_it=m_BoneParts.begin(); bp_it!=m_BoneParts.end(); bp_it++)
        for (int i=0; i<int(bp_it->bones.size()); i++){
        	int idx = FindBoneByNameIdx(bp_it->bones[i].c_str());
            if (idx==-1){
            	bp_it->bones.erase(bp_it->bones.begin()+i);
            	i--;
            }else{
	        	b_use[idx]++;
            }
        }

    for (U8It u_it=b_use.begin(); u_it!=b_use.end(); u_it++)
    	if (*u_it!=1) return false;
    return true;
}

void CEditableObject::PrepareOGFDesc(ogf_desc& desc)
{
	string512			tmp;
	desc.source_file	= m_LibName.c_str();
    desc.create_name	= m_CreateName.c_str();
    desc.create_time	= m_CreateTime;
    desc.modif_name		= m_ModifName.c_str();
    desc.modif_time		= m_ModifTime;
    desc.build_name		= strconcat(tmp,"\\\\",Core.CompName,"\\",Core.UserName);
    ctime				(&desc.build_time);
}

void CEditableObject::SetVersionToCurrent(BOOL bCreate, BOOL bModif)
{
	string512			tmp;
	if (bCreate){
		m_CreateName	= strconcat(tmp,"\\\\",Core.CompName,"\\",Core.UserName);
		m_CreateTime	= time(NULL);
	}
	if (bModif){
		m_ModifName		= strconcat(tmp,"\\\\",Core.CompName,"\\",Core.UserName);
		m_ModifTime		= time(NULL);
	}
}
