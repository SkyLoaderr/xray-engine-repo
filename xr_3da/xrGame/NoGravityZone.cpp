#include "stdafx.h"
#include "nogravityzone.h"
#include "physicsshell.h"
#include "entity_alive.h"
#include "PHMovementControl.h"
void CNoGravityZone::enter_Zone(SZoneObjectInfo& io)
{
	inherited::enter_Zone(io);
	switchGravity(io,false);

}
void CNoGravityZone::exit_Zone(SZoneObjectInfo& io)
{
	switchGravity(io,true);
	inherited::exit_Zone(io);
	
}

void CNoGravityZone::switchGravity(SZoneObjectInfo& io, bool val)
{
	if(io.object->getDestroy()) return;
	CPhysicsShellHolder* sh= smart_cast<CPhysicsShellHolder*>(io.object);
	if(!sh)return;
	CPhysicsShell* shell=sh->PPhysicsShell();
	if(shell&&shell->bActive)
	{
		shell->set_ApplyByGravity(val);
		return;
	}
	if(!io.nonalive_object)
	{
		CEntityAlive* ea=smart_cast<CEntityAlive*>(io.object);
		ea->movement_control()->SetApplyGravity(BOOL(val));
	}
}