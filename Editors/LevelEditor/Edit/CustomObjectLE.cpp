#include "stdafx.h"
#pragma hdrstop

#include "scene.h"
#include "sceneobject.h"
#include "topbar.h"
#include "editorpref.h"
#include "ui_main.h"
#include "PropertiesListHelper.h"

//------------------------------------------------------------------------------
// static part
//------------------------------------------------------------------------------
void CCustomObject::SnapMove(Fvector& pos, Fvector& rot, const Fmatrix& rotRP, const Fvector& amount)
{
// !!! Hide object before test
    SRayPickInfo pinf;
    Fvector up,dn={0,-1,0};
    rotRP.transform_dir(dn);
    up.invert(dn);
    Fvector s2,s1=pos;
    s1.add(amount);
    s2.mad(s1,up,frmEditPrefs->seSnapMoveTo->Value);

    pinf.inf.range=frmEditPrefs->seSnapMoveTo->Value;
    if (Scene.RayPickObject( pinf.inf.range, s1, dn, OBJCLASS_SCENEOBJECT, &pinf, Scene.GetSnapList(false))||Scene.RayPickObject( pinf.inf.range, s2, dn, OBJCLASS_SCENEOBJECT, &pinf, Scene.GetSnapList(false))){
            pos.set(pinf.pt);
            if (fraTopBar->ebNormalAlignment->Down){
                Fvector verts[3];
                pinf.s_obj->GetFaceWorld(pinf.e_mesh,pinf.inf.id,verts);
                Fvector vR,vD,vN;
                vN.mknormal(verts[0],verts[1],verts[2]);

                vD.set(rotRP.k);
                vR.crossproduct	(vN,vD);
                vR.normalize();
                vD.crossproduct	(vR,vN);
                vD.normalize();

                Fmatrix M;
                M.set(vR,vN,vD,vR);
                M.getXYZ(rot);
            }
        }
    else pos.add(amount);
}
void CCustomObject::NormalAlign(Fvector& rot, const Fvector& up)
{
    Fmatrix	mR;
    Fvector vR,vD,vN;
    vN.set(up);
    vD.set(0,0,1);
    if (fabsf(vN.z)>0.99f) vD.set(1,0,0);
    vR.crossproduct(vN,vD); vR.normalize();
    vD.crossproduct(vR,vN); vD.normalize();
    mR.set(vR,vN,vD,vR);
    mR.getXYZ(rot);
}
//------------------------------------------------------------------------------

void CCustomObject::OnDetach()
{
    m_pOwnerObject 		= 0;
    string64 			new_name;
    Scene.GenObjectName(ClassID,new_name,Name);
    Name 				= new_name;
    Scene.AppendObject	(this,false);
    Select				(true);
}

void CCustomObject::OnAttach(CCustomObject* owner)
{
	R_ASSERT(owner);
    R_ASSERT2(!m_pOwnerObject,"Object already has owner!");
    m_pOwnerObject 		= owner;
    Scene.RemoveObject	(this,false);
    Select				(false);
}

void CCustomObject::Move(Fvector& amount)
{
	R_ASSERT(!Locked());
    UI.UpdateScene();
    Fvector v=PPosition;
    Fvector r=PRotation;
    if (fraTopBar->ebMoveToSnap->Down){
        BOOL bVis	= Visible();
        Show		(FALSE);
    	SnapMove	(v,r,FTransformRP,amount);
        Show		(bVis);
    }else{
	    v.add(amount);
    }
    PPosition = v;
    PRotation = r;
}

void CCustomObject::MoveTo(const Fvector& pos, const Fvector& up)
{
	R_ASSERT(!Locked());
    UI.UpdateScene();
    Fvector v=PPosition;
    v.set(pos);
    if (fraTopBar->ebNormalAlignment->Down){
        Fvector r=PRotation;
    	NormalAlign(r,up);
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

void __fastcall CCustomObject::OnObjectNameAfterEdit(PropItem* sender, LPVOID edit_val)
{
	TextValue* V = (TextValue*)sender->GetFrontValue();
	AnsiString* new_name = (AnsiString*)edit_val;
	if (Scene.FindObjectByName(new_name->c_str(),0)) *new_name = V->GetValue();
    else *new_name = new_name->LowerCase();
}

void CCustomObject::FillProp(LPCSTR pref, PropItemVec& items)
{
    PropValue* V = PHelper.CreateText(items,FHelper.PrepareKey(pref, "Name"),FName,sizeof(FName));
    V->Owner()->OnAfterEditEvent = OnObjectNameAfterEdit;
    if (V->Owner()->m_Flags.is(PropItem::flMixed)) V->Owner()->m_Flags.set(PropItem::flDisabled,TRUE);
    PHelper.CreateVector	(items, FHelper.PrepareKey(pref,"Transform\\Position"),	&PPosition,	-10000,	10000,0.01,2);
    PHelper.CreateAngle3	(items, FHelper.PrepareKey(pref,"Transform\\Rotation"),	&PRotation,	-10000,	10000,0.1,1);
    PHelper.CreateVector	(items, FHelper.PrepareKey(pref,"Transform\\Scale"),	&PScale, 	0.01,	10000,0.01,2);
}
//----------------------------------------------------

void CCustomObject::OnShowHint(AStringVec& dest)
{
    dest.push_back(AnsiString("Class: ")+AnsiString(ParentTools->ClassDesc()));
    dest.push_back(AnsiString("Name:  ")+AnsiString(Name));
    dest.push_back(AnsiString("-------"));
}
//----------------------------------------------------

