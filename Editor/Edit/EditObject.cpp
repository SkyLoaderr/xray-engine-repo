//----------------------------------------------------
// file: CEditableObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditObject.h"
#include "UI_Main.h"
#include "EditMesh.h"
#include "Shader.h"
#include "xrShader.h"
#include "Scene.h"
#include "D3DUtils.h"
#include "bottombar.h"
#include "motion.h"
#include "bone.h"
#include "library.h"

//----------------------------------------------------
ERenderPriority	st_Surface::RenderPriority(){
	if (shader&&shader->shader->Flags.bStrictB2F) return rpAlphaLast;
	if (shader&&has_alpha) return rpAlphaNormal;
	return rpNormal;
}
st_Surface::~st_Surface(){
	if (shader) Device.Shader.Delete(shader);
}

// mimimal bounding box size
float g_MinBoxSize 	= 0.05f;

CEditableObject::CEditableObject(CLibObject* parent){
    m_LibParent 	= parent;

	m_DynamicObject = false;
    m_ObjVer.reset	();

	m_Box.set		( 0,0,0, 0,0,0 );

    m_LoadState 	= 0;

    m_ActiveOMotion = 0;
    m_ActiveSMotion = 0;

	t_vPosition.set	(0.f,0.f,0.f);
    t_vScale.set   	(1.f,1.f,1.f);
    t_vRotate.set  	(0.f,0.f,0.f);
}

CEditableObject::~CEditableObject(){
    ClearGeometry();
}
//----------------------------------------------------

LPCSTR CEditableObject::GetName(){
	return m_LibParent->GetName();
}
//----------------------------------------------------

void CEditableObject::SetName(LPCSTR name){
	m_LibParent->SetName(name);
}
//----------------------------------------------------

void CEditableObject::VerifyMeshNames(){
	int idx=0;
    for(EditMeshIt m_def=m_Meshes.begin();m_def!=m_Meshes.end();m_def++){
    	AnsiString nm = (*m_def)->m_Name;
    	while (FindMeshByName(nm.c_str(),*m_def)||nm.IsEmpty())
	    	nm.sprintf("Mesh_#%d",idx++);
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
        if ((Ignore!=(*m))&&(strcmp((*m)->GetName(),name)==0)) return (*m);
    return 0;
}

void CEditableObject::ClearGeometry (){
    ClearRenderBuffers();
    if (!m_Meshes.empty())
        for(EditMeshIt 	m=m_Meshes.begin(); m!=m_Meshes.end();m++)_DELETE(*m);
    if (!m_Surfaces.empty())
        for(SurfaceIt 	s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
            _DELETE(*s_it);
    m_Meshes.clear();
    m_Surfaces.clear();
    // bones
    for(BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end();b_it++)_DELETE(*b_it);
    m_Bones.clear();
    // skeletal motions
    for(SMotionIt s_it=m_SMotions.begin(); s_it!=m_SMotions.end();s_it++)_DELETE(*s_it);
    m_SMotions.clear();
    // object motions
    for(OMotionIt o_it=m_OMotions.begin(); o_it!=m_OMotions.end();o_it++)_DELETE(*o_it);
    m_OMotions.clear();

    m_ActiveOMotion = 0;
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
    st_Surface* surf = FindSurfaceByName(surf_name);
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
//----------------------------------------------------
void CEditableObject::Render(Fmatrix& parent, ERenderPriority flag){
    if(flag==rpNormal){
		if (fraBottomBar->miDrawObjectBones->Checked) RenderBones(parent);
		if (fraBottomBar->miDrawObjectAnimPath->Checked) RenderAnimation(parent);
    }
    if (!(m_LoadState&EOBJECT_LS_RENDERBUFFER)) UpdateRenderBuffers();

    if(UI->bRenderEdgedFaces&&(flag==rpNormal))
        RenderEdge(parent);

    Device.SetTransform(D3DTS_WORLD,parent);
    for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++){
        if ((flag==rpAlphaNormal)&&((*s_it)->RenderPriority()==rpAlphaNormal)){
            Device.Shader.Set((*s_it)->shader);
            for (EditMeshIt _M=m_Meshes.begin(); _M!=m_Meshes.end(); _M++)
                (*_M)->Render(parent,*s_it);
        }
        if ((flag==rpAlphaLast)&&((*s_it)->RenderPriority()==rpAlphaLast)){
            Device.Shader.Set((*s_it)->shader);
            for (EditMeshIt _M=m_Meshes.begin(); _M!=m_Meshes.end(); _M++)
                (*_M)->Render(parent,*s_it);
        }
        if ((flag==rpNormal)&&((*s_it)->RenderPriority()==rpNormal)){
            Device.Shader.Set((*s_it)->shader);
            for (EditMeshIt _M=m_Meshes.begin(); _M!=m_Meshes.end(); _M++)
                (*_M)->Render(parent,*s_it);
        }
    }
}

void CEditableObject::RenderSingle(Fmatrix& parent){
	Render(parent, rpNormal);
	Render(parent, rpAlphaNormal);
	Render(parent, rpAlphaLast);
}

void CEditableObject::RenderAnimation(const Fmatrix& parent){
	if (IsOMotionActive()){
    	float fps = m_ActiveOMotion->FPS();
	    float min_t=(float)m_ActiveOMotion->FrameStart()/fps;
    	float max_t=(float)m_ActiveOMotion->FrameEnd()/fps;

        Fvector T,r;
        FvectorVec v;
        DWORD clr=0xffffffff;
        for (float t=min_t; t<max_t; t+=0.1f){
	        m_ActiveOMotion->Evaluate(t,T,r);
            v.push_back(T);
        }

        // update transform matrix
//        Fmatrix	M,mScale,mTranslate,mRotate;
//        mRotate.setHPB			(vRotate.y, vRotate.x, vRotate.z);
//        mScale.scale			(vScale);
//        mTranslate.translate	(vPosition);
//        M.mul					(mTranslate,mRotate);
//        M.mul					(mScale);

        Device.Shader.Set		(Device.m_WireShader);
        Device.SetTransform		(D3DTS_WORLD,parent);
        DU::DrawPrimitiveL		(D3DPT_LINESTRIP,v.size()-1,v.begin(),v.size(),clr,true,false);
    }
}

void CEditableObject::RenderBones(const Fmatrix& parent){
	if (IsSkeleton()){
		BoneVec& lst = m_Bones;
    	if (IsSMotionActive()){
            Fvector R,T;
            int i=0;
            for(BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++, i++){
	            m_ActiveSMotion->Evaluate(i,m_SMParam.Frame(),T,R);
                (*b_it)->Update(T,R);
            }
            m_SMParam.Update(Device.fTimeDelta);
        }else{
		    for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++) (*b_it)->Reset();
        }
        CalculateAnimation();

        // render
        Device.SetTransform(D3DTS_WORLD,parent);
        Device.Shader.Set(Device.m_WireShader);
        for(BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++){
            Fmatrix& M = (*b_it)->LTransform();
            Fvector p1;
            p1.set			(0,0,0);
            M.transform_tiny(p1);
            DWORD c 		= D3DCOLOR_RGBA(255,255,0,255);
            Fvector p2,d; 	d.set	(0,0,1);
            M.transform_dir	(d);
            p2.direct		(p1,d,(*b_it)->Length());
            DU::DrawLine	(p1,p2,c);
            DU::DrawRomboid	(p1,0.025,c);
        }
    }
}

void CEditableObject::RenderEdge(Fmatrix& parent, CEditableMesh* mesh){
    Device.Shader.Set(Device.m_WireShader);
    DWORD c=D3DCOLOR_RGBA(192,192,192,255);
    if(mesh) mesh->RenderEdge(parent, c);
    else for(EditMeshIt _M = m_Meshes.begin();_M!=m_Meshes.end();_M++)
            (*_M)->RenderEdge(parent, c);
}

void CEditableObject::RenderSelection(Fmatrix& parent){
    DWORD c=D3DCOLOR_RGBA(230,70,70,64);
    Device.SetTransform(D3DTS_WORLD,parent);
    Device.Shader.Set(Device.m_SelectionShader);
    Device.RenderNearer(0.0005);
    for(EditMeshIt _M = m_Meshes.begin();_M!=m_Meshes.end();_M++)
        (*_M)->RenderSelection(parent, c);
    Device.ResetNearer();
}

bool CEditableObject::FrustumPick(const CFrustum& frustum, const Fmatrix& parent){
	for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
		if((*m)->FrustumPick(frustum, parent))	return true;
	return false;
}

bool CEditableObject::RayPick(float& dist, Fvector& S, Fvector& D, Fmatrix& parent, SRayPickInfo* pinf){
	bool picked = false;

    for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
        if( (*m)->RayPick( dist, S, D, parent, pinf ) )
            picked = true;

	return picked;
}

void CEditableObject::BoxPick(const Fbox& box, Fmatrix& parent, SBoxPickInfoVec& pinf){
    for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
        (*m)->BoxPick(box, parent, pinf);
}

void CEditableObject::ClearRenderBuffers(){
	for (EditMeshIt _M=m_Meshes.begin(); _M!=m_Meshes.end(); _M++)
    	if (*_M) (*_M)->ClearRenderBuffers();
    m_LoadState &=~ EOBJECT_LS_RENDERBUFFER;
}

void CEditableObject::UpdateRenderBuffers(){
	ClearRenderBuffers();
    EditMeshIt _M=m_Meshes.begin();
    EditMeshIt _E=m_Meshes.end();
	for (; _M!=_E; _M++) (*_M)->UpdateRenderBuffers();
    m_LoadState |= EOBJECT_LS_RENDERBUFFER;
}
//------------------------------------------------------------------------------

void CEditableObject::RemoveMesh(CEditableMesh* mesh){
	EditMeshIt m_it = find(m_Meshes.begin(),m_Meshes.end(),mesh);
    VERIFY(m_it!=m_Meshes.end());
	m_Meshes.erase(m_it);
    _DELETE(mesh);
}

void CEditableObject::TranslateToWorld(const Fmatrix& parent) {
	EditMeshIt m = m_Meshes.begin();
	for(;m!=m_Meshes.end();m++) (*m)->Transform( parent );
    ClearRenderBuffers();
	UpdateBox();
}

st_Surface*	CEditableObject::FindSurfaceByName(const char* surf_name, int* s_id){
	for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
    	if (strcmp((*s_it)->name,surf_name)==0){ if (s_id) *s_id=s_it-m_Surfaces.begin(); return *s_it;}
    return 0;
}


void CEditableObject::OnDeviceCreate(){
	// пока буфера не аппаратные не нужно пересоздавать их
    //	UpdateRenderBuffers();
	// создать заново shaders
    for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
        (*s_it)->shader = Device.Shader.Create((*s_it)->sh_name.c_str(),(*s_it)->textures);
}

void CEditableObject::OnDeviceDestroy(){
	// пока буфера не аппаратные не нужно пересоздавать их
    //	ClearRenderBuffers();
		// удалить shaders
    for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
        if ((*s_it)->shader){ Device.Shader.Delete((*s_it)->shader); (*s_it)->shader=0; }
}

void CEditableObject::LightenObject(){
	EditMeshIt m = m_Meshes.begin();
	for(;m!=m_Meshes.end();m++){
    	(*m)->UnloadCForm();
    	(*m)->UnloadPNormals();
    }
    ClearRenderBuffers();
}

