#include "stdafx.h"
#include "PHShellCreator.h"
#include "GameObject.h"
#include "PhysicsShell.h"

void CPHShellSimpleCreator::CreatePhysicsShell()
{
	CGameObject* owner = dynamic_cast<CGameObject*>(this); VERIFY(owner);
	if (!owner->Visual()) return;
	
	CKinematics* pKinematics		= PKinematics(owner->Visual());
	VERIFY							(pKinematics);

	if(owner->m_pPhysicsShell)		return;
	owner->m_pPhysicsShell			= P_create_Shell();
	owner->m_pPhysicsShell->build_FromKinematics	(pKinematics,0);

	owner->m_pPhysicsShell->set_PhysicsRefObject	(owner);
	//m_pPhysicsShell->SmoothElementsInertia(0.3f);
	owner->m_pPhysicsShell->mXFORM.set				(owner->XFORM());
	owner->m_pPhysicsShell->SetAirResistance		(0.001f, 0.02f);
}