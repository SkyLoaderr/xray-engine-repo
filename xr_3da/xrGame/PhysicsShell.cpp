#include "stdafx.h"
#include "physicsshell.h"
#include "PHDynamicData.h"
#include "Physics.h"

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

void __stdcall PushOutCallback(bool& do_colide,dContact& c)
{
	if(c.geom.depth>0.3f)
	{
		c.surface.soft_erp=50.f;
		c.surface.soft_cfm=7.f;
	}
	do_colide=true;
//if()
}