#include "stdafx.h"
#pragma hdrstop

#include "actor.h"

void CActor::create_Skeleton(){
	if(m_pPhysicsShell) return;
#ifndef NO_PHYSICS_IN_AI_MOVE
	Movement.GetDeathPosition	(Position());
	Movement.DestroyCharacter();
	//Position().y+=.1f;
	//#else
	//Position().y+=0.1f;
#endif

	if (!Visual())
		return;
	m_pPhysicsShell		= P_create_Shell();
	m_pPhysicsShell->build_FromKinematics(PKinematics(Visual()));
	m_pPhysicsShell->mXFORM.set(XFORM());
	m_pPhysicsShell->SetAirResistance(0.002f*skel_airr_lin_factor,
		0.3f*skel_airr_ang_factor);
	m_pPhysicsShell->SmoothElementsInertia(0.3f);

	m_pPhysicsShell->set_PhysicsRefObject(this);
	//CInifile* ini = PKinematics(Visual())->LL_UserData();
	//R_ASSERT2(ini,"NO INI FILE IN MODEL");

	///////////////////////////car definition///////////////////////////////////////////////////
	//m_pPhysicsShell->set_DisableParams(default_disl*ini->r_float("disable","linear_factor"),default_disw*ini->r_float("disable","angular_factor"));
	m_pPhysicsShell->Activate(true);

	PKinematics(Visual())->Calculate();


}
