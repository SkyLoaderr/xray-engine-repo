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
#include "scene.h"
#include "sceneobject.h"
#include "D3DUtils.h"
#include "GroupObject.h"

#define SCENEOBJECT_CHUNK_PARAMS 		0xF900
#define SCENEOBJECT_CHUNK_LOCK	 		0xF902
#define SCENEOBJECT_CHUNK_TRANSFORM		0xF903
#define SCENEOBJECT_CHUNK_GROUP			0xF904
//----------------------------------------------------

CCustomObject::~CCustomObject()
{
}

void CCustomObject::RemoveFromGroup()
{
    m_pGroupObject = 0;
    string64 new_name;
    Scene.GenObjectName(ClassID,new_name,Name);
    Name = new_name;
    Scene.AddObject(this,false);
    Select(true);
}

void CCustomObject::AppendToGroup(CGroupObject* group)
{
	R_ASSERT(group&&!m_pGroupObject);
    m_pGroupObject = group;
    Scene.RemoveObject(this,false);
    Select(false);
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

    UpdateTransform();

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
    F.Wvector		(PPosition);
    F.Wvector		(PRotation);
    F.Wvector		(PScale);
	F.close_chunk	();
}
//----------------------------------------------------

void CCustomObject::OnFrame()
{
	if (m_bUpdateTransform) OnUpdateTransform();
}

void CCustomObject::Move(Fvector& amount)
{
	R_ASSERT(!Locked());
    UI.UpdateScene();
    Fvector v=PPosition;
    if (fraTopBar->ebMoveToSnap->Down){
        SRayPickInfo pinf;
		Fmatrix	mR=FTransformRP;
        Fvector up,dn={0,-1,0};
        mR.transform_dir(dn);
        up.invert(dn);
        Fvector s2,s1=v;
        s1.add(amount);
        s2.mad(s1,up,frmEditorPreferences->seSnapMoveTo->Value);

        bool bVis=m_bVisible;
        m_bVisible=false;
        pinf.inf.range=frmEditorPreferences->seSnapMoveTo->Value;
        if (Scene.RayPick( s1, dn, OBJCLASS_SCENEOBJECT, &pinf, false, Scene.GetSnapList())||
        	Scene.RayPick( s2, dn, OBJCLASS_SCENEOBJECT, &pinf, false, Scene.GetSnapList())){
	            v.set(pinf.pt);
    			if (fraTopBar->ebNormalAlignment->Down){
					Fvector verts[3];
					pinf.s_obj->GetFaceWorld(pinf.e_mesh,pinf.inf.id,verts);
                    Fvector vR,vD,vN,r;
                    vN.mknormal(verts[0],verts[1],verts[2]);

                    vD.set(mR.k);
					vR.crossproduct	(vN,vD);
                    vR.normalize();
					vD.crossproduct	(vR,vN);
                    vD.normalize();

                    Fmatrix M;
                    M.set(vR,vN,vD,vR);
                    M.getXYZ(r);

                    PRotation = r;
				}
            }
        else v.add(amount);
        m_bVisible=bVis;
    }else{
	    v.add(amount);
    }

    PPosition = v;
}

void CCustomObject::MoveTo(const Fvector& pos, const Fvector& up)
{
	R_ASSERT(!Locked());
    UI.UpdateScene();
    Fvector v=PPosition;
    v.set(pos);
    if (fraTopBar->ebNormalAlignment->Down){
		Fmatrix	mR;
        Fvector vR,vD,vN,r;
        vN.set(up);
        vD.set(0,0,1);
		if (fabsf(vN.z)>0.99f) vD.set(1,0,0);
		vR.crossproduct(vN,vD); vR.normalize();
        vD.crossproduct(vR,vN); vD.normalize();
        mR.set(vR,vN,vD,vR);
        mR.getXYZ(r);
        PRotation = r;
    }
    PPosition = v;
}

void CCustomObject::PivotRotateParent(const Fmatrix& prev_inv, const Fmatrix& current, Fvector& axis, float angle)
{
    Fvector p		= PPosition;
    prev_inv.transform_tiny	(p);
    current.transform_tiny	(p);
    PPosition 			= p;

    Fvector r	= PRotation;
    r.mad		(axis,angle);
    PRotation	= r;
}

void CCustomObject::PivotRotateLocal(const Fmatrix& parent, Fvector& center, Fvector& axis, float angle)
{
	R_ASSERT(!Locked());
    UI.UpdateScene();

	Fmatrix m;
    // rotation
	m.rotation	(axis, angle);
    Fvector r;
    FTransformRP.mulB(m);
    FTransformRP.getXYZ(r);
    PRotation	= r;

    // position
    Fvector A;
    parent.transform_dir(A,axis);
	m.rotation	(A, angle);
    Fvector p	= PPosition;
	p.sub		(center);
    m.transform_tiny(p);
	p.add		(center);
	PPosition 	= p;
}

void CCustomObject::RotateParent(Fvector& axis, float angle)
{
	R_ASSERT(!Locked());
    UI.UpdateScene();
    Fvector r	= PRotation;
    r.mad		(axis,angle);
    PRotation		= r;
}

void CCustomObject::RotateLocal(Fvector& axis, float angle)
{
    Fmatrix m;
    Fvector r;
    m.rotation(axis,angle);
    FTransformRP.mulB(m);
    FTransformRP.getXYZ(r);
    PRotation		= r;
}

void CCustomObject::PivotScale( const Fmatrix& prev_inv, const Fmatrix& current, Fvector& amount )
{
	R_ASSERT(!Locked());
    UI.UpdateScene();
    Fvector p	= PPosition;
    Fvector s	= PScale;
	s.add(amount);
	if (s.x<EPS) s.x=EPS;
	if (s.y<EPS) s.y=EPS;
	if (s.z<EPS) s.z=EPS;
    PScale		= s;

    // translate position
    prev_inv.transform_tiny	(p);
    current.transform_tiny	(p);

    PPosition	= p;
}

void CCustomObject::Scale( Fvector& amount )
{
	R_ASSERT(!Locked());
    UI.UpdateScene();                                                       
    Fvector s	= PScale;
	s.add(amount);
    if (s.x<EPS) s.x=EPS;
    if (s.y<EPS) s.y=EPS;                                 
    if (s.z<EPS) s.z=EPS;
    PScale		= s;
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

