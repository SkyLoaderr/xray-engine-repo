#include "stdafx.h"
#include "torridZone.h"
#include "../objectanimator.h"
#include "xrServer_Objects_ALife_Monsters.h"

BOOL CTorridZone::net_Spawn(LPVOID DC)
{
	if (!inherited::net_Spawn(DC))
		return			(FALSE);

	CSE_Abstract		*abstract=(CSE_Abstract*)(DC);
	CSE_ALifeTorridZone	*zone	= dynamic_cast<CSE_ALifeTorridZone*>(abstract);
	VERIFY				(zone);
/*
	R_ASSERT			(Visual()&&PKinematics(Visual()));
	CSkeletonAnimated	*A= PSkeletonAnimated(Visual());
	if (A) {
		A->PlayCycle	(*zone->startup_animation);
		A->Calculate	();
	}*/

	m_animator->Load	(zone->get_motion());
	m_animator->Play	(true);

	return				(TRUE);
}

void CTorridZone::UpdateCL()
{
	inherited::UpdateCL();
	m_animator->OnFrame	();
}


void CTorridZone::shedule_Update(u32 dt)
{
	inherited::shedule_Update(dt);
}
