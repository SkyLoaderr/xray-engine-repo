#include "stdafx.h"

#include "scene.h"
#include "sceneobject.h"
#include "GroupObject.h"
#include "UI_Main.h"
#include "topbar.h"
#include "editorpref.h"

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


