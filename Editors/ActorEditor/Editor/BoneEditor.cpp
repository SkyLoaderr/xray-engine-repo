#include "stdafx.h"
#pragma hdrstop

#include "bone.h"
#include "envelope.h"
#include "topbar.h"

void SJointIKData::clamp_by_limits(Fvector& dest_xyz)
{
	switch (type){
    case jtRigid:
		dest_xyz.set(0.f,0.f,0.f);
    break;
    case jtJoint:
		clamp(dest_xyz.x,limits[0].limit.x,limits[0].limit.y);
		clamp(dest_xyz.y,limits[1].limit.x,limits[1].limit.y);
		clamp(dest_xyz.z,limits[2].limit.x,limits[2].limit.y);
    break;
    case jtWheelXZ:
		clamp(dest_xyz.x,limits[0].limit.x,limits[0].limit.y);		dest_xyz.y=0;
    break;
    case jtWheelXY:
		clamp(dest_xyz.x,limits[0].limit.x,limits[0].limit.y);		dest_xyz.z=0;
    break;
    case jtWheelYX:
		clamp(dest_xyz.y,limits[1].limit.x,limits[1].limit.y);		dest_xyz.z=0;
    break;
    case jtWheelYZ:
		clamp(dest_xyz.y,limits[1].limit.x,limits[1].limit.y);		dest_xyz.x=0;
    break;
    case jtWheelZX:
		clamp(dest_xyz.z,limits[2].limit.x,limits[2].limit.y);		dest_xyz.y=0;
    break;
    case jtWheelZY:
		clamp(dest_xyz.z,limits[2].limit.x,limits[2].limit.y);		dest_xyz.x=0;
    break;
    }
}

void CBone::ShapeScale(const Fvector& _amount)
{
	switch (shape.type){
    case SBoneShape::stBox:{
        Fvector amount=_amount;
        Fmatrix _IT;_IT.invert(_LTransform());
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
	Fmatrix _IT;_IT.invert(_LTransform());
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
	Fmatrix _IT;_IT.invert(_LTransform());
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
    Fmatrix M; M.mul(parent,_LTransform());
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
        // bind pose CS
            mot_rotate.x += _axis.x*angle; 
            mot_rotate.y += _axis.y*angle;
            mot_rotate.z += _axis.z*angle;
        	
        	ClampByLimits		();
/*            
			Fmatrix mBind,mBindI,mLocal,mRotate,mLocalBP;
            mBind.setXYZi		(rest_rotate);
            mBindI.invert		(mBind);
            mLocal.setXYZi		(mot_rotate);
            Fvector axis;
            mBind.transform		(axis,_axis);
            mRotate.rotation	(axis,angle);
            mLocal.mulA			(mRotate);

            mLocalBP.mul		(mBindI,mLocal);
			Fvector mot;
            mLocalBP.getXYZi	(mot);

            IK_data.clamp_by_limits(mot);

            mLocalBP.setXYZi	(mot);
            mLocal.mul			(mBind,mLocalBP);
            mLocal.getXYZi		(mot_rotate);
*/
        }else{
        // local CS
			Fmatrix mBind,mBindI,mRotate,mLocal,mLocalBP;
            mBind.setXYZi		(rest_rotate);
            mBindI.invert		(mBind);

            Fvector axis;
            _MTransform().transform_dir(axis,_axis);
    
            // rotation
            mRotate.rotation	(axis, angle);
            mLocal.mul			(mRotate,_MTransform());
            mLocal.getXYZi		(mot_rotate); 

            // local clamp
			Fvector mot;
            mLocalBP.mul		(mBindI,mLocal);
            mLocalBP.getXYZi	(mot);

		    IK_data.clamp_by_limits(mot);

            mLocalBP.setXYZi	(mot);
            mLocal.mul			(mBind,mLocalBP);
            mLocal.getXYZi		(mot_rotate);
        }
    }
}

void CBone::ClampByLimits()
{
    Fmatrix mBind,mBindI,mLocal,mRotate,mLocalBP;
    mBind.setXYZi		(rest_rotate);
    mBindI.invert		(mBind);

    mLocal.setXYZi		(mot_rotate);
    mLocalBP.mul		(mBindI,mLocal);
    Fvector mot;
    mLocalBP.getXYZi	(mot);

    IK_data.clamp_by_limits(mot);

    mLocalBP.setXYZi	(mot);
    mLocal.mul			(mBind,mLocalBP);
    mLocal.getXYZi		(mot_rotate);
}

bool CBone::ExportOGF(IWriter& F)
{
	if (!shape.Valid()) return false;
    F.w_stringZ	(game_mtl);
    F.w			(&shape,sizeof(SBoneShape));

    F.w_u32		(IK_data.type);
    for (int k=0; k<3; k++){
    	F.w_float	(_min(-IK_data.limits[k].limit.x,-IK_data.limits[k].limit.y)); // min (swap special for ODE)
    	F.w_float	(_max(-IK_data.limits[k].limit.x,-IK_data.limits[k].limit.y)); // max (swap special for ODE)
        F.w_float	(IK_data.limits[k].spring_factor);
        F.w_float	(IK_data.limits[k].damping_factor);
    }
    F.w_float	(IK_data.spring_factor);
    F.w_float	(IK_data.damping_factor);

    Fvector xyz;
    Fmatrix& R	= _RTransform();
	R.getXYZi	(xyz);

    F.w_fvector3(xyz);
    F.w_fvector3(R.c);
    F.w_float	(mass);
    F.w_fvector3(center_of_mass);
    return true;
}
