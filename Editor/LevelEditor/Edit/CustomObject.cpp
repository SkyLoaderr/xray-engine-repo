//----------------------------------------------------
// file: CustomObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "CustomObject.h"
#include "ui_main.h"

#define SCENEOBJECT_CHUNK_PARAMS 		0xF900
#define SCENEOBJECT_CHUNK_GROUPINDEX	0xF901
#define SCENEOBJECT_CHUNK_LOCK	 		0xF902
#define SCENEOBJECT_CHUNK_TRANSFORM		0xF903
//----------------------------------------------------

CCustomObject::~CCustomObject(){
}

void CCustomObject::OnUpdateTransform(){
	m_bUpdateTransform		= FALSE;
    // update transform matrix
	Fmatrix	mScale,mTranslate,mRotate;
	mRotate.setHPB			(PRotate.y, PRotate.x, PRotate.z);

	mScale.scale			(PScale);
	mTranslate.translate	(PPosition);
	FTransform.mul			(mTranslate,mRotate);
	FTransform.mul			(mScale);
}

void CCustomObject::Select( BOOL flag ){
    if (m_bVisible&&(m_bSelected!=flag)){
        m_bSelected = flag;
        UI.RedrawScene();
    }
};

void CCustomObject::Show( BOOL flag ){
    m_bVisible = flag;
    if (!m_bVisible) m_bSelected = false;
    UI.RedrawScene();
};

void CCustomObject::Lock( BOOL flag ){
	m_bLocked = flag;
};

void CCustomObject::OnShowHint(AStringVec& dest){
    dest.push_back(AnsiString("Class: ")+AnsiString(GetNameByClassID(ClassID)));
    dest.push_back(AnsiString("Name:  ")+AnsiString(Name));
    dest.push_back(AnsiString("-------"));
}

bool CCustomObject::Load(CStream& F){
    R_ASSERT(F.FindChunk(SCENEOBJECT_CHUNK_PARAMS));
	m_bSelected 	= F.Rword();
	m_bVisible   	= F.Rword();
	F.RstringZ		(FName);

    if(F.FindChunk(SCENEOBJECT_CHUNK_GROUPINDEX))
		m_iGroupIndex= F.Rdword();

	if(F.FindChunk(SCENEOBJECT_CHUNK_LOCK))
		m_bLocked	= F.Rword();

	if(F.FindChunk(SCENEOBJECT_CHUNK_TRANSFORM)){
        F.Rvector	(FPosition);
        F.Rvector	(FRotate);
        F.Rvector	(FScale);
    }

    UpdateTransform();

	return true;
}
void CCustomObject::Save(CFS_Base& F){
	F.open_chunk	(SCENEOBJECT_CHUNK_PARAMS);
	F.Wword			(m_bSelected);
	F.Wword			(m_bVisible);
	F.WstringZ		(FName);
	F.close_chunk	();

	F.open_chunk	(SCENEOBJECT_CHUNK_GROUPINDEX);
	F.Wdword		(m_iGroupIndex);
	F.close_chunk	();

	F.open_chunk	(SCENEOBJECT_CHUNK_LOCK);
	F.Wword			(m_bLocked);
	F.close_chunk	();

	F.open_chunk	(SCENEOBJECT_CHUNK_TRANSFORM);
    F.Wvector		(FPosition);
    F.Wvector		(FRotate);
    F.Wvector		(FScale);
	F.close_chunk	();
}
//----------------------------------------------------

void CCustomObject::OnFrame()
{
	if (m_bUpdateTransform) OnUpdateTransform();
}

void CCustomObject::Move(Fvector& amount){
	R_ASSERT(!Locked());
    UI.UpdateScene();
    Fvector v=PPosition;
    v.add(amount);
    PPosition = v;
}

void CCustomObject::Rotate(Fvector& center, Fvector& axis, float angle){
	R_ASSERT(!Locked());
    UI.UpdateScene();

	Fmatrix m;
	m.rotation(axis, -angle);

    Fvector p	= PPosition;
    Fvector r	= PRotate;

	p.sub		(center);
    m.transform_tiny(p);
	p.add		(center);

    r.mad		(axis,axis.z?-angle:angle);
	PPosition 	= p;
    PRotate		= r;
}

void CCustomObject::LocalRotate(Fvector& axis, float angle){
	R_ASSERT(!Locked());
    UI.UpdateScene();
    Fvector r	= PRotate;
    r.mad		(axis,angle);
    PRotate		= r;
}

void CCustomObject::Scale( Fvector& center, Fvector& amount ){
	R_ASSERT(!Locked());
    UI.UpdateScene();
    Fvector p	= PPosition;
    Fvector s	= PScale;
	s.add(amount);
	if (s.x<EPS) s.x=EPS;
	if (s.y<EPS) s.y=EPS;
	if (s.z<EPS) s.z=EPS;

	Fmatrix m;
    Fvector S;
    S.add(amount,1.f);
	m.scale( S );
	p.sub( center );
	m.transform_tiny(p);
	p.add( center );

    PPosition	= p;
    PScale		= s;
}

void CCustomObject::LocalScale( Fvector& amount ){
	R_ASSERT(!Locked());
    UI.UpdateScene();
    Fvector s	= PScale;
	s.add(amount);
    if (s.x<EPS) s.x=EPS;
    if (s.y<EPS) s.y=EPS;
    if (s.z<EPS) s.z=EPS;
    PScale		= s;
}


