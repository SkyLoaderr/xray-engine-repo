#include "stdafx.h"
#pragma hdrstop

#include "UI_Main.h"
#include "topbar.h"
#include "CustomObject.h"

void CCustomObject::OnShowHint(AStringVec& dest)
{
}

void CCustomObject::FillProp(LPCSTR pref, PropItemVec& values)
{
}

void CCustomObject::Move(Fvector& amount)
{
	R_ASSERT(!Locked());
    UI->UpdateScene();
    Fvector v=PPosition;
	v.add(amount);
    PPosition = v;
}

void CCustomObject::MoveTo(const Fvector& pos, const Fvector& up)
{
	R_ASSERT(!Locked());
    UI->UpdateScene();
    PPosition = pos;
}

void CCustomObject::RotatePivot(const Fmatrix& prev_inv, const Fmatrix& current)
{
    Fmatrix 		Ol,On;
    Ol.mul			(prev_inv,FTransformRP);
    On.mul			(current,Ol);
    Fvector 		xyz;
    On.getXYZ		(xyz);
    PRotation		= xyz;
    PPosition		= On.c;
}

void CCustomObject::RotateParent(Fvector& axis, float angle)
{
	R_ASSERT(!Locked());
    UI->UpdateScene();
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

void CCustomObject::ScalePivot( const Fmatrix& prev_inv, const Fmatrix& current, Fvector& amount )
{
	R_ASSERT(!Locked());
    UI->UpdateScene();
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
    UI->UpdateScene();                                                       
    Fvector s	= PScale;
	s.add(amount);
    if (s.x<EPS) s.x=EPS;
    if (s.y<EPS) s.y=EPS;                                 
    if (s.z<EPS) s.z=EPS;
    PScale		= s;
}


