#include "stdafx.h"
#pragma hdrstop

#include "bone.h"
#include "envelope.h"
#include "topbar.h"

void SJointIKData::clamp_by_limits(const Fvector& basis_hpb, Fvector& dest_hpb)
{
	switch (type){
    case jtRigid:
		dest_hpb.set(basis_hpb);
    break;
    case jtJoint:
		clamp(dest_hpb.x,basis_hpb.x+limits[1].limit.x,basis_hpb.x+limits[1].limit.y);
		clamp(dest_hpb.y,basis_hpb.y+limits[0].limit.x,basis_hpb.y+limits[0].limit.y);
		clamp(dest_hpb.z,basis_hpb.z+limits[2].limit.x,basis_hpb.z+limits[2].limit.y);
    break;
    case jtWheel:
//		clamp(dest_hpb.x,basis_hpb.x+limits[0].limit.x,basis_hpb.x+limits[0].limit.y);
		clamp(dest_hpb.y,basis_hpb.x+limits[0].limit.x,basis_hpb.y+limits[0].limit.y);
		dest_hpb.y=basis_hpb.y;
    break;
    }
}

void CBone::ShapeScale(const Fvector& _amount)
{
	switch (shape.type){
    case SBoneShape::stBox:{
        Fvector amount=_amount;
        Fmatrix _IT;_IT.invert(LTransform());
        _IT.transform_dir(amount,_amount);
        if (fraTopBar->ebCSParent->Down) _IT.transform_dir(amount);
    	shape.box.m_halfsize.add(amount);		
        if (shape.box.m_halfsize.x<EPS) shape.box.m_halfsize.x=EPS;
        if (shape.box.m_halfsize.y<EPS) shape.box.m_halfsize.y=EPS;
        if (shape.box.m_halfsize.z<EPS) shape.box.m_halfsize.z=EPS;
    }break;
    case SBoneShape::stSphere:
    	shape.sphere.R += _amount.x;			if (shape.sphere.R<EPS) shape.sphere.R=EPS;
    break;
    case SBoneShape::stCylinder:
    	shape.cylinder.m_height += _amount.z; 	if (shape.cylinder.m_height<EPS) shape.cylinder.m_height=EPS;
    	shape.cylinder.m_radius += _amount.x; 	if (shape.cylinder.m_radius<EPS) shape.cylinder.m_radius=EPS;
    break;
    }
}

void CBone::ShapeRotate(const Fvector& _amount)
{
    Fvector amount=_amount;
	Fmatrix _IT;_IT.invert(LTransform());
    if (fraTopBar->ebCSParent->Down) _IT.transform_dir(amount);
	switch (shape.type){
    case SBoneShape::stBox:{
    	Fmatrix R;
        R.setHPB(amount.y,amount.x,amount.z);
        shape.box.transform(shape.box,R);
    }break;
    case SBoneShape::stSphere:	break;
    case SBoneShape::stCylinder:{
    	Fmatrix R;
        R.setHPB(amount.y,amount.x,amount.z);
        R.transform_dir(shape.cylinder.m_direction);
    }break;
    }
}

void CBone::ShapeMove(const Fvector& _amount)
{
    Fvector amount=_amount;
	Fmatrix _IT;_IT.invert(LTransform());
    if (fraTopBar->ebCSParent->Down) _IT.transform_dir(amount);
	switch (shape.type){
    case SBoneShape::stBox:
    	shape.box.m_translate.add(amount);		
    break;
    case SBoneShape::stSphere:
    	shape.sphere.P.add(amount);
    break;
    case SBoneShape::stCylinder:{
    	shape.cylinder.m_center.add(amount);
    }break;
    }
}

bool CBone::Pick(float& dist, const Fvector& S, const Fvector& D, const Fmatrix& parent)
{
	Fvector start, dir;
    Fmatrix M; M.mul(parent,LTransform());
    M.invert();
    M.transform_tiny(start,S);
    M.transform_dir(dir,D);
	switch (shape.type){
    case SBoneShape::stBox:		return shape.box.intersect		(start,dir,dist);		
    case SBoneShape::stSphere:  return shape.sphere.intersect	(start,dir,dist);
    case SBoneShape::stCylinder:return shape.cylinder.intersect	(start,dir,dist);
    default:
    	Fsphere S;
        S.P.set(0,0,0);
        S.R=0.025f;
        return S.intersect(start,dir,dist);
    }
}

void CBone::BoneRotate(const Fvector& _axis, float angle)
{
    if (!fis_zero(angle)){
		if (fraTopBar->ebCSParent->Down){	
        // parent CS
            mot_rotate.x += _axis.y*angle; 
            mot_rotate.y += _axis.x*angle;
            mot_rotate.z += _axis.z*angle;

            // local clamp
			Fvector mot;
	    	Fmatrix C;
            C.setHPB			(mot_rotate.x,mot_rotate.y,mot_rotate.z);
			C.mulA				(RITransform());
            C.getHPB			(mot);

            Fvector zero		= {0,0,0};
		    IK_data.clamp_by_limits(zero,mot);

            if (fis_zero(_axis.x)) mot.y=0.f; 
            if (fis_zero(_axis.y)) mot.x=0.f; 
            if (fis_zero(_axis.z)) mot.z=0.f; 
            
            C.setHPB			(mot.x,mot.y,mot.z);
            C.mulA				(RTransform());
            
            C.getHPB			(mot_rotate);
        }else{								
        // local CS
            Fvector axis;
            MTransform().transform_dir(axis,_axis);
    
            // rotation
            Fmatrix m;	m.rotation(axis, angle);
            Fmatrix tRP;
            tRP.mul				(m,MTransform());
            tRP.getHPB			(mot_rotate); 
            mot_rotate.mul		(-1.f);

            // local clamp
			Fvector mot;
	    	Fmatrix C;
            C.setHPB			(mot_rotate.x,mot_rotate.y,mot_rotate.z);
			C.mulA				(RITransform());
            C.getHPB			(mot);

			Fvector zero		= {0,0,0};
		    IK_data.clamp_by_limits(zero,mot);

            C.setHPB			(mot.x,mot.y,mot.z);
            C.mulA				(RTransform());
            C.getHPB			(mot_rotate);
        }
    }
}

void CBone::ClampByLimits()
{
    IK_data.clamp_by_limits(rest_rotate,mot_rotate);
}

void CBone::ExportOGF(IWriter& F)
{
    F.w_stringZ	(game_mtl);
    F.w			(&shape,sizeof(SBoneShape));

    F.w_u32		(IK_data.type);
    for (int k=0; k<3; k++){
    	F.w_float	(_min(-IK_data.limits[k].limit.x,-IK_data.limits[k].limit.y)); // min
    	F.w_float	(_max(-IK_data.limits[k].limit.x,-IK_data.limits[k].limit.y)); // max
        F.w_float	(IK_data.limits[k].spring_factor);
        F.w_float	(IK_data.limits[k].damping_factor);
    }
    F.w_float	(IK_data.spring_factor);
    F.w_float	(IK_data.damping_factor);
    Fvector hpb; LTransform().getHPB(hpb);
    F.w_fvector3(hpb);
    F.w_fvector3(LTransform().c);
    F.w_float	(mass);
    F.w_fvector3(center_of_mass);
}
