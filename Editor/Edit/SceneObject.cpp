//----------------------------------------------------
// file: CEditObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "SceneObject.h"
#include "UI_Main.h"
#include "bottombar.h"
#include "scene.h"
#include "d3dutils.h"
#include "library.h"

//----------------------------------------------------
CSceneObject::CSceneObject( char *name ):CCustomObject(){
	Construct();
	strcpy( m_Name, name );
}

CSceneObject::CSceneObject( ):CCustomObject(){
	Construct();
}

void CSceneObject::Construct(){
	m_ClassID = OBJCLASS_SCENEOBJECT;

	mTransform.identity();
    vScale.set(1,1,1);
    vRotate.set(0,0,0);
    vPosition.set(0,0,0);

	m_pRefs 	= 0;
    m_ObjVer.reset();

    m_Center.set(0,0,0);
    m_fRadius = 0;
}

CSceneObject::~CSceneObject(){
	Lib.RemoveEditObject(m_pRefs);
}

//----------------------------------------------------

int CSceneObject::GetFaceCount(){
	return m_pRefs->GetFaceCount();
}

int CSceneObject::GetSurfFaceCount(const char* surf_name){
	return m_pRefs->GetSurfFaceCount(surf_name);
}

int CSceneObject::GetVertexCount(){
	return m_pRefs->GetVertexCount();
}

void CSceneObject::UpdateTransform(const Fvector& T, const Fvector& R, const Fvector& S){
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

void CSceneObject::UpdateTransform( ){
	UpdateTransform			(vPosition,vRotate,vScale);
}

bool CSceneObject::GetBox( Fbox& box ){
    box.transform(m_pRefs->GetBox(),mTransform);
	return true;
}

bool __inline CSceneObject::IsRender(){
    bool bRes = Device.m_Frustum.testSphere(m_Center,m_fRadius);
    if(bRes&&fraBottomBar->miDrawObjectAnimPath->Checked) RenderAnimation();
    return bRes;
}

void CSceneObject::Render(int priority, bool strictB2F){
    Scene.TurnLightsForObject(this);
	m_pRefs->Render(mTransform, priority, strictB2F);
    if ((1==priority)&&(false==strictB2F)){
        if (Selected()){
	        Device.SetShader(Device.m_WireShader);
    	    Device.SetTransform(D3DTS_WORLD,mTransform);
        	DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
	        DU::DrawSelectionBox(m_pRefs->GetBox(),&clr);
        }
    }
}

void CSceneObject::RenderSingle(){
	m_pRefs->RenderSingle(mTransform);
}

void CSceneObject::RenderAnimation(){
	m_pRefs->RenderAnimation(mTransform);
}

void CSceneObject::RenderBones(){
	m_pRefs->RenderBones(mTransform);
}

void CSceneObject::RenderEdge(CEditableMesh* mesh){
    if (Device.m_Frustum.testSphere(m_Center,m_fRadius))
		m_pRefs->RenderEdge(mTransform, mesh);
}

void CSceneObject::RenderSelection(){
	m_pRefs->RenderSelection(mTransform);
}

bool CSceneObject::FrustumPick(const CFrustum& frustum){
    if(Device.m_Frustum.testSphere(m_Center,m_fRadius))
		return m_pRefs->FrustumPick(frustum, mTransform);
    return false;
}

bool CSceneObject::SpherePick(const Fvector& center, float radius){
	float R=radius+m_fRadius;
    float dist_sqr=center.distance_to_sqr(m_Center);
    if (dist_sqr<R*R) return true;
    return false;
}

bool CSceneObject::RayPick(float& dist, Fvector& S, Fvector& D, SRayPickInfo* pinf){
    if (Device.m_Frustum.testSphere(m_Center,m_fRadius))
		if (m_pRefs->RayPick(dist, S, D, mTransform, pinf)){
        	if (pinf) pinf->s_obj = this;
            return true;
        }
	return false;
}

void CSceneObject::BoxPick(const Fbox& box, SBoxPickInfoVec& pinf){
	m_pRefs->BoxPick(box, mTransform, pinf);
}

void CSceneObject::Move(Fvector& amount){
	if (Locked()){
    	ELog.DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI.UpdateScene();
	vPosition.add( amount );
    UpdateTransform();
}

void CSceneObject::Rotate(Fvector& center, Fvector& axis, float angle){
	if (Locked()){
    	ELog.DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI.UpdateScene();

	Fmatrix m;
	m.rotation(axis, -angle);

	vPosition.sub( center );
    m.transform_tiny(vPosition);
	vPosition.add( center );

    vRotate.direct(vRotate,axis,axis.z?-angle:angle);

    UpdateTransform();
}
void CSceneObject::LocalRotate(Fvector& axis, float angle){
	if (Locked()){
    	ELog.DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI.UpdateScene();
    vRotate.direct(vRotate,axis,angle);
    UpdateTransform();
}

void CSceneObject::Scale( Fvector& center, Fvector& amount ){
	if (Locked()){
    	ELog.DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    if (m_pRefs->IsDynamic()){
    	ELog.DlgMsg(mtInformation,"Dynamic object %s - can't scale.", GetName());
        return;
    }
    UI.UpdateScene();
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
}

void CSceneObject::LocalScale( Fvector& amount ){
	if (Locked()){
    	ELog.DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    if (m_pRefs->IsDynamic()){
    	ELog.DlgMsg(mtInformation,"Dynamic object %s - can't scale.", GetName());
        return;
    }
    UI.UpdateScene();
	vScale.add(amount);
    if (vScale.x<EPS) vScale.x=EPS;
    if (vScale.y<EPS) vScale.y=EPS;
    if (vScale.z<EPS) vScale.z=EPS;
}

void CSceneObject::GetFullTransformToWorld( Fmatrix& m ){
    m.set( mTransform );
}

void CSceneObject::GetFullTransformToLocal( Fmatrix& m ){
    m.invert(mTransform);
}

void CSceneObject::RTL_Update(float dT){
}

CEditableObject* CSceneObject::SetReference(LPCSTR ref_name)
{
	if (m_pRefs) Lib.RemoveEditObject(m_pRefs);
	return (m_pRefs=Lib.CreateEditObject(ref_name));
}
