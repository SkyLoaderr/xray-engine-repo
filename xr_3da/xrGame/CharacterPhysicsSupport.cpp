#include "stdafx.h"
#include "CharacterPhysicsSupport.h"
#include "PHMovementControl.h"
#include "CustomMonster.h"
#include "PhysicsShell.h"
#include "../skeletonanimated.h"
#include "Actor.h"
#include "CustomZone.h"
const float default_hinge_friction = 5.f;
CCharacterPhysicsSupport::~CCharacterPhysicsSupport()
{
	if(!b_skeleton_in_shell)xr_delete(m_physics_skeleton);
}

CCharacterPhysicsSupport::CCharacterPhysicsSupport(EType atype,CEntityAlive* aentity) 
: m_PhysicMovementControl(*aentity->PMovement()), 
  m_pPhysicsShell(aentity->PPhysicsShell()),
  m_EntityAlife(*aentity),
  mXFORM(aentity->XFORM()),
  m_ph_sound_player(aentity)
{

m_eType=atype;
m_eState=esAlive;
b_death_anim_on					= false;
m_pPhysicsShell					= NULL;
m_saved_impulse					= 0.f;
m_physics_skeleton				= NULL;
b_skeleton_in_shell				= false;
m_shot_up_factor				=0.f;
m_after_death_velocity_factor	=1.f;
switch(atype)
{
case etActor:
	m_PhysicMovementControl.AllocateCharacterObject(CPHMovementControl::actor);
	m_PhysicMovementControl.SetRestrictionType(CPHCharacter::rtActor);
	break;
case etStalker:
	m_PhysicMovementControl.AllocateCharacterObject(CPHMovementControl::CharacterType::ai);
	m_PhysicMovementControl.SetRestrictionType(CPHCharacter::rtStalker);
	m_PhysicMovementControl.SetActorMovable(false);
	break;
case etBitting:
	m_PhysicMovementControl.AllocateCharacterObject(CPHMovementControl::CharacterType::ai);
}
};

void CCharacterPhysicsSupport::SetRemoved()
{
	m_eState=esRemoved;
	if(b_skeleton_in_shell)
	{
		if(m_pPhysicsShell->isEnabled())
		{
			m_EntityAlife.processing_deactivate();
		}
		m_pPhysicsShell->Deactivate();
		xr_delete(m_pPhysicsShell);
	}
	else
	{
		m_physics_skeleton->Deactivate();
		xr_delete(m_physics_skeleton);
		m_EntityAlife.processing_deactivate();
	}
	
}

void CCharacterPhysicsSupport::in_Load(LPCSTR section)
{

	skel_airr_ang_factor			= pSettings->r_float(section,"ph_skeleton_airr_ang_factor");
	skel_airr_lin_factor			= pSettings->r_float(section,"ph_skeleton_airr_lin_factor");
	hinge_force_factor1				= pSettings->r_float(section,"ph_skeleton_hinger_factor1");
	skel_ddelay						= pSettings->r_s32(section,"ph_skeleton_ddelay");
	skel_fatal_impulse_factor		= pSettings->r_float(section,"ph_skel_fatal_impulse_factor");
	if(pSettings->line_exist(section,"ph_skel_shot_up_factor")) m_shot_up_factor=pSettings->r_float(section,"ph_skel_shot_up_factor");
	if(pSettings->line_exist(section,"ph_after_death_velocity_factor")) m_after_death_velocity_factor=pSettings->r_float(section,"ph_after_death_velocity_factor");
	CPHDestroyable::Load(section);
}

void CCharacterPhysicsSupport::in_NetSpawn(CSE_Abstract* e)
{
	CPHDestroyable::Init();//this zerows colbacks !!;
	if(!m_EntityAlife.g_Alive())
	{
		CSkeletonAnimated*ka= smart_cast<CSkeletonAnimated*>(m_EntityAlife.Visual());
		ka->PlayCycle("death_init");
		ka->CalculateBones_Invalidate();
		ka->CalculateBones();
	}
	CPHSkeleton::Spawn(e);
}

void CCharacterPhysicsSupport::SpawnInitPhysics(CSE_Abstract* e)
{
	if(!m_physics_skeleton)CreateSkeleton(m_physics_skeleton);

	if(m_EntityAlife.g_Alive())
	{
		m_PhysicMovementControl.CreateCharacter();
		m_PhysicMovementControl.SetPhysicsRefObject(&m_EntityAlife);
		//m_PhysicMovementControl.SetMaterial( )
	}
	else
	{
		ActivateShell(NULL);
	}
}
void CCharacterPhysicsSupport::in_NetDestroy()
{
	m_PhysicMovementControl.DestroyCharacter();
	//if((!b_skeleton_in_shell||m_pPhysicsShell)&&m_physics_skeleton)//.
	//{
	//		m_physics_skeleton->Deactivate();
	//		xr_delete(m_physics_skeleton);
	//		b_skeleton_in_shell=false;
	//} 
	if(m_physics_skeleton)
	{
		m_physics_skeleton->Deactivate();
		xr_delete(m_physics_skeleton);
	}
	if(m_pPhysicsShell)
	{
		m_pPhysicsShell->Deactivate();
		xr_delete(m_pPhysicsShell);
	}
	b_skeleton_in_shell=false;
	CPHSkeleton::RespawnInit();
	CPHDestroyable::RespawnInit();
	m_eState=esAlive;
	//if(m_pPhysicsShell)	
	//{
	//	//m_pPhysicsShell->Deactivate();
	//	//m_pPhysicsShell->ZeroCallbacks();
	//	m_pPhysicsShell=0;
	//	b_skeleton_in_shell=false;

	//}
	
	//xr_delete				(m_pPhysicsShell);
}

void	CCharacterPhysicsSupport::in_NetSave(NET_Packet& P)
{
	CPHSkeleton::SaveNetState(P);
}

void CCharacterPhysicsSupport::in_Init()
{
	//b_death_anim_on					= false;
	//m_pPhysicsShell					= NULL;
	//m_saved_impulse					= 0.f;
}

void CCharacterPhysicsSupport::in_shedule_Update(u32 DT)
{
	//CPHSkeleton::Update(DT);
	CPHDestroyable::SheduleUpdate(DT);
}

void CCharacterPhysicsSupport::in_Hit(float P,Fvector &dir, CObject *who,s16 element,Fvector p_in_object_space, float impulse,ALife::EHitType hit_type ,bool is_killing)
{
	is_killing=is_killing||(m_eState==esAlive&&!m_EntityAlife.g_Alive());
	if(m_EntityAlife.g_Alive()&&is_killing&&hit_type==ALife::eHitTypeExplosion&&P>70.f)
		CPHDestroyable::Destroy();
	if((!m_EntityAlife.g_Alive()||is_killing)&&!fis_zero(m_shot_up_factor)&&hit_type!=ALife::eHitTypeExplosion)
	{
		dir.y+=m_shot_up_factor;
		dir.normalize();
	}
	if(!m_pPhysicsShell&&is_killing)
	{
		ActivateShell(who);
		impulse*=(hit_type==ALife::eHitTypeExplosion ? 1.f : skel_fatal_impulse_factor);
	}
	if(!(m_pPhysicsShell&&m_pPhysicsShell->bActive))
	{
		m_saved_impulse=impulse* (hit_type==ALife::eHitTypeExplosion ? 1.f : skel_fatal_impulse_factor);
		m_saved_element=element;
		m_saved_hit_type=hit_type;
		m_saved_hit_dir.set(dir);
		m_saved_hit_position.set(p_in_object_space);

		if(!is_killing&&m_EntityAlife.g_Alive())
			m_PhysicMovementControl.ApplyHit(dir,impulse,hit_type);

	}
	else {
		{//if (!m_EntityAlife.g_Alive()) 
			if(m_pPhysicsShell&&m_pPhysicsShell->bActive) 
				m_pPhysicsShell->applyHit(p_in_object_space,dir,impulse,element,hit_type);
			//m_pPhysicsShell->applyImpulseTrace(position_in_bone_space,dir,impulse);
			else{
				m_saved_hit_dir.set(dir);
				m_saved_impulse=impulse*(hit_type==ALife::eHitTypeExplosion ? 1.f : skel_fatal_impulse_factor);
				m_saved_element=element;
				m_saved_hit_position.set(p_in_object_space);
			}
		}
	}
}

void CCharacterPhysicsSupport::in_UpdateCL()
{
	if(m_eState==esRemoved)
	{
		return;
	}
	if(m_pPhysicsShell&&m_pPhysicsShell->bActive&&!m_pPhysicsShell->bActivating)
	{

		//XFORM().set(m_pPhysicsShell->mXFORM);
		m_pPhysicsShell->InterpolateGlobalTransform(&mXFORM);
	}

	if(m_pPhysicsShell)
	{	
		if(m_pPhysicsShell->bActive)
		{
			if(!m_pPhysicsShell->bActivating&&!b_death_anim_on)
			{
				///if(m_eType==etStalker)
				{
						smart_cast<CSkeletonAnimated*>(m_EntityAlife.Visual())->PlayCycle("death_init");
				}
	
				b_death_anim_on=true;
			}

			//if(!fsimilar(0.f,m_saved_impulse) && !m_pPhysicsShell->bActivating)
			//{
			//	m_pPhysicsShell->applyHit(m_saved_hit_position,m_saved_hit_dir,m_saved_impulse*1.f,m_saved_element,m_saved_hit_type);
			//	m_saved_impulse=0.f;
			//}

			if(skel_ddelay==0)
			{
				m_pPhysicsShell->set_JointResistance(default_hinge_friction*hinge_force_factor1);//5.f*hinge_force_factor1
				//m_pPhysicsShell->SetAirResistance()

			}
			--skel_ddelay;
		}

	}
	else if (!m_EntityAlife.g_Alive())
	{

		//Log("mem use %d",Memory.mem_usage());

		ActivateShell(NULL);
		//CreateSkeleton();
		//Log("mem use %d",Memory.mem_usage());

		m_PhysicMovementControl.DestroyCharacter();
		m_EntityAlife.PHSetPushOut();
	}
}

void CCharacterPhysicsSupport::CreateSkeleton(CPhysicsShell* &pShell)
{

	R_ASSERT2(!pShell,"pShell already initialized!!");
	if (!m_EntityAlife.Visual())
		return;
	pShell		= P_create_Shell();
	pShell->preBuild_FromKinematics(smart_cast<CKinematics*>(m_EntityAlife.Visual()));
	pShell->mXFORM.set(mXFORM);
	pShell->SetAirResistance(0.002f*skel_airr_lin_factor,
		0.3f*skel_airr_ang_factor);
	pShell->SmoothElementsInertia(0.3f);
	pShell->set_JointResistance(0.f);
	pShell->set_PhysicsRefObject(&m_EntityAlife);
	SAllDDOParams disable_params;
	disable_params.Load(smart_cast<CKinematics*>(m_EntityAlife.Visual())->LL_UserData());
	pShell->set_DisableParams(disable_params);

	pShell->Build();

}
void CCharacterPhysicsSupport::CreateSkeleton()
{
	if(m_pPhysicsShell) return;
Fvector velocity;
	m_PhysicMovementControl.GetCharacterVelocity(velocity);
	m_PhysicMovementControl.GetDeathPosition	(m_EntityAlife.Position());
	m_PhysicMovementControl.DestroyCharacter();
	//Position().y+=.1f;
	//#else
	//Position().y+=0.1f;

	if (!m_EntityAlife.Visual())
		return;
	m_pPhysicsShell		= P_create_Shell();
	m_pPhysicsShell->build_FromKinematics(smart_cast<CKinematics*>(m_EntityAlife.Visual()));
	m_pPhysicsShell->mXFORM.set(mXFORM);
	m_pPhysicsShell->SetAirResistance(0.002f*skel_airr_lin_factor,
		0.3f*skel_airr_ang_factor);
	m_pPhysicsShell->SmoothElementsInertia(0.3f);
	m_pPhysicsShell->set_PhysicsRefObject(&m_EntityAlife);
	SAllDDOParams disable_params;
	disable_params.Load(smart_cast<CKinematics*>(m_EntityAlife.Visual())->LL_UserData());
	m_pPhysicsShell->set_DisableParams(disable_params);
	m_pPhysicsShell->set_JointResistance(0.f);
	m_pPhysicsShell->Activate(true);
	velocity.mul(1.25f*m_after_death_velocity_factor);
	m_pPhysicsShell->set_LinearVel(velocity);
	smart_cast<CKinematics*>(m_EntityAlife.Visual())->CalculateBones();
	b_death_anim_on=false;
	m_eState=esDead;
}
void CCharacterPhysicsSupport::ActivateShell			(CObject* who)
{
	if(m_eType==etActor)
	{
		CActor* A=smart_cast<CActor*>(&m_EntityAlife);
		R_ASSERT2(A,"not an actor has actor type");
		if(A->Holder()) return;
		if(m_eState==esRemoved)return;
	}
	if(m_pPhysicsShell) return;
	Fvector velocity;
	m_PhysicMovementControl.GetCharacterVelocity		(velocity);
	velocity.mul(1.3f);
	m_PhysicMovementControl.GetDeathPosition	(m_EntityAlife.Position());
	m_PhysicMovementControl.DestroyCharacter();
	R_ASSERT2(m_physics_skeleton,"No skeleton created!!");

	m_pPhysicsShell=m_physics_skeleton;
	m_physics_skeleton=NULL;
	m_pPhysicsShell->RunSimulation();
	m_pPhysicsShell->mXFORM.set(mXFORM);
	m_pPhysicsShell->SetCallbacks(m_pPhysicsShell->GetBonesCallback());
	if(!smart_cast<CCustomZone*>(who))
	{
		velocity.mul(1.25f*m_after_death_velocity_factor);
	}
	m_pPhysicsShell->set_LinearVel(velocity);
	smart_cast<CKinematics*>(m_EntityAlife.Visual())->CalculateBones	();
	b_death_anim_on=false;
	m_eState=esDead;
	b_skeleton_in_shell=true;
	m_pPhysicsShell->SetPrefereExactIntegration	();
}
void CCharacterPhysicsSupport::in_ChangeVisual()
{
	//R_ASSERT2(m_eState!=esDead,"Cant change visual for dead body");
	if(!m_physics_skeleton&&!m_pPhysicsShell) return;
	xr_delete(m_physics_skeleton);
	CreateSkeleton(m_physics_skeleton);
	if(m_pPhysicsShell)
	{
		xr_delete(m_pPhysicsShell);
		ActivateShell(NULL);
	}


}

bool CCharacterPhysicsSupport::CanRemoveObject()
{
	if(m_eType==etActor)
	{
		return false;
	}
	else
	{
		return !m_EntityAlife.IsPlaying();
	}
}

void CCharacterPhysicsSupport::PHGetLinearVell(Fvector &velocity)
{
	if(m_pPhysicsShell&&m_pPhysicsShell->bActive)
	{
		m_pPhysicsShell->get_LinearVel(velocity);
	}
	else
		m_PhysicMovementControl.GetCharacterVelocity(velocity);
		
}