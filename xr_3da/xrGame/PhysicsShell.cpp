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
