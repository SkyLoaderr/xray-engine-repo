#include "stdafx.h"
#include "CharacterPhysicsSupport.h"
#include "PHMovementControl.h"
#include "CustomMonster.h"
#include "PhysicsShell.h"
CCharacterPhysicsSupport::CCharacterPhysicsSupport(EType atype,CEntityAlive* aentity) 
	: Movement(*aentity->PMovement())
{
	R_ASSERT2(aentity->PMovement()->CharacterExist()!=(!!aentity->PPhysicsShell()),"wrong parameter state");
	if(!aentity->PPhysicsShell() )
	{
		m_eState=esAlife;
	}
	else
	{
		m_eState=esDead;
	}
m_pEntityAlife=aentity;
m_pPhysicsShell=aentity->PPhysicsShell();
};

void CCharacterPhysicsSupport::Activate()
{
}

void CCharacterPhysicsSupport::Deactivate()
{
}

void CCharacterPhysicsSupport::Allocate()
{
}

void CCharacterPhysicsSupport::Clear()
{
}

void CCharacterPhysicsSupport::in_Load()
{
}

void CCharacterPhysicsSupport::in_NetSpawn()
{
}

void CCharacterPhysicsSupport::in_shedule_Update(u32 DT )
{
	if(m_pPhysicsShell)
	{	
		if(m_pPhysicsShell->bActive)
		{
			if(!m_pPhysicsShell->bActivating&&!b_death_anim_on)
			{
				PKinematics(m_pEntityAlife->Visual())->PlayCycle("death_init");
				b_death_anim_on=true;
			}

			if(m_saved_impulse!=0.f)
			{
				m_pPhysicsShell->applyImpulseTrace(m_saved_hit_position,m_saved_hit_dir,m_saved_impulse*1.f,m_saved_element);
				m_saved_impulse=0.f;
			}

			if(skel_ddelay==0)
			{
				m_pPhysicsShell->set_JointResistance(5.f*hinge_force_factor1);//5.f*hinge_force_factor1
				//m_pPhysicsShell->SetAirResistance()

			}
			skel_ddelay--;
		}

	}
	else if (!m_pEntityAlife->g_Alive())
	{

		CreateSkeleton();
#ifndef NO_PHYSICS_IN_AI_MOVE

		Movement.DestroyCharacter();
		m_pEntityAlife->PHSetPushOut();
#endif
	}
}

void CCharacterPhysicsSupport::in_UpdateCL()
{
}

void CCharacterPhysicsSupport::CreateSkeleton()
{
	if(m_pPhysicsShell) return;
#ifndef NO_PHYSICS_IN_AI_MOVE
	Movement.GetDeathPosition	(Position());
	Movement.DestroyCharacter();
	//Position().y+=.1f;
	//#else
	//Position().y+=0.1f;
#endif

	if (!m_pEntityAlife->Visual())
		return;
	m_pPhysicsShell		= P_create_Shell();
	m_pPhysicsShell->build_FromKinematics(PKinematics(m_pEntityAlife->Visual()));
	m_pPhysicsShell->mXFORM.set(m_pEntityAlife->XFORM());
	m_pPhysicsShell->SetAirResistance(0.002f*skel_airr_lin_factor,
		0.3f*skel_airr_ang_factor);
	m_pPhysicsShell->SmoothElementsInertia(0.3f);

	m_pPhysicsShell->set_PhysicsRefObject(m_pEntityAlife);
	CInifile* ini = PKinematics(m_pEntityAlife->Visual())->LL_UserData();
	R_ASSERT2(ini,"NO INI FILE IN MODEL");

	///////////////////////////car definition///////////////////////////////////////////////////
	m_pPhysicsShell->set_DisableParams(default_disl*ini->r_float("disable","linear_factor"),default_disw*ini->r_float("disable","angular_factor"));
	m_pPhysicsShell->Activate(true);

	PKinematics(m_pEntityAlife->Visual())->Calculate();
	b_death_anim_on=false;
}