#include "stdafx.h"
#include "CharacterPhysicsSupport.h"
#include "PHMovementControl.h"
#include "CustomMonster.h"
#include "PhysicsShell.h"
CCharacterPhysicsSupport::~CCharacterPhysicsSupport()
{
	if(!b_skeleton_in_shell)xr_delete(m_physics_skeleton);
}
CCharacterPhysicsSupport::CCharacterPhysicsSupport(EType atype,CEntityAlive* aentity) 
: m_PhysicMovementControl(*aentity->PMovement()), 
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
b_death_anim_on					= false;
m_pPhysicsShell					= NULL;
m_saved_impulse					= 0.f;
m_physics_skeleton				= NULL;
b_skeleton_in_shell				= false;
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

	m_PhysicMovementControl.CreateCharacter();
	m_PhysicMovementControl.SetPhysicsRefObject(&m_EntityAlife);
	
#endif
	if(!m_physics_skeleton)CreateSkeleton(m_physics_skeleton);
}
void CCharacterPhysicsSupport::in_NetDestroy()
{
	m_PhysicMovementControl.DestroyCharacter();
	if((!b_skeleton_in_shell||m_pPhysicsShell)&&m_physics_skeleton)
	{
			m_physics_skeleton->Deactivate();
			xr_delete(m_physics_skeleton);
			b_skeleton_in_shell=false;
	}

	//if(m_pPhysicsShell)	
	//{
	//	//m_pPhysicsShell->Deactivate();
	//	//m_pPhysicsShell->ZeroCallbacks();
	//	m_pPhysicsShell=0;
	//	b_skeleton_in_shell=false;

	//}
	
	//xr_delete				(m_pPhysicsShell);
}

void CCharacterPhysicsSupport::in_Init()
{
	//b_death_anim_on					= false;
	//m_pPhysicsShell					= NULL;
	//m_saved_impulse					= 0.f;
}

void CCharacterPhysicsSupport::in_shedule_Update(u32 /**DT/**/)
{
	if(m_pPhysicsShell)
	{	
		if(m_pPhysicsShell->bActive)
		{
			if(!m_pPhysicsShell->bActivating&&!b_death_anim_on)
			{
				if(m_eType==etStalker)
				{
					PSkeletonAnimated(m_EntityAlife.Visual())->LL_CloseCycle(0) ;
					PSkeletonAnimated(m_EntityAlife.Visual())->LL_CloseCycle(1) ;
					PSkeletonAnimated(m_EntityAlife.Visual())->LL_CloseCycle(2) ;
					PSkeletonAnimated(m_EntityAlife.Visual())->LL_CloseCycle(3) ;
					PSkeletonAnimated(m_EntityAlife.Visual())->PlayCycle("death_init");
				}
				b_death_anim_on=true;
			}

			if(!fsimilar(0.f,m_saved_impulse) && !m_pPhysicsShell->bActivating)
			{
				m_pPhysicsShell->applyImpulseTrace(m_saved_hit_position,m_saved_hit_dir,m_saved_impulse*1.f,m_saved_element);
				m_saved_impulse=0.f;
			}

			if(skel_ddelay==0)
			{
				m_pPhysicsShell->set_JointResistance(5.f*hinge_force_factor1);//5.f*hinge_force_factor1
				//m_pPhysicsShell->SetAirResistance()

			}
			--skel_ddelay;
		}

	}
	else if (!m_EntityAlife.g_Alive())
	{

		//Log("mem use %d",Memory.mem_usage());

		ActivateShell();
		//CreateSkeleton();
		//Log("mem use %d",Memory.mem_usage());
#ifndef NO_PHYSICS_IN_AI_MOVE

		m_PhysicMovementControl.DestroyCharacter();
		m_EntityAlife.PHSetPushOut();
#endif
	}
}

void CCharacterPhysicsSupport::in_Hit(float /**P/**/, Fvector &dir, CObject * /**who/**/,s16 element,Fvector p_in_object_space, float impulse,bool is_killing)
{
	if(!m_pPhysicsShell&&is_killing)ActivateShell();
	if(!(m_pPhysicsShell&&m_pPhysicsShell->bActive))
	{
		m_saved_impulse=impulse*skel_fatal_impulse_factor;
		m_saved_element=element;
		m_saved_hit_dir.set(dir);
		m_saved_hit_position.set(p_in_object_space);
#ifndef NO_PHYSICS_IN_AI_MOVE
		if(!is_killing&&m_EntityAlife.g_Alive())
			m_PhysicMovementControl.ApplyImpulse(dir,impulse);
#endif

	}
	else {
		{//if (!m_EntityAlife.g_Alive()) 
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

void CCharacterPhysicsSupport::CreateSkeleton(CPhysicsShell* &pShell)
{

	R_ASSERT2(!pShell,"pShell already initialized!!");
	if (!m_EntityAlife.Visual())
		return;
	pShell		= P_create_Shell();
	pShell->preBuild_FromKinematics(PKinematics(m_EntityAlife.Visual()));
	pShell->mXFORM.set(mXFORM);
	pShell->SetAirResistance(0.002f*skel_airr_lin_factor,
		0.3f*skel_airr_ang_factor);
	pShell->SmoothElementsInertia(0.3f);

	pShell->set_PhysicsRefObject(&m_EntityAlife);
	if(m_eType==etStalker)
	{
	CInifile* ini = PKinematics(m_EntityAlife.Visual())->LL_UserData();
	R_ASSERT2(ini,"NO INI FILE IN MODEL");
	pShell->set_DisableParams(default_disl*ini->r_float("disable","linear_factor"),default_disw*ini->r_float("disable","angular_factor"));
	}
	pShell->Build();

}
void CCharacterPhysicsSupport::CreateSkeleton()
{
	if(m_pPhysicsShell) return;
#ifndef NO_PHYSICS_IN_AI_MOVE
	m_PhysicMovementControl.GetDeathPosition	(m_EntityAlife.Position());
	m_PhysicMovementControl.DestroyCharacter();
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
void CCharacterPhysicsSupport::ActivateShell()
{
	if(m_pPhysicsShell) return;
#ifndef NO_PHYSICS_IN_AI_MOVE
	m_PhysicMovementControl.GetDeathPosition	(m_EntityAlife.Position());
	m_PhysicMovementControl.DestroyCharacter();
#endif
	R_ASSERT2(m_physics_skeleton,"No skeleton created!!");

	m_pPhysicsShell=m_physics_skeleton;
	m_pPhysicsShell->RunSimulation();
	m_pPhysicsShell->mXFORM.set(mXFORM);
	m_pPhysicsShell->SetCallbacks();
	PKinematics(m_EntityAlife.Visual())->Calculate();
	b_death_anim_on=false;
	m_eState=esDead;
	b_skeleton_in_shell=true;
}