#include "stdafx.h"
#pragma hdrstop

#include "EditObject.h"
#include "EditMesh.h"
#include "bottombar.h"
#include "motion.h"
#include "D3DUtils.h"
#include "bone.h"
#include "ExportSkeleton.h"
#include "ExportObjectOGF.h"

#define EPS_LOD 1.f/64.f

static FVF::LIT LOD[24]={
	-1.0f, 1.0f, 0.0f,  0xFFFFFFFF, 0.0f,	0.0f, // F 0
	 1.0f, 1.0f, 0.0f,  0xFFFFFFFF, 0.5f,	0.0f, // F 1
	 1.0f,-1.0f, 0.0f,  0xFFFFFFFF, 0.5f,	0.5f, // F 2
	-1.0f, 1.0f, 0.0f,  0xFFFFFFFF, 0.0f,	0.0f, // F 0
	 1.0f,-1.0f, 0.0f,  0xFFFFFFFF, 0.5f,	0.5f, // F 2
	-1.0f,-1.0f, 0.0f,  0xFFFFFFFF, 0.0f,	0.5f, // F 3

	 1.0f, 1.0f, 0.0f,  0xFFFFFFFF, 0.5f,0.0f, // B
	-1.0f, 1.0f, 0.0f,  0xFFFFFFFF, 1.0f,0.0f, // B
    -1.0f,-1.0f, 0.0f,  0xFFFFFFFF, 1.0f,0.5f, // B
	 1.0f, 1.0f, 0.0f,  0xFFFFFFFF, 0.5f,0.0f, // B
    -1.0f,-1.0f, 0.0f,  0xFFFFFFFF, 1.0f,0.5f, // B
     1.0f,-1.0f, 0.0f,  0xFFFFFFFF, 0.5f,0.5f, // B

	 0.0f, 1.0f,  1.0f, 0xFFFFFFFF, 0.0f,0.5f, // L
	 0.0f, 1.0f, -1.0f, 0xFFFFFFFF, 0.5f,0.5f, // L
     0.0f,-1.0f, -1.0f, 0xFFFFFFFF, 0.5f,1.0f, // L

	 0.0f, 1.0f,  1.0f, 0xFFFFFFFF, 0.0f,0.5f, // L
     0.0f,-1.0f, -1.0f, 0xFFFFFFFF, 0.5f,1.0f, // L

     0.0f,-1.0f,  1.0f, 0xFFFFFFFF, 0.0f,1.0f, // L

	 0.0f, 1.0f, -1.0f, 0xFFFFFFFF, 0.5f,0.5f, // R
	 0.0f, 1.0f,  1.0f, 0xFFFFFFFF, 1.0f,0.5f, // R
     0.0f,-1.0f,  1.0f, 0xFFFFFFFF, 1.0f,1.0f, // R

	 0.0f, 1.0f, -1.0f, 0xFFFFFFFF, 0.5f,0.5f, // R
     0.0f,-1.0f,  1.0f, 0xFFFFFFFF, 1.0f,1.0f, // R

     0.0f,-1.0f, -1.0f, 0xFFFFFFFF, 0.5f,1.0f, // R
};

bool CEditableObject::Reload()
{
	ClearGeometry();
    return Load(m_LoadName.c_str());
}

bool CEditableObject::RayPick(float& dist, Fvector& S, Fvector& D, Fmatrix& inv_parent, SRayPickInfo* pinf){
	bool picked = false;
    for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
        if( (*m)->RayPick( dist, S, D, inv_parent, pinf ) )
            picked = true;
	return picked;
}

#ifdef _LEVEL_EDITOR
bool CEditableObject::FrustumPick(const CFrustum& frustum, const Fmatrix& parent){
	for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
		if((*m)->FrustumPick(frustum, parent))	return true;
	return false;
}

bool CEditableObject::BoxPick(CSceneObject* obj, const Fbox& box, Fmatrix& parent, SBoxPickInfoVec& pinf){
	bool picked = false;
    for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
        if ((*m)->BoxPick(box, parent, pinf)){
        	pinf.back().s_obj = obj;
            picked = true;
        }
	return picked;
}
#endif

void CEditableObject::UpdateRenderBuffers(){
	ClearRenderBuffers();
    EditMeshIt _M=m_Meshes.begin();
    EditMeshIt _E=m_Meshes.end();
	for (; _M!=_E; _M++) (*_M)->UpdateRenderBuffers();
    m_LoadState |= EOBJECT_LS_RENDERBUFFER;
}

void CEditableObject::Render(const Fmatrix& parent, int priority, bool strictB2F){
    if (!(m_LoadState&EOBJECT_LS_RENDERBUFFER)) UpdateRenderBuffers();

    if (IsFlag(eoUsingLOD)){
    	RenderLOD(parent);
    }else{
        if(psDeviceFlags&rsEdgedFaces&&(1==priority)&&(false==strictB2F))
            RenderEdge(parent);

        Device.SetTransform(D3DTS_WORLD,parent);
        for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++){
            if ((priority==(*s_it)->_Priority())&&(strictB2F==(*s_it)->_StrictB2F())){
                Device.SetShader((*s_it)->_Shader());
                for (EditMeshIt _M=m_Meshes.begin(); _M!=m_Meshes.end(); _M++)
                    (*_M)->Render(parent,*s_it);
            }
        }
    }
}

void CEditableObject::RenderSkeletonSingle(const Fmatrix& parent)
{
	RenderSingle(parent);
//	for (int i=0; i<4; i++){
//		Render(parent, 0, false);
//		Render(parent, 0, true);
//    }
    if (fraBottomBar->miDrawObjectBones->Checked) RenderBones(parent);
}

void CEditableObject::RenderSingle(const Fmatrix& parent)
{
	for (int i=0; i<4; i++){
		Render(parent, i, false);
		Render(parent, i, true);
    }
}

void CEditableObject::RenderAnimation(const Fmatrix& parent){
}

void CEditableObject::RenderBones(const Fmatrix& parent){
	if (IsSkeleton()){
        // render
        Device.SetTransform(D3DTS_WORLD,parent);
        Device.SetShader(Device.m_WireShader);      
		BoneVec& lst = m_Bones;
        for(BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++){
            Fmatrix& M = (*b_it)->LTransform();
            Fvector p1;
            p1.set			(0,0,0);
            M.transform_tiny(p1);
            DWORD c 		= D3DCOLOR_RGBA(255,255,0,255);
            Fvector p2,d; 	d.set	(0,0,1);
            M.transform_dir	(d);
            p2.mad			(p1,d,(*b_it)->Length());
            DU::DrawLine	(p1,p2,c);
            DU::DrawRomboid	(p1,0.025,c);
        }
    }
}

void CEditableObject::RenderEdge(const Fmatrix& parent, CEditableMesh* mesh, DWORD color){
    if (!(m_LoadState&EOBJECT_LS_RENDERBUFFER)) UpdateRenderBuffers();

    Device.SetShader(Device.m_WireShader);
    if(mesh) mesh->RenderEdge(parent, color);
    else for(EditMeshIt _M = m_Meshes.begin();_M!=m_Meshes.end();_M++)
            (*_M)->RenderEdge(parent, color);
}

void CEditableObject::RenderSelection(const Fmatrix& parent, CEditableMesh* mesh, DWORD color)
{
    if (!(m_LoadState&EOBJECT_LS_RENDERBUFFER)) UpdateRenderBuffers();

    Device.SetTransform(D3DTS_WORLD,parent);
    Device.SetShader(Device.m_SelectionShader);
    Device.RenderNearer(0.0005);
    if(mesh) mesh->RenderSelection(parent, color);
    else for(EditMeshIt _M = m_Meshes.begin();_M!=m_Meshes.end();_M++)
         	(*_M)->RenderSelection(parent, color);
    Device.ResetNearer();
}

void CEditableObject::RenderLOD(const Fmatrix& parent)
{
/*	Fvector offs, size;
	m_Box.get_CD(offs,size);
	Device.SetShader(Device.m_WireShader);
	DU::DrawBox(offs,size,true,0xFFFFFFFF);
*/
    Shader* S = Device.Shader.Create("def_shaders\\def_aref_v_lod","lod\\lod_trees_tree_green3");
    Fvector P,R;
    m_Box.get_CD(P,R);
    Fmatrix matrix;
    matrix.translate(P);
    matrix.scale_over(R);
    matrix.mulA(parent);
    Device.SetTransform(D3DTS_WORLD,matrix);
    Device.SetShader(S);
//	if (!m_TexName.IsEmpty()&&!m_ShaderName.IsEmpty()) m_GShader = Device.Shader.Create(m_ShaderName.c_str(),m_TexName.c_str());
    DU::DrawPrimitiveLIT(D3DPT_TRIANGLELIST, 8, LOD, 24, true, false);
    Device.Shader.Delete(S);
}

void CEditableObject::OnDeviceCreate(){
	// пока буфера не аппаратные не нужно пересоздавать их
    //	UpdateRenderBuffers();
	// создать заново shaders
    for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
       (*s_it)->CreateShader();
}

void CEditableObject::OnDeviceDestroy(){
	// пока буфера не аппаратные не нужно пересоздавать их
    //	ClearRenderBuffers();
		// удалить shaders
    for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
        (*s_it)->DeleteShader();
}

void CEditableObject::LightenObject(){
	EditMeshIt m = m_Meshes.begin();
	for(;m!=m_Meshes.end();m++){
    	(*m)->UnloadCForm();
    	(*m)->UnloadPNormals();
    }
    ClearRenderBuffers();
}

bool CEditableObject::PrepareOGF(CFS_Base& F)
{
    CExportObjectOGF E(this);
    return E.ExportGeometry(F);
}

bool CEditableObject::PrepareSVGeometry(CFS_Base& F)
{
    CExportSkeleton E(this);
    return E.ExportGeometry(F);
}

bool CEditableObject::PrepareSVMotions(CFS_Base& F)
{
    CExportSkeleton E(this);
    return E.ExportMotions(F);
}

bool CEditableObject::PrepareSV(CFS_Base& F)
{
    CExportSkeleton E(this);
    return E.Export(F);
}

