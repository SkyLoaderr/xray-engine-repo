#include "stdafx.h"
#pragma hdrstop

#include "EditObject.h"
#include "EditMesh.h"
#include "bottombar.h"
#include "motion.h"
#include "bone.h"
#include "ExportSkeleton.h"
#include "ExportObjectOGF.h"
#include "d3dutils.h"
#include "ui_main.h"
#include "render.h"
#include "PropertiesListHelper.h"

const float tex_w	= LOD_SAMPLE_COUNT*LOD_IMAGE_SIZE;
const float tex_h	= 1*LOD_IMAGE_SIZE;
const float half_p_x= 0.5f*(1.f/tex_w);
const float half_p_y= 0.5f*(1.f/tex_h);
const float offs_x 	= 1.f/tex_w;
const float offs_y 	= 1.f/tex_h;

static Fvector LOD_pos[4]={
	-1.0f+offs_x, 1.0f-offs_y, 0.0f,
	 1.0f-offs_x, 1.0f-offs_y, 0.0f,
	 1.0f-offs_x,-1.0f+offs_y, 0.0f,
	-1.0f+offs_x,-1.0f+offs_y, 0.0f
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

bool CEditableObject::RayPick(float& dist, const Fvector& S, const Fvector& D, const Fmatrix& inv_parent, SRayPickInfo* pinf)
{
	bool picked = false;
    for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
        if( (*m)->RayPick( dist, S, D, inv_parent, pinf ) )
            picked = true;
	return picked;
}

void CEditableObject::RayQuery(const Fmatrix& parent, const Fmatrix& inv_parent, SPickQuery& pinf)
{
    for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
        (*m)->RayQuery(parent, inv_parent, pinf);
}

void CEditableObject::BoxQuery(const Fmatrix& parent, const Fmatrix& inv_parent, SPickQuery& pinf)
{
    for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
        (*m)->BoxQuery(parent, inv_parent, pinf);
}

#ifdef _LEVEL_EDITOR
bool CEditableObject::FrustumPick(const CFrustum& frustum, const Fmatrix& parent){
	for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
		if((*m)->FrustumPick(frustum, parent))	return true;
	return false;
}

bool CEditableObject::BoxPick(CSceneObject* obj, const Fbox& box, const Fmatrix& inv_parent, SBoxPickInfoVec& pinf){
	bool picked = false;
    for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
        if ((*m)->BoxPick(box, inv_parent, pinf)){
        	pinf.back().s_obj = obj;
            picked = true;
        }
	return picked;
}
#endif

extern float 	ssaLIMIT;
extern float	g_fSCREEN;
static const float ssaLim = 64.f*64.f/(640*480);
void CEditableObject::Render(const Fmatrix& parent, int priority, bool strictB2F){
    if (!(m_LoadState.is(LS_RBUFFERS))) DefferedLoadRP();

	Fvector v; float r;
    Fbox bb; bb.xform(m_Box,parent); bb.getsphere(v,r);

    if (m_Flags.is(eoUsingLOD)&&(CalcSSA(v,r)<ssaLim)){
		if ((1==priority)&&(true==strictB2F)) RenderLOD(parent);
    }else{
//. 	Device.Models.Render(m_Visual,parent,priority,strictB2F,1.f);
//*
		RCache.set_xform_world	(parent);
	    if (m_Flags.is(eoHOM)){
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
            if(psDeviceFlags.is(rsEdgedFaces)&&(1==priority)&&(false==strictB2F))
                RenderEdge(parent);

            for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++){
                if ((priority==(*s_it)->_Priority())&&(strictB2F==(*s_it)->_StrictB2F())){
                    Device.SetShader((*s_it)->_Shader());
                    for (EditMeshIt _M=m_Meshes.begin(); _M!=m_Meshes.end(); _M++)
                        (*_M)->Render(parent,*s_it);
                }
            }
        }
//*/
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
        RCache.set_xform_world(parent);
        Device.SetShader(Device.m_WireShader);
		BoneVec& lst = m_Bones;
        for(BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++){
            Fmatrix& M = (*b_it)->LTransform();
            Fvector p1;
            p1.set			(0,0,0);
            M.transform_tiny(p1);
            u32 c 		= D3DCOLOR_RGBA(255,255,0,255);
            Fvector p2,d; 	d.set	(0,0,1);
            M.transform_dir	(d);
            p2.mad			(p1,d,(*b_it)->Length());
            DU.DrawLine	(p1,p2,c);
            DU.DrawRomboid	(p1,0.025,c);
        }
    }
}

void CEditableObject::RenderEdge(const Fmatrix& parent, CEditableMesh* mesh, u32 color){
    if (!(m_LoadState.is(LS_RBUFFERS))) DefferedLoadRP();

    Device.SetShader(Device.m_WireShader);
    if(mesh) mesh->RenderEdge(parent, color);
    else for(EditMeshIt _M = m_Meshes.begin();_M!=m_Meshes.end();_M++)
            (*_M)->RenderEdge(parent, color);
}

void CEditableObject::RenderSelection(const Fmatrix& parent, CEditableMesh* mesh, u32 color)
{
    if (!(m_LoadState.is(LS_RBUFFERS))) DefferedLoadRP();

    RCache.set_xform_world(parent);
    Device.SetShader(Device.m_SelectionShader);
    Device.RenderNearer(0.0005);
    if(mesh) mesh->RenderSelection(parent, color);
    else for(EditMeshIt _M = m_Meshes.begin();_M!=m_Meshes.end();_M++)
         	(*_M)->RenderSelection(parent, color);
    Device.ResetNearer();
}

IC static void CalculateLODTC(int frame, int w_cnt, int h_cnt, Fvector2& lt, Fvector2& rb)
{
	Fvector2	ts;
    ts.set		(1.f/(float)w_cnt,1.f/(float)h_cnt);
    lt.x        = (frame%w_cnt+0)*ts.x+half_p_x;
    lt.y        = (frame/w_cnt+0)*ts.y+half_p_y;
    rb.x        = (frame%w_cnt+1)*ts.x-half_p_x;
    rb.y        = (frame/w_cnt+1)*ts.y-half_p_y;
}

void CEditableObject::GetLODFrame(int frame, Fvector p[4], Fvector2 t[4], const Fmatrix* parent)
{
	R_ASSERT(m_Flags.is(eoUsingLOD));
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
    Fvector HPB;
    parent.getHPB	(HPB);

    for (int frame=0; frame<LOD_SAMPLE_COUNT; frame++){
    	float angle = angle_normalize(frame*(PI_MUL_2/LOD_SAMPLE_COUNT)+HPB.x);

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
    	RCache.set_xform_world(parent);
        Device.SetShader		(m_LODShader?m_LODShader:Device.m_WireShader);
    	DU.DrawPrimitiveLIT	(D3DPT_TRIANGLEFAN, 2, LOD, 4, true, false);
    }
}

AnsiString CEditableObject::GetLODTextureName()
{
	AnsiString l_name;
    string256 nm; strcpy(nm,m_LibName.c_str()); _ChangeSymbol(nm,'\\','_');
    l_name = "lod_"+AnsiString(nm);
    l_name = EFS.UpdateTextureNameWithFolder(l_name);
    return l_name;
}

void CEditableObject::OnDeviceCreate()
{
}

void CEditableObject::OnDeviceDestroy()
{
	DefferedUnloadRP();
}

void CEditableObject::DefferedLoadRP()
{
	if (m_LoadState.is(LS_RBUFFERS)) return;
/*    
    CMemoryWriter 	F;
    PrepareOGF		(F);
	IReader R		(F.pointer(), F.size());
    m_Visual 		= Device.Models.Create(&R);
    //..
/*/

	// ������� ������ shaders
    for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
       (*s_it)->OnDeviceCreate();
//*/
	// ������� LOD shader
	AnsiString l_name = GetLODTextureName();
    AnsiString fname = l_name+AnsiString(".tga");
    Device.Shader.Delete(m_LODShader);
    if (FS.exist(_textures_,fname.c_str()))
    	m_LODShader = Device.Shader.Create(GetLODShaderName(),l_name.c_str());
    m_LoadState.set(LS_RBUFFERS,TRUE);
}
void CEditableObject::DefferedUnloadRP()
{
	if (!(m_LoadState.is(LS_RBUFFERS))) return;
	for (EditMeshIt _M=m_Meshes.begin(); _M!=m_Meshes.end(); _M++)
    	if (*_M) (*_M)->ClearRenderBuffers();
	// ������� shaders
    for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
        (*s_it)->OnDeviceDestroy();
    // LOD
    Device.Shader.Delete(m_LODShader);
    m_LoadState.set(LS_RBUFFERS,FALSE);
}
void CEditableObject::EvictObject(){
	EditMeshIt m = m_Meshes.begin();
	for(;m!=m_Meshes.end();m++){
    	(*m)->UnloadCForm();
    	(*m)->UnloadPNormals();
    }
    DefferedUnloadRP	();
}

bool CEditableObject::PrepareOGF(IWriter& F)
{
    CExportObjectOGF E(this);
    return E.ExportGeometry(F);
}

bool CEditableObject::PrepareSVGeometry(IWriter& F)
{
    CExportSkeleton E(this);
    return E.ExportGeometry(F);
}

bool CEditableObject::PrepareSVKeys(IWriter& F)
{
    CExportSkeleton E(this);
    return E.ExportMotionKeys(F);
}

bool CEditableObject::PrepareSVDefs(IWriter& F)
{
    CExportSkeleton E(this);
    return E.ExportMotionDefs(F);
}

bool CEditableObject::PrepareSV(IWriter& F)
{
    CExportSkeleton E(this);
    return E.Export(F);
}
//---------------------------------------------------------------------------

void __fastcall	CEditableObject::OnChangeShader(PropValue* prop)
{
    OnDeviceDestroy	();
    UI.RedrawScene	();
}
//---------------------------------------------------------------------------

void __fastcall CEditableObject::OnChangeTransform(PropValue* sender)
{
	UI.RedrawScene();
}
//---------------------------------------------------------------------------

void CEditableObject::FillPropSurf(LPCSTR pref, PropItemVec& items)
{
    for (SurfaceIt s_it=FirstSurface(); s_it!=LastSurface(); s_it++){
        CSurface* SURF			= *s_it;
        AnsiString nm 			= AnsiString(pref)+AnsiString("Surfaces\\")+SURF->_Name();
        AnsiString f_cnt		= GetSurfFaceCount(SURF->_Name());
        PropValue* V=0;
		V=PHelper.CreateATexture(items, AnsiString(nm+"\\Texture").c_str(), 	&SURF->m_Texture);					V->SetEvents(0,0,OnChangeShader);
        V=PHelper.CreateAEShader(items, AnsiString(nm+"\\Shader").c_str(), 		&SURF->m_ShaderName);				V->SetEvents(0,0,OnChangeShader);
        PHelper.CreateACShader	(items, AnsiString(nm+"\\Compile").c_str(), 	&SURF->m_ShaderXRLCName);
        V=PHelper.CreateAGameMtl(items, AnsiString(nm+"\\Game Mtl").c_str(),	&SURF->m_GameMtlName);				V->SetEvents(0,0,SURF->OnChangeGameMtl);
        V=PHelper.CreateFlag32	(items, AnsiString(nm+"\\2 Sided").c_str(), 	&SURF->m_Flags, CSurface::sf2Sided);V->SetEvents(0,0,OnChangeShader);
        PHelper.CreateCaption	(items, AnsiString(nm+"\\Face count").c_str(),  f_cnt.c_str());
    }
}
//---------------------------------------------------------------------------

void CEditableObject::FillBasicProps(LPCSTR pref, PropItemVec& items)
{
	PropValue* V=0;
	PHelper.CreateCaption	(items, PHelper.PrepareKey(pref,"Reference Name"),		m_LibName.c_str());
    PHelper.CreateFlag32	(items,	PHelper.PrepareKey(pref,"Flags\\Dynamic"),		&m_Flags,		CEditableObject::eoDynamic);
    PHelper.CreateFlag32	(items,	PHelper.PrepareKey(pref,"Flags\\HOM"),			&m_Flags,		CEditableObject::eoHOM);
    PHelper.CreateFlag32	(items,	PHelper.PrepareKey(pref,"Flags\\Use LOD"),		&m_Flags,		CEditableObject::eoUsingLOD);
    PHelper.CreateFlag32	(items,	PHelper.PrepareKey(pref,"Flags\\Multiple Usage"),&m_Flags,		CEditableObject::eoMultipleUsage);
    V=PHelper.CreateVector	(items, PHelper.PrepareKey(pref,"Transform\\Position"),	&t_vPosition,	-10000,	10000,0.01,2); 	V->SetEvents(0,0,OnChangeTransform);
    V=PHelper.CreateVector	(items, PHelper.PrepareKey(pref,"Transform\\Rotation"),	&t_vRotate, 	-10000,	10000,0.1,1);
    V->SetEvents			(PHelper.FvectorRDOnAfterEdit,PHelper.FvectorRDOnBeforeEdit,OnChangeTransform);
    V->Owner()->SetEvents	(PHelper.FvectorRDOnDraw);
    V=PHelper.CreateVector	(items, PHelper.PrepareKey(pref,"Transform\\Scale"),	&t_vScale, 		0.01,	10000,0.01,2);	V->SetEvents(0,0,OnChangeTransform);

    FillSummaryProps		(pref,items);
}
//---------------------------------------------------------------------------

void CEditableObject::FillSummaryProps(LPCSTR pref, PropItemVec& items)
{
    AnsiString t; t.sprintf("V: %d, F: %d",		GetVertexCount(),GetFaceCount());
    PHelper.CreateCaption(items,PHelper.PrepareKey(pref,"Summary\\Object"),t.c_str());
    for (EditMeshIt m_it=FirstMesh(); m_it!=LastMesh(); m_it++){
        CEditableMesh* MESH=*m_it;
        t.sprintf("V: %d, F: %d",MESH->GetVertexCount(),MESH->GetFaceCount());
	    PHelper.CreateCaption(items,PHelper.PrepareKey(pref,AnsiString(AnsiString("Summary\\Meshes\\")+MESH->GetName()).c_str()),t.c_str());
    }
    PHelper.CreateAText(items,PHelper.PrepareKey(pref, "Game options\\Script"),&m_ClassScript);
}
//---------------------------------------------------------------------------

#include "Blender.h"
IC BOOL BE      (BOOL A, BOOL B)
{
    bool a = !!A;
    bool b = !!B;
    return a==b;
}
bool CEditableObject::CheckShaderCompatible()
{
	bool bRes 			= true;
	for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
    {
    	CBlender* 		B = Device.Shader._FindBlender((*s_it)->m_ShaderName.c_str()); 
        Shader_xrLC* 	C = Device.ShaderXRLC.Get((*s_it)->m_ShaderXRLCName.c_str());
        R_ASSERT(B&&C);
    	if (!BE(B->canBeLMAPped(),!C->flags.bLIGHT_Vertex)){ 
        	ELog.Msg	(mtError,"Object '%s': engine shader '%s' non compatible with compiler shader '%s'",GetName(),(*s_it)->m_ShaderName.c_str(),(*s_it)->m_ShaderXRLCName.c_str());
            bRes 		= false;
        }
    }
    return bRes;
}
//---------------------------------------------------------------------------

#ifdef _LEVEL_EDITOR
#include "Scene.h"
bool CEditableObject::GetSummaryInfo(SSceneSummary* inf)
{
	if (IsStatic()||IsMUStatic()){
        for(SurfaceIt 	s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
            inf->textures.insert(ChangeFileExt((*s_it)->m_Texture,"").LowerCase());
        if (m_Flags.is(eoUsingLOD)){
            inf->textures.insert(GetLODTextureName());
            inf->lod_objects.insert(m_LibName);
            inf->object_lod_ref_cnt++;
        }
        if (m_Flags.is(eoMultipleUsage)){
            inf->mu_objects.insert(m_LibName);
            inf->object_mu_ref_cnt++;
        }

        inf->face_cnt		+= GetFaceCount	();
        inf->vert_cnt		+= GetVertexCount();
    }
	if (m_Flags.is(eoHOM)){
    	inf->hom_face_cnt	+= GetFaceCount	();
    	inf->hom_vert_cnt	+= GetVertexCount();
    }

	return true;
}
#endif

