#include "stdafx.h"
#pragma hdrstop

#include "EditObject.h"

bool CEditableObject::Reload()
{
	ClearGeometry();
    return Load(m_LoadName.c_str());
}

bool CEditableObject::RayPick(float& dist, Fvector& S, Fvector& D, Fmatrix& parent, SRayPickInfo* pinf){
	bool picked = false;

    for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
        if( (*m)->RayPick( dist, S, D, parent, pinf ) )
            picked = true;

	return picked;
}

#ifdef _LEVEL_EDITOR
bool CEditableObject::FrustumPick(const CFrustum& frustum, const Fmatrix& parent){
	for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
		if((*m)->FrustumPick(frustum, parent))	return true;
	return false;
}

void CEditableObject::BoxPick(const Fbox& box, Fmatrix& parent, SBoxPickInfoVec& pinf){
    for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
        (*m)->BoxPick(box, parent, pinf);
}
#endif

void CEditableObject::UpdateRenderBuffers(){
	ClearRenderBuffers();
    EditMeshIt _M=m_Meshes.begin();
    EditMeshIt _E=m_Meshes.end();
	for (; _M!=_E; _M++) (*_M)->UpdateRenderBuffers();
    m_LoadState |= EOBJECT_LS_RENDERBUFFER;
}

void CEditableObject::Render(Fmatrix& parent, int priority, bool strictB2F){
    if (!(m_LoadState&EOBJECT_LS_RENDERBUFFER)) UpdateRenderBuffers();

    if(psDeviceFlags&rsEdgedFaces&&(1==priority)&&(false==strictB2F))
        RenderEdge(parent);

    Device.SetTransform(D3DTS_WORLD,parent);
    for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++){
//    	if (!(*s_it)->Shader) continue;
        if ((priority==(*s_it)->_Priority())&&(strictB2F==(*s_it)->_StrictB2F())){
            Device.SetShader((*s_it)->_Shader());
            for (EditMeshIt _M=m_Meshes.begin(); _M!=m_Meshes.end(); _M++)
                (*_M)->Render(parent,*s_it);
        }
    }
}

void CEditableObject::RenderSingle(Fmatrix& parent){
	Render(parent, 0, false);
	Render(parent, 0, true);
	Render(parent, 1, false);
    if (fraBottomBar->miDrawObjectBones) RenderBones(precalc_identity);
	Render(parent, 1, true);
	Render(parent, 2, false);
	Render(parent, 2, true);
	Render(parent, 3, false);
	Render(parent, 3, true);
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

        Device.SetShader		(Device.m_WireShader);
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
        Device.SetShader(Device.m_WireShader);
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
    Device.SetShader(Device.m_WireShader);
    DWORD c=D3DCOLOR_RGBA(192,192,192,255);
    if(mesh) mesh->RenderEdge(parent, c);
    else for(EditMeshIt _M = m_Meshes.begin();_M!=m_Meshes.end();_M++)
            (*_M)->RenderEdge(parent, c);
}

void CEditableObject::RenderSelection(Fmatrix& parent){
    DWORD c=D3DCOLOR_RGBA(230,70,70,64);
    Device.SetTransform(D3DTS_WORLD,parent);
    Device.SetShader(Device.m_SelectionShader);
    Device.RenderNearer(0.0005);
    for(EditMeshIt _M = m_Meshes.begin();_M!=m_Meshes.end();_M++)
        (*_M)->RenderSelection(parent, c);
    Device.ResetNearer();
}

void CEditableObject::OnDeviceCreate(){
	// пока буфера не аппаратные не нужно пересоздавать их
    //	UpdateRenderBuffers();
	// создать заново shaders
    for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
       (*s_it)->SetShader((*s_it)->_ShaderName(), Device.Shader.Create((*s_it)->_ShaderName(),(*s_it)->_Texture()));
}

void CEditableObject::OnDeviceDestroy(){
	// пока буфера не аппаратные не нужно пересоздавать их
    //	ClearRenderBuffers();
		// удалить shaders
    for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
        if ((*s_it)->_Shader()) Device.Shader.Delete((*s_it)->_Shader());
}

void CEditableObject::LightenObject(){
	EditMeshIt m = m_Meshes.begin();
	for(;m!=m_Meshes.end();m++){
    	(*m)->UnloadCForm();
    	(*m)->UnloadPNormals();
    }
    ClearRenderBuffers();
}

