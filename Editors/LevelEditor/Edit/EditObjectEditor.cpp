#include "stdafx.h"
#pragma hdrstop

#include "EditObject.h"
#include "EditMesh.h"
#include "bottombar.h"
#include "motion.h"
#include "bone.h"
#include "ExportSkeleton.h"
#include "ExportObjectOGF.h"
#include "xr_trims.h"
#include "d3dutils.h"

static Fvector LOD_pos[4]={
	-1.0f, 1.0f, 0.0f,
	 1.0f, 1.0f, 0.0f,
	 1.0f,-1.0f, 0.0f,
	-1.0f,-1.0f, 0.0f
};
static FVF::LIT LOD[4]={
	-1.0f, 1.0f, 0.0f,  0xFFFFFFFF, 0.0f,0.0f, // F 0
	 1.0f, 1.0f, 0.0f,  0xFFFFFFFF, 0.0f,0.0f, // F 1
	 1.0f,-1.0f, 0.0f,  0xFFFFFFFF, 0.0f,0.0f, // F 2
	-1.0f,-1.0f, 0.0f,  0xFFFFFFFF, 0.0f,0.0f, // F 3
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

void CEditableObject::ClearRenderBuffers(){
	if (!(m_LoadState&EOBJECT_LS_RENDERBUFFER)) return;
	for (EditMeshIt _M=m_Meshes.begin(); _M!=m_Meshes.end(); _M++)
    	if (*_M) (*_M)->OnDeviceDestroy();
    m_LoadState &=~ EOBJECT_LS_RENDERBUFFER;
}

void CEditableObject::UpdateRenderBuffers(){
	if (m_LoadState&EOBJECT_LS_RENDERBUFFER) ClearRenderBuffers();
    EditMeshIt _M=m_Meshes.begin();
    EditMeshIt _E=m_Meshes.end();
	for (; _M!=_E; _M++) (*_M)->OnDeviceCreate();
    m_LoadState |= EOBJECT_LS_RENDERBUFFER;
}

void CEditableObject::Render(const Fmatrix& parent, int priority, bool strictB2F){
    if (!(m_LoadState&EOBJECT_LS_RENDERBUFFER)) UpdateRenderBuffers();

    if (IsFlag(eoUsingLOD)){
		if ((1==priority)&&(true==strictB2F)) RenderLOD(parent);
    }else{
		Device.SetTransform(D3DTS_WORLD,parent);
        
	    if (IsFlag(eoHOM)){
        	if ((1==priority)&&(false==strictB2F)){
	            RenderEdge(parent,0,0x00000000);
                Device.SetShader(Device.m_WireShader);
                for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++){
                    if ((priority==(*s_it)->_Priority())&&(strictB2F==(*s_it)->_StrictB2F())){
                        for (EditMeshIt _M=m_Meshes.begin(); _M!=m_Meshes.end(); _M++)
                            (*_M)->Render(parent,*s_it);
                    }
                }
            }
        }else{
            if(psDeviceFlags&rsEdgedFaces&&(1==priority)&&(false==strictB2F))
                RenderEdge(parent);

            for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++){
                if ((priority==(*s_it)->_Priority())&&(strictB2F==(*s_it)->_StrictB2F())){
                    Device.SetShader((*s_it)->_Shader());
                    for (EditMeshIt _M=m_Meshes.begin(); _M!=m_Meshes.end(); _M++)
                        (*_M)->Render(parent,*s_it);
                }
            }
        }
    }
}

void CEditableObject::RenderSkeletonSingle(const Fmatrix& parent)
{
	RenderSingle(parent);
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

IC static void CalculateLODTC(int frame, int w_cnt, int h_cnt, Fvector2& lt, Fvector2& rb){
	Fvector2	ts;
    ts.set		(1.f/(float)w_cnt,1.f/(float)h_cnt);
    lt.x        = (frame%w_cnt)*ts.x;
    lt.y        = (frame/w_cnt)*ts.y+0.5f*(1.f/64.f);
    rb.x        = lt.x+ts.x;
    rb.y        = lt.y+ts.y;
}

void CEditableObject::GetLODFrame(int frame, Fvector p[4], Fvector2 t[4], const Fmatrix* parent)
{
	R_ASSERT(IsFlag(eoUsingLOD));
    Fvector P,S;
    m_Box.get_CD(P,S);
    float r = max(S.x,S.z);//sqrtf(S.x*S.x+S.z*S.z);
    Fmatrix T,matrix,rot;
    T.scale(r,S.y,r);
    T.translate_over(P);
    if (parent) T.mulA(*parent);

    float angle = frame*(PI_MUL_2/LOD_SAMPLE_COUNT);
    rot.rotateY(-angle);
    matrix.mul(T,rot);
    Fvector2 lt, rb;
    CalculateLODTC(frame,LOD_SAMPLE_COUNT,1,lt,rb);
    t[0].set(lt);
    t[1].set(rb.x,lt.y);
    t[2].set(rb);
    t[3].set(lt.x,rb.y);
    matrix.transform_tiny(p[0],LOD_pos[0]);
    matrix.transform_tiny(p[1],LOD_pos[1]);
    matrix.transform_tiny(p[2],LOD_pos[2]);
    matrix.transform_tiny(p[3],LOD_pos[3]);
}

void CEditableObject::RenderLOD(const Fmatrix& parent)
{
    Fvector C;
    C.sub			(parent.c,Device.m_Camera.GetPosition()); C.y = 0;
    float m 		= C.magnitude();
    if (m<EPS) return;
    C.div			(m);
	int max_frame;
    float max_dot	= 0;
    for (int frame=0; frame<LOD_SAMPLE_COUNT; frame++){
    	float angle = frame*(PI_MUL_2/LOD_SAMPLE_COUNT);

        Fvector D;
        D.setHP(angle,0);
        float dot = C.dotproduct(D);
        if (dot<0.7072f) continue;

        if (dot>max_dot){
        	max_dot = dot;
            max_frame = frame;
        }
    }
	{
    	Fvector    	p[4];
        Fvector2 	t[4];
    	GetLODFrame(max_frame,p,t);
        for (int i=0; i<4; i++){ LOD[i].p.set(p[i]); LOD[i].t.set(t[i]); }
    	Device.SetTransform		(D3DTS_WORLD,parent);
        Device.SetShader		(m_LODShader?m_LODShader:Device.m_WireShader);
    	DU::DrawPrimitiveLIT	(D3DPT_TRIANGLEFAN, 2, LOD, 4, true, false);
    }
}

LPCSTR CEditableObject::GetLODTextureName(AnsiString& l_name)
{
    string256 nm; strcpy(nm,m_LibName.c_str()); _ChangeSymbol(nm,'\\','_');
    l_name = "lod_"+AnsiString(nm);
    l_name = Engine.FS.UpdateTextureNameWithFolder(l_name);
    return l_name.c_str();
}

void CEditableObject::UpdateLODShader()
{
	AnsiString l_name;
    GetLODTextureName(l_name);
    AnsiString fname = l_name+AnsiString(".tga");
    Device.Shader.Delete(m_LODShader);
    if (Engine.FS.Exist(&Engine.FS.m_Textures,fname.c_str()))
    	m_LODShader = Device.Shader.Create(GetLODShaderName(),l_name.c_str());
}

void CEditableObject::OnDeviceCreate()
{
	// создать буфера
    for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
    	(*m)->OnDeviceCreate();
	// создать заново shaders
    for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
       (*s_it)->CreateShader();
    UpdateLODShader();
}

void CEditableObject::OnDeviceDestroy()
{
	// удалить буфера
    for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
    	(*m)->OnDeviceDestroy();
	// удалить shaders
    for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
        (*s_it)->DeleteShader();
    // LOD
    Device.Shader.Delete(m_LODShader);
}

void CEditableObject::EvictObject(){
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

