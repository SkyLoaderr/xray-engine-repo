//----------------------------------------------------
// file: CEditObject.cpp
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

//----------------------------------------------------
ERenderPriority	st_Surface::RenderPriority(){
	if (shader&&shader->shader->Flags.bStrictB2F) return rpAlphaLast;
	if (shader&&has_alpha) return rpAlphaNormal;
	return rpNormal;
}
st_Surface::~st_Surface(){
	if (shader) UI->Device.Shader.Delete(shader);
}

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

CEditObject::CEditObject(CEditObject* source):SceneObject((SceneObject*)source){
    CloneFrom(source,true);
}

void CEditObject::Construct(){
	m_ClassID = OBJCLASS_EDITOBJECT;

	mTransform.identity();
    vScale.set(1,1,1);
    vRotate.set(0,0,0);
    vPosition.set(0,0,0);

	m_DynamicObject = false;

	m_LibRef 	= 0;
    m_ObjVer.reset();

	m_Box.set( 0,0,0, 0,0,0 );
    m_Center.set(0,0,0);
    m_fRadius = 0;

    m_LoadState = 0;

    m_ActiveOMotion = 0;
    m_ActiveSMotion = 0;
}

CEditObject::~CEditObject(){
    ClearGeometry();
}

//----------------------------------------------------

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
    m_ClassScript = source->m_ClassScript;
    
    ClearGeometry();
    CopyGeometry(source);
    
	m_LibRef = source->m_LibRef;
    bLibItem = source->bLibItem;
}

void CEditObject::VerifyMeshNames(){
    VERIFY2(m_LibRef==0,"Can't generate mesh names in ref object.");
	int idx=0;
    for(EditMeshIt m_def=m_Meshes.begin();m_def!=m_Meshes.end();m_def++){
    	AnsiString nm = (*m_def)->m_Name;
    	while (FindMeshByName(nm.c_str(),*m_def)||nm.IsEmpty())
	    	nm.sprintf("Mesh_#%d",idx++);
        strcpy((*m_def)->m_Name,nm.c_str());
    }
}

void CEditObject::GenerateMeshNames(){
    VERIFY2(m_LibRef==0,"Can't generate mesh names in ref object.");
	int idx=0;
    for(EditMeshIt m_def=m_Meshes.begin();m_def!=m_Meshes.end();m_def++,idx++)
    	sprintf((*m_def)->m_Name,"Mesh#%d",idx);
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
}

void CEditObject::CopyGeometry (CEditObject* source){
    for(SurfaceIt sf_it=source->m_Surfaces.begin(); sf_it!=source->m_Surfaces.end(); sf_it++){
    	m_Surfaces.push_back(new st_Surface(**sf_it));
        if ((*sf_it)->shader){
	        m_Surfaces.back()->shader = UI->Device.Shader.Create((*sf_it)->shader->shader->cName,(*sf_it)->textures);
            SH_ShaderDef* sh_base = SHLib->FindShader(AnsiString((*sf_it)->shader->shader->cName));
            if (sh_base)	m_Surfaces.back()->has_alpha = (sh_base->Passes_Count)?sh_base->Passes[0].Flags.bABlend:false;
        }
    }
    // meshes
    for(EditMeshIt m = source->m_Meshes.begin();m!=source->m_Meshes.end();m++)
        m_Meshes.push_back(new CEditMesh(*m,this));
    // bones
    for(BoneIt b_it=source->m_Bones.begin(); b_it!=source->m_Bones.end();b_it++)
        m_Bones.push_back(new CBone(**b_it));
    // skeleton motions
    for(SMotionIt s_it=source->m_SMotions.begin(); s_it!=source->m_SMotions.end();s_it++)
        m_SMotions.push_back(new CSMotion(*s_it));
    // object motions
    for(OMotionIt o_it=source->m_OMotions.begin(); o_it!=source->m_OMotions.end();o_it++)
        m_OMotions.push_back(new COMotion(*o_it));

    ClearRenderBuffers();
//    UpdateRenderBuffers();
}

int CEditObject::GetFaceCount(){
	int cnt=0;
    CEditObject* _O=IsReference()?m_LibRef:this;
    for(EditMeshIt m = _O->m_Meshes.begin();m!=_O->m_Meshes.end();m++)
        cnt+=(*m)->GetFaceCount();
	return cnt;
}

int CEditObject::GetSurfFaceCount(const char* surf_name){
	int cnt=0;
    st_Surface* surf = FindSurfaceByName(surf_name);
    CEditObject* _O=IsReference()?m_LibRef:this;
    for(EditMeshIt m = _O->m_Meshes.begin();m!=_O->m_Meshes.end();m++)
        cnt+=(*m)->GetSurfFaceCount(surf);
	return cnt;
}

int CEditObject::GetVertexCount(){
	int cnt=0;
    CEditObject* _O=IsReference()?m_LibRef:this;
    for(EditMeshIt m = _O->m_Meshes.begin();m!=_O->m_Meshes.end();m++)
        cnt+=(*m)->GetVertexCount();
	return cnt;
}

void CEditObject::UpdateTransform(const Fvector& T, const Fvector& R, const Fvector& S){
    // update transform matrix
	Fmatrix	mScale,mTranslate,mRotate;
	mRotate.setHPB			(R.y, R.x, R.z);

	mScale.scale			(S);
	mTranslate.translate	(T);
	mTransform.mul			(mTranslate,mRotate);
	mTransform.mul			(mScale);

    // update bounding volume
    Fbox BB; GetBox			(BB);
    BB.getsphere			(m_Center,m_fRadius);
}

void CEditObject::UpdateTransform( ){
	UpdateTransform			(vPosition,vRotate,vScale);
}

void CEditObject::UpdateBox(){
    UI->RedrawScene();
    if(!bLibItem) UpdateTransform();
	if(IsReference()){
		m_LibRef->UpdateBox();
		m_Box.set(m_LibRef->m_Box);
	}else{
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
    // update transform matrix
    if (!bLibItem) UpdateTransform();
}
//----------------------------------------------------

bool CEditObject::GetBox( Fbox& box ){
    box.transform(m_Box,mTransform);
	return true;
}

bool __inline CEditObject::IsRender(Fmatrix& parent){
    bool bRes = bLibItem||UI->Device.m_Frustum.testSphere(m_Center,m_fRadius);
    if(bRes&&fraBottomBar->miDrawObjectAnimPath->Checked) RenderAnimation(precalc_identity);
    return bRes;
}

void CEditObject::Render(Fmatrix& parent, ERenderPriority flag){
    if ((UI->GetEState()==esEditScene)&&(!bLibItem)) 	Scene->TurnLightsForObject(this);
    else if (UI->GetEState()==esEditLibrary) 			Scene->TurnLightsForObject(this);

    if(!bLibItem&&(flag==rpNormal)){
    	Fmatrix matrix; matrix.mul(parent,mTransform);
        if (Selected()){
	        UI->Device.Shader.Set(UI->Device.m_WireShader);
    	    UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,matrix);
        	DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
	        DU::DrawSelectionBox(m_Box,&clr);
        }
		if (fraBottomBar->miDrawObjectBones->Checked) RenderBones(matrix);
		if (fraBottomBar->miDrawObjectAnimPath->Checked) RenderAnimation(matrix);
    }
    if(IsReference()){
    	Fmatrix matrix;
        matrix.mul(parent,mTransform);
        m_LibRef->Render(matrix, flag);
    }else{
    	if (!(m_LoadState&EOBJECT_LS_RENDERBUFFER)) UpdateRenderBuffers();

    	Fmatrix matrix;
        matrix.mul(parent,mTransform);

        if(UI->bRenderEdgedFaces&&(flag==rpNormal))
        	RenderEdge(matrix);

	    UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,matrix);
        for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++){
        	if ((flag==rpAlphaNormal)&&((*s_it)->RenderPriority()==rpAlphaNormal)){
				UI->Device.Shader.Set((*s_it)->shader);
    	        for (EditMeshIt _M=m_Meshes.begin(); _M!=m_Meshes.end(); _M++)	
                	(*_M)->Render(matrix,*s_it);
            }
        	if ((flag==rpAlphaLast)&&((*s_it)->RenderPriority()==rpAlphaLast)){
				UI->Device.Shader.Set((*s_it)->shader);
    	        for (EditMeshIt _M=m_Meshes.begin(); _M!=m_Meshes.end(); _M++)
					(*_M)->Render(matrix,*s_it);
            }
            if ((flag==rpNormal)&&((*s_it)->RenderPriority()==rpNormal)){
				UI->Device.Shader.Set((*s_it)->shader);
                // Now iterate on passes
				for (EditMeshIt _M=m_Meshes.begin(); _M!=m_Meshes.end(); _M++)
    	    		(*_M)->Render(matrix,*s_it);
            }
        }
    }
}

void CEditObject::RenderSingle(Fmatrix& parent){
	Render(parent, rpNormal);
	Render(parent, rpAlphaNormal);
	Render(parent, rpAlphaLast);
}
 
void CEditObject::RenderAnimation(const Fmatrix& parent){
	if (IsOMotionActive()){
    	float fps = m_ActiveOMotion->FPS();
	    float min_t=(float)m_ActiveOMotion->FrameStart()/fps; 
    	float max_t=(float)m_ActiveOMotion->FrameEnd()/fps;

        Fvector T,r;
        FLvertexVec v;
        DWORD clr=0xffffffff;
        for (float t=min_t; t<max_t; t+=0.1f){
            v.push_back(FVF::L());
	        m_ActiveOMotion->Evaluate(t,T,r);
            v.back().set(T,clr);
        }

        // update transform matrix
        Fmatrix	M,mScale,mTranslate,mRotate;
        mRotate.setHPB			(vRotate.y, vRotate.x, vRotate.z);

        mScale.scale			(vScale);
        mTranslate.translate	(vPosition);
        M.mul					(mTranslate,mRotate);
        M.mul					(mScale);

        UI->Device.Shader.Set(UI->Device.m_WireShader);
        UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,M);
        UI->Device.DP(D3DPT_LINESTRIP,FVF::F_L,v.begin(),v.size());
    }
}

void CEditObject::RenderBones(const Fmatrix& parent){
	if (IsSkeleton()){
		BoneVec& lst = (m_LibRef)?m_LibRef->m_Bones:m_Bones;
    	if (IsSMotionActive()){
            Fvector R,T;
            int i=0;
            for(BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++, i++){
	            m_ActiveSMotion->Evaluate(i,m_SMParam.Frame(),T,R);
                (*b_it)->Update(T,R);
            }
            m_SMParam.Update(UI->Device.m_FrameDTime);
        }else{
		    for (BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++) (*b_it)->Reset();
        }
        CalculateAnimation();

        // render
        UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,parent);
        UI->Device.Shader.Set(UI->Device.m_WireShader);
        for(BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++){
            Fmatrix& M = (*b_it)->LTransform();
            Fvector p1;
            p1.set			(0,0,0);
            M.transform_tiny(p1);
            DWORD c = RGBA_MAKE(255,255,0,255);
            Fvector p2,d; 	d.set	(0,0,1);
            M.transform_dir	(d);
            p2.direct		(p1,d,(*b_it)->Length());
            DU::DrawLine	(p1,p2,c);
            DU::DrawRomboid	(p1,0.025,c);
        }
    }
/*
    CEditMesh* M = m_Meshes[0];
    if (!(M->m_LoadState&EMESH_LS_SVERTICES)) M->GenerateSVertices();

    for (SVertIt sv_it=M->m_SVertices.begin(); sv_it!=M->m_SVertices.end(); sv_it++){
        Fmatrix& T = m_Bones[sv_it->bone]->LTransform();
        Fvector v;
        Fcolor c;
        c.set(1,0,0,0);
        T.transform_tiny(v,sv_it->offs);
        DU_DrawRomboid	(v,0.025,c);
    }
*/
}

void CEditObject::RenderEdge(Fmatrix& parent, CEditMesh* mesh){
    if (bLibItem || UI->Device.m_Frustum.testSphere(m_Center,m_fRadius)){
		UI->Device.Shader.Set(UI->Device.m_WireShader);
        VERIFY(!IsReference());
        DWORD c=RGBA_MAKE(192,192,192,255);
        if(mesh) mesh->RenderEdge(parent, c);
        else for(EditMeshIt _M = m_Meshes.begin();_M!=m_Meshes.end();_M++)
				(*_M)->RenderEdge(parent, c);
    }
}

void CEditObject::RenderSelection(Fmatrix& parent){
    if(IsReference()){
    	Fmatrix matrix;
        matrix.mul(parent,mTransform);
        m_LibRef->RenderSelection(matrix);
    }else{
        DWORD c=RGBA_MAKE(230,70,70,64);
	    UI->Device.SetTransform(D3DTRANSFORMSTATE_WORLD,parent);
		UI->Device.Shader.Set(UI->Device.m_SelectionShader);
		UI->Device.RenderNearer(0.0005);
        for(EditMeshIt _M = m_Meshes.begin();_M!=m_Meshes.end();_M++)
			(*_M)->RenderSelection(parent, c);
// проба рендерить целым мешем из буфера
//        for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++){
//            for(EditMeshIt _M = m_Meshes.begin();_M!=m_Meshes.end();_M++)
//                if ((*_M)->m_Visible||fraBottomBar->miDrawHiddenMeshes->Checked) 
//                    (*_M)->RenderSelection(parent, c, *s_it);
//        }
	    UI->Device.ResetNearer();
    }
}

bool CEditObject::FrustumPick(const CFrustum& frustum, const Fmatrix& parent){
    if (!bLibItem&&!UI->Device.m_Frustum.testSphere(m_Center,m_fRadius))return false;
	if(IsReference()) return m_LibRef->FrustumPick( frustum, mTransform );
    Fmatrix matrix;
    matrix.mul(parent, mTransform);
	for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
		if((*m)->FrustumPick(frustum, matrix))	return true;
	return false;
}

bool CEditObject::SpherePick(const Fvector& center, float radius, const Fmatrix& parent){
	float R=radius+m_fRadius;
    float dist_sqr=center.distance_to_sqr(m_Center);
    if (dist_sqr<R*R) return true;
    return false;
}

bool CEditObject::RayPick(float& dist, Fvector& S, Fvector& D, Fmatrix& parent, SRayPickInfo* pinf){
    if (!bLibItem&&!UI->Device.m_Frustum.testSphere(m_Center,m_fRadius)) return false;

	bool picked = false;

	if(IsReference()){
		picked = m_LibRef->RayPick( dist, S, D, mTransform, pinf );
        if (pinf&&picked) pinf->obj = this;
	}else{
    	Fmatrix matrix;
        matrix.mul(parent, mTransform);
		for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
            if( (*m)->RayPick( dist, S, D, matrix, pinf ) )
            	picked = true;
	}

	return picked;
}

void CEditObject::BoxPick(const Fbox& box, Fmatrix& parent, SBoxPickInfoVec& pinf){
	if(IsReference()){
    	int offs = pinf.size();
		m_LibRef->BoxPick(box,mTransform,pinf);
        if (pinf.size()>offs)
        	for (SBoxPickInfoIt it=pinf.begin()+offs; it!=pinf.end(); it++)
            	it->obj=this;
	}else{
    	Fmatrix matrix;
        matrix.mul(parent, mTransform);
		for(EditMeshIt m = m_Meshes.begin();m!=m_Meshes.end();m++)
            (*m)->BoxPick(box, matrix, pinf);
	}
}

void CEditObject::Move(Fvector& amount){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI->UpdateScene();
	vPosition.add( amount );
    UpdateTransform();
}

void CEditObject::Rotate(Fvector& center, Fvector& axis, float angle){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI->UpdateScene();

	Fmatrix m;
	m.rotation(axis, -angle);

	vPosition.sub( center );
    m.transform_tiny(vPosition);
	vPosition.add( center );

    vRotate.direct(vRotate,axis,axis.z?-angle:angle);

    UpdateTransform();
}
void CEditObject::LocalRotate(Fvector& axis, float angle){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI->UpdateScene();
    vRotate.direct(vRotate,axis,angle);
    UpdateTransform();
}

void CEditObject::Scale( Fvector& center, Fvector& amount ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    if (IsDynamic()){
    	Log->DlgMsg(mtInformation,"Dynamic object %s - can't scale.", GetName());
        return;
    }
    UI->UpdateScene();
	vScale.add(amount);
	if (vScale.x<EPS) vScale.x=EPS;
	if (vScale.y<EPS) vScale.y=EPS;
	if (vScale.z<EPS) vScale.z=EPS;

	Fmatrix m;
    Fvector S;
    S.add(amount,1.f);
	m.scale( S );
	vPosition.sub( center );
	m.transform_tiny(vPosition);
	vPosition.add( center );
    
    UpdateBox();
}

void CEditObject::LocalScale( Fvector& amount ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    if (IsDynamic()){
    	Log->DlgMsg(mtInformation,"Dynamic object %s - can't scale.", GetName());
        return;
    }
    UI->UpdateScene();
	vScale.add(amount);
    if (vScale.x<EPS) vScale.x=EPS;
    if (vScale.y<EPS) vScale.y=EPS;
    if (vScale.z<EPS) vScale.z=EPS;
    UpdateBox();
}

void CEditObject::ClearRenderBuffers(){
	for (EditMeshIt _M=m_Meshes.begin(); _M!=m_Meshes.end(); _M++)
    	if (*_M) (*_M)->ClearRenderBuffers();
    m_LoadState &=~ EOBJECT_LS_RENDERBUFFER;
}

extern bool BeginLightVertices(const Fmatrix& parent);
extern void EndLightVertices();

void CEditObject::UpdateRenderBuffers(){
	ClearRenderBuffers();
    DWORD dwCaps;
    if (UI->Device.m_Caps.dwDevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT) dwCaps = D3DVBCAPS_WRITEONLY;
    else dwCaps = D3DVBCAPS_WRITEONLY | D3DVBCAPS_SYSTEMMEMORY;
    EditMeshIt _M;
	for (_M=m_Meshes.begin(); _M!=m_Meshes.end(); _M++) (*_M)->UpdateRenderBuffers(UI->Device.GetD3D(),dwCaps);
    m_LoadState |= EOBJECT_LS_RENDERBUFFER;
}
//------------------------------------------------------------------------------

void CEditObject::RemoveMesh(CEditMesh* mesh){
	EditMeshIt m_it = find(m_Meshes.begin(),m_Meshes.end(),mesh);
    VERIFY(m_it!=m_Meshes.end());
	m_Meshes.erase(m_it);
    _DELETE(mesh);
}

void CEditObject::CloneFromLib( SceneObject *source ){
	VERIFY( source );
	VERIFY( source->ClassID() == ClassID() );
	CEditObject *obj = (CEditObject*)source;
    CloneFrom(obj,true);
    bLibItem = false;
}

void CEditObject::ResolveReference() {
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
	if( IsReference() ){
        CloneFrom(m_LibRef,false);
		m_LibRef = 0;
        bLibItem = false;
		UpdateBox();
	}
}

void CEditObject::TranslateToWorld() {
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI->UpdateScene();
	VERIFY( !IsReference() );
	EditMeshIt m = m_Meshes.begin();
	for(;m!=m_Meshes.end();m++) (*m)->Transform( mTransform );

    // reset placement
    vPosition.set(0,0,0);
    vScale.set(1,1,1);
    vRotate.set(0,0,0);
    UpdateTransform();

    ClearRenderBuffers();
//    UpdateRenderBuffers();
    
	UpdateBox();
}

void CEditObject::LibReference( SceneObject *source ){
	VERIFY( source );
	VERIFY( source->ClassID() == ClassID() );

	CEditObject *obj = (CEditObject*)source;
	m_LibRef = obj;

	m_Meshes.clear();
	m_Surfaces.clear();
	m_DynamicObject	= obj->m_DynamicObject;
    m_ClassScript 	= obj->m_ClassScript;

    // reset placement
    vPosition.set(0,0,0);
    vScale.set(1,1,1);
    vRotate.set(0,0,0);
	UpdateBox();
}

void CEditObject::GetFullTransformToWorld( Fmatrix& m ){
    m.set( mTransform );
}

void CEditObject::GetFullTransformToLocal( Fmatrix& m ){
    m.set( mTransform );
    m.invert();
}

st_Surface*	CEditObject::FindSurfaceByName(const char* surf_name, int* s_id){
	for(SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++)
    	if (strcmp((*s_it)->name,surf_name)==0){ if (s_id) *s_id=s_it-m_Surfaces.begin(); return *s_it;}
    return 0;
}


void CEditObject::OnDeviceCreate(){
//    UpdateRenderBuffers();
}

void CEditObject::OnDeviceDestroy(){
	ClearRenderBuffers();
}

void CEditObject::LightenObject(){
	EditMeshIt m = m_Meshes.begin();
	for(;m!=m_Meshes.end();m++){ 
    	(*m)->UnloadCForm();
    	(*m)->UnloadPNormals();
    }
    ClearRenderBuffers();
}


