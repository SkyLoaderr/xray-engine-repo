#include "stdafx.h"
#include "physicsshell.h"
#include "script_space.h"

using namespace luabind;

void CPhysicsShell::script_register(lua_State *L)
{
	module(L)
		[
			class_<CPhysicsShell>("physics_shell")
			.def("apply_force",					(void (CPhysicsShell::*)(float,float,float))(CPhysicsShell::applyForce))
		];
}

void CPhysicsElement::script_register(lua_State *L)
{
	module(L)
		[
			class_<CPhysicsElement>("physics_shell")
			.def("apply_force",					(void (CPhysicsElement::*)(float,float,float))(CPhysicsElement::applyForce))
		];
}

void CPhysicsJoint::script_register(lua_State *L)
{
	module(L)
		[
			class_<CPhysicsJoint>("CPhysicsJoint")

		];
}