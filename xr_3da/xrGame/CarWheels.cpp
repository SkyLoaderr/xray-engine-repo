#include "stdafx.h"
#include "car.h"
#include "level.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCar::SWheel::Init()
{
	if(inited) return;
	BONE_P_PAIR_CIT bone=bone_map.find(bone_id);
	R_ASSERT2(bone->second.element,"No Element was created for wheel-check collision is set");
	bone->second.element->set_DynamicLimits(default_l_limit,default_w_limit*100.f);
	radius=bone->second.element->getRadius();
	R_ASSERT2(bone->second.joint,"No wheel joint was set for a wheel");
	joint=bone->second.joint->GetDJoint();
	R_ASSERT2(dJointGetType(joint)==dJointTypeHinge2,"No wheel join was set for a wheel, only wheel-joint valid!!!");
	dJointSetHinge2Param(joint, dParamFMax2,0.f);//car->m_axle_friction
	dJointSetHinge2Param(joint, dParamVel2, 0.f);
	inited=true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCar::SWheelDrive::Init()
{
	pwheel->Init();
	gear_factor=pwheel->radius/pwheel->car->m_ref_radius;
	CBoneData& bone_data= PKinematics(pwheel->car->Visual())->LL_GetData(u16(pwheel->bone_id));
	switch(bone_data.IK_data.type)
	{
	case jtWheel:
		pos_fvd=bone_map.find(pwheel->bone_id)->second.element->mXFORM.k.x;
		break;

	default: NODEFAULT;
	}

	pos_fvd=pos_fvd>0.f ? -1.f : 1.f;


}
void CCar::SWheelDrive::Drive()
{
	float cur_speed=pwheel->car->RefWheelMaxSpeed()/gear_factor;
	dJointSetHinge2Param(pwheel->joint, dParamVel2, pos_fvd*cur_speed);
	(cur_speed<0.f) ? (cur_speed=-cur_speed) :cur_speed;
	dJointSetHinge2Param(pwheel->joint, dParamFMax2, pwheel->car->RefWheelCurTorque()/gear_factor);

}
void CCar::SWheelDrive::UpdatePower()
{
	float cur_speed=pwheel->car->RefWheelMaxSpeed()/gear_factor;
	(cur_speed<0.f) ? (cur_speed=-cur_speed) :cur_speed;
	dJointSetHinge2Param(pwheel->joint, dParamFMax2, pwheel->car->RefWheelCurTorque()/gear_factor);
}
void CCar::SWheelDrive::Neutral()
{
	dJointSetHinge2Param(pwheel->joint, dParamFMax2, pwheel->car->m_axle_friction);
	dJointSetHinge2Param(pwheel->joint, dParamVel2, 0.f);
}

float CCar::SWheelDrive::ASpeed()
{
	return dFabs(dJointGetHinge2Angle2Rate(pwheel->joint));
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCar::SWheelSteer::Init()
{
	CKinematics* pKinematics=PKinematics(pwheel->car->Visual());
	pwheel->Init();
	(bone_map.find(pwheel->bone_id))->second.joint->GetLimits(lo_limit,hi_limit,0);
	CBoneData& bone_data= pKinematics->LL_GetData(u16(pwheel->bone_id));
	switch(bone_data.IK_data.type)
	{
	case jtWheel:	
		//pos_right=bone_map.find(pwheel->bone_id)->second.element->mXFORM.i.dotproduct(pwheel->car->m_root_transform.j);
		pos_right=bone_map.find(pwheel->bone_id)->second.element->mXFORM.i.y;//.dotproduct(pwheel->car->m_root_transform.j);
		break;
/*
	case jtWheelXZ:	
	case jtWheelXY:
		pos_right=bone_map.find(pwheel->bone_id)->second.element->mXFORM.i.dotproduct(pwheel->car->m_root_transform.j);
		break;
	case jtWheelYZ:	
	case jtWheelYX:
		pos_right=bone_map.find(pwheel->bone_id)->second.element->mXFORM.j.dotproduct(pwheel->car->m_root_transform.j);
		break;
	case jtWheelZY:	
	case jtWheelZX:
		pos_right=bone_map.find(pwheel->bone_id)->second.element->mXFORM.k.dotproduct(pwheel->car->m_root_transform.j);
		break;
*/
	default: NODEFAULT;
	}
	
	pos_right=pos_right>0.f ? -1.f : 1.f;
	float steering_torque=pKinematics->LL_UserData()->r_float("car_definition","steering_torque");
	dJointSetHinge2Param(pwheel->joint, dParamFMax, steering_torque);
	dJointSetHinge2Param(pwheel->joint, dParamFudgeFactor, 0.005f/steering_torque);
	dJointSetHinge2Param(pwheel->joint, dParamVel, 0.f);
	limited=false;
}

void CCar::SWheelSteer::SteerRight()
{
	limited=true;						//no need to limit wheels when steering
	if(pos_right>0)
	{

		dJointSetHinge2Param(pwheel->joint, dParamHiStop, hi_limit);
		dJointSetHinge2Param(pwheel->joint, dParamVel, pwheel->car->m_steering_speed);

	}
	else
	{
		dJointSetHinge2Param(pwheel->joint, dParamLoStop, lo_limit);
		dJointSetHinge2Param(pwheel->joint, dParamVel, -pwheel->car->m_steering_speed);
	}
}
void CCar::SWheelSteer::SteerLeft()
{
	limited=true;						//no need to limit wheels when steering
	if(pos_right<0)
	{

		dJointSetHinge2Param(pwheel->joint, dParamHiStop, hi_limit);
		dJointSetHinge2Param(pwheel->joint, dParamVel, pwheel->car->m_steering_speed);

	}
	else
	{
		dJointSetHinge2Param(pwheel->joint, dParamLoStop, lo_limit);
		dJointSetHinge2Param(pwheel->joint, dParamVel, -pwheel->car->m_steering_speed);
	}
}
void CCar::SWheelSteer::SteerIdle()
{
	limited=false;
	if(pwheel->car->e_state_steer==right)
	{
		if(pos_right<0)
		{

			dJointSetHinge2Param(pwheel->joint, dParamHiStop, 0.f);
			dJointSetHinge2Param(pwheel->joint, dParamVel, pwheel->car->m_steering_speed);

		}
		else
		{
			dJointSetHinge2Param(pwheel->joint, dParamLoStop, 0.f);
			dJointSetHinge2Param(pwheel->joint, dParamVel, -pwheel->car->m_steering_speed);
		}
	}
	else
	{
		if(pos_right>0)
		{

			dJointSetHinge2Param(pwheel->joint, dParamHiStop,0.f);
			dJointSetHinge2Param(pwheel->joint, dParamVel, pwheel->car->m_steering_speed);

		}
		else
		{
			dJointSetHinge2Param(pwheel->joint, dParamLoStop, 0.f);
			dJointSetHinge2Param(pwheel->joint, dParamVel, -pwheel->car->m_steering_speed);
		}
	}

}

void CCar::SWheelSteer::Limit()
{
	if(!limited)
	{
		dJointID joint=pwheel->joint;
		dReal angle = dJointGetHinge2Angle1(joint);
		if(dFabs(angle)<M_PI/180.f)
		{
			dJointSetHinge2Param(joint, dParamHiStop, 0);
			dJointSetHinge2Param(joint, dParamLoStop, 0);///
			dJointSetHinge2Param(joint, dParamVel, 0);
			limited=true;
		}
	}
	pwheel->car->b_wheels_limited=pwheel->car->b_wheels_limited&&limited;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CCar::SWheelBreak::Init()
{
	pwheel->Init();
	break_torque=pwheel->car->m_break_torque*pwheel->radius/pwheel->car->m_ref_radius;
	hand_break_torque=pwheel->car->m_hand_break_torque*pwheel->radius/pwheel->car->m_ref_radius;
}

void CCar::SWheelBreak::Break()
{
	dJointSetHinge2Param(pwheel->joint, dParamFMax2, 100000.f*break_torque);//
	dJointSetHinge2Param(pwheel->joint, dParamVel2, 0.f);
}

void CCar::SWheelBreak::HandBreak()
{
	dJointSetHinge2Param(pwheel->joint, dParamFMax2, hand_break_torque);
	dJointSetHinge2Param(pwheel->joint, dParamVel2, 0.f);
}

void CCar::SWheelBreak::Neutral()
{
	dJointSetHinge2Param(pwheel->joint, dParamFMax2, pwheel->car->m_axle_friction);
	dJointSetHinge2Param(pwheel->joint, dParamVel2, 0.f);
}