#include "stdafx.h"
#include "PHJointDestroyInfo.h"
#include "PhysicsCommon.h"
CPHJointDestroyInfo::CPHJointDestroyInfo(u16 bone_id, float break_force,float break_torque)
{
	m_bone_id=bone_id;
	m_sq_break_force=break_force*break_force;
	m_sq_break_torque=break_torque*break_torque;
	m_breaked=false;
}

bool CPHJointDestroyInfo::Update()
{
	dReal sq_breack_force=m_sq_break_force/phBreakCommonFactor;
	if(dDOT(m_joint_feedback.f1,m_joint_feedback.f1)>sq_breack_force)
	{
		m_breaked=true;
		return true;
	}
	if(dDOT(m_joint_feedback.f2,m_joint_feedback.f2)>sq_breack_force)
	{
		m_breaked=true;
		return true;
	}
	if(dDOT(m_joint_feedback.t1,m_joint_feedback.t1)>sq_breack_force)
	{
		m_breaked=true;
		return true;
	}
	if(dDOT(m_joint_feedback.t2,m_joint_feedback.t2)>sq_breack_force)
	{
		m_breaked=true;
		return true;
	}
	return false;
}