#include "stdafx.h"
#include "car.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCar::SDoor::Init()
{
	update=false;
	joint=bone_map.find(bone_id)->second.joint;
	if(!joint) return;
	Fvector door_position,door_axis;
	dJointGetHingeAnchor (joint->GetDJoint(),(float*) &door_position);
	dJointGetHingeAxis (joint->GetDJoint(), (float*) &door_axis);
	door_position.sub(pcar->XFORM().c);

	Fmatrix door_transform;
	joint->PSecond_element()->InterpolateGlobalTransform(&door_transform);
	closed_door_form_in_object.set(joint->PSecond_element()->mXFORM);
	Fvector jaxis,janchor;

	float lo_ext,hi_ext,ext;
	joint->GetAxisDirDynamic(0,jaxis);
	joint->GetAnchorDynamic(janchor);
	joint->PSecond_element()->get_Extensions(jaxis,janchor.dotproduct(jaxis),lo_ext,hi_ext);
	door_plane_ext.x=hi_ext-lo_ext;
	Fvector jaxis_in_door;
	Fmatrix inv_door_transform;
	inv_door_transform.set(door_transform);
	inv_door_transform.invert();
	inv_door_transform.transform_dir(jaxis_in_door,jaxis);
	if(jaxis_in_door.x>jaxis_in_door.y)
	{
		if(jaxis_in_door.x>jaxis_in_door.z)
		{
			joint->PSecond_element()->get_Extensions(door_transform.j,janchor.dotproduct(door_transform.j),lo_ext,hi_ext);
			door_plane_ext.y=hi_ext-lo_ext;
			door_plane_axes.x=0;
			door_plane_axes.y=1;
			joint->PSecond_element()->get_Extensions(door_transform.k,janchor.dotproduct(door_transform.k),lo_ext,hi_ext);
			ext=hi_ext-lo_ext;
			if(ext>door_plane_ext.y)
			{
				door_plane_ext.y=ext;
				door_plane_axes.y=2;
			}
		}
		else
		{
			joint->PSecond_element()->get_Extensions(door_transform.j,janchor.dotproduct(door_transform.j),lo_ext,hi_ext);
			door_plane_ext.y=hi_ext-lo_ext;
			door_plane_axes.x=2;
			door_plane_axes.y=1;
			joint->PSecond_element()->get_Extensions(door_transform.i,janchor.dotproduct(door_transform.i),lo_ext,hi_ext);
			ext=hi_ext-lo_ext;
			if(ext>door_plane_ext.y)
			{
				door_plane_ext.y=ext;
				door_plane_axes.y=0;
			}
		}
	}
	else
	{
		if(jaxis_in_door.y>jaxis_in_door.z)	
		{
			joint->PSecond_element()->get_Extensions(door_transform.i,janchor.dotproduct(door_transform.i),lo_ext,hi_ext);
			door_plane_ext.y=hi_ext-lo_ext;
			door_plane_axes.x=1;
			door_plane_axes.y=0;
			joint->PSecond_element()->get_Extensions(door_transform.k,janchor.dotproduct(door_transform.k),lo_ext,hi_ext);
			ext=hi_ext-lo_ext;
			if(ext>door_plane_ext.y)
			{
				door_plane_ext.y=ext;
				door_plane_axes.y=2;
			}
		}
		else
		{
			joint->PSecond_element()->get_Extensions(door_transform.j,janchor.dotproduct(door_transform.j),lo_ext,hi_ext);
			door_plane_ext.y=hi_ext-lo_ext;
			door_plane_axes.x=2;
			door_plane_axes.y=1;
			joint->PSecond_element()->get_Extensions(door_transform.i,janchor.dotproduct(door_transform.i),lo_ext,hi_ext);
			ext=hi_ext-lo_ext;
			if(ext>door_plane_ext.y)
			{
				door_plane_ext.y=ext;
				door_plane_axes.y=0;
			}
		}
	}
	switch(door_plane_axes.y) 
	{
	case 0:
		door_dir_in_door.set(1.f,0.f,0.f);
		break;
	case 1:
		door_dir_in_door.set(0.f,1.f,0.f);
		break;
	case 2:
		door_dir_in_door.set(0.f,0.f,1.f);
		break;
	default: NODEFAULT;
	}
	///////////////////////////define positive open///////////////////////////////////
	Fvector door_dir,cr_dr_pos;
	door_transform.transform_dir(door_dir,door_dir_in_door);
	cr_dr_pos.crossproduct(door_dir,door_position);
	pos_open=-cr_dr_pos.dotproduct(door_axis);
	//pos_open=door_position.dotproduct(pcar->m_root_transform.i)*door_axis.dotproduct(pcar->m_root_transform.j);
	if(pos_open>0.f)
	{
		pos_open=1.f;
		joint->GetLimits(closed_angle,opened_angle,0);
		//	closed_angle+=2.f*M_PI/180.f;
		//opened_angle-=4.f*M_PI/180.f;
		opened_angle-=opened_angle/4.f;
	}
	else
	{
		pos_open=-1.f;
		joint->GetLimits(opened_angle,closed_angle,0);
		opened_angle+=2.f*M_PI/180.f;
		closed_angle-=2.f*M_PI/180.f;
	}

	Close();
}
void CCar::SDoor::Open()
{

	if(!joint)
	{
		state=opened;

		return;
	}


	if(state!=closing)
	{
		ClosedToOpening();
		PlaceInUpdate();
	}

	state=opening;
	ApplyOpenTorque();
}

void CCar::SDoor::Close()
{
	if(!joint)
	{
		state=closed;
		return;
	}
	if(state!=opening)
	{
		PlaceInUpdate();
	}

	state=closing;
	ApplyCloseTorque();

}

void CCar::SDoor::PlaceInUpdate()
{
	if(update) return;
	pcar->m_doors_update.push_back(this);
	//list_iterator=(--pcar->m_doors_update.end());
	update=true;
}

void CCar::SDoor::RemoveFromUpdate()
{
	update=false;
}
void CCar::SDoor::Update()
{
	switch(state) 
	{
	case closing:
		{
			if(pos_open*closed_angle>pos_open*GetAngle()) ClosingToClosed();

			break;
		}
	case opening:
		{
			if(pos_open*opened_angle<pos_open*GetAngle()) 
			{
				NeutralTorque(torque);
				open_time=Device.dwTimeGlobal;
				state=opened;
			}
			break;

		}
	case opened:
		{
			if(Device.dwTimeGlobal-open_time>1000) 
			{
				NeutralTorque(0.f);
				RemoveFromUpdate();
			}
		}
	}
}

void CCar::SDoor::Use()
{
	switch(state) {
case opened:
case opening: 
	Close();
	break;
case closed: 
case closing:
	Open();
	break;
default:	return;
	}
}

void CCar::SDoor::ApplyOpenTorque()
{
	if(!joint->bActive)return;
	joint->PSecond_element()->Enable();
	dJointSetHingeParam(joint->GetDJoint(),dParamFMax,torque);
	dJointSetHingeParam(joint->GetDJoint(),dParamVel,a_vel*pos_open);
}

void CCar::SDoor::ApplyCloseTorque()
{
	if(!joint->bActive)return;
	joint->PSecond_element()->Enable();
	dJointSetHingeParam(joint->GetDJoint(),dParamFMax,torque);
	dJointSetHingeParam(joint->GetDJoint(),dParamVel,-a_vel*pos_open);
}

void CCar::SDoor::NeutralTorque(float atorque)
{
	if(!joint->bActive)return;
	//joint->PSecond_element()->Enable();
	dJointSetHingeParam(joint->GetDJoint(),dParamFMax,atorque);
	dJointSetHingeParam(joint->GetDJoint(),dParamVel,0);
}



void CCar::SDoor::ClosedToOpening()
{
	if(joint->bActive)return;
	Fmatrix door_form,root_form;
	CKinematics* pKinematics=PKinematics(pcar->Visual());
	CBoneData& bone_data= pKinematics->LL_GetData(bone_id);
	CBoneInstance& bone_instance=pKinematics->LL_GetInstance(bone_id);
	bone_instance.set_callback(pcar->PPhysicsShell()->GetBonesCallback1(),joint->PSecond_element());
	door_form.setXYZi(bone_data.bind_xyz);
	door_form.c.set(bone_data.bind_translate);
	//door_form.mulB(pcar->XFORM());
	joint->PSecond_element()->mXFORM.set(door_form);
	pcar->m_pPhysicsShell->GetGlobalTransformDynamic(&root_form);
	joint->PSecond_element()->Activate(root_form,false);
	joint->Activate();
	pKinematics->Calculate();
}

void CCar::SDoor::ClosingToClosed()
{
	state =closed;
	PKinematics(pcar->Visual())->Calculate();

	Fmatrix door_form;
	CKinematics* pKinematics=PKinematics(pcar->Visual());
	CBoneData& bone_data= pKinematics->LL_GetData(bone_id);
	CBoneInstance& bone_instance=pKinematics->LL_GetInstance(bone_id);
	bone_instance.set_callback(0,joint->PFirst_element());
	joint->PSecond_element()->Deactivate();
	joint->Deactivate();
	RemoveFromUpdate();

	door_form.setXYZi(bone_data.bind_xyz);
	door_form.c.set(bone_data.bind_translate);
	bone_instance.mTransform.set(door_form);
}



float CCar::SDoor::GetAngle()
{
	if(!joint->bActive) return 0.f;
	return dJointGetHingeAngle(joint->GetDJoint());
}



bool CCar::SDoor::IsInArea(const Fvector& pos)
{
	Fmatrix closed_door_form,door_form;
	Fvector closed_door_dir,door_dir,anchor_to_pos,door_axis;
	joint->GetAxisDirDynamic(0,door_axis);
	joint->PSecond_element()->InterpolateGlobalTransform(&door_form);
	closed_door_form.mul(pcar->XFORM(),closed_door_form_in_object);
	closed_door_form.transform_dir(closed_door_dir,door_dir_in_door);
	door_form.transform_dir(door_dir,door_dir_in_door);
	door_dir.normalize();
	closed_door_dir.normalize();
	float cprg=door_dir.dotproduct(door_form.c);
	float loe,hie;
	joint->PSecond_element()->get_Extensions(door_dir,cprg,loe,hie);
	float signum=(hie>-loe) ? 1.f : -1.f;

	Fvector closed_door_norm,door_norm;
	closed_door_norm.crossproduct(door_axis,closed_door_dir);
	door_norm.crossproduct(door_axis,door_dir);
	anchor_to_pos.sub(pos,closed_door_form.c);
	float a,b,c;
	a=anchor_to_pos.dotproduct(closed_door_dir)*signum;
	b=anchor_to_pos.dotproduct(door_dir)*signum;
	c= anchor_to_pos.dotproduct(closed_door_norm)*anchor_to_pos.dotproduct(door_norm);
	if(anchor_to_pos.dotproduct(closed_door_dir)*signum	>0.f&&
		anchor_to_pos.dotproduct(door_dir)*signum		>0.f&&
		anchor_to_pos.dotproduct(closed_door_norm)*anchor_to_pos.dotproduct(door_norm)<0.f
		)return true;
	else return false;
}

bool CCar::SDoor::CanExit(const Fvector& pos,const Fvector& dir)
{
	//if(state==opened) return true;
	//return false;
	if(!joint) return true;//temp for fake doors
	if(state==closed)return false;
	return TestPass(pos,dir);
}

void CCar::SDoor::GetExitPosition(Fvector& pos)
{
	if(!joint) 
	{
		pos.set(pcar->Position());
		return;
	}
	float lo_ext,hi_ext;
	Fvector door_axis,door_pos,door_dir,closed_door_dir,add;
	joint->GetAxisDirDynamic(0,door_axis);
	joint->GetAnchorDynamic(door_pos);

	Fmatrix door_form,closed_door_form,root_form;
	root_form.mul(pcar->m_root_transform,pcar->XFORM());
	joint->PSecond_element()->InterpolateGlobalTransform(&door_form);
	door_form.transform_dir(door_dir,door_dir_in_door);
	closed_door_form.mul(closed_door_form_in_object,pcar->XFORM());
	closed_door_form.transform_dir(closed_door_dir,door_dir_in_door);


	pos.set(door_pos);
	door_axis.normalize();
	float center_prg=door_axis.dotproduct(door_pos);
	joint->PSecond_element()->get_Extensions(door_axis,center_prg,lo_ext,hi_ext);
	add.set(door_axis);
	if(door_axis.dotproduct(root_form.j)>0.f) add.mul(lo_ext);
	else									  add.mul(hi_ext);
	pos.add(add);

	door_dir.normalize();
	center_prg=door_pos.dotproduct(door_dir);
	joint->PSecond_element()->get_Extensions(door_dir,center_prg,lo_ext,hi_ext);
	closed_door_dir.normalize();
	add.set(closed_door_dir);
	if(hi_ext>-lo_ext)add.mul(hi_ext);
	else			  add.mul(lo_ext);
	pos.add(add);
}


bool CCar::SDoor::TestPass(const Fvector& pos,const Fvector& dir)
{
	if(!joint) return false;
	float lo_ext,hi_ext;
	Fvector door_axis,door_pos,door_dir,closed_door_dir;
	joint->GetAxisDirDynamic(0,door_axis);
	joint->GetAnchorDynamic(door_pos);

	Fmatrix door_form,closed_door_form,root_form;
	root_form.mul(pcar->m_root_transform,pcar->XFORM());
	joint->PSecond_element()->InterpolateGlobalTransform(&door_form);
	door_form.transform_dir(door_dir,door_dir_in_door);
	closed_door_form.mul(closed_door_form_in_object,pcar->XFORM());
	closed_door_form.transform_dir(closed_door_dir,door_dir_in_door);

	door_axis.normalize();

	door_dir.normalize();
	closed_door_dir.normalize();

	Fvector closed_door_norm;

	closed_door_norm.crossproduct(door_axis,closed_door_dir);

	Fvector point_on_door,add,sub;
	add.set(dir);
	sub.sub(pos,door_pos);
	add.mul(-sub.dotproduct(closed_door_norm)/(dir.dotproduct(closed_door_norm)));
	point_on_door.add(pos,add);

	float center_prg=door_pos.dotproduct(door_dir);
	joint->PSecond_element()->get_Extensions(door_dir,center_prg,lo_ext,hi_ext);

	float point_prg=point_on_door.dotproduct(closed_door_dir);
	center_prg=door_pos.dotproduct(closed_door_dir);
	if(!(center_prg+hi_ext>point_prg)||!(center_prg+lo_ext<point_prg)) return false;

	center_prg=door_axis.dotproduct(door_pos);
	joint->PSecond_element()->get_Extensions(door_axis,center_prg,lo_ext,hi_ext);

	point_prg=point_on_door.dotproduct(door_axis);
	if(!(center_prg+hi_ext>point_prg)||!(center_prg+lo_ext<point_prg)) return false;

	return true;
}

bool CCar::SDoor::CanEnter(const Fvector& pos,const Fvector& dir)
{
	if(!joint) return true;//temp for fake doors
	return state==opened && IsInArea(pos) && TestPass(pos,dir);
}