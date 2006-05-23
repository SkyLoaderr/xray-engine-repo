#include "stdafx.h"
#include "alife_space.h"
#include "hit.h"
#include "PHDestroyable.h"
#include "CharacterPhysicsSupport.h"
#include "PHMovementControl.h"
#include "CustomMonster.h"
#include "PhysicsShell.h"
#include "../skeletonanimated.h"
#include "Actor.h"
#include "CustomZone.h"
#include "Extendedgeom.h"
#include "Physics.h"
#include "level.h"
#include "PHActivationShape.h"
#include "IKLimbsController.h"
#include "PHCapture.h"
const float default_hinge_friction = 5.f;
#ifdef DEBUG
#include "PHDebug.h"
#endif
void  NodynamicsCollide(bool& do_colide,bool bo1,dContact& c,SGameMtl * /*material_1*/,SGameMtl * /*material_2*/)
{
	dBodyID body1=dGeomGetBody(c.geom.g1);
	dBodyID body2=dGeomGetBody(c.geom.g2);
	if(!body1||!body2||(dGeomUserDataHasCallback(c.geom.g1,NodynamicsCollide)&&dGeomUserDataHasCallback(c.geom.g2,NodynamicsCollide)))return;
	do_colide=false; 
}

CCharacterPhysicsSupport::~CCharacterPhysicsSupport()
{
	if(m_flags.test(fl_skeleton_in_shell))
	{
		if(m_physics_skeleton)m_physics_skeleton->Deactivate();
		xr_delete(m_physics_skeleton);//!b_skeleton_in_shell
	}
	xr_delete(m_PhysicMovementControl);
}

CCharacterPhysicsSupport::CCharacterPhysicsSupport(EType atype,CEntityAlive* aentity) 
: m_pPhysicsShell(aentity->PPhysicsShell()),
  m_EntityAlife(*aentity),
  mXFORM(aentity->XFORM()),
  m_ph_sound_player(aentity)
{
m_PhysicMovementControl=xr_new<CPHMovementControl>(aentity);
m_flags.assign(0);
m_eType=atype;
m_eState=esAlive;
//b_death_anim_on					= false;
m_flags.set(fl_death_anim_on,FALSE);
m_pPhysicsShell					=	NULL;
//m_saved_impulse					= 0.f;
m_physics_skeleton				=	NULL;
//b_skeleton_in_shell				= false;
m_flags.set(fl_skeleton_in_shell,FALSE);
m_shot_up_factor				=0.f;
m_after_death_velocity_factor	=1.f;
m_ik_controller					=	NULL;
m_BonceDamageFactor				=1.f;
m_collision_hit_callback		=	NULL;
switch(atype)
{
case etActor:
	m_PhysicMovementControl->AllocateCharacterObject(CPHMovementControl::actor);
	m_PhysicMovementControl->SetRestrictionType(CPHCharacter::rtActor);
	break;
case etStalker:
	m_PhysicMovementControl->AllocateCharacterObject(CPHMovementControl::CharacterType::ai);
	m_PhysicMovementControl->SetRestrictionType(CPHCharacter::rtStalker);
	m_PhysicMovementControl->SetActorMovable(false);
	break;
case etBitting:
	m_PhysicMovementControl->AllocateCharacterObject(CPHMovementControl::CharacterType::ai);
}
};

void CCharacterPhysicsSupport::SetRemoved()
{
	m_eState=esRemoved;
	if(m_flags.test(fl_skeleton_in_shell))//b_skeleton_in_shell
	{
		if(m_pPhysicsShell->isEnabled())
		{
			m_EntityAlife.processing_deactivate();
		}
		if(m_pPhysicsShell)m_pPhysicsShell->Deactivate();
		xr_delete(m_pPhysicsShell);
	}
	else
	{
		if(m_physics_skeleton)m_physics_skeleton->Deactivate();
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
	m_flags.set(fl_specific_bonce_demager,TRUE);
	if(pSettings->line_exist(section,"bonce_damage_factor"))
	{
		
		m_BonceDamageFactor=pSettings->r_float(section,"bonce_damage_factor_for_objects");
	}else
	{
		m_BonceDamageFactor=pSettings->r_float("collision_damage","bonce_damage_factor_for_objects");
	}
	CPHDestroyable::Load(section);
}

void CCharacterPhysicsSupport::in_NetSpawn(CSE_Abstract* e)
{
	CPHDestroyable::Init();//this zerows colbacks !!;
	CKinematicsAnimated*ka= smart_cast<CKinematicsAnimated*>(m_EntityAlife.Visual());
	if(!m_EntityAlife.g_Alive())
	{
		
		if(m_eType==etStalker)
			ka->PlayCycle("waunded_1_idle_0");
		else
			ka->PlayCycle("death_init");

	}else
	{
		ka->PlayCycle("death_init");
	}
	ka->CalculateBones_Invalidate();
	ka->CalculateBones();
	CPHSkeleton::Spawn(e);
	movement()->EnableCharacter();
	movement()->SetPosition(m_EntityAlife.Position());
	movement()->SetVelocity	(0,0,0);
	if(m_eType!=etActor)
	{
		m_flags.set(fl_specific_bonce_demager,TRUE);
		m_BonceDamageFactor=1.f;
	}
}

void CCharacterPhysicsSupport::SpawnInitPhysics(CSE_Abstract* e)
{
	//if(!m_physics_skeleton)CreateSkeleton(m_physics_skeleton);

	if(m_EntityAlife.g_Alive())
	{
#ifdef DEBUG
		if(ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject)&&stricmp(PH_DBG_ObjectTrack(),*m_EntityAlife.cName())==0)
		{
			Msg("CCharacterPhysicsSupport::SpawnInitPhysics obj %s before collision correction %f,%f,%f",PH_DBG_ObjectTrack(),m_EntityAlife.Position().x,m_EntityAlife.Position().y,m_EntityAlife.Position().z);
		}
#endif
		CreateIKController();
		CollisionCorrectObjPos(m_EntityAlife.Position());
		m_PhysicMovementControl->CreateCharacter();
		m_PhysicMovementControl->SetPhysicsRefObject(&m_EntityAlife);
		m_PhysicMovementControl->SetPosition	(m_EntityAlife.Position());
#ifdef DEBUG
		if(ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject)&&stricmp(PH_DBG_ObjectTrack(),*m_EntityAlife.cName())==0)
		{
			Msg("CCharacterPhysicsSupport::SpawnInitPhysics obj %s after collision correction %f,%f,%f",PH_DBG_ObjectTrack(),m_EntityAlife.Position().x,m_EntityAlife.Position().y,m_EntityAlife.Position().z);
		}
#endif
		//m_PhysicMovementControl.SetMaterial( )
	}
	else
	{
		ActivateShell(NULL);
	}
}
void CCharacterPhysicsSupport::in_NetDestroy()
{
	m_PhysicMovementControl->DestroyCharacter();
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
	//b_skeleton_in_shell=false;
	m_flags.set(fl_skeleton_in_shell,FALSE);
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
	DestroyIKController();
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
	if(!(m_pPhysicsShell&&m_pPhysicsShell->isActive()))
	{
		//m_saved_impulse=impulse* (hit_type==ALife::eHitTypeExplosion ? 1.f : skel_fatal_impulse_factor);
		//m_saved_element=element;
		//m_saved_hit_type=hit_type;
		//m_saved_hit_dir.set(dir);
		//m_saved_hit_position.set(p_in_object_space);

		if(!is_killing&&m_EntityAlife.g_Alive())
			m_PhysicMovementControl->ApplyHit(dir,impulse,hit_type);

	}
	else {
		{//if (!m_EntityAlife.g_Alive()) 
			if(m_pPhysicsShell&&m_pPhysicsShell->isActive()) 
				m_pPhysicsShell->applyHit(p_in_object_space,dir,impulse,element,hit_type);
			//m_pPhysicsShell->applyImpulseTrace(position_in_bone_space,dir,impulse);
//			else{
	/*			m_saved_hit_dir.set(dir);
				m_saved_impulse=impulse*(hit_type==ALife::eHitTypeExplosion ? 1.f : skel_fatal_impulse_factor);
				m_saved_element=element;
				m_saved_hit_position.set(p_in_object_space);*/
//			}
		}
	}
}

void CCharacterPhysicsSupport::in_UpdateCL()
{
	if(m_eState==esRemoved)
	{
		return;
	}
	if(m_pPhysicsShell&&m_pPhysicsShell->isFullActive())
	{

		//XFORM().set(m_pPhysicsShell->mXFORM);
		m_pPhysicsShell->InterpolateGlobalTransform(&mXFORM);
	}

	if(m_pPhysicsShell)
	{	
		if(m_pPhysicsShell->isActive())
		{
			if(!m_flags.test(fl_death_anim_on)&&m_pPhysicsShell->isFullActive())//!b_death_anim_on&&m_pPhysicsShell->isFullActive()
			{
				///if(m_eType==etStalker)
				{
						smart_cast<CKinematicsAnimated*>(m_EntityAlife.Visual())->PlayCycle("death_init");
				}
	
				//b_death_anim_on=true;
				m_flags.set(fl_death_anim_on,TRUE);
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

		m_PhysicMovementControl->DestroyCharacter();
		//m_EntityAlife.PHSetPushOut();
	}
}

void CCharacterPhysicsSupport::CreateSkeleton(CPhysicsShell* &pShell)
{

	R_ASSERT2(!pShell,"pShell already initialized!!");
	if (!m_EntityAlife.Visual())
		return;
#ifdef DEBUG
	CTimer t;t.Start();
#endif	
	pShell		= P_create_Shell();
	pShell->preBuild_FromKinematics(smart_cast<CKinematics*>(m_EntityAlife.Visual()));
	pShell->mXFORM.set(mXFORM);
	pShell->SetAirResistance(0.002f*skel_airr_lin_factor,
		0.3f*skel_airr_ang_factor);
	pShell->SmoothElementsInertia(0.3f);
	//pShell->set_JointResistance(0.f);
	pShell->set_PhysicsRefObject(&m_EntityAlife);
	SAllDDOParams disable_params;
	disable_params.Load(smart_cast<CKinematics*>(m_EntityAlife.Visual())->LL_UserData());
	pShell->set_DisableParams(disable_params);

	pShell->Build();
#ifdef DEBUG	
	Msg("shell for %s[%d] created in %f ms",*m_EntityAlife.cName(),m_EntityAlife.ID(),t.GetElapsed_sec()*1000.f);
#endif
}
void CCharacterPhysicsSupport::CreateSkeleton()
{
	if(m_pPhysicsShell) return;
Fvector velocity;
	m_PhysicMovementControl->GetCharacterVelocity(velocity);
	m_PhysicMovementControl->GetDeathPosition	(m_EntityAlife.Position());
	m_PhysicMovementControl->DestroyCharacter();
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
	//m_pPhysicsShell->SmoothElementsInertia(0.3f);
	m_pPhysicsShell->set_PhysicsRefObject(&m_EntityAlife);
	SAllDDOParams disable_params;
	disable_params.Load(smart_cast<CKinematics*>(m_EntityAlife.Visual())->LL_UserData());
	m_pPhysicsShell->set_DisableParams(disable_params);
	m_pPhysicsShell->set_JointResistance(0.f);
	m_pPhysicsShell->Activate(true);
	velocity.mul(1.25f*m_after_death_velocity_factor);
	m_pPhysicsShell->set_LinearVel(velocity);
	smart_cast<CKinematics*>(m_EntityAlife.Visual())->CalculateBones();
	//b_death_anim_on=false;
	m_flags.set(fl_death_anim_on,FALSE);
	m_eState=esDead;
}
void CCharacterPhysicsSupport::CollisionCorrectObjPos(const Fvector& start_from)
{
	Fvector shift;shift.sub(start_from,m_EntityAlife.Position());
	Fvector activation_pos;m_EntityAlife.Center(activation_pos);
	Fvector center_shift;center_shift.sub(m_EntityAlife.Position(),activation_pos);
	activation_pos.add(shift);
	CPHActivationShape activation_shape;
	Fbox box;box.set(m_EntityAlife.BoundingBox());
	Fvector vbox;
	box.getsize(vbox);
	activation_shape.Create(activation_pos,vbox,&m_EntityAlife);
	activation_shape.Activate(vbox,1,1.f,M_PI/8.f);
	m_EntityAlife.Position().set(activation_shape.Position());
	m_EntityAlife.Position().add(center_shift);
	activation_shape.Destroy();
}
void CCharacterPhysicsSupport::ActivateShell			(CObject* who)
{
	DestroyIKController();
	if(!m_physics_skeleton)CreateSkeleton(m_physics_skeleton);
	if(m_eType==etActor)
	{
		CActor* A=smart_cast<CActor*>(&m_EntityAlife);
		R_ASSERT2(A,"not an actor has actor type");
		if(A->Holder()) return;
		if(m_eState==esRemoved)return;
	}
	CKinematics* K=smart_cast<CKinematics*>(m_EntityAlife.Visual());
//////////////////////this needs to evaluate object box//////////////////////////////////////////////////////
	for(u16 I=K->LL_BoneCount()-1;I!=u16(-1);--I)K->LL_GetBoneInstance(I).reset_callback();

	K->CalculateBones_Invalidate();
	K->CalculateBones	();
////////////////////////////////////////////////////////////////////////////
	if(m_pPhysicsShell) return;
	Fvector velocity;
	m_PhysicMovementControl->GetCharacterVelocity		(velocity);
	velocity.mul(1.3f);
	Fvector dp;

	if(!m_PhysicMovementControl->CharacterExist())
									dp.set(m_EntityAlife.Position());
	else m_PhysicMovementControl->GetDeathPosition(dp);
	m_PhysicMovementControl->DestroyCharacter();

	CollisionCorrectObjPos(dp);



	R_ASSERT2(m_physics_skeleton,"No skeleton created!!");

	m_pPhysicsShell=m_physics_skeleton;
	m_physics_skeleton=NULL;
	
	m_pPhysicsShell->set_Kinematics(K);
	m_pPhysicsShell->RunSimulation();
	m_pPhysicsShell->mXFORM.set(mXFORM);
	m_pPhysicsShell->SetCallbacks(m_pPhysicsShell->GetBonesCallback());
	if(!smart_cast<CCustomZone*>(who))
	{
		velocity.mul(1.25f*m_after_death_velocity_factor);
	}
	m_pPhysicsShell->set_LinearVel(velocity);
	
	K->CalculateBones_Invalidate();
	K->CalculateBones	();
	//b_death_anim_on=false;
	m_flags.set(fl_death_anim_on,FALSE);
	m_eState=esDead;
	//b_skeleton_in_shell=true;
	m_flags.set(fl_skeleton_in_shell,TRUE);
	
	if(IsGameTypeSingle())
	{
		m_pPhysicsShell->SetPrefereExactIntegration	();//use exact integration for ragdolls in single
		m_pPhysicsShell->SetRemoveCharacterCollLADisable();
	}
	else
	{
		m_pPhysicsShell->SetIgnoreDynamic();
	}
	m_pPhysicsShell->SetIgnoreSmall();
}
void CCharacterPhysicsSupport::in_ChangeVisual()
{
	
	if(!m_physics_skeleton&&!m_pPhysicsShell) return;

	if(m_pPhysicsShell)
	{
		VERIFY(m_eType!=etStalker);
		if(m_physics_skeleton)
		{
			m_EntityAlife.processing_deactivate()	;
			m_physics_skeleton->Deactivate()		;
			xr_delete(m_physics_skeleton)			;
		}
		CreateSkeleton(m_physics_skeleton);
		if(m_pPhysicsShell)m_pPhysicsShell->Deactivate();
		xr_delete(m_pPhysicsShell);
		ActivateShell(NULL);
	}
	if(m_ik_controller)
	{
		DestroyIKController();
		CreateIKController();
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
	if(m_pPhysicsShell&&m_pPhysicsShell->isActive())
	{
		m_pPhysicsShell->get_LinearVel(velocity);
	}
	else
		movement()->GetCharacterVelocity(velocity);
		
}
 void _stdcall CCharacterPhysicsSupport:: IKVisualCallback(CKinematics* K)
{
	CEntityAlive* EA=smart_cast<CEntityAlive*>((CGameObject*)K->Update_Callback_Param);
	VERIFY(EA);
	CCharacterPhysicsSupport* chs=EA->character_physics_support();
	VERIFY(chs);
	chs->CalculateIK(K);
}
void CCharacterPhysicsSupport::CreateIKController()
{

	//VERIFY(!m_ik_controller);
	//m_ik_controller=xr_new<CIKLimbsController>();
	//CKinematics* K=smart_cast<CKinematics*>(m_EntityAlife.Visual());
	//m_ik_controller->Create(K,&m_EntityAlife);
	//m_EntityAlife.add_visual_callback(IKVisualCallback);
}
void CCharacterPhysicsSupport::DestroyIKController()
{
	//if(!m_ik_controller)return;
	//m_EntityAlife.remove_visual_callback(IKVisualCallback);
	//m_ik_controller->Destroy();
	//xr_delete(m_ik_controller);
}

void CCharacterPhysicsSupport::CalculateIK(CKinematics* K)
{
	m_ik_controller->Calculate(K,mXFORM);
}

void		 CCharacterPhysicsSupport::in_NetRelcase(CObject* O)																													
{
	CPHCapture* c=m_PhysicMovementControl->PHCapture();
	if(c)
	{
		c->net_Relcase(O);
	}
}
 
bool CCharacterPhysicsSupport::set_collision_hit_callback(SCollisionHitCallback* cc)
{
	if(!cc)
	{
		m_collision_hit_callback=NULL;
		return true;
	}
	if(m_pPhysicsShell)
	{
		VERIFY2(cc->m_collision_hit_callback!=0,"No callback function");
		m_collision_hit_callback=cc;
		return true;
	}else return false;
}
SCollisionHitCallback * CCharacterPhysicsSupport::get_collision_hit_callback()
{
	return m_collision_hit_callback;
}