#include "stdafx.h"
#include "physicsshell.h"
#include "script_space.h"

using namespace luabind;

void CPhysicsShell::script_register(lua_State *L)
{
	module(L)
		[
			class_<CPhysicsShell>("physics_shell")
			.def("apply_force",					(void (CPhysicsShell::*)(float,float,float))(&CPhysicsShell::applyForce))
			.def("get_element_by_bone_name",	(CPhysicsElement*(CPhysicsShell::*)(LPCSTR))(&CPhysicsShell::get_Element))
			.def("get_element_by_bone_id",		(CPhysicsElement*(CPhysicsShell::*)(u16))(&CPhysicsShell::get_Element))
			.def("get_element_by_order",		&CPhysicsShell::get_ElementByStoreOrder)
			.def("get_elements_number",			&CPhysicsShell::get_ElementsNumber)
			.def("get_joint_by_bone_name",		(CPhysicsJoint*(CPhysicsShell::*)(LPCSTR))(&CPhysicsShell::get_Joint))
			.def("get_joint_by_bone_id",		(CPhysicsJoint*(CPhysicsShell::*)(u16))(&CPhysicsShell::get_Joint))
			.def("get_joint_by_order",			&CPhysicsShell::get_JointByStoreOrder)
			.def("get_joints_number",			&CPhysicsShell::get_JointsNumber)
		];
}

void CPhysicsElement::script_register(lua_State *L)
{
	module(L)
		[
			class_<CPhysicsElement>("physics_element")
			.def("apply_force",					(void (CPhysicsElement::*)(float,float,float))(&CPhysicsElement::applyForce))

		];
}

void CPhysicsJoint::script_register(lua_State *L)
{
	module(L)
		[
			class_<CPhysicsJoint>("physics_joint")
			.def("get_bone_id",							&CPhysicsJoint::BoneID)
			.def("get_first_element",					&CPhysicsJoint::PFirst_element)
			.def("get_first_element",					&CPhysicsJoint::PSecond_element)
			.def("set_anchor_global",					(void(CPhysicsJoint::*)(const float,const float,const float))(&CPhysicsJoint::SetAnchor))
			.def("set_anchor_vs_first_element",			(void(CPhysicsJoint::*)(const float,const float,const float))(&CPhysicsJoint::SetAnchorVsFirstElement))
			.def("set_anchor_vs_second_element",		(void(CPhysicsJoint::*)(const float,const float,const float))(&CPhysicsJoint::SetAnchorVsSecondElement))
			.def("get_axes_number",						&CPhysicsJoint::GetAxesNumber)
			.def("set_axis_spring_dumping_factors",		&CPhysicsJoint::SetAxisSDfactors)
			.def("set_joint_spring_dumping_factors",	&CPhysicsJoint::SetJointSDfactors)
			.def("set_axis_dir_global",					(void(CPhysicsJoint::*)(const float,const float,const float,const int ))(&CPhysicsJoint::SetAxisDir))
			.def("set_axis_dir_vs_first_element",		(void(CPhysicsJoint::*)(const float,const float,const float,const int ))(&CPhysicsJoint::SetAxisDirVsFirstElement))
			.def("set_axis_dir_vs_second_element",		(void(CPhysicsJoint::*)(const float,const float,const float,const int ))(&CPhysicsJoint::SetAxisDirVsSecondElement))
			.def("set_limits",							&CPhysicsJoint::SetLimits)
			.def("set_max_force_and_velocity",			&CPhysicsJoint::SetForceAndVelocity)
			.def("get_max_force_and_velocity",			&CPhysicsJoint::GetMaxForceAndVelocity)
			.def("get_axis_angle",						&CPhysicsJoint::GetAxisAngle)
			.def("get_limits",							&CPhysicsJoint::GetLimits)
			.def("get_axis_dir",						&CPhysicsJoint::GetAxisDirDynamic)
			.def("get_anchor",							&CPhysicsJoint::GetAnchorDynamic)
		];
}