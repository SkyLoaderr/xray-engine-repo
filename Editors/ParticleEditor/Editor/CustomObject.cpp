//----------------------------------------------------
// file: CustomObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "CustomObject.h"
#include "ui_main.h"

#include "bottombar.h"
#include "topbar.h"
#include "editorpref.h"
#include "D3DUtils.h"

#define SCENEOBJECT_CHUNK_PARAMS 		0xF900
#define SCENEOBJECT_CHUNK_LOCK	 		0xF902
#define SCENEOBJECT_CHUNK_TRANSFORM		0xF903
#define SCENEOBJECT_CHUNK_GROUP			0xF904
//----------------------------------------------------

CCustomObject::~CCustomObject()
{
}

void CCustomObject::OnUpdateTransform()
{
	m_bUpdateTransform		= FALSE;
    // update transform matrix
	Fmatrix	mScale,mTranslate,mRotate;
	mRotate.setHPB			(PRotation.y, PRotation.x, PRotation.z);

	mScale.scale			(PScale);
	mTranslate.translate	(PPosition);
	FTransformRP.mul		(mTranslate,mRotate);
	FTransform.mul			(FTransformRP,mScale);
    FITransformRP.invert	(FTransformRP);
    FITransform.invert		(FTransform);
}

void CCustomObject::Select( int flag )
{
    if (m_bVisible){
        m_bSelected = (flag==-1)?(m_bSelected?false:true):flag;
        UI.RedrawScene();
    }
};

void CCustomObject::Show( BOOL flag )
{
    m_bVisible = flag;
    if (!m_bVisible) m_bSelected = false;
    UI.RedrawScene();
};

void CCustomObject::Lock( BOOL flag )
{
	m_bLocked = flag;
};

void CCustomObject::OnShowHint(AStringVec& dest)
{
    dest.push_back(AnsiString("Class: ")+AnsiString(GetNameByClassID(ClassID)));
    dest.push_back(AnsiString("Name:  ")+AnsiString(Name));
    dest.push_back(AnsiString("-------"));
}

bool CCustomObject::Load(CStream& F)
{
    R_ASSERT(F.FindChunk(SCENEOBJECT_CHUNK_PARAMS));
	m_bSelected 	= F.Rword();
	m_bVisible   	= F.Rword();
	F.RstringZ		(FName);

	if(F.FindChunk(SCENEOBJECT_CHUNK_LOCK))
		m_bLocked	= F.Rword();

	if(F.FindChunk(SCENEOBJECT_CHUNK_TRANSFORM)){
        F.Rvector	(FPosition);
        F.Rvector	(FRotation);
        F.Rvector	(FScale);
    }

//	UpdateTransform	(true);
//	m_bUpdateTransform = TRUE;
	UpdateTransform	();

	return true;
}

void CCustomObject::Save(CFS_Base& F)
{
	F.open_chunk	(SCENEOBJECT_CHUNK_PARAMS);
	F.Wword			(m_bSelected);
	F.Wword			(m_bVisible);
	F.WstringZ		(FName);
	F.close_chunk	();

	F.open_chunk	(SCENEOBJECT_CHUNK_LOCK);
	F.Wword			(m_bLocked);
	F.close_chunk	();

	F.open_chunk	(SCENEOBJECT_CHUNK_TRANSFORM);
    F.Wvector		(FPosition);
    F.Wvector		(FRotation);
    F.Wvector		(FScale);
	F.close_chunk	();
}
//----------------------------------------------------

void CCustomObject::OnFrame()
{
	if (m_bUpdateTransform) OnUpdateTransform();
}

void CCustomObject::Render(int priority, bool strictB2F)
{
	if (fraBottomBar->miDrawObjectsPivot->Checked&&(1==priority)&&(false==strictB2F)&&(Selected())){
    	Device.SetShader(Device.m_WireShader);
    	DU::DrawObjectAxis(FTransformRP);
    }
}

bool CCustomObject::RaySelect(int flag, Fvector& start,Fvector& dir, bool bRayTest){
	if ((bRayTest&&RayPick(UI.ZFar(),start,dir))||!bRayTest){
		Select(flag);
        return true;
    }
    return false;
};

bool CCustomObject::FrustumSelect(int flag, const CFrustum& frustum){
	if (FrustumPick(frustum)){
    	Select(flag);
        return true;
    }
	return false;
};

