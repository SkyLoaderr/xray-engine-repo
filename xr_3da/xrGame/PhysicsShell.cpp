#include "stdafx.h"
#include "physicsshell.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "PHJoint.h"
#include "PHShell.h"
#include "PHJoint.h"

CPhysicsElement*			P_create_Element		()
{
	CPHElement* element=xr_new<CPHElement>	(ph_world->GetSpace());
	return element;
}

CPhysicsShell*				P_create_Shell			()
{
	CPhysicsShell* shell=xr_new<CPHShell>	();
	return shell;
}

CPhysicsJoint*				P_create_Joint			(CPhysicsJoint::enumType type ,CPhysicsElement* first,CPhysicsElement* second)
{
	CPhysicsJoint* joint=xr_new<CPHJoint>	( type , first, second);
	return joint;
}


CPhysicsJoint::SPHAxis::SPHAxis(){
	high=M_PI/15.f;
	low=-M_PI/15.f;;
	zero=0.f;
	//erp=ERP(world_spring/5.f,world_damping*5.f);
	//cfm=CFM(world_spring/5.f,world_damping*5.f);
#ifndef ODE_SLOW_SOLVER
	erp=world_erp;
	cfm=world_cfm;
#else
	erp=0.3f;
	cfm=0.000001f;
#endif
	direction.set(0,0,1);
	vs=vs_first;
	force=5.f;
	velocity=0.f;
}