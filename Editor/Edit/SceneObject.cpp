//----------------------------------------------------
// file: CEditObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "SceneObject.h"
#include "UI_Main.h"
#include "bottombar.h"

//----------------------------------------------------
// mimimal bounding box size
float g_MinBoxSize = 0.05f;

CSceneObject::CSceneObject( char *name ):CCustomObject(){
	Construct();
	strcpy( m_Name, name );
}

CSceneObject::CSceneObject( ):CCustomObject(){
	Construct();
}

void CSceneObject::Construct(){
	m_ClassID = OBJCLASS_EDITOBJECT;

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
//    box.transform(m_pRefs->GetBox(),mTransform);
	return true;
}

bool __inline CSceneObject::IsRender(Fmatrix& parent){
    bool bRes = Device.m_Frustum.testSphere(m_Center,m_fRadius);
    if(bRes&&fraBottomBar->miDrawObjectAnimPath->Checked) RenderAnimation(precalc_identity);
    return bRes;
}

void CSceneObject::Render(Fmatrix& parent, ERenderPriority flag){
	m_pRefs->Render(parent, flag);
}

void CSceneObject::RenderSingle(Fmatrix& parent){
	m_pRefs->RenderSingle(parent);
}

void CSceneObject::RenderAnimation(const Fmatrix& parent){
	m_pRefs->RenderAnimation(parent);
}

void CSceneObject::RenderBones(const Fmatrix& parent){
	m_pRefs->RenderBones(parent);
}

void CSceneObject::RenderEdge(Fmatrix& parent, CEditMesh* mesh){
    if (Device.m_Frustum.testSphere(m_Center,m_fRadius))
		m_pRefs->RenderEdge(parent, mesh);
}

void CSceneObject::RenderSelection(Fmatrix& parent){
	m_pRefs->RenderSelection(parent);
}

bool CSceneObject::FrustumPick(const CFrustum& frustum, const Fmatrix& parent){
    if(Device.m_Frustum.testSphere(m_Center,m_fRadius))
		return m_pRefs->FrustumPick(frustum, parent);
    return false;
}

bool CSceneObject::SpherePick(const Fvector& center, float radius, const Fmatrix& parent){
	float R=radius+m_fRadius;
    float dist_sqr=center.distance_to_sqr(m_Center);
    if (dist_sqr<R*R) return true;
    return false;
}

bool CSceneObject::RayPick(float& dist, Fvector& S, Fvector& D, Fmatrix& parent, SRayPickInfo* pinf){
    if (Device.m_Frustum.testSphere(m_Center,m_fRadius))
		return m_pRefs->RayPick(dist, S, D, parent, pinf);
	return false;
}

void CSceneObject::BoxPick(const Fbox& box, Fmatrix& parent, SBoxPickInfoVec& pinf){
	m_pRefs->BoxPick(box, parent, pinf);
}

void CSceneObject::Move(Fvector& amount){
	if (Locked()){
    	ELog.DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI->UpdateScene();
	vPosition.add( amount );
    UpdateTransform();
}

void CSceneObject::Rotate(Fvector& center, Fvector& axis, float angle){
	if (Locked()){
    	ELog.DlgMsg(mtInformation,"Object %s - locked.", GetName());
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
void CSceneObject::LocalRotate(Fvector& axis, float angle){
	if (Locked()){
    	ELog.DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI->UpdateScene();
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
    UI->UpdateScene();
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

