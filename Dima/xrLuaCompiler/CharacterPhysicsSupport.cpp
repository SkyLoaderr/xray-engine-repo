#include "stdafx.h"
#include "CharacterPhysicsSupport.h"
#include "PHMovementControl.h"
#include "CustomMonster.h"
#include "PhysicsShell.h"
CCharacterPhysicsSupport::CCharacterPhysicsSupport(EType atype,CEntityAlive* aentity) 
: Movement(*aentity->PMovement()), 
  m_pPhysicsShell(aentity->PPhysicsShell()),
  m_EntityAlife(*aentity),
  mXFORM(aentity->XFORM())
{
	//R_ASSERT2(aentity->PMovement()->CharacterExist()!=
	//			((!!aentity->PPhysicsShell())
	//			&&aentity->PPhysicsShell()->bActive)
	//			,"wrong parameter state");
	//if(!aentity->PPhysicsShell() )
	//{
	//	m_eState=esAlive;
	//}
	//else
	//{
	//	m_eState=esDead;
	//}
m_eType=atype;
m_eState=esAlive;
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

void CCharacterPhysicsSupport::in_Load(LPCSTR section)
{

	skel_density_factor				= pSettings->r_float(section,"ph_skeleton_mass_factor");
	skel_airr_lin_factor			= pSettings->r_float(section,"ph_skeleton_airr_lin_factor");
	skel_airr_ang_factor			= pSettings->r_float(section,"ph_skeleton_airr_ang_factor");
	hinge_force_factor				= pSettings->r_float(section,"ph_skeleton_hinger_factor");
	hinge_force_factor1				= pSettings->r_float(section,"ph_skeleton_hinger_factor1");
	skel_ddelay						= pSettings->r_s32(section,"ph_skeleton_ddelay");
	hinge_force_factor2				= pSettings->r_float(section,"ph_skeleton_hinger_factor2");
	hinge_vel						= pSettings->r_float(section,"ph_skeleton_hinge_vel");
	skel_fatal_impulse_factor		= pSettings->r_float(section,"ph_skel_fatal_impulse_factor");
}

void CCharacterPhysicsSupport::in_NetSpawn()
{
#ifndef NO_PHYSICS_IN_AI_MOVE
	Movement.CreateCharacter();
	Movement.SetPhysicsRefObject(&m_EntityAlife);
#endif
}
void CCharacterPhysicsSupport::in_NetDestroy()
{
	Movement.DestroyCharacter();
	if(m_pPhysicsShell)	
	{
		m_pPhysicsShell->Deactivate();
		m_pPhysicsShell->ZeroCallbacks();
	}
	xr_delete				(m_pPhysicsShell);
}

void CCharacterPhysicsSupport::in_Init()
{
	b_death_anim_on					= false;
	m_pPhysicsShell					= NULL;
	m_saved_impulse					= 0.f;
}

void CCharacterPhysicsSupport::in_shedule_Update(u32 DT )
{
	if(m_pPhysicsShell)
	{	
		if(m_pPhysicsShell->bActive)
		{
			if(!m_pPhysicsShell->bActivating&&!b_death_anim_on)
			{
				if(m_eType==etStalker)
				{
					PKinematics(m_EntityAlife.Visual())->LL_CloseCycle(0) ;
					PKinematics(m_EntityAlife.Visual())->LL_CloseCycle(1) ;
					PKinematics(m_EntityAlife.Visual())->LL_CloseCycle(2) ;
					PKinematics(m_EntityAlife.Visual())->LL_CloseCycle(3) ;
					PKinematics(m_EntityAlife.Visual())->PlayCycle("death_init");
				}
				b_death_anim_on=true;
			}

			if(m_saved_impulse!=0.f && !m_pPhysicsShell->bActivating)
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
	else if (!m_EntityAlife.g_Alive())
	{

		CreateSkeleton();
#ifndef NO_PHYSICS_IN_AI_MOVE

		Movement.DestroyCharacter();
		m_EntityAlife.PHSetPushOut();
#endif
	}
}

void CCharacterPhysicsSupport::in_Hit(float P, Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse)
{
	if(!(m_pPhysicsShell&&m_pPhysicsShell->bActive))
	{
		m_saved_impulse=impulse*skel_fatal_impulse_factor;
		m_saved_element=element;
		m_saved_hit_dir.set(dir);
		m_saved_hit_position.set(p_in_object_space);
#ifndef NO_PHYSICS_IN_AI_MOVE
		Movement.ApplyImpulse(dir,impulse);
#endif
	}
	else {
		if (!m_EntityAlife.g_Alive()) {
			if(m_pPhysicsShell&&m_pPhysicsShell->bActive) 
				m_pPhysicsShell->applyImpulseTrace(p_in_object_space,dir,impulse,element);
			//m_pPhysicsShell->applyImpulseTrace(position_in_bone_space,dir,impulse);
			else{
				m_saved_hit_dir.set(dir);
				m_saved_impulse=impulse*skel_fatal_impulse_factor;
				m_saved_element=element;
				m_saved_hit_position.set(p_in_object_space);
			}
		}
	}
}

void CCharacterPhysicsSupport::in_UpdateCL()
{
	if(m_pPhysicsShell&&m_pPhysicsShell->bActive&&!m_pPhysicsShell->bActivating)
	{

		//XFORM().set(m_pPhysicsShell->mXFORM);
		m_pPhysicsShell->InterpolateGlobalTransform(&mXFORM);
	}
}

void CCharacterPhysicsSupport::CreateSkeleton()
{
	if(m_pPhysicsShell) return;
#ifndef NO_PHYSICS_IN_AI_MOVE
	Movement.GetDeathPosition	(m_EntityAlife.Position());
	Movement.DestroyCharacter();
	//Position().y+=.1f;
	//#else
	//Position().y+=0.1f;
#endif

	if (!m_EntityAlife.Visual())
		return;
	m_pPhysicsShell		= P_create_Shell();
	m_pPhysicsShell->build_FromKinematics(PKinematics(m_EntityAlife.Visual()));
	m_pPhysicsShell->mXFORM.set(mXFORM);
	m_pPhysicsShell->SetAirResistance(0.002f*skel_airr_lin_factor,
		0.3f*skel_airr_ang_factor);
	m_pPhysicsShell->SmoothElementsInertia(0.3f);

	m_pPhysicsShell->set_PhysicsRefObject(&m_EntityAlife);
	if(m_eType==etStalker)
	{
	CInifile* ini = PKinematics(m_EntityAlife.Visual())->LL_UserData();
	R_ASSERT2(ini,"NO INI FILE IN MODEL");
	m_pPhysicsShell->set_DisableParams(default_disl*ini->r_float("disable","linear_factor"),default_disw*ini->r_float("disable","angular_factor"));
	}
	m_pPhysicsShell->Activate(true);

	PKinematics(m_EntityAlife.Visual())->Calculate();
	b_death_anim_on=false;
	m_eState=esDead;
}
