#include "stdafx.h"
#include "car.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCar::SDoor::Init()
{
	update=false;
	joint=bone_map.find(bone_id)->second.joint;
	if(!joint) return;
	R_ASSERT2(dJointGetType(joint->GetDJoint())==dJointTypeHinge,"Wrong door joint!!! Only simple joint valid for a door and only one axis can be active, check other axes are zerro limited !!!");
	joint->SetBackRef(&joint);
	Fvector door_position,door_axis;
	dJointGetHingeAnchor (joint->GetDJoint(),(float*) &door_position);
	dJointGetHingeAxis (joint->GetDJoint(), (float*) &door_axis);
	door_position.sub(pcar->XFORM().c);

	Fmatrix door_transform;
	joint->PSecond_element()->InterpolateGlobalTransform(&door_transform);
	closed_door_form_in_object.set(joint->PSecond_element()->mXFORM);
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
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

	float	door_dir_sign;
	if(jaxis_in_door.x>jaxis_in_door.y)
	{
		if(jaxis_in_door.x>jaxis_in_door.z)
		{
			joint->PSecond_element()->get_Extensions(door_transform.j,janchor.dotproduct(door_transform.j),lo_ext,hi_ext);
			door_plane_ext.y=hi_ext-lo_ext;
			door_dir_sign=hi_ext>-lo_ext ? 1.f : -1.f;
			door_plane_axes.x=0;
			door_plane_axes.y=1;
			joint->PSecond_element()->get_Extensions(door_transform.k,janchor.dotproduct(door_transform.k),lo_ext,hi_ext);
			ext=hi_ext-lo_ext;
			if(ext>door_plane_ext.y)
			{
				door_dir_sign=hi_ext>-lo_ext ? 1.f : -1.f;
				door_plane_ext.y=ext;
				door_plane_axes.y=2;
			}
		}
		else
		{
			joint->PSecond_element()->get_Extensions(door_transform.j,janchor.dotproduct(door_transform.j),lo_ext,hi_ext);
			door_plane_ext.y=hi_ext-lo_ext;
			door_dir_sign=hi_ext>-lo_ext ? 1.f : -1.f;
			door_plane_axes.x=2;
			door_plane_axes.y=1;
			joint->PSecond_element()->get_Extensions(door_transform.i,janchor.dotproduct(door_transform.i),lo_ext,hi_ext);
			ext=hi_ext-lo_ext;
			if(ext>door_plane_ext.y)
			{
				door_dir_sign=hi_ext>-lo_ext ? 1.f : -1.f;
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
			door_dir_sign=hi_ext>-lo_ext ? 1.f : -1.f;
			door_plane_axes.x=1;
			door_plane_axes.y=0;
			joint->PSecond_element()->get_Extensions(door_transform.k,janchor.dotproduct(door_transform.k),lo_ext,hi_ext);
			ext=hi_ext-lo_ext;
			if(ext>door_plane_ext.y)
			{
				door_dir_sign=hi_ext>-lo_ext ? 1.f : -1.f;
				door_plane_ext.y=ext;
				door_plane_axes.y=2;
			}
		}
		else
		{
			joint->PSecond_element()->get_Extensions(door_transform.j,janchor.dotproduct(door_transform.j),lo_ext,hi_ext);
			door_plane_ext.y=hi_ext-lo_ext;
			door_dir_sign=hi_ext>-lo_ext ? 1.f : -1.f;
			door_plane_axes.x=2;
			door_plane_axes.y=1;
			joint->PSecond_element()->get_Extensions(door_transform.i,janchor.dotproduct(door_transform.i),lo_ext,hi_ext);
			ext=hi_ext-lo_ext;
			if(ext>door_plane_ext.y)
			{
				door_dir_sign=hi_ext>-lo_ext ? 1.f : -1.f;
				door_plane_ext.y=ext;
				door_plane_axes.y=0;
			}
		}
	}
	switch(door_plane_axes.y) 
	{
		case 0:
			door_dir_in_door.set(door_dir_sign,0.f,0.f);
			break;
		case 1:
			door_dir_in_door.set(0.f,door_dir_sign,0.f);
			break;
		case 2:
			door_dir_in_door.set(0.f,0.f,door_dir_sign);
			break;
		default: NODEFAULT;
	}
///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
	///////////////////////////define positive open///////////////////////////////////
   	Fvector door_dir,door_test;
	door_transform.transform_dir(door_dir,door_dir_in_door);
	//cr_dr_pos.crossproduct(door_dir,door_position);
	door_test.crossproduct(door_dir,door_axis);
	door_test.normalize();
	joint->PFirst_element()->get_Extensions(door_test,door_transform.c.dotproduct(door_test),lo_ext,hi_ext);
	if(hi_ext>-lo_ext)	pos_open=-1.f;
	 else				pos_open=1.f;
	//pos_open=-cr_dr_pos.dotproduct(door_axis);
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
	Fvector shoulder;

	shoulder.sub(door_transform.c,joint->PSecond_element()->mass_Center());
	torque=shoulder.magnitude()*joint->PSecond_element()->getMass()*pcar->m_doors_torque_factor*10.f;
	state=opened;
//	Close();
}
void CCar::SDoor::Open()
{

	if(!joint)
	{
		state=opened;

		return;
	}

	switch(state) 
	{
	case closed:
			ClosedToOpening();
			PlaceInUpdate();
	case closing:
			state=opening;
			ApplyOpenTorque();
	case opened:
	case opening: break;
	default: NODEFAULT;
	}

}

void CCar::SDoor::Close()
{
	if(!joint)
	{
		state=closed;
		return;
	}

	switch(state)
	{
	case opened:
		PlaceInUpdate();
	case opening:
		state=closing;
		ApplyCloseTorque();
	case closed:
	case closing:
		break;
	default: NODEFAULT;
	}

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
				ApplyTorque(torque/5.f,a_vel);
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

void CCar::SDoor::Switch()
{
	switch(state) {
case opened:
	Close();
	break;
case closed: 
	Open();
	break;
default:	return;
	}
}

void CCar::SDoor::ApplyTorque(float atorque,float aa_vel)
{
	if(!joint||!joint->bActive)return;
	joint->PSecond_element()->Enable();
	dJointSetHingeParam(joint->GetDJoint(),dParamFMax,atorque);
	dJointSetHingeParam(joint->GetDJoint(),dParamVel,aa_vel*pos_open);
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
	CKinematics* pKinematics=smart_cast<CKinematics*>(pcar->Visual());
//	CBoneData& bone_data= pKinematics->LL_GetData(u16(bone_id));
	CBoneInstance& bone_instance=pKinematics->LL_GetBoneInstance(u16(bone_id));
	bone_instance.set_callback(pcar->PPhysicsShell()->GetBonesCallback(),joint->PSecond_element());
	
	door_form.set(bone_instance.mTransform);
	//door_form.mulB(pcar->XFORM());
	joint->PSecond_element()->mXFORM.set(door_form);
	pcar->m_pPhysicsShell->GetGlobalTransformDynamic(&root_form);
	joint->PSecond_element()->Activate(root_form,false);
	pcar->m_pPhysicsShell->Enable();
	joint->Activate();
	pKinematics->CalculateBones();
}

void CCar::SDoor::ClosingToClosed()
{
	state =closed;
	if(!joint) return;
	smart_cast<CKinematics*>(pcar->Visual())->CalculateBones();

//	Fmatrix door_form;
	CKinematics* pKinematics=smart_cast<CKinematics*>(pcar->Visual());
//	CBoneData& bone_data= pKinematics->LL_GetData(u16(bone_id));
	CBoneInstance& bone_instance=pKinematics->LL_GetBoneInstance(u16(bone_id));
	bone_instance.set_callback(0,joint->PFirst_element());
	bone_instance.Callback_overwrite=FALSE;
	joint->PSecond_element()->Deactivate();
	joint->Deactivate();
	RemoveFromUpdate();

	//door_form.set(bone_data.bind_transform);
	//bone_instance.mTransform.set(door_form);
}



float CCar::SDoor::GetAngle()
{
	if(!joint||!joint->bActive) return 0.f;
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
	//closed_door_form_in_object.transform_dir(closed_door_dir,door_dir_in_door);
	//pcar->XFORM().transform_dir(closed_door_dir);


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
	if(
		a	<	(signum>0.f ? hie : -loe)	&&	a	>	0.f	&&
		b	<	(signum>0.f ? hie : -loe)	&&	b	>	0.f	&&
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

	Fmatrix door_form,root_form;
	root_form.mul(pcar->m_root_transform,pcar->XFORM());
	joint->PSecond_element()->InterpolateGlobalTransform(&door_form);
	door_form.transform_dir(door_dir,door_dir_in_door);

	
	closed_door_form_in_object.transform_dir(closed_door_dir,door_dir_in_door);
	pcar->XFORM().transform_dir(closed_door_dir);


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
	add.add(closed_door_dir,door_dir);
	add.normalize();
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

	Fmatrix door_form,root_form;
	root_form.mul(pcar->m_root_transform,pcar->XFORM());
	joint->PSecond_element()->InterpolateGlobalTransform(&door_form);
	door_form.transform_dir(door_dir,door_dir_in_door);
//	closed_door_form.mul(closed_door_form_in_object,pcar->XFORM());
	closed_door_form_in_object.transform_dir(closed_door_dir,door_dir_in_door);
	pcar->XFORM().transform_dir(closed_door_dir);
	door_axis.normalize();

	door_dir.normalize();
	closed_door_dir.normalize();

	Fvector closed_door_norm;

	closed_door_norm.crossproduct(door_axis,closed_door_dir);

	Fvector point_on_door,add,sub;
	add.set(dir);
	sub.sub(pos,door_pos);
	add.mul(-sub.dotproduct(closed_door_norm)/(dir.dotproduct(closed_door_norm)));
	
	if(add.dotproduct(dir)<0.f) return false;

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

bool CCar::SDoor::CanEnter(const Fvector& pos,const Fvector& dir,const Fvector& foot_pos)
{
	if(!joint) return true;//temp for fake doors
	return state==opened && TestPass(foot_pos,dir)&& IsInArea(pos);//
}

void CCar::SDoor::SaveNetState(NET_Packet& P)
{
	P.w_u8(u8(state));
}

void CCar::SDoor::RestoreNetState(u8 a_state)
{
	eState lstate=eState(a_state);
	if(lstate==closed)	ClosingToClosed();
}

void CCar::SDoor::SetDefaultNetState()
{
	ClosingToClosed();
}

CCar::SDoor::SDoorway::SDoorway()
{
	door=NULL;
	door_plane_ext.set(0.f,0.f);
	door_plane_axes.set(0,0);
}
void CCar::SDoor::SDoorway::Init(SDoor* adoor)
{
door=adoor;	
Fmatrix door_transform;
door->joint->PSecond_element()->InterpolateGlobalTransform(&door_transform);
door->closed_door_form_in_object.set(door->joint->PSecond_element()->mXFORM);
Fvector jaxis,janchor;
door->joint->GetAxisDirDynamic(0,jaxis);
door->joint->GetAnchorDynamic(janchor);
Fmatrix inv_door_transform;
inv_door_transform.set(door_transform);
inv_door_transform.invert();
Fvector door_axis_in_door;
inv_door_transform.transform_dir(door_axis_in_door,jaxis);
float lo_ext,hi_ext,ext;

if(_abs(door_axis_in_door.x)>_abs(door_axis_in_door.y))
{
	if(_abs(door_axis_in_door.x)>_abs(door_axis_in_door.z))
	{
		//door axis aligned along x
		door_plane_axes.y=0;		   //door axis is x (door_plane_axes.y stores door axis direction (i,j,k)=(0,1,2)
		door->joint->PSecond_element()->get_Extensions(door_transform.i,janchor.dotproduct(door_transform.i),lo_ext,hi_ext);
		door->door_plane_ext.y=hi_ext-lo_ext; //door extension along door axis

		door->joint->PSecond_element()->get_Extensions(door_transform.j,janchor.dotproduct(door_transform.j),lo_ext,hi_ext);
		door_plane_ext.x=hi_ext-lo_ext;//door extensions
		door_plane_axes.x=1;		   //door_plane_axes.x stores door direction it may be j or k in this point

		door->joint->PSecond_element()->get_Extensions(door_transform.k,janchor.dotproduct(door_transform.k),lo_ext,hi_ext);
		ext=hi_ext-lo_ext;
		if(_abs(ext)>_abs(door_axis_in_door.x))
		{
			door->door_plane_ext.x=ext;
			door->door_plane_axes.x=2;
		}
 
	}
	else
	{
		door->joint->PSecond_element()->get_Extensions(door_transform.j,janchor.dotproduct(door_transform.j),lo_ext,hi_ext);
		//door->door_plane_ext.y=hi_ext-lo_ext;
		//door_dir_sign=hi_ext>-lo_ext ? 1.f : -1.f;
		//door->door_plane_axes.x=2;
		//door->door_plane_axes.y=1;

		door->joint->PSecond_element()->get_Extensions(door_transform.i,janchor.dotproduct(door_transform.i),lo_ext,hi_ext);
		ext=hi_ext-lo_ext;
		if(_abs(ext)>door->door_plane_ext.y)
		{
			//door_dir_sign=hi_ext>-lo_ext ? 1.f : -1.f;
			//door->door_plane_ext.y=ext;
			//door->door_plane_axes.y=0;

		}
	}
}
else
{
	if(door_axis_in_door.y>door_axis_in_door.z)	
	{
		door->joint->PSecond_element()->get_Extensions(door_transform.i,janchor.dotproduct(door_transform.i),lo_ext,hi_ext);
		door->door_plane_ext.y=hi_ext-lo_ext;
		//door_dir_sign=hi_ext>-lo_ext ? 1.f : -1.f;
		door->door_plane_axes.x=1;
		door->door_plane_axes.y=0;
		door->joint->PSecond_element()->get_Extensions(door_transform.k,janchor.dotproduct(door_transform.k),lo_ext,hi_ext);
		ext=hi_ext-lo_ext;
		if(ext>door->door_plane_ext.y)
		{
			//door_dir_sign=hi_ext>-lo_ext ? 1.f : -1.f;
			door->door_plane_ext.y=ext;
			door->door_plane_axes.y=2;
		}
	}
	else
	{
		door->joint->PSecond_element()->get_Extensions(door_transform.j,janchor.dotproduct(door_transform.j),lo_ext,hi_ext);
		door->door_plane_ext.y=hi_ext-lo_ext;
		//door_dir_sign=hi_ext>-lo_ext ? 1.f : -1.f;
		door->door_plane_axes.x=2;
		door->door_plane_axes.y=1;
		door->joint->PSecond_element()->get_Extensions(door_transform.i,janchor.dotproduct(door_transform.i),lo_ext,hi_ext);
		ext=hi_ext-lo_ext;
		if(ext>door->door_plane_ext.y)
		{
			//door_dir_sign=hi_ext>-lo_ext ? 1.f : -1.f;
			door->door_plane_ext.y=ext;
			door->door_plane_axes.y=0;
		}
	}
}
/*
switch(door->door_plane_axes.y) 
{
case 0:
	door_dir.set(door_dir_sign,0.f,0.f);
	break;
case 1:
	door_dir.set(0.f,door_dir_sign,0.f);
	break;
case 2:
	door_dir.set(0.f,0.f,door_dir_sign);
	break;
default: NODEFAULT;
}
*/
}
void CCar::SDoor::SDoorway::Trace(const Fvector &point,const Fvector &dir)
{

}
